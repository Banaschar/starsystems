#include "terrainmanager.hpp"

#include <glm/gtx/norm.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/string_cast.hpp>
#include <cmath>

//#include <cstdlib>

#include "global.hpp"
#include "drawablefactory.hpp"

TerrainManager::TerrainManager(TerrainGenerator *terrainGen, int initialDimension, int lodLevels, TerrainType type, glm::vec3 origin) : terrainGenerator_(terrainGen) {
    if (!createQuadTree(initialDimension, lodLevels, type, origin)) {
        fprintf(stdout, "TERRAINMANAGER: Could not create Quad Tree\n");
    }
}

TerrainManager::~TerrainManager() {
    if (terrainQuadTree_)
        delete terrainQuadTree_;

    delete terrainGenerator_;
}

void TerrainManager::createDefaultTerrainGenerator() {
    PerlinNoise pNoise = PerlinNoise(5, 10.0f, 0.09f, 0);
    terrainGenerator_ = new TerrainGenerator(pNoise);
}

/*
 * TODO: FIX ISSUE IF DIMENSION IS LARGER THAN 2^6 * 60 = 3840
 * PROVIDE a list of possible lod levels
 */
bool TerrainManager::createQuadTree(int initialDimension, int lodLevels, TerrainType type, glm::vec3 origin) {
    for (int i = 2; i <= 12; i += 2) {
        if (initialDimension % i != 0) {
            fprintf(stdout, "[TERRAINMANAGER] Error: Dimension not divisible by %i\n", i);
            return false;
        }
    }

    int tmp = initialDimension;
    int lod = 1;
    while (tmp > 60) {
        lod = lod == 1 ? 2 : lod + 2;
        tmp /= 2;
    }

    if (type == TerrainType::SPHERE) {
        terrainGenerator_->setSphereRadius(initialDimension / 2);
        terrainGenerator_->setSphereOrigin(origin);
        terrainCubeTree_ = new TerrainCubeTree(terrainGenerator_, initialDimension, lod, origin);
    } else
        terrainQuadTree_ = new TerrainQuadTree(initialDimension, lod, terrainGenerator_);

    return true;
}

void TerrainManager::update(glm::vec3 &camPosition, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist) {
    if (terrainQuadTree_)
        terrainQuadTree_->update(camPosition, tlist, wlist);
    else
        terrainCubeTree_->update(camPosition, tlist, wlist);
}

/*
 * MINIMUM PLANET DIMENSION: 3 * 60! Then there fits exactly one quadtree with 9 root nodes inside one side
 */
TerrainCubeTree::TerrainCubeTree(TerrainGenerator *terrainGen, int dimension, int lod, glm::vec3 sphereOrigin) 
            : terrainGenerator_(terrainGen), cubeSideDimension_(dimension), maxLodQuadTree_(lod), sphereOrigin_(sphereOrigin) {
    sphereRadius_ = cubeSideDimension_ / 2;
    initTree(dimension);
}

TerrainCubeTree::~TerrainCubeTree() {
    for (TerrainChunk *t : cubeSides_) {
        delete t;
    }

    for (auto &kv : cubeSideMap_)
        delete kv.second;
}

void TerrainCubeTree::createCubeSideTree(glm::vec3 &camPos) {
    // TODO: usefull value retrival of cubeSideRootNodeDimension
    int cubeSideRootNodeDimension = cubeSideDimension_ / 4;
    int lod = 4;
    for (auto &kv : axisIntegerMap_) {
        cubeSideMap_[kv.first] = new CubeSideTree(kv.first, terrainGenerator_, &cubeSideMap_, &axisIntegerMap_, sphereOrigin_, cubeSideDimension_, cubeSideRootNodeDimension, lod);
    }

    glm::vec3 axis;
    glm::vec3 cubePosition;
    computeInitialCubePositionAndAxis(camPos, &axis, &cubePosition);
    cubeSideMap_[axis]->setInitialRun();
    currentCubeSide_ = cubeSideMap_[axis];

    fprintf(stdout, "[TERRAINCUBETREE]->(createCubeSideTree): Done. Current Axis: %s\n", glm::to_string(currentCubeSide_->getAxis()).c_str());
}

void TerrainCubeTree::destroyCubeSideTree() {
    for (auto &kv : cubeSideMap_)
        delete kv.second;

    cubeSideMap_.clear();
}

bool TerrainCubeTree::verifyPosIsOnAxis(glm::vec3 &axis, glm::vec3 &pos) {
    glm::vec3 border(sphereOrigin_.x + sphereRadius_, sphereOrigin_.y + sphereRadius_, sphereOrigin_.z + sphereRadius_);
    bool ret = true;
    switch (axisIntegerMap_[axis]) {
        case 0:
        case 1:
            if (abs(pos.y) > border.y || abs(pos.z) > border.z)
                ret = false;
            break;
        case 2:
        case 3:
            if (abs(pos.x) > border.x || abs(pos.z) > border.z)
                ret = false;
            break;
        case 4:
        case 5:
            if (abs(pos.x) > border.x || abs(pos.y) > border.y)
                ret = false;
            break;
        default:
            fprintf(stdout, "[TERRAINCUBETREE] Error: Unexpected Key in CubeSideMap: %i\n", axisIntegerMap_[axis]);
    }
}

