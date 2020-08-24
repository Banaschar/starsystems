#ifndef TERRAINMANAGER_H
#define TERRAINMANAGER_H

#include "terraingenerator.hpp"
#include "terrainchunk.hpp"
#include <glm/gtx/hash.hpp>
#include <unordered_map>

typedef std::unordered_map<glm::vec2, TerrainChunk*> RootNodeMap;

class TerrainQuadTree {
  public:
    TerrainQuadTree(int initialDimension, int minChunkSize, int maxLod, 
                    TerrainGenerator *terrainGen, int numRootChunks = 9);
    ~TerrainQuadTree();

    void update(glm::vec3 &camPosition, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist);

  private:
    glm::vec2 currentMiddleChunk_;
    RootNodeMap rootMap_;
    int numRootChunks_;
    int rootDimension_;
    int minDimension_;
    int maxLod_;
    TerrainGenerator *terrainGenerator_;
    void initTree();
    void createRootNode(glm::vec2 position);
    void createChildren(TerrainChunk *node);
    void updateRoots(glm::vec3 &camPosition);
    void update_(TerrainChunk *node, glm::vec3 &camPosition, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist);
};

class TerrainManager {
public:
    TerrainManager();
    TerrainManager(TerrainGenerator *terrainGen);
    /*
     * Initial dimension is the maximum size of a terrain chunk
     * minChunkSize is the minimum size
     * initialDimension needs to be a multiple of minChunkSize
     * For performance, initialDimension should be divisible by all
     * even numbers up to at least 10, better 12 -> More different levels of detail 
     */
    TerrainManager(int initialDimension, int minChunkSize, TerrainGenerator *terrainGen = NULL);
    ~TerrainManager();

    void update(glm::vec3 &camPosition, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist);
    bool createQuadTree(int initialDimension, int minChunkSize);

private:
    TerrainQuadTree *terrainQuadTree_ = NULL;
    TerrainGenerator *terrainGenerator_;
    void createDefaultTerrainGenerator();
};
#endif