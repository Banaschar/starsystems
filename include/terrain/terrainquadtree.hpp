#ifndef TERRAINQUADTREE_H
#define TERRAINQUADTREE_H

class TerrainQuadTree {
  public:
    TerrainQuadTree();
    TerrainQuadTree(TerrainGenerator *terrainGen);
    ~TerrainQuadTree();

    void update();
    void getTerrainList();

  private:
    int rootChunks_ = 1;
    int chunkDimension_ = 100;
    TerrainChunk *rootNode_;
    TerrainGenerator *terrainGenerator_;
    void initTree();
};
#endif