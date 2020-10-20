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
    for (TerrainObject *t : terrainObjectList_)
        delete t;
}

void TerrainManager::addTerrainObject(TerrainObject *obj) {
    terrainObjectList_.push_back(obj);
    terrainObjectRenderDataVector_.emplace_back();
}

TerrainType TerrainManager::getType() {
    return type_;
}

void TerrainManager::update(View *view) {

    for (int i = 0; i < terrainObjectRenderDataVector_.size(); ++i) {
        terrainObjectList_[i]->update(view, &terrainObjectRenderDataVector_[i]);
    }
}

TerrainRenderDataVector &TerrainManager::getTerrainRenderData() {
    return terrainObjectRenderDataVector_;
}

/* Terrain Chunk Tree (Test tree) */
TerrainChunkTree::TerrainChunkTree() {
    terrainDrawData_.size = 1;
    terrainAttributes_.bodyOrigin = glm::vec3(0,0,0);
    terrainAttributes_.bodyRadius = 0.0f;
    terrainAttributes_.hasAtmosphere = false;
    terrainAttributes_.hasWater = false;
    terrainAttributes_.landShaderType = ShaderType::SHADER_TYPE_TERRAIN;
}

void TerrainChunkTree::update(View *view, TerrainObjectRenderData *terrainObjectRenderData) {
    terrainObjectRenderData->land = &terrainDrawData_;
    terrainObjectRenderData->attributes = &terrainAttributes_;
}

void TerrainChunkTree::addTerrainChunk(Drawable *drawable) {
    terrainDrawData_.drawableList.push_back(drawable);
}

/* A planet has 3 levels: 
 * 1. Very high distance: Just a small sphere with minimal details 
 * 2. Medium distance, still whole planet visible. Tree with one root node per cubeSide. HeightMap must be the
 *      size of 2*planet radius (But with a detail level of 12)
 * 3. Close up: 3*3 grid with current pos in the middle. Only one inside the atmosphere. Maybe radius + 0.7 * atmosphereHeight
 */
PlanetCdlodImplementation::PlanetCdlodImplementation(TerrainGenerator *terrainGen, TerrainObjectAttributes *terrainAttribs) 
                                                        : terrainGen_(terrainGen), planetAttributes_(terrainAttribs) {}

void PlanetCdlodImplementation::createTree(CdlodTreeData &treeData) {
    rootNodeDimension_ = planetAttributes_->bodyRadius * 2;
    int lodLevelCount = 6;
    float maxViewDistance = planetAttributes_->bodyRadius * 25; // Distance from which the planet is visible in lod detail

    int leafNodeSize = rootNodeDimension_;
    for (int i = 0; i < lodLevelCount; ++i) {
        leafNodeSize /= 2;
    }

    float minLodDist = (float)leafNodeSize;
    treeData.ranges->resize(lodLevelCount);
    for (int i = 0; i < lodLevelCount; ++i)
        (*treeData.ranges)[i] = minLodDist * pow(2, i);

    (*treeData.ranges)[lodLevelCount - 1] = maxViewDistance;

    *treeData.lodLevelCount = lodLevelCount;

    /* Create initial root nodes for each cube side */
    glm::vec3 axis;
    int cnt = 0;
    for (int i = 0; i < 3; i++) {
        axis = glm::vec3(0,0,0);
        axis[i] = 1;
        glm::vec2 planePos = vec3ToVec2CubeSide(planetAttributes_->bodyOrigin, axis);
        createRootNode(treeData, planePos, axis);
        axis[i] = -1;
        planePos = vec3ToVec2CubeSide(planetAttributes_->bodyOrigin, axis);
        createRootNode(treeData, planePos, axis);
    }
}

void PlanetCdlodImplementation::updateRootNodes(CdlodTreeData &data, std::vector<TerrainNode_ *> &heightMapDemandList) {
    ;
}

