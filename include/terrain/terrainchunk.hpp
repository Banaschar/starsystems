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
    TerrainChunk(Drawable *terrain, Drawable *water);
    ~TerrainChunk();

    void setParent(TerrainChunk *parent);

    bool addChild(TerrainChunk *child);

    void buildTerrainList(std::vector<Terrain*> *terrainList);

    void update();

  private:
    int index_ = 0;
    TerrainChunk *parent_;
    std::array<TerrainChunk *, 4> children_{NULL};
    Drawable *terrain_;
    Drawable *water_;
};
#endif