void TerrainCubeTree::computeInitialCubePositionAndAxis(glm::vec3 &camPos, glm::vec3 *axisOut, glm::vec3 *positionOut) {
    int halfdim = cubeSideDimension_ / 2;
    float intersectionDistance;
    float distanceCenterCorner = glm::distance(glm::vec3(sphereOrigin_.x + halfdim, sphereOrigin_.y + halfdim, sphereOrigin_.z + halfdim), sphereOrigin_);
    
    for (int i = 0; i < 3; i++) {
        glm::vec3 axis(0,0,0);
        axis[i] = -1;
        // intersectRayPlane(rayOrigin, rayDirection, planeOrigin, planeNormal, intersectionDistance)
        if (glm::intersectRayPlane(sphereOrigin_, glm::normalize(camPos - sphereOrigin_), glm::vec3(sphereOrigin_.x - halfdim, sphereOrigin_.y - halfdim, sphereOrigin_.z - halfdim), axis, intersectionDistance)) {
            if (intersectionDistance < distanceCenterCorner) {
                *axisOut = axis;
                *positionOut = sphereOrigin_ + intersectionDistance * glm::normalize(camPos - sphereOrigin_);
                if (verifyPosIsOnAxis(*axisOut, *positionOut))
                    break;
            }
        }
        axis[i] = 1;
        if (glm::intersectRayPlane(sphereOrigin_, glm::normalize(camPos - sphereOrigin_), glm::vec3(sphereOrigin_.x + halfdim, sphereOrigin_.y + halfdim, sphereOrigin_.z + halfdim), axis, intersectionDistance)) {
            if (intersectionDistance < distanceCenterCorner) {
                *axisOut = axis;
                *positionOut = sphereOrigin_ + intersectionDistance * glm::normalize(camPos - sphereOrigin_);
                if (verifyPosIsOnAxis(*axisOut, *positionOut))
                    break;
            }
        }
    }

    *positionOut = sphereOrigin_ + intersectionDistance * glm::normalize(camPos - sphereOrigin_);
}

glm::vec3 TerrainCubeTree::getChildPosition(glm::vec3 &pos, int x, int z, int offset, glm::vec3 &axis) {
    glm::vec3 ret;
    switch (axisIntegerMap_.at(axis)) {
        case 0:
        case 1:
            ret = glm::vec3(pos.x, pos.y + z * offset, pos.z + x * offset);
            break;
        case 2:
        case 3:
            ret = glm::vec3(pos.x + x * offset, pos.y, pos.z + z * offset);
            break;
        case 4:
        case 5:
            ret = glm::vec3(pos.x + x * offset, pos.y + z * offset, pos.z);
            break;
        default:
            fprintf(stdout, "[TERRAINCUBETREE] getChildPosition: Key error: %i\n", axisIntegerMap_[axis]);
            ret = glm::vec3(-1,-1,-1);
            break;
    }

    return ret;
}

glm::vec3 TerrainCubeTree::cubeWorldPosToSpherePos(glm::vec3 &cubePos) {
    return glm::vec3(sphereOrigin_ + (float)sphereRadius_ * glm::normalize(cubePos - sphereOrigin_));
}

void TerrainCubeTree::createChildren(TerrainChunk *node) {
    int childDimension = (node->getDimension() - 1) / 2;
    int childPosOffset = childDimension / 2;
    int childLod = glm::max(1, node->getLod() - 2);

    glm::vec3 nodePosition = node->getPosition();

    for (int z = -1; z < 2; z += 2) {
        for (int x = -1; x < 2; x += 2) {
            glm::vec3 pos = getChildPosition(node->getPosition(), x, z, childPosOffset, node->getTerrain()->getAxis());
            node->addChild(new TerrainChunk(new Terrain(terrainGenerator_,
                                                childDimension + 1,
                                                pos, childLod, node->getTerrain()->getAxis()),
                                            new WaterTile(terrainGenerator_, childDimension + 1, pos, planetSizeLod_, node->getTerrain()->getAxis())));
        }
    }
}

// TODO: Move the update nodes function to terrainCubeTree, provide kv.second as first argument
void TerrainCubeTree::updateCubeSides(glm::vec3 &camPosition, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist) {
    int cnt = 0;
    for (auto &kv : cubeSideMap_) {
        if (kv.second->hasActiveNodes()) {
            kv.second->updateNodes(camPosition, tlist, wlist);
            cnt += kv.second->getNumRootNodes();
        }
    }
    if (cnt > 9)
        fprintf(stdout, "[TERRAINCUBETREE] ERROR: Num root nodes: %i\n", cnt);
}

/*
 * Create children for CubeTree -> max 3 (more?) levels before switching to CubeSideTree
 */