void PlanetCdlodImplementation::createRootNode(CdlodTreeData &treeData, glm::vec2 &cornerPos, glm::vec3 &axis) {
    HeightMap *heightMap = new HeightMap(terrainGen_, cornerPos, axis, rootNodeDimension_, *treeData.heightMapIndex);
    treeData.rootNodes->push_back(new TerrainNode_(heightMap, rootNodeDimension_, *treeData.lodLevelCount - 1, cornerPos));
    heightMap->cleanUpMapData();
    (*treeData.heightMaps)[*treeData.heightMapIndex] = heightMap;
    
    (*treeData.heightMapTextures)[*treeData.heightMapIndex];
    (*treeData.heightMapTextures)[*treeData.heightMapIndex].emplace_back(heightMap->getHeightTexture(), "texture_height");
    (*treeData.heightMapTextures)[*treeData.heightMapIndex].emplace_back(heightMap->getNormalTexture(), "texture_normal");
    (*treeData.baseMeshListMap)[*treeData.heightMapIndex] = DrawableList(1, DrawableFactory::createPrimitivePlane(axis, *treeData.leafNodeSize));
    ++(*treeData.heightMapIndex);
}

glm::vec2 PlanetCdlodImplementation::vec3ToVec2CubeSide(glm::vec3 &pos, glm::vec3 &axis) {
    glm::vec2 ret;

    if (axis.x)
        ret = glm::vec2(pos.y, pos.z);
    else if (axis.y)
        ret = glm::vec2(pos.x, pos.z);
    else
        ret = glm::vec2(pos.x, pos.y);

    return ret;
}

/*
PlaneCdlodImplementation::PlaneCdlodImplementation(TerrainGenerator *terrainGen) : terrainGen_(terrainGen) {}

PlaneCdlodImplementation::createTree(CdlodTreeData &data) {
    nodesToDraw_.reserve(32);

    int leafNodeSize = 16.0f;
    lodLevelCount_ = 5;
    int heightMapSize = 256;

    int rootNodeSize = leafNodeSize * pow(2, lodLevelCount_-1);

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
*/

CdlodTree::CdlodTree(CdlodTreeImplementation *imple, TerrainObjectAttributes *attribs) : treeImplementation_(imple), terrainAttributes_(attribs){
    init();
}

CdlodTree::~CdlodTree() {
    // TODO: Delete everything
    delete treeImplementation_;
}

void CdlodTree::init() {
    publicTreeData_.rootNodes = &rootNodeList_;
    publicTreeData_.heightMaps = &heightMaps_;
    publicTreeData_.heightMapTextures = &heightMapTextures_;
    publicTreeData_.baseMeshListMap = &meshInstanceData_.baseMeshListMap;
    publicTreeData_.ranges = &ranges_;
    publicTreeData_.heightMapIndex = &heightMapIndex_;
    publicTreeData_.leafNodeSize = &leafNodeSize_;
    publicTreeData_.lodLevelCount = &lodLevelCount_;
    treeImplementation_->createTree(publicTreeData_);
}

void CdlodTree::createRootNodes() {
    treeImplementation_->updateRootNodes(publicTreeData_, heightMapCreationList_);
}


