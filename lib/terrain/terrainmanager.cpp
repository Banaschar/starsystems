#include "terrainmanager.hpp"

#include <glm/gtx/norm.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/string_cast.hpp>
#include <cmath>

//#include <cstdlib>

#include "global.hpp"
#include "drawablefactory.hpp"
TerrainManager::TerrainManager() {}
TerrainManager::TerrainManager(TerrainObject *obj) {
    addTerrainObject(obj);
}

TerrainManager::~TerrainManager() {
    for (TerrainObject *t : terrainObjectList)
        delete t;
}

void TerrainManager::addTerrainObject(TerrainObject *obj) {
    terrainObjectList.push_back(obj);
    terrainObjectRenderDataVector_.emplace_back();
}

TerrainType TerrainManager::getType() {
    return type_;
}

void TerrainManager::update(View *view) {

    for (int i = 0; i < terrainObjectRenderDataVector_.size(); ++i) {
        terrainObjectRenderDataVector_[i].land.clear();
        terrainObjectRenderDataVector_[i].water.clear();
        terrainObjectList[i]->update(view, &terrainObjectRenderDataVector_[i]);
    }
}

TerrainDrawDataContainer &TerrainManager::getTerrainRenderData() {
    return terrainRenderData_;
}

/* Terrain Chunk Tree (Test tree) */
TerrainChunkTree::TerrainChunkTree() {
    baseDrawData.size = 1;
}

void TerrainChunkTree::update(View *view, TerrainObjectRenderData *terrainObjectRenderData) {
    terrainObjectRenderData->land = &terrainDrawData;
    terrainObjectRenderData->attributes = &terrainAttributes_;
}

void TerrainChunkTree::addTerrainChunk(Drawable *drawable) {
    terrainDrawData.drawableList.push_back(drawable);
}

/* A planet has 3 levels: 
 * 1. Very high distance: Just a small sphere with minimal details 
 * 2. Medium distance, still whole planet visible. Tree with one root node per cubeSide. HeightMap must be the
 *      size of 2*planet radius (But with a detail level of 12)
 * 3. Close up: 3*3 grid with current pos in the middle. Only one inside the atmosphere. Maybe radius + 0.7 * atmosphereHeight
 */
PlanetTree::PlanetTree(TerrainGenerator *terrainGen) terrainGen_(terrainGen) {
    initPlanet();
}

void PlanetTree::initPlanet() {
    int rootNodeDimension_ = terrainGen_->getSphereRadius() * 2;
    int lodLevelCount_ = 6;
    float maxViewDistance = terrainGen_->getSphereRadius() * 25; // Distance from which the planet is visible in lod detail

    leafNodeSize_ = rootNodeDimension_;
    for (int i = 0; i < lodLevelCount_; ++i) {
        leafNodeSize_ /= 2;
    }

    float minLodDist = (float)leafNodeSize_;
    planetRanges_.resize(lodLevelCount_);
    for (int i = 0; i < lodLevelCountMedium; ++i)
        planetRanges_[i] = planetRanges_ * pow(2, i);

    planetRanges_[lodLevelCount_ - 1] = maxViewDistance;

    /* Create initial root nodes for each cube side */
    glm::vec3 axis;
    int cnt = 0;
    for (int i = 0; i < 3; i++) {
        axis = glm::vec3(0,0,0);
        axis[i] = 1;
        axisIntegerMap_[axis] = cnt++;
        axis[i] = -1;
        axisIntegerMap_[axis] = cnt++;
    }

    for (auto &kv : axisIntegerMap_) {
        glm::vec2 planePos = vec3ToVec2CubeSide(planetOrigin - planetRadius, kv.first);
        rootNodeList_.push_back(createRootNode(planePos, kv.first));
    }
}

void PlanetTree::initGlobalTextures() {

}

TerrainNode_ *PlanetTree::createRootNode(glm::vec2 &cornerPos, glm::vec3 &axis) {
    HeightMap *heightMap = new HeightMap(terrainGen_, cornerPos, axis, rootNodeDimension_, heightMapIndex_);
    TerrainNode_ *node = new TerrainNode_(heightMap, rootNodeDimension_, lodLevelCount_-1, cornerPos);
    heightMap->cleanUpMapData();
    heightMaps_[heightMapIndex_] = heightMap;
    
    heightMapTextures_[heightMapIndex_];
    heightMapTextures_[heightMapIndex_].emplace_back(heightMap->getHeightTexture(), "texture_height");
    heightMapTextures_[heightMapIndex_].emplace_back(heightMap->getNormalTexture(), "texture_normal");
    instanceData_.createNewInstance(heightMapIndex_, DrawableFactory::createPrimitive(PrimitiveType::PLANE, ShaderType::SHADER_TYPE_TERRAIN, leafNodeSize_));
    ++heightMapIndex_;

    return node;
}