void TerrainCubeTree::updateNode_(TerrainChunk *node, glm::vec3 camWorldPos, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist) {
    if (!node)
        return; 

    if ((node->getLod() <= planetSizeLod_ - 6) ||
            (glm::distance2(camWorldPos, cubeWorldPosToSpherePos(node->getPosition())) > glm::pow(node->getDimension() * 2.5f, 2))) {
        tlist->push_back(node->getTerrain());
        wlist->push_back(node->getWater());
    } else {
        if (node->getIndex() != 4) {
            tlist->push_back(node->getTerrain());
            wlist->push_back(node->getWater());

            if (!node->isScheduled()) {
                node->setChildrenScheduled();
                threadPool->addJob(std::bind(&TerrainCubeTree::createChildren, this, node));
            }
        } else {
            for (TerrainChunk *child : node->getChildren())
                updateNode_(child, camWorldPos, tlist, wlist);
        }
    }
}

void TerrainCubeTree::update(glm::vec3 &camPosition, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist) {
    tlist->clear();
    wlist->clear();

    if (glm::distance2(camPosition, sphereOrigin_) < glm::pow(sphereRadius_ * 1.5f, 2)) {
        if (currentCubeSide_) {
            currentCubeSide_ = currentCubeSide_->update(camPosition);
            updateCubeSides(camPosition, tlist, wlist);
        } else {
            createCubeSideTree(camPosition);
        }
    } else {
        if (currentCubeSide_) {
            destroyCubeSideTree();
            currentCubeSide_ = NULL;
        }
        for (TerrainChunk *t : cubeSides_)
            updateNode_(t, camPosition, tlist, wlist);
    }
}

void TerrainCubeTree::initTree(int dimension) {
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
        //cubeSides_.push_back(new TerrainChunk(new Terrain(terrainGenerator_, dimension + 1, glm::vec3(0,0,0), planetSizeLod_, kv.first), NULL));
        glm::vec3 pos = glm::vec3(0,0,0);
        if (kv.first.x)
            pos.x = kv.first.x;
        else if (kv.first.y)
            pos.y = kv.first.y;
        else
            pos.z = kv.first.z;
        cubeSides_.push_back(new TerrainChunk(new Terrain(terrainGenerator_, dimension + 1, pos, planetSizeLod_, kv.first), 
                                new WaterTile(terrainGenerator_, dimension + 1, pos, planetSizeLod_, kv.first)));
    }
}

/*
 *
 * CubeSideTree
 *
 */
CubeSideTree::CubeSideTree(glm::vec3 axis, TerrainGenerator *terrainGen, CubeSideMap *cubeSideMap, AxisIntegerMap *axisIntegerMap, 
                            glm::vec3 &sphereOrigin, int cubeSideDimension, int rootDimension, int rootLod)
                                : axis_(axis), terrainGenerator_(terrainGen), cubeSideMap_(cubeSideMap), axisIntegerMap_(axisIntegerMap), 
                                sphereOrigin_(sphereOrigin), cubeSideDimension_(cubeSideDimension), rootDimension_(rootDimension), rootLod_(rootLod) {
    if (cubeSideDimension_ % rootDimension_ != 0)
        fprintf(stderr, "[CUBESIDETREE] Error: Planet dimension not divisible by QuadTree Dimension\n");
    if (cubeSideDimension_ % 2 != 0)
        fprintf(stderr, "[CUBESIDETREE] Error: cubeSideDimension not divisible by 2\n");

    sphereRadius_ = cubeSideDimension_ / 2;
    maxGridPosition_ = (cubeSideDimension_ / rootDimension_) - 1;
    computeCubeSideOrigin();
}

CubeSideTree::~CubeSideTree() {
    for (auto &kv : rootNodeMap_) {
        if (kv.second)
            delete kv.second;
    }
}

glm::vec3 CubeSideTree::computeCubeSideOrigin() {
    int halfDim = cubeSideDimension_ / 2;
    glm::vec3 ret;
    switch (axisIntegerMap_->at(axis_)) {
        case 0:
        case 1:
            ret = cubeSideOrigin_ = glm::vec3(sphereRadius_ * axis_.x, sphereOrigin_.y - halfDim, sphereOrigin_.z - halfDim);
            break;
        case 2:
        case 3:
            ret = cubeSideOrigin_ = glm::vec3(sphereOrigin_.x - halfDim, sphereRadius_ * axis_.y, sphereOrigin_.z - halfDim);
            break;
        case 4:
        case 5:
            ret = cubeSideOrigin_ = glm::vec3(sphereOrigin_.x - halfDim, sphereOrigin_.y - halfDim, sphereRadius_ * axis_.z);
            break;
        default:
            printDefaultCubeSideMapError("computeCubeSideOrigin", axisIntegerMap_->at(axis_));
            ret = glm::vec3(0,0,0);
            break;
    }

    return ret;
}

void CubeSideTree::setInitialRun() {
    initialRun_ = true;
    containsCurrentCenterNode_ = true;
}

bool CubeSideTree::hasActiveNodes() {
    return containsActiveRootNode_;
}

unsigned int CubeSideTree::getNumRootNodes() {
    return rootNodeMap_.size();
}

