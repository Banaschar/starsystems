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
 *
 * TODO: For big planets, I need to switch from terrainCubeTree to terrainQuadTree(with 9 root nodes) when close to the ground
 * e.g. Everything else needs to be deleted
 * If the camera reaches a certain distance, I create a quad tree where the central chunk has the coordinates directly below the camera
 * ----> how to get that point? The point on the line between the camera position and the planet origin that has distance radius from the origin
 *
 * ----->>> PRECALCULATE EDGE VALUES FOR EACH SIDE!
 * E.G. if I'm initially on axis 0 (1,0,0), have a vec2 that contains the max y and z values which are still on the cube side
 * -----------> NO! That does not help, because I need the position on the cube side for the quad tree movement...
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
    // init cubeSides
    glm::vec3 axis;
    int cnt = 0;
    for (int i = 0; i < 3; i++) {
        axis = glm::vec3(0,0,0);
        axis[i] = 1;
        axisIntegerMap_[axis] = cnt++;
        axis[i] = -1;
        axisIntegerMap_[axis] = cnt++;
    }

    // TODO: usefull value retrival of cubeSideRootNodeDimension
    int cubeSideRootNodeDimension = cubeSideDimension_ / 8;
    int lod = 6;
    for (auto &kv : axisIntegerMap_) {
        cubeSideMap_[kv.first] = new CubeSideTree(kv.first, terrainGenerator_, &cubeSideMap_, &axisIntegerMap_, cubeSideDimension_, cubeSideRootNodeDimension, lod);
    }

    // compute initial position, set appropriate cubeSide as initial and give it the needed stuff
    glm::vec3 cubePosition;
    computeInitialCubePositionAndAxis(camPos, &axis, &cubePosition);
    cubeSideMap_[axis]->setInitialRun();
    currentCubeSide_ = cubeSideMap_[axis];
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

void TerrainCubeTree::updateCubeSides(glm::vec3 &camPosition, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist) {
    for (auto &kv : cubeSideMap_) {
      if (kv.second->hasActiveNodes())
        // TODO: Move the update nodes function to terrainCubeTree, provide kv.second as first argument
        kv.second->updateNodes(camPosition, tlist, wlist);
    }
}

/*
 * Each side needs at least one lod level below the whole side.
 * So when I get nearer to the planet, first the farAwayCubeSideList creates new children on the sides facing the player.
 * And only after that, when the camera is close to the planet, e.g. just the horizons are visible, the cubeSide datastructure is used.
 */
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
        for (TerrainChunk *t : cubeSides_)
            tlist->push_back(t->getTerrain());
    }
}

void TerrainCubeTree::initTree(int dimension) {
    TerrainChunk *t = new TerrainChunk(new Terrain(terrainGenerator_, dimension + 1, glm::vec3(0,0,0), planetSizeLod_, glm::vec3(1,0,0)), NULL);
    TerrainChunk *t1 = new TerrainChunk(new Terrain(terrainGenerator_, dimension + 1, glm::vec3(0,0,0), planetSizeLod_, glm::vec3(-1,0,0)), NULL);
    TerrainChunk *t2 = new TerrainChunk(new Terrain(terrainGenerator_, dimension + 1, glm::vec3(0,0,0), planetSizeLod_, glm::vec3(0,1,0)), NULL);
    TerrainChunk *t3 = new TerrainChunk(new Terrain(terrainGenerator_, dimension + 1, glm::vec3(0,0,0), planetSizeLod_, glm::vec3(0,-1,0)), NULL);
    TerrainChunk *t4 = new TerrainChunk(new Terrain(terrainGenerator_, dimension + 1, glm::vec3(0,0,0), planetSizeLod_, glm::vec3(0,0,1)), NULL);
    TerrainChunk *t5 = new TerrainChunk(new Terrain(terrainGenerator_, dimension + 1, glm::vec3(0,0,0), planetSizeLod_, glm::vec3(0,0,-1)), NULL);

    cubeSides_.push_back(t);
    cubeSides_.push_back(t1);
    cubeSides_.push_back(t2);
    cubeSides_.push_back(t3);
    cubeSides_.push_back(t4);
    cubeSides_.push_back(t5);
}

