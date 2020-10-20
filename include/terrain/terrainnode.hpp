#ifndef TERRAINNODE_H
#define TERRAINNODE_H

#include <array>
#include "terraintile.hpp"
#include "heightmap.hpp"
#include "terraindatatypes.hpp"

/*
 * Terrainchunk used in the Quad tree.
 * Seperate draw methods for water and terrain, because switching shaders and
 * textures every chunk is more costly then iterating over the tree twice.
 *
 * As a child has never more then 4 children in a quad tree, we can use a static array
 */
class TerrainNode {
  public:
    TerrainNode(TerrainTile *terrain, TerrainTile *water);
    ~TerrainNode();

    void setParent(TerrainNode *parent);

    bool addChild(TerrainNode *child);

    void update();

    std::array<TerrainNode *, 4> &getChildren();

    TerrainTile *getTerrain();
    TerrainTile *getWater();

    int getDimension();

    int getLod();

    glm::vec2 &getPosition();

    int getIndex();

    bool isScheduled();
    void setChildrenScheduled();
    void destroyChildren();

  private:
    int index_ = 0;
    bool childrenScheduled_ = false;
    TerrainNode *parent_ = nullptr;
    std::array<TerrainNode *, 4> children_{nullptr};
    TerrainTile *terrain_;
    TerrainTile *water_;
};

class View;

class TerrainNode_ {
public:
    TerrainNode_(HeightMap *heightMap, int nodeDimension, int lod, glm::vec2 pos);
    ~TerrainNode_();
    bool lodSelect(std::vector<float> &ranges, int lodLevel, View *view, IndexedTerrainNodeListMap &nodeMap, std::vector<TerrainNode_ *> &creationList);
    float getNodeMaxHeight();
    float getNodeMinHeight();
    glm::vec2 &getPosition();
    float getRange();
    int getSize();
    int getLodLevel();
    
private:
    glm::vec2 nodePos_;
    int nodeDimension_;
    int lodLevel_;
    float nodeMaxHeight_ = 0;
    float nodeMinHeight_ = 0;
    int heightMapIndex_; // We need a mutex for this
    std::array<TerrainNode_ *, 4> children_{nullptr};
    void createChildren(HeightMap *heightMap, int dim, int lod);
    void insertNode(IndexedTerrainNodeListMap &nodeMap, TerrainNode_ *node);
    float currentLodRange_;

    bool childrenCreationScheduled_ = false;
    bool childrenCreated_ = false; // We need a mutex for this
};
#endif