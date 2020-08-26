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

TerrainCubeTree::TerrainCubeTree(TerrainGenerator *terrainGen, int dimension, int lod) : terrainGenerator_(terrainGen) {
    initTree(dimension, lod);
}

TerrainCubeTree::~TerrainCubeTree() {
    for (Terrain *t : cubeSides_) {
        if (t)
            delete t;
    }
}

void TerrainCubeTree::update(glm::vec3 &camPosition, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist) {
    tlist->clear();
    //wlist->clear();

    for (Terrain *t : cubeSides_)
        tlist->push_back(t);
}

void TerrainCubeTree::initTree(int dimension, int lod) {
    
    Terrain *t = new Terrain(terrainGenerator_, dimension + 1, glm::vec3(0,0,0), lod, glm::vec3(1,0,0));
    Terrain *t1 = new Terrain(terrainGenerator_, dimension + 1, glm::vec3(0,0,0), lod, glm::vec3(-1,0,0));
    Terrain *t2 = new Terrain(terrainGenerator_, dimension + 1, glm::vec3(0,0,0), lod, glm::vec3(0,1,0));
    Terrain *t3 = new Terrain(terrainGenerator_, dimension + 1, glm::vec3(0,0,0), lod, glm::vec3(0,-1,0));
    Terrain *t4 = new Terrain(terrainGenerator_, dimension + 1, glm::vec3(0,0,0), lod, glm::vec3(0,0,1));
    Terrain *t5 = new Terrain(terrainGenerator_, dimension + 1, glm::vec3(0,0,0), lod, glm::vec3(0,0,-1));

    /*
    //x
    glm::vec3 rotAx(1,0,0);
    //glm::vec3 trans(0,0,radius);
    t->transform(NULL, NULL, &rotAx, 90);
    //-z
    rotAx = glm::vec3(0,0,1);
    //trans = glm::vec3(-radius,0,0);
    t1->transform(NULL, NULL, &rotAx, 90);

    //y
    //rotAx = glm::vec3(0,1,0);
    //trans = glm::vec3(0,dimension / 2,0);
    //t2->transform(NULL, &trans, NULL);
    //-y
    rotAx = glm::vec3(1,0,0);
    //trans = glm::vec3(0,-radius,0);
    t3->transform(NULL, NULL, &rotAx, 180);

    // -x
    rotAx = glm::vec3(-1,0,0);
    //trans = glm::vec3(0,0,-radius);
    t4->transform(NULL, NULL, &rotAx, 90);

    //z
    rotAx = glm::vec3(0,0,-1);
    //trans = glm::vec3(radius,0,0);
    t5->transform(NULL, NULL, &rotAx, 90);
    */


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
            glm::vec3 pos(node->getPosition().x + x * childPosOffset, 0, node->getPosition().z + z * childPosOffset);
            node->addChild(new TerrainChunk(new Terrain(terrainGenerator_,
                                                childDimension + 1,
                                                pos.x, pos.z, childLod),
                                            DrawableFactory::createWaterTile(pos, childPosOffset, glm::vec3(0,0,1))));
        }
    }
}