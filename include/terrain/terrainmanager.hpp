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
typedef std::unordered_map<glm::vec3, int> AxisIntegerMap;
typedef std::unordered_map<glm::vec3, CubeSide*> CubeSideMap;
typedef std::unordered_map<glm::vec2, std::tuple<glm::vec3, glm::vec2>> CubeTreeMap;

/*
 * A cube side holds the axis and references to the cube sides we go to on overflow.
 * Each cube side has a RootNodeMap
 * As cube sides only have to be divisible by at least 3, it's not guaranteed there is a center.
 * --> divide in squares with coordinates (0,0) starting bottom left ->
 *
 * Every CubeSide has a containsActiveQuadTrees boolean, that is set, once a quadtree is set on that side.
 * -> ALso a value to indicate if it contains the centerTree?
 *
 * !!!!! I need to provide a distance function -> If the center quadTree is changing, quadTrees that are farther away than 2 must be deleted.
 * ------> Or just delete the opposite? But how to do that?
 * ------------> TRANSLATION: In quad tree I go: delete (tmp - opp) -> If the result is not on this side I need a translation function that maps the position
 * to the cube side it overflowed to.
 * EXAMPLE: CurrentRootNode: (0,2) on axis -z (0,0,-1). RootNode change in -x direction. Causes an overflow. 
 *          New CurrentRootNode: Take tree to the left, coordinates (maxGridPosition, 2)
 *
 * -> Member: CubeSide need max coordinates, so if the side is divided in 9 quads, the max coordinates are (2,2) for the upper right corner.
 *
 * The CubeSphere class manages the cubeSide class and contains a reference to the side with the current center rootTree
 */
 
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
class CubeSide {
public:
  /*
   * Only set initialRun for the CubeSide that contains the initial center piece.
   */
  CubeSide(glm::vec3 axis, CubeSideMap *cubeSideMap, RootNodeMap *cubeTreeMap, int planetDimension, int rootDimension,
          AxisIntegerMap *axisIntegerMap, bool initialRun_ = false) : axis_(axis), cubeSideMap_(cubeSideMap), planetDimension_(planetDimension), rootDimension_(rootDimension) {
    if (planetDimension % rootDimension_ != 0)
      fprintf(stdout, "[CUBESIDE] Error: Planet dimension not divisible by QuadTree Dimension\n");
    maxGridPosition_ = planetDimension_ / rootDimension_;
  }

  getOverflow() {
    
  }

  /*
   * Calculate rootNode position in the rootNodeMap of this side
   * Used as key in the rootNodeMap for a new TerrainChunk
   */
  glm::vec2 getNodePosition(glm::vec3 &pos) {
    switch (*cubeSideMap[axis_]) {
      case 0: // x
      case 1: // -x
        return glm::vec2(glm::floor(abs(pos.z - cubeSideOrigin_.z) / rootDimension_), glm::floor(abs(pos.y - cubeSideOrigin_.y) / rootDimension_));
        break;
      case 2: // y
      case 3: // -y
        return glm::vec2(glm::floor(abs(pos.x - cubeSideOrigin_.x) / rootDimension_), glm::floor(abs(pos.z - cubeSideOrigin_.z) / rootDimension_));
        break;
      case 4: // z
      case 5: // -z
        return glm::vec2(glm::floor(abs(pos.x - cubeSideOrigin_.x) / rootDimension_), glm::floor(abs(pos.y - cubeSideOrigin_.y) / rootDimension_));
        break;
      default:

        return
    }
    
  }

  glm::vec3 getCubePosFromSphere(glm::vec3 &camPos) {
    // intersectRayPlane(rayOrigin, rayDirection, planeOrigin, planeNormal, intersectionDistance)
    float intersectionDistance;
    glm::intersectRayPlane(sphereOrigin_, glm::normalize(camPos - sphereOrigin_), cubeSideOrigin_, axis_, &intersectionDistance);
    return glm::vec3(sphereOrigin_ + intersectionDistance * glm::normalize(camPos - sphereOrigin_));
  }

