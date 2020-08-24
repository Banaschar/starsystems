#ifndef TERRAINCHUNK_H
#define TERRAINCHUNK_H

#include "terrain.hpp"
#include <array>

/*
 * Terrainchunk used in the Quad tree.
 * Seperate draw methods for water and terrain, because switching shaders and
 * textures every chunk is more costly then iterating over the tree twice.
 *
 * As a child has never more then 4 children in a quad tree, we can use a static array
 */
class TerrainChunk {
  public:
    TerrainChunk(Terrain *terrain, Drawable *water);
    ~TerrainChunk();

    void setParent(TerrainChunk *parent);

    bool addChild(TerrainChunk *child);

    void update();

    std::array<TerrainChunk *, 4> &getChildren();

    Terrain *getTerrain();
    Drawable *getWater();

    int getDimension();

    int getLod();

    glm::vec3 &getPosition();

    int getIndex();

    bool isScheduled();
    void setChildrenScheduled();

  private:
    int index_ = 0;
    bool childrenScheduled_ = false;
    TerrainChunk *parent_ = NULL;
    std::array<TerrainChunk *, 4> children_{NULL};
    Terrain *terrain_;
    Drawable *water_;
};
#endif