/*
 *
 * CUBE SIDES
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
CubeSideTree::CubeSideTree(glm::vec3 axis, TerrainGenerator *terrainGen, CubeSideMap *cubeSideMap, AxisIntegerMap *axisIntegerMap, int cubeSideDimension, int rootDimension, int rootLod)
             : axis_(axis), terrainGenerator_(terrainGen), cubeSideMap_(cubeSideMap), axisIntegerMap_(axisIntegerMap), cubeSideDimension_(cubeSideDimension), rootDimension_(rootDimension), rootLod_(rootLod) {
    if (cubeSideDimension_ % rootDimension_ != 0)
        fprintf(stdout, "[CUBESIDETREE] Error: Planet dimension not divisible by QuadTree Dimension\n");
    if (cubeSideDimension_ % 2 != 0)
        fprintf(stdout, "CUBESIDETREE] Error: cubeSideDimension not divisible by 2\n");

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
        case 1:
        case 2:
            ret = cubeSideOrigin_ = glm::vec3(sphereRadius_ * axis_.x, sphereOrigin_.y - halfDim, sphereOrigin_.z - halfDim);
        case 3:
        case 4:
            ret = cubeSideOrigin_ = glm::vec3(sphereOrigin_.x - halfDim, sphereRadius_ * axis_.y, sphereOrigin_.z - halfDim);
        case 5:
        case 6:
            ret = cubeSideOrigin_ = glm::vec3(sphereOrigin_.x - halfDim, sphereOrigin_.y - halfDim, sphereRadius_ * axis_.z);
            break;
        default:
            printDefaultCubeSideMapError();
            ret = glm::vec3(0,0,0);
    }

    return ret;
}

void CubeSideTree::setInitialRun() {
    initialRun_ = true;
}

bool CubeSideTree::hasActiveNodes() {
    return containsActiveRootNode_;
}

void CubeSideTree::printDefaultCubeSideMapError() {
    fprintf(stdout, "Error\n");
}

glm::vec2 CubeSideTree::worldCubePosToGridPos(glm::vec3 &pos) {
    glm::vec2 ret;
    switch (axisIntegerMap_->at(axis_)) {
        case 0: // x
        case 1: // -x
        ret = glm::vec2(glm::floor(abs(abs(pos.z) - abs(cubeSideOrigin_.z)) / rootDimension_), glm::floor(abs(abs(pos.y) - abs(cubeSideOrigin_.y)) / rootDimension_));
        break;
        case 2: // y
        case 3: // -y
            ret = glm::vec2(glm::floor(abs(abs(pos.x) - abs(cubeSideOrigin_.x)) / rootDimension_), glm::floor(abs(abs(pos.z) - abs(cubeSideOrigin_.z)) / rootDimension_));
            break;
        case 4: // z
        case 5: // -z
            ret = glm::vec2(glm::floor(abs(abs(pos.x) - abs(cubeSideOrigin_.x)) / rootDimension_), glm::floor(abs(abs(pos.y) - abs(cubeSideOrigin_.y)) / rootDimension_));
            break;
        default:
            printDefaultCubeSideMapError();
            ret = glm::vec2(-1,-1); 
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
            printDefaultCubeSideMapError();
            ret = glm::vec3(-1,-1,-1);
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
 */
