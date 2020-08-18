#ifndef TERRAINQUADTREE_H
#define TERRAINQUADTREE_H

#include "terraingenerator.hpp"
#include "terrainchunk.hpp"

class TerrainQuadTree {
  public:
    TerrainQuadTree();
    TerrainQuadTree(TerrainGenerator *terrainGen);
    ~TerrainQuadTree();

    bool update();
    void getTerrainList(std::vector<Drawable *> *tlist);

  private:
    int rootChunks_ = 1;
    int chunkDimension_ = 100;
    TerrainChunk *rootNode_;
    TerrainGenerator *terrainGenerator_;
    void initTree();
};
#endif