glm::vec3 &CubeSideTree::getAxis() {
    return axis_;
}

// TODO: Remove, old debug function?
RootNodeMap &CubeSideTree::getRootNodeMap() {
    return rootNodeMap_;
}

void CubeSideTree::printDefaultCubeSideMapError(std::string name, int key) {
    fprintf(stdout, "[CUBESIDETREE] Error, invalid AxisIntegerMap in func: %s. Key: %i\n", name.c_str(), key);
}

glm::vec2 CubeSideTree::worldCubePosToGridPos(glm::vec3 &pos) {
    glm::vec2 ret;
    switch (axisIntegerMap_->at(axis_)) {
        case 0: // x
        case 1: // -x
            ret = glm::vec2(glm::floor(abs(pos.z - cubeSideOrigin_.z) / rootDimension_), glm::floor(abs(pos.y - cubeSideOrigin_.y) / rootDimension_));
            break;
        case 2: // y
        case 3: // -y
            ret = glm::vec2(glm::floor(abs(pos.x - cubeSideOrigin_.x) / rootDimension_), glm::floor(abs(pos.z - cubeSideOrigin_.z) / rootDimension_));
            break;
        case 4: // z
        case 5: // -z
            ret = glm::vec2(glm::floor(abs(pos.x - cubeSideOrigin_.x) / rootDimension_), glm::floor(abs(pos.y - cubeSideOrigin_.y) / rootDimension_));
            break;
        default:
            printDefaultCubeSideMapError("worldCubePosToGridPos", axisIntegerMap_->at(axis_));
            ret = glm::vec2(-1,-1); 
            break;
    }
    return ret;
}

glm::vec3 CubeSideTree::gridPosToWorldCubePos(glm::vec2 &gridPos) {
    glm::vec3 ret;
    int halfDim = rootDimension_ / 2;
    switch (axisIntegerMap_->at(axis_)) {
        case 0: // x
        case 1: // -x
            ret = glm::vec3(sphereRadius_ * axis_.x, cubeSideOrigin_.y + (gridPos.y * rootDimension_) + halfDim, cubeSideOrigin_.z + (gridPos.x * rootDimension_) + halfDim);
            break;
        case 2: // y
        case 3: // -y
            ret = glm::vec3(cubeSideOrigin_.x + (gridPos.x * rootDimension_) + halfDim, sphereRadius_ * axis_.y, cubeSideOrigin_.z + (gridPos.y * rootDimension_) + halfDim);
            break;
        case 4: // z
        case 5: // -z
            ret = glm::vec3(cubeSideOrigin_.x + (gridPos.x * rootDimension_) + halfDim, cubeSideOrigin_.y + (gridPos.y * rootDimension_) + halfDim, sphereRadius_ * axis_.z);
            break;
        default:
            printDefaultCubeSideMapError("gridPosToWorldCubePos", axisIntegerMap_->at(axis_));
            ret = glm::vec3(-1,-1,-1);
            break;
    }

    return ret;
}

glm::vec3 CubeSideTree::getCubePosFromSphere(glm::vec3 &camPos) {
    float intersectionDistance;
    // intersectRayPlane(rayOrigin, rayDirection, planeOrigin, planeNormal, intersectionDistance)
    glm::intersectRayPlane(sphereOrigin_, glm::normalize(camPos - sphereOrigin_), cubeSideOrigin_, axis_, intersectionDistance);
    return glm::vec3(sphereOrigin_ + intersectionDistance * glm::normalize(camPos - sphereOrigin_));
}

/*
 * TODO: handle edge cae overflow?
 * Returns true if axisOut is a new axis
 */
bool CubeSideTree::overflow(glm::vec3 &pos, glm::vec3 &axisOut) {
    bool ret = false;
    switch (axisIntegerMap_->at(axis_)) {
        case 0:
        case 1:
            if (pos.z < cubeSideOrigin_.z)
                axisOut = glm::vec3(0,0,-1);
            else if (pos.z > cubeSideOrigin_.z + cubeSideDimension_)
                axisOut = glm::vec3(0,0,1);
            else if (pos.y < cubeSideOrigin_.y)
                axisOut = glm::vec3(0,-1,0);
            else if (pos.y > cubeSideOrigin_.y + cubeSideDimension_)
                axisOut = glm::vec3(0,1,0);
            break;
        case 2: // y
        case 3: // -y
            if (pos.x < cubeSideOrigin_.x)
                axisOut = glm::vec3(-1,0,0);
            else if (pos.x > cubeSideOrigin_.x + cubeSideDimension_)
                axisOut = glm::vec3(1,0,0);
            else if (pos.z < cubeSideOrigin_.z)
                axisOut = glm::vec3(0,0,-1);
            else if (pos.z > cubeSideOrigin_.z + cubeSideDimension_)
                axisOut = glm::vec3(0,0,1);
            break;
        case 4: // z
        case 5: // -z
            if (pos.x < cubeSideOrigin_.x)
                axisOut = glm::vec3(-1,0,0);
            else if (pos.x > cubeSideOrigin_.x + cubeSideDimension_)
                axisOut = glm::vec3(1,0,0);
            else if (pos.y < cubeSideOrigin_.y)
                axisOut = glm::vec3(0,-1,0);
            else if (pos.y > cubeSideOrigin_.y + cubeSideDimension_) {
                axisOut = glm::vec3(0,1,0);
            }
            break;
        default:
            printDefaultCubeSideMapError("overflow", axisIntegerMap_->at(axis_));
            break;
    }
    return axisOut != axis_;
}

