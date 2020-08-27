#include "terrainmanager.hpp"

#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <cmath>

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
        terrainCubeTree_ = new TerrainCubeTree(terrainGenerator_, initialDimension, lod);
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
 * TODO: For big planets, I need to switch from terrainCubeTree to terrainQuadTree(with 9 root nodes) when close to the ground
 * e.g. Everything else needs to be deleted
 * If the camera reaches a certain distance, I create a quad tree where the central chunk has the coordinates directly below the camera
 * ----> how to get that point? The point on the line between the camera position and the planet origin that has distance radius from the origin
 */
TerrainCubeTree::TerrainCubeTree(TerrainGenerator *terrainGen, int dimension, int lod) : terrainGenerator_(terrainGen), maxLodQuadTree_(lod) {
    initTree(dimension);
}

TerrainCubeTree::~TerrainCubeTree() {
    for (TerrainChunk *t : cubeSides_) {
        delete t;
    }
}

void TerrainCubeTree::update_(TerrainChunk *node, glm::vec3 &camPosition, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist) {
    if (!node) // unfinished chunk, waiting on thread
        return;

    if (node_->getLod() == 1 || (glm::distance2(camPosition, node->getPosition()) > 
                                glm::pow(node->getDimension() / 2, 2))) {
        tlist->push_back(node->getTerrain());
    } else {
        if (node->getIndex() != 4) {

        }
    }
}

void TerrainCubeTree::update(glm::vec3 &camPosition, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist) {
    tlist->clear();
    wlist->clear();

    //if (distanceToOrigin < radius+10) switchToQuadTree()
    // !!!! QuadTreeRoot node size needs to fit in CubeTree side. So either cubeTreeSideDimension % 3 == 0 or larger
    // (better) quadTreeRootNodeDimension < cubeTreeSideDimension / 6 (with cubeTreeSideDimension % 6 == 0)
    // NOOOO !!!!!!!!!! BETTER:
    // Just wrap around. If the next rootNode is on a different side of the cube, just do so.
    // but no square can overlap the edges... FUCK.

    // create terrainQUADtree with radius and sphereOrigin as argument

    for (Terrain *t : cubeSides_)
        tlist->push_back(t);
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

/*
 * Get axis of new root node
 * If more then one axis ---> edge case, don't create, don't draw
 */
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
}

void TerrainQuadTree::computePositionAndAxis(glm::vec3 &camPos, glm::vec3 *axisOut, glm::vec3 *positionOut) {
    int halfdim = rootDimension_ / 2;
    float intersectionDistance;
    
    for (int i = 0; i < 3; i++) {
        glm::vec3 axis(0,0,0);
        axis[i] = -1;
        glm::intersectRayPlane(sphereOrigin_, glm::normalize(camPos - sphereOrigin_), glm::vec3(-halfdim, - halfdim, -halfdim), axis, &intersectionDistance);
        if (intersectionDistance < distanceCenterCorner_) {
            *axisOut = axis;
            break;
        }
        axis[i] = 1;
        glm::intersectRayPlane(sphereOrigin_, glm::normalize(camPos - sphereOrigin_), glm::vec3(halfdim, halfdim, halfdim), axis, &intersectionDistance);
        if (intersectionDistance < distanceCenterCorner_) {
            *axisOut = axis;
        }
    }

    *positionOut = sphereOrigin_ + intersectionDistance * glm::normalize(camPos - sphereOrigin_);
}

/*
 * Root node middle positions can't lie on the border of planet sides. Distance to border must be at leat rootDim/2 or mutliples of it
 */
void TerrainQuadTree::updateRootsSphere(glm::vec3 &camPosition) {
    if (camPosition.x - currentMiddleChunkPosition_.x < rootDimension_ / 4 &&
        camPosition.y - currentMiddleChunkPosition_.y < rootDimension_ / 4 &&
        camPosition.z - currentMiddleChunkPosition_.z < rootDimension_ / 4)
        return;

    glm::vec3 positionOnCubeSide;
    glm::vec3 axis;
    computePositionAndAxis(camPosition, &axis, &positionOnCubeSide);

    glm::vec2 currentPos;
    if (abs(camPosition.x) > abs(currentMiddleChunkPosition_.x) + rootDimension_ / 2)


    if (currentPos != currentMiddleChunk_) {
        glm::vec2 opp = (currentPos - currentMiddleChunk_);
        currentMiddleChunkPosition_ = rootMap_[currentPos]->getPosition();
        bool currentMiddleIsEdgeCase = isEdgeCase(currentMiddleChunkPosition_);
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
                    threadPool->addJob(std::bind(&TerrainQuadTree::createRootNodeSphere, this, tmp, currentMiddleChunkPosition_));
                
                    // delete opposite
                    delete rootMap_.at(tmp - opp);
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