void PlanetTree::update(View *view, TerrainObjectRenderData *renderData) {
    planetDrawData_.listOfDrawableLists.clear();
    planetDrawData_.listOfTextureLists.clear();

    for (auto &kv : rootNodeMap_) {
        kv.second->lodSelect(planetRanges_, lodLevelCount_-1, view, selectedNodes_, heighMapCreateList_);
    }

    glm::vec3 currentAxis;
    glm::vec3 rotationAxis = glm::vec3(1,0,0);
    float rotationDegree = 0.0f;
    int instanceIndex = 0;
    for (auto &mapIndexNodeList : selectedNodes_) {
        if (mapIndexNodeList.second.empty())
            continue;
        
        instanceData_.updateInstanceSize(mapIndexNodeList.first, mapIndexNodeList.second.size());
        glm::vec3 currentAxis = heightMaps_[currentHeightMapIndex]->getAxis();
        glm::vec3 rotationAxis = getRotationAxis(currentAxis, &rotationDegree);
        int instanceIndex = 0;

        for (TerrainNode_ *node : mapIndexNodeList.second) {
            float scale = node->getSize() / leafNodeSize_;
            glm::vec3 translate = vec2ToVec3CubeSide(node->getPosition(), currentAxis);
            glm::vec3 scale = glm::vec3(scale);
            baseMesh_->transform(instanceIndex++, &scale, &translate, &rotationAxis, rotationDegree);
            instanceData_.insertAttribute(node->getRange(), getPrevRange(node->getLodLevel()), scale);
        }

        instanceData_.finishInstance(mapIndexNodeList.first);
        mapIndexNodeList.second.clear(); /* Clear list for next frame */

        planetDrawData_.listOfDrawableLists.push_back(&instanceData_.baseMeshListMap[mapIndexNodeList.first]);
        planetDrawData_.listOfTextureLists.push_back(&heightMapTextures_[mapIndexNodeList.first]);
    }

    renderData->land = &planetDrawData_;
    renderData->attributes = &planetAttributes_;
}

float PlanetTree::getPrevRange(int lodLevel) {
    return lodLevel == 0 ? 0.0f : ranges_[lodLevel - 1];
}

glm::vec3 PlanetTree::vec2ToVec3CubeSide(glm::vec2 &pos, glm::vec3 &axis) {
    glm::vec3 ret;
    if (axis.x)
        ret = glm::vec3(planetOrigin_ + axis.x * planetRadius_, pos.y, pos.x);
    else if (axis.y)
        ret = glm::vec3(pos.x, planetOrigin_ + axis.y * planetRadius_, pos.y);
    else
        ret = glm::vec3(pos.x, pos.y, planetOrigin_ + axis.z * planetRadius_);

    return ret;
}

glm::vec2 PlanetTree::vec3ToVec2CubeSide(glm::vec3 &pos, glm::vec3 &axis) {
    glm::vec2 ret;

    if (axis.x)
        ret = glm::vec2(pos.y, pos.z);
    else if (axis.y)
        ret = glm::vec2(pos.x, pos.z);
    else
        ret = glm::vec2(pos.x, pos.y);

    return ret;
}

glm::vec3 PlanetTree::getRotationAxis(glm::vec3 &axis, float *degree) {
    glm::vec3 rotAx;

    if (axis.x) {
        rotAx = glm::vec3(0,0,1);
        *degree = 90.0f;
    }
    else if (axis.y) {
        rotAx = glm::vec3(0,1,0);
        *degree = 0.0f;
    }
    else {
        rotAx = glm::vec3(1,0,0);
        *degree = 90.0f;
    }

    return rotAx;
}

/*
 * Based on paper CDLOD by strugar
 * 
 * I need a grid of root trees for every side. So from space only the root nodes
 * of the tree sides visible are rendered. Coming closer, parts of the cube sides need to be culled as well.
 * Very close up, it should be the 9 root nodes moving over the cube.
 */
