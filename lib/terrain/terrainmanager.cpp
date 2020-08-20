#include "terrainmanager.hpp"

#include <glm/gtx/norm.hpp>
#include <cmath>
#include "global.hpp"

#include <glm/gtx/string_cast.hpp>

TerrainManager::TerrainManager() {
    createDefaultTerrainGenerator();
}

TerrainManager::TerrainManager(TerrainGenerator *terrainGen) : terrainGenerator_(terrainGen) {}

TerrainManager::TerrainManager(int initialDimension, int minChunkSize, TerrainGenerator *terrainGen) {
    if (terrainGen)
        terrainGenerator_ = terrainGen;
    else
        createDefaultTerrainGenerator();

    if (!createQuadTree(initialDimension, minChunkSize)) {
        fprintf(stdout, "TERRAINMANAGER: Could not create Quad Tree\n");
    }
}

TerrainManager::~TerrainManager() {
    if (terrainQuadTree_)
        delete terrainQuadTree_;

    delete terrainGenerator_;
}

void TerrainManager::createDefaultTerrainGenerator() {
    PerlinNoise pNoise = PerlinNoise(5, 10.0f, 0.09f);
    terrainGenerator_ = new TerrainGenerator(pNoise);
}

bool TerrainManager::createQuadTree(int initialDimension, int minChunkSize) {
    if (minChunkSize % 60 != 0 || initialDimension % 120 != 0) {
        fprintf(stdout, "TERRAINMANAGER: Dimension or chunk size invalid\n");
        return false;
    } 
    int tmp = minChunkSize;
    int lod = 1;
    while (tmp < initialDimension) {
        lod = lod == 1 ? 2 : lod + 2;
        tmp *= 2;
    }

    terrainQuadTree_ = new TerrainQuadTree(initialDimension, minChunkSize, lod,
                                            terrainGenerator_);

    return true;
}

void TerrainManager::update(glm::vec3 &camPosition, std::vector<Drawable *> *tlist) {
    if (terrainQuadTree_)
        terrainQuadTree_->update(camPosition, tlist);
}

/*
 * TODO: Make numrootchunks depend on max view distance
 */
TerrainQuadTree::TerrainQuadTree(int initialDimension, int minChunkSize, int maxLod, 
                                TerrainGenerator *terrainGen, int numRootChunks) :
                                rootDimension_(initialDimension), minDimension_(minChunkSize), 
                                maxLod_(maxLod), terrainGenerator_(terrainGen),
                                numRootChunks_(numRootChunks) {
    initTree();
}

TerrainQuadTree::~TerrainQuadTree() {
    for (auto &kv : rootMap_)
        delete kv.second;
}

/*
void TerrainQuadTree::initTree() {
    fprintf(stdout, "MAX LOD: %i\n", maxLod_);
    currentPosition_ = glm::vec2(0,0);
    rootNodes_.push_back(new TerrainChunk(new Terrain(terrainGenerator_, 
                                            rootDimension_ + 1, 0, 0, maxLod_),
                                            NULL));
    createChildren(rootNodes_[0]);

    if (numRootChunks_ > 1) {
        int xoff[] = {-1, 0, 1, -1, 1, -1, 0, 1};
        int zoff[] = {1, 1, 1, 0, 0, -1, -1, -1};
        for (int i = 0; i < 8; i++) {
            rootNodes_.push_back(new TerrainChunk(new Terrain(terrainGenerator_, 
                                                rootDimension_ + 1, xoff[i] * rootDimension_, zoff[i] * rootDimension_, maxLod_),
                                                NULL));
        }
    }
}
*/

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

    createChildren(rootMap_.at(currentMiddleChunk_));
}

/*
 * position should be an existing key in rootMap_
 */
void TerrainQuadTree::createRootNode(glm::vec2 position) {
    rootMap_[position] = new TerrainChunk(new Terrain(terrainGenerator_,
                                                rootDimension_ + 1,
                                                position.x * rootDimension_,
                                                position.y * rootDimension_, maxLod_), NULL);  
}

void TerrainQuadTree::update_(TerrainChunk *node, glm::vec3 &camPosition, std::vector<Drawable *> *tlist) {
    if (!node) // Unfinished chunk, waiting for thread
        return;

    if ((node->getLod() == 1) ||
        (glm::distance2(camPosition, node->getPosition()) > 
        glm::pow(node->getDimension(), 2))) {

        tlist->push_back(node->getTerrain());
    } else {
        if (node->getIndex() != 4) {
            tlist->push_back(node->getTerrain());
            if (!node->isScheduled()) {
                node->setChildrenScheduled();
                threadPool->addJob(std::bind(&TerrainQuadTree::createChildren, this, node));
            }
        } else {
            for (TerrainChunk *child : node->getChildren())
                update_(child, camPosition, tlist);
        }
    }
}

void TerrainQuadTree::update(glm::vec3 &camPosition, std::vector<Drawable *> *tlist) {
    tlist->clear();
    updateRoots(camPosition);
    for (auto &kv : rootMap_)
        update_(kv.second, camPosition, tlist);
}

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

    node->addChild(new TerrainChunk(new Terrain(terrainGenerator_, 
                                    childDimension + 1, 
                                    node->getPosition().x + childPosOffset,
                                    node->getPosition().z + childPosOffset,
                                    childLod), NULL));

    node->addChild(new TerrainChunk(new Terrain(terrainGenerator_, 
                                    childDimension + 1, 
                                    node->getPosition().x - childPosOffset,
                                    node->getPosition().z - childPosOffset,
                                    childLod), NULL));

    node->addChild(new TerrainChunk(new Terrain(terrainGenerator_, 
                                    childDimension + 1, 
                                    node->getPosition().x + childPosOffset,
                                    node->getPosition().z - childPosOffset,
                                    childLod), NULL));

    node->addChild(new TerrainChunk(new Terrain(terrainGenerator_, 
                                    childDimension + 1, 
                                    node->getPosition().x - childPosOffset,
                                    node->getPosition().z + childPosOffset,
                                    childLod), NULL));
}