  // Could just as easily return axis, and have every CubeSide maintain a map mapping axis to *CubeSide pointers.
  bool overflow(glm::vec3 &pos, glm::vec3 *axisOut) {
    axisOut = NULL;
    switch (*cubeSideMap_[axis]) {
      case 0:
      case 1:
        if (pos.z < cubeSideOrigin_.z)
          *axis = glm::vec3(0,0,-1);
        else if (pos.z > cubeSideOrigin_.z + planetDimension)
          *axis = (0,0,1);
        else if (pos.y < cubeSideOrigin_.y)
          *axis = glm::vec3(0,-1,0);
        else if (pos.y > cubeSideOrigin_.y + planetDimension_)
          *axis = glm::vec3(0,1,1);
        break;
      case 2: // y
      case 3: // -y
        if (pos.x < cubeSideOrigin_.x)
          *axis = glm::vec3(-1,0,0);
        else if (pos.x > cubeSideOrigin_.x + planetDimension_)
          *axis = glm::vec3(1,0,0);
        else if (pos.z < cubeSideOrigin_.z)
          *axis = glm::vec3(0,0,-1);
        else if (pos.z > cubeSideOrigin_.z + planetDimension_)
          *axis = glm::vec3(0,0,1);
        break;
      case 4: // z
      case 5: // -z
        if (pos.x < cubeSideOrigin_.x)
          *axis = glm::vec3(-1,0,0);
        else if (pos.x > cubeSideOrigin_ + planetDimension_)
          *axis = glm::vec3(1,0,0);
        else if (pos.y < cubeSideOrigin_.y)
          *axis = glm::vec3(0,-1,0);
        else if (pos.y > cubeSideOrigin_.y)
          *axis = glm::vec3(0,1,0);
        break;
      default:
        printDefaultCubeSideMapError();
    }

    return axisOut ? true : false;
  }

  void destroyRootNode(glm::vec2 &pos) {
    if (rootNodeMap_.find(pos) != rootNodeMap_.end()) {
      TerrainChunk *t = rootNodeMap_[pos];
      if (t)
        delete t;
      rootNodeMap_.erase(pos);
    }

    if (rootNodeMap_.empty())
      containsActiveRootNode_ = false;
  }

  // Make this CubeSide the one containing the centerNode
  void axisChanged(glm::vec3 &posSphere) {
    glm::vec3 pos = getCubePosFromSphere(posSphere);
  }

  // axisFrom is the cube side from which the request came -> so I know on which side of my cubeSide the new node needs to be created.
  void handleCreateNodeRequest(glm::vec2 pos, glm::vec2 cubeTreeMapPosition) {
    // --------------_> Needs another argument, describing which node to create
    // find out which rootNodeMap position it is on my side
    // create the root node
    // assign it to cubeTreeMap[cubeTreeMapPosition]
  }

  /*
   * Maps a position from this axis to the neighbor axis specified by change
   * Offset 0 means a border node.
   * Offset > 0 only used to delete old nodes.
   */
  glm::vec2 mapPosOnNewAxis(glm::vec2 &pos, glm::vec2 &change, int offset = 0) {
    glm::vec2 newPos;
    switch (axisIntegerMap_[axis_]) {
      case 0: // +x
        if (change.x != 0)
          newPos = glm::vec2(maxGridPosition_ - offset, pos.y);
        else if (change.y != 0)
          newPos = glm::vec2(maxGridPosition_ - offset, pos.x)
        break;
      case 1: // -x
        if (change.x != 0)
          newPos = glm::vec2(0 + offset, pos.y); // 0 oder pos.x, das selbe
        else if (change.y != 0)
          newPos = glm::vec2(0 + offset, pos.x);
        break;
      case 2: // +y
        if (change.x != 0)
          newPos = glm::vec2(pos.y, maxGridPosition_ - offset);
        if (change.y != 0)
          newPos = glm::vec2(pos.x, maxGridPosition_ - offset);
        break;
      case 3: // -y
        if (change.x != 0)
          newPos = glm::vec2(pos.y, 0 + offset);
        if (change.y != 0)
          newPos = glm::vec2(pos.x, 0 + offset);
        break;
      case 4: // z
        if (change.x != 0)
          newPos = glm::vec2(maxGridPosition_ - offset, pos.y);
        if (change.y != 0)
          newPos = glm::vec2(pos.x, maxGridPosition_ - offset);
        break;
      case 5: // -z
        if (change.x != 0)
          newPos = glm::vec2(0 + offset, pos.y);
        if (change.y != 0)
          newPos = glm::vec2(0 + offset, pos.x);
        break;
      default:
        printDefaultCubeSideMapError();
        newPos = glm::vec2(-1,-1,-1);
    }

    return newPos;
  }