bool CubeSideTree::destroyRootNode(glm::vec2 pos) {
    bool ret = false;
    if (rootNodeMap_.find(pos) != rootNodeMap_.end()) {
        TerrainChunk *t = rootNodeMap_[pos];
        if (t)
            delete t;
        rootNodeMap_.erase(pos);
        ret = true;
        //fprintf(stdout, "[CUBESIDETREE::destroyRootNode] Axis: %s, Node: %s\n", glm::to_string(axis_).c_str(), glm::to_string(pos).c_str());
    }

    if (rootNodeMap_.empty())
        containsActiveRootNode_ = false;

    return ret;
}

/*
 * Maps border positions on this axis on the adjacent axis in the direction specified by vector change
 * Offset determines if the pos on the new axis is on the border or further inside
 * Does NOT handle positions outside of this axis cube side!
 */
glm::vec2 CubeSideTree::mapPosOnNewAxis(glm::vec2 &pos, glm::vec2 &change, int offset) {
    glm::vec2 newPos;
    switch (axisIntegerMap_->at(axis_)) {
        case 0: // +x
            if (change.x != 0)
                newPos = glm::vec2(maxGridPosition_ - offset, pos.y);
            else if (change.y != 0)
                newPos = glm::vec2(maxGridPosition_ - offset, pos.x);
            break;
        case 1: // -x
            if (change.x != 0)
                newPos = glm::vec2(0 + offset, pos.y);
            else if (change.y != 0)
                newPos = glm::vec2(0 + offset, pos.x);
            break;
        case 2: // +y
            if (change.x != 0)
                newPos = glm::vec2(pos.y, maxGridPosition_ - offset);
            if (change.y != 0)
                newPos = glm::vec2(pos.x, maxGridPosition_ - offset);
            break;
        case 3: // -y
            if (change.x != 0)
                newPos = glm::vec2(pos.y, 0 + offset);
            if (change.y != 0)
                newPos = glm::vec2(pos.x, 0 + offset);
            break;
        case 4: // z
            if (change.x != 0)
                newPos = glm::vec2(maxGridPosition_ - offset, pos.y);
            if (change.y != 0)
                newPos = glm::vec2(pos.x, maxGridPosition_ - offset);
            break;
        case 5: // -z
            if (change.x != 0)
                newPos = glm::vec2(0 + offset, pos.y);
            if (change.y != 0)
                newPos = glm::vec2(pos.x, 0 + offset);
            break;
        default:
            printDefaultCubeSideMapError("mapPosOnNewAxis", axisIntegerMap_->at(axis_));
            newPos = glm::vec2(-1,-1);
            break;
    }

    return newPos;
}

glm::vec3 CubeSideTree::getNextAxis(glm::vec2 &change) {
    glm::vec3 ret;
    switch (axisIntegerMap_->at(axis_)) {
        case 0:
        case 1:
            if (change.y != 0)
              ret = glm::vec3(0,change.y,0);
            else
              ret = glm::vec3(0,0,change.x);
            break;
        case 2:
        case 3:
            if (change.y != 0)
              ret = glm::vec3(0,0,change.y);
            else
              ret = glm::vec3(change.x,0,0);
            break;
        case 4:
        case 5:
            if (change.y != 0)
              ret = glm::vec3(0, change.y, 0);
            else
              ret = glm::vec3(change.x,0,0);
            break;
        default:
            printDefaultCubeSideMapError("getNextAxis", axisIntegerMap_->at(axis_));
            ret = glm::vec3(0,0,0);
            break;
    }

    return ret;
}

bool CubeSideTree::handleOnDifferentCubeSide(glm::vec2 &pos, glm::vec2 &change, bool &edgeCase) {
    bool ret = false;
    if (pos.x < 0) {
        if (pos.y < 0 || pos.y > maxGridPosition_) { // Edge case, no node here
            ret = edgeCase = true;
            change = glm::vec2(-1, pos.y / abs(pos.y));
        } else {
            ret = true;
            pos.x = 0;
            change = glm::vec2(-1, 0);
        }
    } else if (pos.x > maxGridPosition_) {
        if (pos.y < 0 || pos.y > maxGridPosition_) { // Edge case, no node here
            ret = edgeCase = true;
            change = glm::vec2(1, pos.y / abs(pos.y));
        } else {
            ret = true;
            pos.x = maxGridPosition_;
            change = glm::vec2(1, 0);
        }
    } else if (pos.y < 0) {
        ret = true;
        pos.y = 0;
        change = glm::vec2(0, -1);
    } else if (pos.y > maxGridPosition_) {
        ret = true;
        pos.y = maxGridPosition_;
        change = glm::vec2(0, 1);
    }

    return ret;
}