bool CubeSideTree::overflow(glm::vec3 &pos, glm::vec3 *axisOut) {
    axisOut = NULL;
    fprintf(stdout, "overflow func\n");
    switch (axisIntegerMap_->at(axis_)) {
        case 0:
        case 1:
            if (pos.z < cubeSideOrigin_.z)
                axisOut = new glm::vec3(0,0,-1);
            else if (pos.z > cubeSideOrigin_.z + cubeSideDimension_)
                axisOut = new glm::vec3(0,0,1);
            else if (pos.y < cubeSideOrigin_.y)
                axisOut = new glm::vec3(0,-1,0);
            else if (pos.y > cubeSideOrigin_.y + cubeSideDimension_)
                axisOut = new glm::vec3(0,1,0);
            break;
        case 2: // y
        case 3: // -y
            if (pos.x < cubeSideOrigin_.x)
                axisOut = new glm::vec3(-1,0,0);
            else if (pos.x > cubeSideOrigin_.x + cubeSideDimension_)
                axisOut = new glm::vec3(1,0,0);
            else if (pos.z < cubeSideOrigin_.z)
                axisOut = new glm::vec3(0,0,-1);
            else if (pos.z > cubeSideOrigin_.z + cubeSideDimension_)
                axisOut = new glm::vec3(0,0,1);
            break;
        case 4: // z
        case 5: // -z
            fprintf(stdout, "overflow -z. Pos: %s. Origin: %s\n", glm::to_string(pos).c_str(), glm::to_string(cubeSideOrigin_).c_str());
            if (pos.x < cubeSideOrigin_.x)
                axisOut = new glm::vec3(-1,0,0);
            else if (pos.x > cubeSideOrigin_.x + cubeSideDimension_)
                axisOut = new glm::vec3(1,0,0);
            else if (pos.y < cubeSideOrigin_.y)
                axisOut = new glm::vec3(0,-1,0);
            else if (pos.y > cubeSideOrigin_.y) {
                fprintf(stdout, "HAAAAAAAALLLLOOOO\n");
                axisOut = new glm::vec3(0,1,0);
            }
            fprintf(stdout, "overflow func end\n");
            break;
        default:
            printDefaultCubeSideMapError();
    }
    fprintf(stdout, "overflow func end\n");
    return axisOut ? true : false;
}

void CubeSideTree::destroyRootNode(glm::vec2 pos) {
    if (rootNodeMap_.find(pos) != rootNodeMap_.end()) {
        TerrainChunk *t = rootNodeMap_[pos];
        if (t)
            delete t;
        rootNodeMap_.erase(pos);
    }

    if (rootNodeMap_.empty())
        containsActiveRootNode_ = false;
}

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
              newPos = glm::vec2(0 + offset, pos.y); // 0 oder pos.x, das selbe
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
              newPos = glm::vec2(0 + offset, pos.x);
            break;
        default:
            printDefaultCubeSideMapError();
            newPos = glm::vec2(-1,-1);
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
            printDefaultCubeSideMapError();
            ret = glm::vec3(0,0,0);
    }

    return ret;
}

bool CubeSideTree::handleOnDifferentCubeSide(glm::vec2 pos, bool destroy) {
    bool ret = false;
    bool edgeCase = false;
    glm::vec2 change;
    if (pos.x < 0) {
        if (pos.y < 0) // Edge case, no node here
            ret = edgeCase = true;
        else {
            ret = true;
            pos.x = 0;
            change = glm::vec2(-1,0);
        }
    }

    if (pos.x > maxGridPosition_) {
        if (pos.y > maxGridPosition_) // Edge case, no node here
            ret = edgeCase = true;
        else {
            ret = true;
            pos.x = maxGridPosition_;
            change = glm::vec2(1,0);
        }
    }

    if (pos.y < 0) {
        if (pos.x > maxGridPosition_) // edge case, no node here
            ret = edgeCase = true;
        else {
            ret = true;
            pos.y = 0;
            change = glm::vec2(0,-1);
        }
    }
    
    if (pos.y > maxGridPosition_) {
        if (pos.x < 0) // edge case, no node here
            ret = edgeCase = true;
        else {
            ret = true;
            pos.y = maxGridPosition_;
            change = glm::vec2(0, 1);
        }
    }

    if (ret && !edgeCase && !destroy) {
        glm::vec3 newAxis = getNextAxis(change);
        cubeSideMap_->at(newAxis)->handleRootNodeCreation(mapPosOnNewAxis(pos, change));
    } else if (ret && destroy) {
        glm::vec3 newAxis = getNextAxis(change);
        int offset;
        if (change.x != 0)
            offset = pos.x > 0 ? pos.x - (maxGridPosition_ + 1) : pos.x + 1;
        else
            offset = pos.y > 0 ? pos.y - (maxGridPosition_ + 1) : pos.y + 1;

        cubeSideMap_->at(newAxis)->destroyRootNode(mapPosOnNewAxis(pos, change, offset));
    }

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
            printDefaultCubeSideMapError();
            ret = glm::vec3(-1,-1,-1);
    }

    return ret;
}