  glm::vec3 getNextAxis(glm::vec2 &change) {
    glm::vec3 ret;
    switch (axisIntegerMap_[axis_]) {
      case 0:
      case 1:
        if (change.y != 0)
          ret = glm::vec3(0,change.y,0);
        else
          ret = glm::vec3(0,0,change.x);
        break;
      case 2:
      case 3:
        if (change.y != 0)
          ret = glm::vec3(0,0,change.y);
        else
          ret = glm::vec3(change.x,0,0);
        break;
      case 4:
      case 5:
        if (change.y != 0)
          ret = glm::vec3(0, change.y, 0);
        else
          ret = glm::vec3(change.x,0,0);
        break;
      default:
        printDefaultCubeSideMapError();
        ret = glm::vec3(0,0,0);
    }

    return ret;
  }

  /* 
   * Returns true if handled on different
   * Modifies pos...
   */
  bool handleOnDifferentCubeSide(glm::vec2 &pos, glm::vec2 &cubeMapPos, bool destroy = false) {
    bool ret = false;
    bool edgeCase = false;
    glm::vec2 change;
    if (pos.x < 0) {
      if (pos.y < 0) {// Edge case, no node here
        ret = edgeCase = true;
        if (!destroy)
          cubeTreeMap_[cubeMapPos] = NULL;
      } else {
        ret = true;
        pos.x = 0;
        change = glm::vec2(-1,0);
      }
    }

    if (pos.x > maxGridPosition_) {
      if (pos.y > maxGridPosition_) {// Edge case, no node here
        ret = edgeCase = true;
        if (!destroy)
          cubeTreeMap_[cubeMapPos] = NULL;
      } else {
        ret = true;
        pos.x = maxGridPosition_;
        change = glm::vec2(1,0);
      }
    }

    if (pos.y < 0) {
      if (pos.x > maxGridPosition_) { // edge case, no node here
        ret = edgeCase = true;
        if (!destroy)
          cubeTreeMap_[cubeMapPos] = NULL;
      } else {
        ret = true;
        pos.y = 0;
        change = glm::vec2(0,-1);
      }
    }
    
    if (pos.y > maxGridPosition_) {
      if (pos.x < 0) { // edge case, no node here
        ret = edgeCase = true;
        if (!destroy)
          cubeTreeMap_[cubeMapPos] = NULL;
      } else {
        ret = true;
        pos.y = maxGridPosition_;
        change = glm::vec2(0, 1);
      }
    }

    if (ret && !edgeCase && !destroy) {
      glm::vec3 newAxis = getNextAxis(change);
      cubeSideMap[newAxis]->handleCreateNodeRequest(mapPosOnNewAxis(pos, change), cubeMapPos);
    } else if (ret && destroy) {
      glm::vec3 newAxis = getNextAxis(change);
      int offset;
      if (change.x != 0)
        offset = pos.x > 0 ? pos.x - (maxGridPosition_ + 1) : pos.x + 1;
      else
        offset = pos.y > 0 ? pos.y - (maxGridPosition_ + 1) : pos.y + 1;
      cubeSideMap[newAxis]->destroyRootNode(mapPosOnNewAxis(pos, change, offset))
    }

    return ret;
  }

/*
 * args: 
 *      pos: Current camera position on the plane made up by this cube side. Could be out of the cube side
 * 
 * Returns the cubeSide that contains the current centerRootNode
 *
 * Manually set containsCurrentCenterNode_ in the appropriate cubeSide before first run!
 */
CubeSide *update(glm::vec3 &posSphere) {
  glm::vec3 posCube = getCubePosFromSphere(posSphere);
  // Check for overflow
  glm::vec3 *newAxis;
  if(overflow(posCube, newAxis)) {
    containsCurrentCenterNode_ = false;
    return cubeSideMap[newAxis]->update(posSphere); 
  } 
  else {
    glm::vec2 currentPosition = getNodePosition(posCube);

    if (!containsCurrentCenterNode_) { // Axis change, create previousCenterNode outside, for deletion
      currentCenterNode_ = currentPosition;
      if (currentPosition.x == 0)
        currentCenterNode_.x = -1;
      else if (currentPosition.x == maxGridPosition_)
        currentCenterNode_.x == maxGridPosition_ + 1;
      if (currentPosition.y == 0)
        currentCenterNode_.y = -1;
      else if (currentPosition.y == maxGridPosition_)
        currentCenterNode_.y == maxGridPosition_ + 1;
    }

    if (currentPosition != currentCenterNode_) {
      glm::vec2 opp = (currentPosition - currentCenterNode_) * 3.0f; // our structure is a 3*3 grid of quadTrees
      RootNodeMap::iterator it;
      for (int y = -1; y < 2; y++) {
        for (int x = -1; x < 2; x++) {
          glm::vec2 tmp = glm::vec2(currentPosition.x + x, currentPosition.y + y);
          glm::vec2 cubeMapPos(x,y);
            if (!handleOnDifferentCubeSide(tmp, cubeMapPos)) {
              it = rootMap_.find(tmp)

              // Node not yet in map. Create and delete old
              if (it == rootMap_.end()) {
                TerrainChunk *t = NULL;
                rootMap_.insert(it, RootNodeMap::value_type(tmp, t));
                threadPool->addJob(std::bind(&CubeSide::createRootNode, this, tmp, cubeMapPos));

                // delete opposite
                if (!initialRun_) {
                  if (!handleOnDifferentCubeSide(tmp - opp, cubeMapPos, true)) {
                    destroyRootNode(tmp - opp);
                  }
                }

              }
            }
        }
      }
    }
    currentCenterNode_ = currentPosition;
  }

  // updateAllRootNodes(if they need to create children........ HOW DO I DO THAT?)
  for (auto &kv : cubeTreeMap_)
        update_(kv.second, camPosition, tlist, wlist); // kv.second is tuple (axis, rootNodeMapPosition)

  return this;
}

void createRootNode(glm::vec2 cubeSidePos, glm::vec2 cubeMapPos) {
  TerrainChunk *t = new TerrainChunk(new Terrain(terrainGenerator_, rootDimension_ + 1, sphereOrigin_, maxLod_, axis_), NULL);
  rootNodeMap_[cubeSidePos] = t;
  cubeTreeMap_[cubeMapPos] = t;
}

private:
  glm::vec3 axis_;
  glm::vec3 cubeSideOrigin_; // the lower corner of the cubeSide, the (0,0) quad position in the RootNodeMap
  int maxGridPosition_; // max value for the RootNodeMap key. planetDimension / rootDimension (> 3)
  int rootDimension_;
  glm::vec3 sphereOrigin_;
  int planetDimension_;
  bool initialRun_ = false;
  bool containsActiveRootNode_ = false;
  bool containsCurrentCenterNode_ = false;
  glm::vec2 currentCenterNode_ = glm::vec2(-1,-1);
  RootNodeMap rootNodeMap_; // Map of terrain chunks
  AxisIntegerMap *axisIntegerMap_; // maps the axis on integers -> initialized by TerrainCubeTree
  CubeSideMap *cubeSideMap_; // holds the references to the other cubeSides
  CubeTreeMap *cubeTreeMap_; // maps the 9 quadtrees to terrainchunks on the cubesides rootNodeMap -> This one is the main one, the update function in CubeTree loops through the keys
}

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
  TerrainCubeTree(TerrainGenerator *terrainGen, int dimension, int lod);
  ~TerrainCubeTree();
  void update(glm::vec3 &camPosition, std::vector<Drawable *> *tlist, std::vector<Drawable *> *wlist);

private:
  TerrainGenerator *terrainGenerator_;
  std::vector<TerrainChunk *> cubeSides_;
  int planetSizeLod_ = 12;
  int maxLodQuadTree_;
  void initTree(int dimension);
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