glm::vec2 CubeSideTree::getPreviousCenterNode(glm::vec3 &newAxis, glm::vec2 &gridPos) {
    if (gridPos.x != 0 && gridPos.x != maxGridPosition_ && gridPos.y != 0 && gridPos.y != maxGridPosition_)
        fprintf(stderr, "[CUBESIDETREE::getPreviousCenterNode] Axis switch but new center node not at border\n");

    glm::vec2 ret;
    glm::vec2 change;
    switch (axisIntegerMap_->at(axis_)) {
        case 0:
        case 1:
            change = glm::vec2(newAxis.z, newAxis.y);
            break;
        case 2:
        case 3:
            change = glm::vec2(newAxis.x, newAxis.z);
            break;
        case 4:
        case 5:
            change = glm::vec2(newAxis.x, newAxis.y);
            break;
        default:
            printDefaultCubeSideMapError("getPreviousCenterNode", axisIntegerMap_->at(axis_));
            change = glm::vec2(0,0);
    }

    if (change.x)
        ret = glm::vec2(gridPos.x == maxGridPosition_ ? maxGridPosition_ + 1 : -1, gridPos.y);
    else
        ret = glm::vec2(gridPos.x, gridPos.y == maxGridPosition_ ? maxGridPosition_ + 1 : -1);

    return ret;
}

glm::vec3 CubeSideTree::cubeWorldPosToSpherePos(glm::vec3 &cubePos) {
    return glm::vec3(sphereOrigin_ + (float)sphereRadius_ * glm::normalize(cubePos - sphereOrigin_));
}

glm::vec3 CubeSideTree::getChildPosition(glm::vec3 &pos, int x, int z, int offset) {
    glm::vec3 ret;
    switch (axisIntegerMap_->at(axis_)) {
        case 0:
        case 1:
            ret = glm::vec3(pos.x, pos.y + z * offset, pos.z + x * offset);
            break;
        case 2:
        case 3:
            ret = glm::vec3(pos.x + x * offset, pos.y, pos.z + z * offset);
            break;
        case 4:
        case 5:
            ret = glm::vec3(pos.x + x * offset, pos.y + z * offset, pos.z);
            break;
        default:
            printDefaultCubeSideMapError("getChildPosition", axisIntegerMap_->at(axis_));
            ret = glm::vec3(-1,-1,-1);
            break;
    }

    return ret;
}

void CubeSideTree::createChildren(TerrainChunk *node) {
    int childDimension = (node->getDimension() - 1) / 2;
    int childPosOffset = childDimension / 2;
    int childLod = glm::max(1, node->getLod() - 2);

    for (int z = -1; z < 2; z += 2) {
        for (int x = -1; x < 2; x += 2) {
            glm::vec3 pos = getChildPosition(node->getPosition(), x, z, childPosOffset);
            node->addChild(new TerrainChunk(new Terrain(terrainGenerator_,
                                                childDimension + 1,
                                                pos, childLod, axis_),
                                            new WaterTile(terrainGenerator_, childDimension + 1, pos, rootLod_, axis_)));
        }
    }
}

void CubeSideTree::createRootNode(glm::vec2 cubeSideGridPos) {
    glm::vec3 worldPos = gridPosToWorldCubePos(cubeSideGridPos);
    rootNodeMap_[cubeSideGridPos] = new TerrainChunk(new Terrain(terrainGenerator_, rootDimension_ + 1, worldPos, rootLod_, axis_), 
                                                        new WaterTile(terrainGenerator_, rootDimension_ + 1, worldPos, rootLod_, axis_));
    containsActiveRootNode_ = true;
}

bool CubeSideTree::handleRootNodeCreation(glm::vec2 pos) {
    bool ret = false;
    if (rootNodeMap_.find(pos) == rootNodeMap_.end()) {
        TerrainChunk *t = NULL;
        rootNodeMap_[pos] = t;
        threadPool->addJob(std::bind(&CubeSideTree::createRootNode, this, pos));
        ret = true;
        //fprintf(stdout, "[CUBESIDETREE::CreateRootNode] Axis: %s. GridPos: %s. WorldPos: %s\n", glm::to_string(axis_).c_str(), glm::to_string(pos).c_str(), glm::to_string(gridPosToWorldCubePos(pos)).c_str());
    }

    return ret;
}

void CubeSideTree::updateNode_(TerrainChunk *node, glm::vec3 camWorldPos, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist) {
    if (!node)
        return; 

    if ((node->getLod() == 1) ||
            (glm::distance2(camWorldPos, cubeWorldPosToSpherePos(node->getPosition())) > glm::pow(node->getDimension(), 2))) {
        tlist->push_back(node->getTerrain());
        wlist->push_back(node->getWater());
    } else {
        if (node->getIndex() != 4) {
            tlist->push_back(node->getTerrain());
            wlist->push_back(node->getWater());

            if (!node->isScheduled()) {
                node->setChildrenScheduled();
                threadPool->addJob(std::bind(&CubeSideTree::createChildren, this, node));
            }
        } else {
            for (TerrainChunk *child : node->getChildren())
                updateNode_(child, camWorldPos, tlist, wlist);
        }
    }
}

