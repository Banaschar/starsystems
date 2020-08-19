#ifndef TERRAINMANAGER_H
#define TERRAINMANAGER_H

#include "terraingenerator.hpp"
#include "terrainchunk.hpp"

class TerrainQuadTree {
  public:
    TerrainQuadTree(int initialDimension, int minChunkSize, int maxLod, 
                    TerrainGenerator *terrainGen, int numRootChunks = 1);
    ~TerrainQuadTree();

    void update(glm::vec3 &camPosition, std::vector<Drawable *> *tlist);

  private:
    int numRootChunks_;
    int rootDimension_;
    int minDimension_;
    int maxLod_;
    std::vector<TerrainChunk *> rootNodes_;
    TerrainGenerator *terrainGenerator_;
    void initTree();
    void createChildren(TerrainChunk *node);
    void update_(TerrainChunk *node, glm::vec3 &camPosition, std::vector<Drawable *> *tlist);
};

class TerrainManager {
public:
    TerrainManager();
    TerrainManager(TerrainGenerator *terrainGen);
    TerrainManager(int initialDimension, int minChunkSize, TerrainGenerator *terrainGen = NULL);
    ~TerrainManager();

    void update(glm::vec3 &camPosition, std::vector<Drawable *> *tlist);
    bool createQuadTree(int initialDimension, int minChunkSize);

private:
    TerrainQuadTree *terrainQuadTree_ = NULL;
    TerrainGenerator *terrainGenerator_;
    void createDefaultTerrainGenerator();
};
#endif