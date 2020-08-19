#include "terrainmanager.hpp"

#include <glm/gtx/norm.hpp>

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

    terrainQuadTree_ = new TerrainQuadTree(initialDimension+1, minChunkSize+1, glm::max(12, ((initialDimension / minChunkSize) - 1) * 2),
                                            terrainGenerator_);

    return true;
}

void TerrainManager::update(glm::vec3 &camPosition, std::vector<Drawable *> *tlist) {
    if (terrainQuadTree_)
        terrainQuadTree_->update(camPosition, tlist);
}

TerrainQuadTree::TerrainQuadTree(int initialDimension, int minChunkSize, int maxLod, 
                                TerrainGenerator *terrainGen, int numRootChunks) :
                                rootDimension_(initialDimension), minDimension_(minChunkSize), 
                                maxLod_(maxLod), terrainGenerator_(terrainGen),
                                numRootChunks_(numRootChunks) {
    initTree();
}

TerrainQuadTree::~TerrainQuadTree() {
    for (TerrainChunk *t : rootNodes_)
        delete t;
}

void TerrainQuadTree::initTree() {

    rootNodes_.push_back(new TerrainChunk(new Terrain(terrainGenerator_, 
                                            rootDimension_, 0, 0, maxLod_),
                                            NULL));
    createChildren(rootNodes_[0]);

    if (numRootChunks_ > 1) {
        int xoff[] = {-1, 0, 1, -1, 1, -1, 0, 1};
        int zoff[] = {1, 1, 1, 0, 0, -1, -1, -1};
        for (int i = 0; i < 8; i++) {
            rootNodes_.push_back(new TerrainChunk(new Terrain(terrainGenerator_, 
                                                rootDimension_, xoff[i] * (rootDimension_ - 1), zoff[i] * (rootDimension_ - 1), maxLod_),
                                                NULL));
        }
    }
}

void TerrainQuadTree::update_(TerrainChunk *node, glm::vec3 &camPosition, std::vector<Drawable *> *tlist) {
    if ((node->getLod() == 1) ||
        (glm::distance2(camPosition, node->getPosition()) > 
        glm::pow(node->getDimension(), 2))) {

        tlist->push_back(node->getTerrain());
    } else {
        if (node->getIndex() == 0)
            createChildren(node);

        for (TerrainChunk *child : node->getChildren())
            update_(child, camPosition, tlist);
    }
}

void TerrainQuadTree::update(glm::vec3 &camPosition, std::vector<Drawable *> *tlist) {
    tlist->clear();

    for (TerrainChunk *root : rootNodes_)
        update_(root, camPosition, tlist);
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