void CubeSideTree::updateNodes(glm::vec3 &camWorldPos, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist) {
  for (auto &kv : rootNodeMap_)
    updateNode_(kv.second, camWorldPos, tlist, wlist);
}

CubeSideTree *CubeSideTree::update(glm::vec3 &posSphere, CubeSideTree *previous) {
    glm::vec3 cubeWorldPos = getCubePosFromSphere(posSphere);

    glm::vec3 newAxis = axis_;
    if (overflow(cubeWorldPos, newAxis)) {
        containsCurrentCenterNode_ = false;
        return cubeSideMap_->at(newAxis)->update(posSphere, this); 
    } 
    else {
        glm::vec2 currentGridPosition = worldCubePosToGridPos(cubeWorldPos);
        
        if (!containsCurrentCenterNode_ && !initialRun_) { // Axis change, create previousCenterNode, for deletion
            if (!previous) {
                fprintf(stdout, "[CUBESIDETREE::update] CRITICAL ERROR: PREVIOUS == NULL\n");
                currentCenterNode_ = glm::vec2(0,0);
            } else
                currentCenterNode_ = getPreviousCenterNode(previous->getAxis(), currentGridPosition);
                
            containsCurrentCenterNode_ = true;
        }
    
        if (currentGridPosition != currentCenterNode_) {
            glm::vec2 opp = (currentGridPosition - currentCenterNode_) * 3.0f; // our structure is a 3*3 grid of quadTrees
            RootNodeMap::iterator it;
            for (int y = -1; y < 2; y++) {
                for (int x = -1; x < 2; x++) {
                    glm::vec2 newRootGridPos = glm::vec2(currentGridPosition.x + x, currentGridPosition.y + y);
                    
                    glm::vec2 change;
                    glm::vec2 gridPosOverflow = newRootGridPos;
                    bool edgeCase = false;
                    bool inserted = false;
                    if (handleOnDifferentCubeSide(gridPosOverflow, change, edgeCase)) {
                        if (!edgeCase) 
                            inserted = cubeSideMap_->at(getNextAxis(change))->handleRootNodeCreation(mapPosOnNewAxis(gridPosOverflow, change));
                    } else 
                        inserted = handleRootNodeCreation(newRootGridPos);

                    bool deleted = false;
                    if ((inserted && !initialRun_) || (edgeCase && !initialRun_)) {
                        //fprintf(stdout, "[HANDLE DELETION] EdgeCase: %d, gridPos: %s, opp: %s, axis: %s\n", edgeCase, glm::to_string(newRootGridPos).c_str(), glm::to_string(opp).c_str(), glm::to_string(axis_).c_str());
                        deleted = handleNodeDeletion(newRootGridPos - opp);
                    }
                }
            }
            if (initialRun_)
                initialRun_ = false;
            currentCenterNode_ = currentGridPosition;
        }
    }

    return this;
}

bool CubeSideTree::handleNodeDeletion(glm::vec2 destroyPos) {
    bool ret = false;
    glm::vec2 tmp = destroyPos;
    glm::vec2 change;
    bool edgeCase = false;
    if (handleOnDifferentCubeSide(tmp, change, edgeCase)) {
        if (edgeCase)
            ret = handleEdgeCaseNodeDeletion(destroyPos);
        else {
            int offset = change.x ? (destroyPos.x > maxGridPosition_ ? destroyPos.x - maxGridPosition_ : abs(destroyPos.x)) - 1 :
                            (destroyPos.y > maxGridPosition_ ? destroyPos.y - maxGridPosition_ : abs(destroyPos.y)) - 1;
            ret = cubeSideMap_->at(getNextAxis(change))->destroyRootNode(mapPosOnNewAxis(tmp, change, offset));
        }
    } else {
        ret = destroyRootNode(destroyPos);
    }

    return ret;
}

bool CubeSideTree::handleEdgeCaseNodeDeletion(glm::vec2 &destroyPos) {
    int x = destroyPos.x > maxGridPosition_ ? destroyPos.x - maxGridPosition_ : abs(destroyPos.x);
    int y = destroyPos.y > maxGridPosition_ ? destroyPos.y - maxGridPosition_ : abs(destroyPos.y);
    int offset = x > y ? x : y;
    bool ret = false;
    bool t = false;
    glm::vec2 tmp = glm::vec2(destroyPos.x > maxGridPosition_ ? maxGridPosition_ : 0, destroyPos.y > maxGridPosition_ ? maxGridPosition_ + offset : 0 - offset);
    ret = handleNodeDeletion(tmp);

    tmp = glm::vec2(destroyPos.x > maxGridPosition_ ? maxGridPosition_ + offset : 0 - offset, destroyPos.y > maxGridPosition_ ? maxGridPosition_ : 0);
    t = handleNodeDeletion(tmp);

    return ret || t;
}

/*
 *
 * TERRAIN QUAD TREE (plane only)
 *
 */