void CubeSideTree::createChildren(TerrainChunk *node) {
    int childDimension = (node->getDimension() - 1) / 2;
    int childPosOffset = childDimension / 2;
    int childLod = glm::max(1, node->getLod() - 2);

    glm::vec3 nodePosition = node->getPosition();

    for (int z = -1; z < 2; z += 2) {
        for (int x = -1; x < 2; x += 2) {
            glm::vec3 pos = getChildPosition(node->getPosition(), x, z, childPosOffset);
            node->addChild(new TerrainChunk(new Terrain(terrainGenerator_,
                                                childDimension + 1,
                                                pos, childLod, axis_),
                                            NULL));
        }
    }
}

void CubeSideTree::createRootNode(glm::vec2 cubeSideGridPos) {
    rootNodeMap_[cubeSideGridPos] = new TerrainChunk(new Terrain(terrainGenerator_, rootDimension_ + 1, gridPosToWorldCubePos(cubeSideGridPos), rootLod_, axis_), NULL);
    containsActiveRootNode_ = true;
}

void CubeSideTree::handleRootNodeCreation(glm::vec2 pos) {
    TerrainChunk *t = NULL;
    rootNodeMap_[pos] = t;
    threadPool->addJob(std::bind(&CubeSideTree::createRootNode, this, pos));
}

