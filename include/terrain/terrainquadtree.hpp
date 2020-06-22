#ifndef TERRAINQUADTREE_H
#define TERRAINQUADTREE_H

class TerrainQuadTree {
  public:
    TerrainQuadTree();

  private:
    int rootChunks = 1;
    TerrainChunk *rootNode;
};
#endif