TerrainQuadTree::TerrainQuadTree(int initialDimension, int maxLod, 
                                TerrainGenerator *terrainGen) :
                                rootDimension_(initialDimension), 
                                maxLod_(maxLod), terrainGenerator_(terrainGen) {
    initTree();
}

TerrainQuadTree::~TerrainQuadTree() {
    for (auto &kv : rootMap_)
        delete kv.second;
}

void TerrainQuadTree::initTree() {
    currentMiddleChunk_ = glm::vec2(0,0);

    for (int z = -1; z < 2; z++) {
        for (int x = -1; x < 2; x++) {
            glm::vec2 pos(x,z);
            TerrainChunk *chunk = NULL;
            rootMap_.insert(RootNodeMap::value_type(pos, chunk));
            createRootNode(pos);
        }
    }

    if (maxLod_ > 1)
        createChildren(rootMap_.at(currentMiddleChunk_));
}

/*
 * position should be an existing key in rootMap_
 */
void TerrainQuadTree::createRootNode(glm::vec2 position) {
    rootMap_[position] = new TerrainChunk(new Terrain(terrainGenerator_,
                                                rootDimension_ + 1,
                                                position.x * rootDimension_,
                                                position.y * rootDimension_, maxLod_), 
                                            DrawableFactory::createWaterTile(glm::vec3(position.x * rootDimension_, 0, position.y * rootDimension_), rootDimension_ / 2, glm::vec3(0,0,1))); 
}

/*
 * TODO: Better distance methods, to calculate from quad edges, not the center
 */
void TerrainQuadTree::update_(TerrainChunk *node, glm::vec3 &camPosition, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist) {
    if (!node) // Unfinished chunk, waiting for thread
        return;

    if ((node->getLod() == 1) ||
        (glm::distance2(camPosition, node->getPosition()) > 
        glm::pow(node->getDimension(), 2))) {

        tlist->push_back(node->getTerrain());
        wlist->push_back(node->getWater());
    } else {
        if (node->getIndex() != 4) {
            tlist->push_back(node->getTerrain());
            wlist->push_back(node->getWater());
            if (!node->isScheduled()) {
                node->setChildrenScheduled();
                threadPool->addJob(std::bind(&TerrainQuadTree::createChildren, this, node));
            }
        } else {
            for (TerrainChunk *child : node->getChildren())
                update_(child, camPosition, tlist, wlist);
        }
    }
}

void TerrainQuadTree::update(glm::vec3 &camPosition, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist) {
    tlist->clear();
    wlist->clear();
    updateRoots(camPosition);
    for (auto &kv : rootMap_)
        update_(kv.second, camPosition, tlist, wlist);
}

/*
 * Creates new root nodes in visible distance and deletes old ones
 * TODO: Implement maxViewDistance and numChunksVisible instead of the current hardcoded numbers
 */
void TerrainQuadTree::updateRoots(glm::vec3 &camPosition) {
    glm::vec2 currentPos = glm::vec2((int)std::round(camPosition.x / rootDimension_), (int)std::round(camPosition.z / rootDimension_));

    if (currentPos != currentMiddleChunk_) {
        glm::vec2 opp = (currentPos - currentMiddleChunk_);
        opp.x *= 3; // If having 8 quadtrees around the central one
        opp.y *= 3; // -> with numChunksvisible: numChunksVisible * 2 + 1
        RootNodeMap::iterator it;
        for (int z = -1; z < 2; z++) { // use numChunksVisible -> determined by maxViewDistance / rootDimension
            for (int x = -1; x < 2; x++) {
                glm::vec2 tmp = glm::vec2(currentPos.x + x, currentPos.y + z);
                it = rootMap_.find(tmp);
                /*
                 * Node not yet in map
                 * create and delete the one opposite
                 */
                if (it == rootMap_.end()) {
                    TerrainChunk *t = NULL;
                    rootMap_.insert(it, RootNodeMap::value_type(tmp, t));
                    threadPool->addJob(std::bind(&TerrainQuadTree::createRootNode, this, tmp));
                
                    // delete opposite
                    delete rootMap_.at(tmp - opp);
                    rootMap_.erase(tmp - opp);
                }
            }
        }
        currentMiddleChunk_ = currentPos;
    }
}

void TerrainQuadTree::createChildren(TerrainChunk *node) {
    int childDimension = (node->getDimension() - 1) / 2;
    int childPosOffset = childDimension / 2;
    int childLod = glm::max(1, node->getLod() - 2);

    glm::vec3 nodePosition = node->getPosition();

    for (int z = -1; z < 2; z += 2) {
        for (int x = -1; x < 2; x += 2) {
            glm::vec3 pos(nodePosition.x + x * childPosOffset, 0, nodePosition.z + z * childPosOffset);
            node->addChild(new TerrainChunk(new Terrain(terrainGenerator_,
                                                childDimension + 1,
                                                pos.x, pos.z, childLod),
                                            DrawableFactory::createWaterTile(pos, childPosOffset, glm::vec3(0,0,1))));
        }
    }
}