void CubeSideTree::updateNode_(TerrainChunk *node, glm::vec3 camWorldPos, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist) {
    // Node center position is in cube world position. -> get spherePosition
    if (!node) // unfinished terrain chunk, waiting on thread
        return; 

    if ((node->getLod() == 1) ||
            (glm::distance2(camWorldPos, cubeWorldPosToSpherePos(node->getPosition())) > glm::pow(node->getDimension() / 2, 2))) {
        tlist->push_back(node->getTerrain());
    } else {
        if (node->getIndex() != 4) {
            tlist->push_back(node->getTerrain());
        
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

CubeSideTree *CubeSideTree::update(glm::vec3 &posSphere) {
    fprintf(stdout, "CUBESIDETREE: update\n");
    glm::vec3 cubeWorldPos = getCubePosFromSphere(posSphere);
    fprintf(stdout, "have cubePos\n");
    // Check for overflow
    glm::vec3 *newAxis;
    if (overflow(cubeWorldPos, newAxis)) {
        fprintf(stdout, "Overflow\n");
        containsCurrentCenterNode_ = false;
        glm::vec3 ax = *newAxis;
        delete newAxis;
        return cubeSideMap_->at(ax)->update(posSphere); 
    } 
    else {
        fprintf(stdout, "No Overflow\n");
        glm::vec2 currentGridPosition = worldCubePosToGridPos(cubeWorldPos);
        fprintf(stdout, "GRID POS: %s\n", glm::to_string(currentGridPosition).c_str());
        if (!containsCurrentCenterNode_ && !initialRun_) { // Axis change, create previousCenterNode outside, for deletion
            currentCenterNode_ = currentGridPosition;
            if (currentGridPosition.x == 0)
                currentCenterNode_.x = -1;
            else if (currentGridPosition.x == maxGridPosition_)
                currentCenterNode_.x == maxGridPosition_ + 1;
            if (currentGridPosition.y == 0)
                currentCenterNode_.y = -1;
            else if (currentGridPosition.y == maxGridPosition_)
                currentCenterNode_.y == maxGridPosition_ + 1;

            containsCurrentCenterNode_ = true;
        }

        if (currentGridPosition != currentCenterNode_) {
            glm::vec2 opp = (currentGridPosition - currentCenterNode_) * 3.0f; // our structure is a 3*3 grid of quadTrees
            RootNodeMap::iterator it;
            for (int y = -1; y < 2; y++) {
                for (int x = -1; x < 2; x++) {
                    glm::vec2 newRootGridPos = glm::vec2(currentGridPosition.x + x, currentGridPosition.y + y);
    
                    if (!handleOnDifferentCubeSide(newRootGridPos)) {
                        it = rootNodeMap_.find(newRootGridPos);

                        // Node not yet in map. Create and delete old
                        if (it == rootNodeMap_.end()) {
                            handleRootNodeCreation(newRootGridPos);

                            // delete opposite
                            if (!initialRun_) {
                                if (!handleOnDifferentCubeSide(newRootGridPos - opp, true)) 
                                    destroyRootNode(newRootGridPos - opp);
                                
                            }

                        }
                    }
                }
            }
            if (initialRun_)
                initialRun_ == false;
            currentCenterNode_ = currentGridPosition;
        }
    }

    return this;
}
/*
 *
 *
 *
 *
 * TERRAIN QUAD TREE (2D)
 *
 *
 *
 *
 *

/*
 * TODO: Make numrootchunks depend on max view distance
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

/*
 *
 * OLD cubetree stuff. Delete if the new one works.
 */

/*
glm::vec3 TerrainQuadTree::getRootNodeSpherePosition(glm::vec3 &axis, glm::vec2 &position, glm::vec3 &middlePosition) {
    if (axis.x != 0) 
        return glm::vec3(radius * axis.x, middlePosition.y + position.y * dimension, middlePosition.z + position.x * dimension); 
     else if (axis.y != 0) 
        return glm::vec3(middlePosition.x + position.x * dimension, radius * axis.y, middlePosition.z + position.y + zoff);
     else 
        return glm::vec3(middlePosition + position.x * dimension, middlePosition + position.y * dimension, radius * axis.z);
}

glm::vec3 TerrainQuadTree::getRootNodeSpherePositionAxisChange(glm::vec3 &axis, glm::vec3 &oldAxis, glm::vec2 &pos, glm::vec3 &middlePosition) {
    if (axis.x != 0 && oldAxis.y != 0)
        return glm::vec3(sphereRadius_ * axis.x, middlePosition.y + axis.y * (rootDimension_ / 2), middlePosition.z + pos.x * axis.y * rootDimension_);
    else if (axis.x != 0 && oldAxis.z != 0)
        return glm::vec3(sphereRadius_ * axis.x, middlePosition.y + pos.x * axis.z * rootDimension_, middlePosition.z + axis.z * (rootDimension_ / 2));
    else if (axis.y != 0 && oldAxis.z != 0)
        return glm::vec3(middlePosition.x + pos.x * axis.z * rootDimension_, sphereRadius_ * axis.y, middlePosition.z + axis.z * (rootDimension_ / 2));
    else if (axis.y != 0 && oldAxis.x != 0)
        return glm::vec3(middlePosition.x + axis.x * (rootDimension_ / 2), sphereRadius_ * axis.y, middlePosition.z + pos.x * axis.x * rootDimension_);
    else if (axis.z != 0 && oldAxis.x != 0)
        return glm::vec3(middlePosition.x + axis.x * (rootDimension_ / 2), middlePosition.y + pos.x * axis.x * rootDimension_, sphereRadius_ * axis.z);
    else if (axis.z != 0 && oldAxis.y != 0)
        return glm::vec3(middlePosition.x + pos.x * axis.y * rootDimension_, middlePosition.y + axis.y * (rootDimension_ / 2), sphereRadius_ * axis.z);
}


glm::vec3 TerrainQuadTree::getSphereAxis(glm::vec3 &position) {
    glm::vec3 border(sphereOrigin_.x + (planetDimension_ / 2), sphereOrigin_.y + (planetDimension_ / 2), sphereOrigin_.z + (planetDimension_ / 2));
    glm::vec3 axis(0,0,0)
    if (abs(position.x) == sphereRadius_) { //CAREFUL, position.x is float?
        if (abs(position.y) > border.y) 
            axis.y = position.y / abs(position.y);
        if (abs(position.z) > border.z)
            axis.z = position.z / abs(position.z);
        else
            axis.x = position.x / abs(position.x);
    } else if (abs(position.y) == sphereRadius) {
        if (abs(position.x) > border.x)
            axis.x = position.x / abs(position.x);
        if (position.z > border.z)
            axis.z = position.z / abs(position.z);
        else
            axis.y = position.y / abs(position.y);
    } else {
        if (abs(position.x > border.x))
            axis.x = position.x / abs(position.x);
        if (abs(position.y) > border.y)
            axis.y = position.y / abs(position.y);
        else
            axis.z = position.z / abs(position.z);
    }

    return axis;
}

void TerrainQuadTree::createRootNodeSphere(glm::vec2 position, axis, currentMiddleChunkPosition) {
    glm::vec3 tmpPos = getRootNodeSpherePosition(axis, position, currentMiddleChunkPosition);
    glm::vec3 newAxis = getSphereAxis(tmpPos);

    if (isEdgeCase(newAxis))
        return;

    if (newAxis != axis)
        tmpPos = getRootNodeSpherePositionAxisChange(newAxis, axis, position, currentMiddleChunkPosition)

    rootMap_[position] = new TerrainChunk(new Terrain(terrainGenerator_,
                                                rootDimension_ + 1,
                                                tmpPos, maxLod_, newAxis), 
                                            NULL); 
}

bool TerrainQuadTree::isEdgeCase(glm::vec3 &axis) {
    int cnt = 0;
    if (axis.x != 0)
        cnt++;
    if (axis.y != 0)
        cnt++;
    if (axis.z != 0)
        cnt++;

    return cnt > 1;
}

void TerrainQuadTree::initSphereConstants() {
    int dim = rootDimension_ / 2; 
    distanceCenterCorner_ = glm::sqrt(dim * dim + glm::sqrt(2*dim*dim));
    int cnt = 0;

    glm::vec3 tmp;
    for (int i = 0; i < 3, i++) {
        tmp = glm::vec3(0,0,0);
        tmp[i] = 1;
        cubeSideMap_[tmp] = cnt++;
        tmp[i] = -1;
        cubeSideMap_[tmp] = cnt++;
    }
}

bool TerrainQuadTree::verifyPosIsOnAxis(glm::vec3 *axis, glm::vec3 *pos) {
    glm::vec3 border(sphereOrigin_.x + sphereRadius_, sphereOrigin_.y + sphereRadius_, sphereOrigin_.z + sphereRadius_);
    bool ret = true;
    switch (cubeSideMap_[*axis]) {
        case 0:
        case 1:
            if (abs(pos->y) > border.y || abs(pos->z) > border.z)
                ret = false;
            break;
        case 2:
        case 3:
            if (abs(pos->x) > border.x || abs(pos->z) > border.z)
                ret = false;
            break;
        case 4:
        case 5:
            if (abs(pos->x) > border.x || abs(pos->y) > border.y)
                ret = false;
            break;
        default:
            fprintf(stdout, "[TERRAINQUADTREE] Error: Unexpected Key in CubeSideMap: %i\n", cubeSideMap_[axis]);
    }
}

void TerrainQuadTree::computeInitialCubePositionAndAxis(glm::vec3 &camPos, glm::vec3 *axisOut, glm::vec3 *positionOut) {
    int halfdim = rootDimension_ / 2;
    float intersectionDistance;
    
    for (int i = 0; i < 3; i++) {
        glm::vec3 axis(0,0,0);
        axis[i] = -1;
        // intersectRayPlane(rayOrigin, rayDirection, planeOrigin, planeNormal, intersectionDistance)
        glm::intersectRayPlane(sphereOrigin_, glm::normalize(camPos - sphereOrigin_), glm::vec3(sphereOrigin_ - halfdim, sphereOrigin_ - halfdim, sphereOrigin_ - halfdim), axis, &intersectionDistance);
        if (intersectionDistance < distanceCenterCorner_) {
            *axisOut = axis;
            *positionOut = sphereOrigin_ + intersectionDistance * glm::normalize(camPos - sphereOrigin_);
            if (verifyPosIsOnAxis(axisOut, positionOut))
                break;
        }
        axis[i] = 1;
        glm::intersectRayPlane(sphereOrigin_, glm::normalize(camPos - sphereOrigin_), glm::vec3(sphereOrigin_ + halfdim, sphereOrigin_ + halfdim, sphereOrigin_ + halfdim), axis, &intersectionDistance);
        if (intersectionDistance < distanceCenterCorner_) {
            *axisOut = axis;
            *positionOut = sphereOrigin_ + intersectionDistance * glm::normalize(camPos - sphereOrigin_);
            if (verifyPosIsOnAxis(axisOut, positionOut))
                break;
        }
    }

    *positionOut = sphereOrigin_ + intersectionDistance * glm::normalize(camPos - sphereOrigin_);
}

void TerrainQuadTree::computeCubePosition(glm::vec3 &camPos, glm::vec3 &axis) {
    int halfdim = rootDimension_ / 2;
    float intersectionDistance;
    int dir = (axis[0] < 0 || axis[1] < 0 || axis[2] < 0) ? -1 : 1;
    glm::intersectRayPlane(sphereOrigin_, glm::normalize(camPos - sphereOrigin_), glm::vec3(dir*halfdim, dir*halfdim, dir*halfdim), axis, &intersectionDistance)
}

glm::vec2 TerrainQuadTree::computeQuadMapPosition(glm::vec3 &pos, glm::vec3 &axis, glm::vec3 &current) {
    glm::vec2 res(0,0)
    switch (cubeSideMap_[axis]) {
        case 0:
        case 1:
            if (abs(pos.y) > current.y + rootDimension_ / 2)  
                res.x = pos.y / abs(pos.y);
            if (abs(pos.z) > current.z + rootDimension_ / 2)
                res.y = pos.z / abs(pos.z);
            break;
        case 2:
        case 3:
            if (abs(pos.x) > current.x + rootDimension_ / 2)
                res.x = pos.x / abs(pos.x);
            if (abs(pos.z) > current.z + rootDimension_ / 2)
                res.y = pos.z / abs(pos.z):
            break;
        case 4:
        case 5:
            if (abs(pos.x) > current.x + rootDimension_ / 2)
                res.x = pos.x / abs(pos.x);
            if (abs(pos.y) > current.y + rootDimension_ / 2)
                res.y = pos.y / abs(pos.y);
            break;
        default:
            fprintf(stdout, "[TERRAINQUADTREE] Error: Unexpected Key in CubeSideMap: %i\n", cubeSideMap_[axis]);
    }
    return res;
}

glm::vec3 TerrainQuadTree::CubeToSpherePos(glm::vec3 &pos) {
    return sphereOrigin_ + (float)sphereRadius_ * glm::normalize(pos - sphereOrigin_);
}


 * Root node middle positions can't lie on the border of planet sides. Distance to border must be at leat rootDim/2 or mutliples of it
 *
 * IDEA(not used yet): Divide each cube side in a board of root nodes (need to be at least 9 in reality of course)
 * --> Current axis is in map position as well
 * Current axis value is radius=? 
 * 
 * ---> JA, weil ich brauch verschiedene keys !
 * |------------------|
 * |         |        |
 * |         |        |
 * |(x,1,0   |(x,1,1) |
 * |------------------|
 * |         |        |
 * |         |        |
 * |(x,0,0)  |(x,0,1) | 
 * |------------------|
 *
 
void TerrainQuadTree::updateRootsSphere(glm::vec3 &camPosition) {
    if (camPosition.x - currentMiddleChunkPosition_.x < rootDimension_ / 4 &&
        camPosition.y - currentMiddleChunkPosition_.y < rootDimension_ / 4 &&
        camPosition.z - currentMiddleChunkPosition_.z < rootDimension_ / 4)
        return;

    glm::vec3 positionOnCubeSide;
    glm::vec3 axis;
    computePositionAndAxis(camPosition, &axis, &positionOnCubeSide);

    glm::vec2 currentPos = computeQuadMapPosition(positionOnCubeSide, axis, currentMiddleChunk_);

    if (currentPos != currentMiddleChunk_) {
        glm::vec2 opp = (currentPos - currentMiddleChunk_);
        currentMiddleChunkPosition_ = rootMap_[currentPos]->getPosition();
        
        opp.x *= 3; // If having 8 quadtrees around the central one
        opp.y *= 3; // -> with numChunksvisible: numChunksVisible * 2 + 1
        RootNodeMap::iterator it;
        for (int z = -1; z < 2; z++) { // use numChunksVisible -> determined by maxViewDistance / rootDimension
            for (int x = -1; x < 2; x++) {
                glm::vec2 tmp = glm::vec2(currentPos.x + x, currentPos.y + z);  // FUCK FUCK FUCK. Wenn der key noch nicht existiert, ich aber immer die gleichen benutze...dat geht nicht
                it = rootMap_.find(tmp);

                if (it == rootMap_.end()) {
                    TerrainChunk *t = NULL;
                    rootMap_.insert(it, RootNodeMap::value_type(tmp, t));
                    threadPool->addJob(std::bind(&TerrainQuadTree::createRootNodeSphere, this, tmp, currentMiddleChunkPosition_));
                
                    // delete opposite
                    TerrainChunk *d = rootMap_.at(tmp - opp);
                    if (d)
                        delete d;
                    rootMap_.erase(tmp - opp);
                }
            }
        }
        currentMiddleChunk_ = currentPos;
    }
}

glm::vec3 TerrainQuadTree::getSpherePos(glm::vec3 &axis, int radius, glm::vec3 &position, int xoff, int zoff) {
    if (axis.x != 0) 
        return glm::vec3(position.x, position.y + xoff, position.z + zoff); 
     else if (axis.y != 0) 
        return glm::vec3(position.x + xoff, position.y, position.z + zoff);
     else 
        return glm::vec3(position.x + xoff, position.y + zoff, position.z);
}

void TerrainQuadTree::createChildrenSphere(TerrainChunk *node) {
    int childDimension = (node->getDimension() - 1) / 2;
    int childPosOffset = childDimension / 2;
    int childLod = glm::max(1, node->getLod() - 2);
    glm::vec3 nodePosition = node->getPosition();
    glm::vec3 axis = node->getTerrain()->getAxis();

    int direction;
    if (axis.x != 0)
        direction = axis.x;
    else if (axis.y != 0)
        direction = axis.y;
    else
        direction = axis.z;

    for (int z = -1; z < 2; z += 2) {
        for (int x = -1; x < 2; x += 2) {
            glm::vec3 pos = getSpherePos(axis, radius, nodePosition, x * childPosOffset, z * childPosOffset);
            node->addChild(new TerrainChunk(new Terrain(terrainGenerator_,
                                                childDimension + 1,
                                                pos, childLod, axis),
                                            NULL));
        }
    }
}
*/