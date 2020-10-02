#ifndef TERRAINMANAGER_H
#define TERRAINMANAGER_H

#include "terraingenerator.hpp"
#include "terrainnode.hpp"
#include "view.hpp"
#include <glm/gtx/hash.hpp>
#include <unordered_map>

class CubeSideTree;
typedef std::unordered_map<glm::vec2, TerrainNode*> RootNodeMap;
typedef std::unordered_map<glm::vec3, int> AxisIntegerMap;
typedef std::unordered_map<glm::vec3, CubeSideTree*> CubeSideMap;
typedef std::unordered_map<glm::vec2, std::tuple<glm::vec3, glm::vec2>> CubeTreeMap;

typedef std::vector<Drawable *> DrawableList;

class TerrainTreeImplementation {
public:
  virtual void update (View *view, DrawableList *terrainList, DrawableList *waterList) = 0;
};

 /*
  * Convention: On the x-axis sides of the cube, z is used as first (x-) coordinate
  *
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
 *      previous: Contains the previous cubeSideTree instance on axis change
 * Returns the cubeSide that contains the current centerRootNode
 * Call setInitialRun() in the appropriate cubeSide before first run!
 */
CubeSideTree *update(glm::vec3 &posSphere, CubeSideTree *previous = NULL);
void updateNodes(glm::vec3 &camWorldPos, glm::vec3 &camDirection, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist);
void setInitialRun();
bool hasActiveNodes();
unsigned int getNumRootNodes();
glm::vec3 &getAxis();
RootNodeMap &getRootNodeMap();

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
  AxisIntegerMap *axisIntegerMap_; // Maps the axis on integers
  CubeSideMap *cubeSideMap_; // Holds the references to the other cubeSides

  void printDefaultCubeSideMapError(std::string name, int key);
  glm::vec3 computeCubeSideOrigin();
  glm::vec3 cubeWorldPosToSpherePos(glm::vec3 &cubePos);
  void updateNode_(TerrainNode *node, glm::vec3 &camWorldPos, glm::vec3 &camDirection, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist);
  bool handleRootNodeCreation(glm::vec2 pos);
  void createRootNode(glm::vec2 cubeSideGridPos);
  glm::vec3 getChildPosition(glm::vec3 &pos, int x, int z, int childPosOffset);
  void createChildren(TerrainNode *node);
  glm::vec3 getNextAxis(glm::vec2 &change);
  glm::vec2 getPreviousCenterNode(glm::vec3 &newAxis, glm::vec2 &gridPos);
    /*
   * Maps a position from this axis to the neighbor axis specified by change
   * Offset 0 means a border node.
   * Offset > 0 only used to delete nodes.
   */
  glm::vec2 mapPosOnNewAxis(glm::vec2 &pos, glm::vec2 &change, int offset = 0);
    /* 
   * Returns true if handled on different
   * Modifies pos to be a border node if it lies on another side. 
   */
  bool handleOnDifferentCubeSide(glm::vec2 &pos, glm::vec2 &change, bool &edgeCase);
    /*
   * Sets axisOut to the new cubeSide axis if overflow
   */
  bool overflow(glm::vec3 &pos, glm::vec3 &axisOut);

  bool handleNodeDeletion(glm::vec2 destroyPos);
  bool handleEdgeCaseNodeDeletion(glm::vec2 &destroyPos);
  bool destroyRootNode(glm::vec2 pos);
    /*
   * Calculate node grid position in the rootNodeMap of this side
   * Used as key in the rootNodeMap for a new TerrainNode
   */
  glm::vec2 worldCubePosToGridPos(glm::vec3 &pos);

  glm::vec3 gridPosToWorldCubePos(glm::vec2 &gridPos);

  glm::vec3 getCubePosFromSphere(glm::vec3 &camPos);
};

class TerrainQuadTree : public TerrainTreeImplementation {
  public:
    TerrainQuadTree(int initialDimension, int maxLod, 
                    TerrainGenerator *terrainGen);
    ~TerrainQuadTree();

    void update(View *view, DrawableList *terrainList, DrawableList *waterList);

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
    void createChildren(TerrainNode *node);
    void updateRoots(glm::vec3 &camPosition);
    void update_(TerrainNode *node, glm::vec3 &camPosition, glm::vec3 &camDirection, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist);
};

class TerrainCubeTree : public TerrainTreeImplementation {
public:
  TerrainCubeTree(TerrainGenerator *terrainGen, int dimension, int lod, glm::vec3 sphereOrigin);
  ~TerrainCubeTree();
  void update(View *view, DrawableList *terrainList, DrawableList *waterList);

private:
  int cubeSideDimension_;
  TerrainGenerator *terrainGenerator_;
  std::vector<TerrainNode *> cubeSides_;
  int planetSizeLod_ = 12;
  int maxLodQuadTree_;
  glm::vec3 sphereOrigin_;
  int sphereRadius_;
  CubeSideTree *currentCubeSide_ = NULL;
  AxisIntegerMap axisIntegerMap_;
  CubeSideMap cubeSideMap_;
  void initTree(int dimension);
  void createCubeSideTree(glm::vec3 &camPos);
  void updateCubeSides(glm::vec3 &camPosition, glm::vec3 &camDirection, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist);
  void computeInitialCubePositionAndAxis(glm::vec3 &camPos, glm::vec3 *axisOut, glm::vec3 *positionOut);
  bool verifyPosIsOnAxis(glm::vec3 &axis, glm::vec3 &pos);
  glm::vec3 cubeWorldPosToSpherePos(glm::vec3 &cubePos);
  void createChildren(TerrainNode *node);
  glm::vec3 getChildPosition(glm::vec3 &pos, int x, int z, int offset, glm::vec3 &axis);
  void updateNode_(TerrainNode *node, glm::vec3 camWorldPos, glm::vec3 &camDirection, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist);
  void destroyCubeSideTree();
};

class TerrainCDLODTree : public TerrainTreeImplementation {
public:
  TerrainCDLODTree(TerrainGenerator *terrainGen);
  ~TerrainCDLODTree();
  void update(View *view, DrawableList *terrainList, DrawableList *waterList);
  
private:
     std::vector<float> ranges_;
     VertexAttributeData rangeAttribData_;
     std::vector<glm::vec3> instanceVecMorphAttribs_;
     std::vector<std::vector<TerrainNode_ *>> grid_;
     int heightMapIndex_ = 0;
     int lodLevelCount_;
     Drawable *basePatch_;

     float getNextRange(int lodLevel);
};

enum class TerrainType {DEFAULT, SPHERE, PLANE, CDLOD};

class TerrainManager {
public:
    /*
     * Initial dimension is the maximum size of a terrain chunk or one side of a planet cubeTree
     * For performance, initialDimension should be divisible by all
     * even numbers up to at least 10, better 12 -> More different levels of detail 
     */
    TerrainManager(TerrainGenerator *terrainGen, int initialDimension, int lodLevels, TerrainType type, glm::vec3 origin);
    ~TerrainManager();

    void update(View *view, DrawableList *terrainList, DrawableList *waterList);
    TerrainType getType();

private:
    TerrainType type_;
    TerrainTreeImplementation *terrainTreeImplementation_ = nullptr;
    bool createTerrainTree(TerrainGenerator *terrainGen, int initialDimension, int lodLevels, TerrainType type, glm::vec3 origin);
};
#endif