TerrainCDLODTree::TerrainCDLODTree(TerrainGenerator *terrainGen) : terrainGen_(terrainGen) {
    fprintf(stdout, "Creating CDLOD Tree\n");
    nodesToDraw_.reserve(32);

    int leafNodeSize = 16.0f;
    lodLevelCount_ = 5;
    int heightMapSize = 256;

    int rootNodeSize = leafNodeSize * pow(2, lodLevelCount_-1);

    /* TODO: I have to either create one basePatch per root node, with the height and normal map added as textures
     * Or the renderer has to apply the textures based on heightmapindex...hm, doesn't work...
     */
    HeightMap *heightMap = new HeightMap(terrainGen, glm::vec3(0,0,0), glm::vec3(0,1,0), heightMapSize, heightMapIndex_++);
    glm::vec3 nodePos = glm::vec3(0,0,0);

    // From cdlod ogl imple
    float minLodDist = 16.0f;
    ranges_.resize(lodLevelCount_);
    for (int i = 0; i < lodLevelCount_; ++i) {
        ranges_[i] = minLodDist * pow(2, i);
        fprintf(stdout, "LodRange %i: %f\n", i, ranges_[i]);
    }

    grid_.resize(1);
    grid_[0].resize(1);
    grid_[0][0] = new TerrainNode_(heightMap, rootNodeSize, lodLevelCount_-1, nodePos);

    heightMap->cleanUpMapData();

    basePatch_ = DrawableFactory::createPrimitive(PrimitiveType::PLANE, ShaderType::SHADER_TYPE_TERRAIN, 16);
    
    Texture hmap;
    Texture nMap;
    hmap.id = heightMap->getHeightTexture();
    hmap.type = "texture_height";
    nMap.id = heightMap->getNormalTexture();
    nMap.type = "texture_normal";
    basePatch_->addTexture(hmap);
    basePatch_->addTexture(nMap);

    rangeAttribData_.numElements = 3;
    rangeAttribData_.sizeOfDataType = sizeof(glm::vec3);
}

void TerrainCDLODTree::init(glm::vec3 pos) {

}

TerrainNode_ *TerrainCDLODTree::createNode(glm::vec2 rootNodeOrigin, glm::vec3 rootNodeAxis) {

}

TerrainCDLODTree::~TerrainCDLODTree() {
    delete basePatch_;
}

float TerrainCDLODTree::getPrevRange(int lodLevel) {
    return lodLevel == 0 ? 0.0f : ranges_[lodLevel - 1];
}

void TerrainCDLODTree::updateRootNodes(glm::vec3 &camPos) {

}

void TerrainCDLODTree::update(View *view) {
    terrainList->clear();
    nodesToDraw_.clear();
    
    /* Select nodes */
    for (std::vector<TerrainNode_ *> &gridRow : grid_) {
        for (TerrainNode_ *root : gridRow) {
            root->lodSelect(ranges_, lodLevelCount_-1, view, &nodesToDraw_);
        }
    }
    
    if (nodesToDraw_.empty()) {
        return;
    }

    /* Build the instance model matrices (translation and scale) and the list of ranges*/
    /* TODO: Use emplace_back for better performance! */
    basePatch_->updateInstanceSize(nodesToDraw_.size());
    instanceVecMorphAttribs_.resize(nodesToDraw_.size());
    rangeAttribData_.size = nodesToDraw_.size();
    for (int i = 0; i < nodesToDraw_.size(); ++i) {
        glm::vec3 translate = nodesToDraw_[i]->getPosition();
        glm::vec3 scale = glm::vec3(nodesToDraw_[i]->getSize() / 16.0f); // Node dimension / mesh dimension
        basePatch_->transform(i, &scale, &translate, NULL);
        instanceVecMorphAttribs_[i] = glm::vec3(nodesToDraw_[i]->getRange(), getPrevRange(nodesToDraw_[i]->getLodLevel()), nodesToDraw_[i]->getSize() / 16.0f);
    }

    rangeAttribData_.data = static_cast<void *>(instanceVecMorphAttribs_.data());
    basePatch_->updateMeshInstances(&rangeAttribData_);

    terrainList->push_back(basePatch_);
}

CdlodRenderer::update() {

}