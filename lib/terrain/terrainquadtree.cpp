#include "terrainquadtree.hpp"

TerrainQuadTree::TerrainQuadTree() {
    PerlinNoise pNoise = PerlinNoise(5, 10.0f, 0.09f);
    terrainGenerator_ = new TerrainGenerator(pNoise);
    initTree();
}

TerrainQuadTree::TerrainQuadTree(TerrainGenerator *terrainGen) : terrainGenerator_(terrainGen) {
    initTree();
}

TerrainQuadTree::~TerrainQuadTree() {
    delete terrainGenerator_;
    delete rootNode_;
}

TerrainQuadTree::initTree() {
    Terrain *terr = new Terrain(terrainGenerator_, terrainDimension, 0, 0);
    rootNode_ = new TerrainChunk(terr, NULL);
    rootNode_->setParent(NULL);
}

bool TerrainQuadTree::update() {
    return false;
}

/*
 * TODO: Rather inefficient to clear and rebuild the entire list each time
 */
void TerrainQuadTree::getTerrainList(std::vector<Drawable *> *tlist) {
    tlist->clear();
    return rootNode_->buildTerrainList(tlist);
}