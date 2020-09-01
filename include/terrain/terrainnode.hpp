#ifndef TERRAINNODE_H
#define TERRAINNODE_H

#include "terraintile.hpp"
#include <array>

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

    glm::vec3 &getPosition();

    int getIndex();

    bool isScheduled();
    void setChildrenScheduled();
    void destroyChildren();

  private:
    int index_ = 0;
    bool childrenScheduled_ = false;
    TerrainNode *parent_ = NULL;
    std::array<TerrainNode *, 4> children_{NULL};
    TerrainTile *terrain_;
    TerrainTile *water_;
};
#endif