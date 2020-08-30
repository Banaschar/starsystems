#ifndef TERRAINMANAGER_H
#define TERRAINMANAGER_H

#include "terraingenerator.hpp"
#include "terrainchunk.hpp"
#include <glm/gtx/hash.hpp>
#include <unordered_map>

/*
 * TODO: Create terrainTree base class and derive the others from it. TerrainManager only has a base TerrainTree
 */
class CubeSideTree;
typedef std::unordered_map<glm::vec2, TerrainChunk*> RootNodeMap;
typedef std::unordered_map<glm::vec3, int> AxisIntegerMap;
typedef std::unordered_map<glm::vec3, CubeSideTree*> CubeSideMap;
typedef std::unordered_map<glm::vec2, std::tuple<glm::vec3, glm::vec2>> CubeTreeMap;
 
 /*
  * CONVENTION: On each side, x is always the first coordinate (x, ?), except on the x axis ((1,0,0) and (-1,0,0)), here z is the first (z, y)
  *               Or more logically said: y is always the second coordiante
  *
  * The corner is always where the x and y coordinates are lowest !!!!!!!!!!!!!!!!! (so it's not always the lowe left)
  *
  * That means, going sideways around the cube is easy, the (0,0) coordiantes are in the same place, along the first axis
  * Going from -z to +y is also straigh forward. From +z to +y is inverted...etc.
  *
  *
  * EDGE CASES: What if the camera is exactly on an edge of the cube. If PlanetDim = 120, rootDim = 30 -> maxGridPosition == 3, but 120 / 30 == 4
  *               -> Hm, maybe the next edge handles that
  */
class CubeSideTree {
public:
  /*
   * .
   */
  CubeSideTree(glm::vec3 axis, TerrainGenerator *terrainGen, CubeSideMap *cubeSideMap, AxisIntegerMap *axisIntegerMap, glm::vec3 &sphereOrigin, int planetDimension, int rootDimension, int rootLod);
  ~CubeSideTree();
/*
 * args: 
 *      pos: Current camera position on the plane made up by this cube side. Could be out of the cube side
 * 
 * Returns the cubeSide that contains the current centerRootNode
 *
 * Manually set containsCurrentCenterNode_ in the appropriate cubeSide before first run!
 */
CubeSideTree *update(glm::vec3 &posSphere);
void updateNodes(glm::vec3 &camWorldPos, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist);
void setInitialRun();
bool hasActiveNodes();

private:
  glm::vec3 axis_;
  glm::vec3 cubeSideOrigin_; // the lower corner of the cubeSide, the (0,0) quad position in the RootNodeMap
  int maxGridPosition_; // max value for the RootNodeMap key. planetDimension / rootDimension - 1 (>= 3)
  int rootDimension_;
  glm::vec3 sphereOrigin_;
  int cubeSideDimension_;
  int sphereRadius_;
  int rootLod_;
  bool initialRun_ = false;
  bool containsActiveRootNode_ = false;
  bool containsCurrentCenterNode_ = false;
  glm::vec2 currentCenterNode_ = glm::vec2(-1,-1);
  TerrainGenerator *terrainGenerator_;
  RootNodeMap rootNodeMap_; // Map of terrain chunks
  AxisIntegerMap *axisIntegerMap_; // maps the axis on integers -> initialized by TerrainCubeTree
  CubeSideMap *cubeSideMap_; // holds the references to the other cubeSides

  void printDefaultCubeSideMapError(std::string name, int key);
  glm::vec3 computeCubeSideOrigin();
  glm::vec3 cubeWorldPosToSpherePos(glm::vec3 &cubePos);
  void updateNode_(TerrainChunk *node, glm::vec3 camWorldPos, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist);
  void handleRootNodeCreation(glm::vec2 pos);
  void createRootNode(glm::vec2 cubeSideGridPos);
  glm::vec3 getChildPosition(glm::vec3 &pos, int x, int z, int childPosOffset);
  void createChildren(TerrainChunk *node);
  glm::vec3 getNextAxis(glm::vec2 &change);
    /*
   * Maps a position from this axis to the neighbor axis specified by change
   * Offset 0 means a border node.
   * Offset > 0 only used to delete old nodes.
   */
  glm::vec2 mapPosOnNewAxis(glm::vec2 &pos, glm::vec2 &change, int offset = 0);
    /* 
   * Returns true if handled on different
   * Modifies pos...
   */
  bool handleOnDifferentCubeSide(glm::vec2 pos, bool destroy = false);
    /*
   * Sets axisOut to the new cubeSide axis if overflow
   */
  bool overflow(glm::vec3 &pos, glm::vec3 &axisOut);

  void destroyRootNode(glm::vec2 pos);
    /*
   * Calculate node grid position in the rootNodeMap of this side
   * Used as key in the rootNodeMap for a new TerrainChunk
   */
  glm::vec2 worldCubePosToGridPos(glm::vec3 &pos);

  glm::vec3 gridPosToWorldCubePos(glm::vec2 &gridPos);

  glm::vec3 getCubePosFromSphere(glm::vec3 &camPos);
};

class TerrainQuadTree {
  public:
    TerrainQuadTree(int initialDimension, int maxLod, 
                    TerrainGenerator *terrainGen);
    ~TerrainQuadTree();

    void update(glm::vec3 &camPosition, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist);

  private:
    glm::vec2 currentMiddleChunk_;
    glm::vec3 currentMiddleChunkPosition_;
    int sphereRadius_;
    glm::vec3 sphereOrigin_;
    glm::vec3 currentAxis_;
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
  TerrainCubeTree(TerrainGenerator *terrainGen, int dimension, int lod, glm::vec3 sphereOrigin);
  ~TerrainCubeTree();
  void update(glm::vec3 &camPosition, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist);

private:
  int cubeSideDimension_;
  TerrainGenerator *terrainGenerator_;
  std::vector<TerrainChunk *> cubeSides_;
  int planetSizeLod_ = 12;
  int maxLodQuadTree_;
  glm::vec3 sphereOrigin_;
  int sphereRadius_;
  CubeSideTree *currentCubeSide_ = NULL;
  AxisIntegerMap axisIntegerMap_;
  CubeSideMap cubeSideMap_;
  void initTree(int dimension);
  void createCubeSideTree(glm::vec3 &camPos);
  void updateCubeSides(glm::vec3 &camPosition, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist);
  void computeInitialCubePositionAndAxis(glm::vec3 &camPos, glm::vec3 *axisOut, glm::vec3 *positionOut);
  bool verifyPosIsOnAxis(glm::vec3 &axis, glm::vec3 &pos);
  glm::vec3 cubeWorldPosToSpherePos(glm::vec3 &cubePos);
  void createChildren(TerrainChunk *node);
  glm::vec3 getChildPosition(glm::vec3 &pos, int x, int z, int offset, glm::vec3 &axis);
  void updateNode_(TerrainChunk *node, glm::vec3 camWorldPos, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist);
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