void CdlodTree::update(View *view, TerrainObjectRenderData *renderData) {
    landDrawData_.listOfDrawableLists.clear();
    landDrawData_.listOfTextureLists.clear();
    landDrawData_.size = 0;
    selectedNodes_.clear();

    for (TerrainNode_ *node : rootNodeList_) {
        node->lodSelect(ranges_, lodLevelCount_-1, view, selectedNodes_, heightMapCreationList_);
    }

    glm::vec3 currentAxis;
    glm::vec3 rotationAxis = glm::vec3(1,0,0);
    float rotationDegree = 0.0f;
    for (auto &mapIndexNodeList : selectedNodes_) {
        if (mapIndexNodeList.second.empty())
            continue;
        
        ++landDrawData_.size;
        // Todo: Bad and costly lookup. This has to go.
        if (meshInstanceData_.instanceAttribListMap.find(mapIndexNodeList.first) == meshInstanceData_.instanceAttribListMap.end())
            meshInstanceData_.createNewInstance(mapIndexNodeList.first);
        meshInstanceData_.updateInstanceSize(mapIndexNodeList.first, mapIndexNodeList.second.size());
        glm::vec3 currentAxis = heightMaps_[mapIndexNodeList.first]->getAxis();
        glm::vec3 rotationAxis = getRotationAxis(currentAxis, &rotationDegree);
        int instanceIndex = 0;

        /* Update model matrices and additional attributes for each instance belonging to the current heightmap */
        for (TerrainNode_ *node : mapIndexNodeList.second) {
            float scale = node->getSize() / leafNodeSize_;
            glm::vec3 translate = terrainAttributes_->bodyRadius ? vec2ToVec3CubeSide(node->getPosition(), currentAxis) : glm::vec3(node->getPosition().x, 0, node->getPosition().y);
            glm::vec3 scaleVec = glm::vec3(scale);
            meshInstanceData_.baseMeshListMap[mapIndexNodeList.first][0]->transform(instanceIndex++, &scaleVec, &translate, &rotationAxis, rotationDegree);
            meshInstanceData_.insertAttribute(node->getRange(), getPrevRange(node->getLodLevel()), scale);
        }

        meshInstanceData_.finishInstance(mapIndexNodeList.first);

        landDrawData_.listOfDrawableLists.push_back(&meshInstanceData_.baseMeshListMap[mapIndexNodeList.first]);
        landDrawData_.listOfTextureLists.push_back(&heightMapTextures_[mapIndexNodeList.first]);
    }

    renderData->land = &landDrawData_;
    renderData->attributes = terrainAttributes_;

    /* Handle heightmap creation list */
    if (heightMapCreationList_.size()) {
        //gl_threadPool.addJob();
        ;
    }
}

glm::vec3 CdlodTree::vec2ToVec3CubeSide(glm::vec2 &pos, glm::vec3 &axis) {
    glm::vec3 ret;
    if (axis.x)
        ret = glm::vec3(terrainAttributes_->bodyOrigin.x + axis.x * terrainAttributes_->bodyRadius, pos.y, pos.x);
    else if (axis.y)
        ret = glm::vec3(pos.x, terrainAttributes_->bodyOrigin.y + axis.y * terrainAttributes_->bodyRadius, pos.y);
    else
        ret = glm::vec3(pos.x, pos.y, terrainAttributes_->bodyOrigin.z + axis.z * terrainAttributes_->bodyRadius);

    return ret;
}

glm::vec3 CdlodTree::getRotationAxis(glm::vec3 &axis, float *degree) {
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

float CdlodTree::getPrevRange(int lodLevel) {
    return lodLevel == 0 ? 0.0f : ranges_[lodLevel - 1];
}

// TODO: Radius and origin at 2 different positions in memory...think about it and change
// TODO: Provide terrainAttributes to the class
Planet::Planet(TerrainGenerator *terrainGen) : terrainGen_(terrainGen) {
    terrainAttributes_.bodyOrigin = terrainGen_->getSphereOrigin();
    terrainAttributes_.bodyRadius = terrainGen_->getSphereRadius();
    terrainAttributes_.hasAtmosphere = false;
    terrainAttributes_.hasWater = false;
    terrainAttributes_.landShaderType = ShaderType::SHADER_TYPE_TERRAIN;

    initPlanet();
}

Planet::~Planet() {
    //TODO: delete everything
}

void Planet::initPlanet() {
    PlanetCdlodImplementation *imple = new PlanetCdlodImplementation(terrainGen_, &terrainAttributes_);
    lodTree_ = new CdlodTree(imple, &terrainAttributes_);
}

void Planet::update(View *view, TerrainObjectRenderData *terrainObjectRenderData) {
    lodTree_->update(view, terrainObjectRenderData);
}