#ifndef TERRAINMANAGER_H
#define TERRAINMANAGER_H

#include "terraingenerator.hpp"
#include "terrainchunk.hpp"
#include <glm/gtx/hash.hpp>
#include <unordered_map>

/*
 * TODO: Create terrainTree base class and derive the others from it. TerrainManager only has a base TerrainTree
 */

typedef std::unordered_map<glm::vec2, TerrainChunk*> RootNodeMap;

class TerrainQuadTree {
  public:
    TerrainQuadTree(int initialDimension, int maxLod, 
                    TerrainGenerator *terrainGen);
    ~TerrainQuadTree();

    void update(glm::vec3 &camPosition, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist);

  private:
    glm::vec2 currentMiddleChunk_;
    RootNodeMap rootMap_;
    int rootDimension_;
    int maxLod_;
    TerrainGenerator *terrainGenerator_;
    void initTree();
    void createRootNode(glm::vec2 position);
    void createChildren(TerrainChunk *node);
    void updateRoots(glm::vec3 &camPosition);
    void update_(TerrainChunk *node, glm::vec3 &camPosition, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist);
};

class TerrainCubeTree {
public:
  TerrainCubeTree(TerrainGenerator *terrainGen, int dimension, int lod);
  ~TerrainCubeTree();
  void update(glm::vec3 &camPosition, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist);

private:
  TerrainGenerator *terrainGenerator_;
  std::vector<Terrain *> cubeSides_;
  void initTree(int dimension, int lod);
  void createRootNode();
  void update_();
};

enum class TerrainType {SPHERE, PLANE};

class TerrainManager {
public:
    /*
     * Initial dimension is the maximum size of a terrain chunk
     * minChunkSize is the minimum size
     * initialDimension needs to be a multiple of minChunkSize
     * For performance, initialDimension should be divisible by all
     * even numbers up to at least 10, better 12 -> More different levels of detail 
     */
    TerrainManager(TerrainGenerator *terrainGen, int initialDimension, int lodLevels, TerrainType type, glm::vec3 origin);
    ~TerrainManager();

    void update(glm::vec3 &camPosition, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist);
    bool createQuadTree(int initialDimension, int lodLevels, TerrainType type, glm::vec3 origin);

private:
    TerrainCubeTree *terrainCubeTree_ = NULL;
    TerrainQuadTree *terrainQuadTree_ = NULL;
    TerrainGenerator *terrainGenerator_;
    void createDefaultTerrainGenerator();
};
#endif