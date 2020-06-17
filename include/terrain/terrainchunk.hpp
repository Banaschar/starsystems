#ifndef TERRAINCHUNK_H
#define TERRAINCHUNK_H

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
    TerrainChunk(Drawable *terrain, Drawable *water) : terrain_(terrain), water_(water) {}
    ~TerrainChunk() {
        for (TerrainChunk *t : children_)
            delete t;

        if (terrain_)
            delete terrain_

        if (water_)
            delete water_;
    }

    void setParent(TerrainChunk *parent) {
        parent_ = parent;
    }

    bool addChild(TerrainChunk *child) {
        if (index_ == 4)
            return false;

        child->setParent(this);
        children_[index++] = child;
        return true;
    }

    void update() {
        terrain_->update();
        water_->update();
        for (TerrainChunk *child : children_)
            child->update();
    }

    void drawTerrain() {
        terrain_->draw();

        for (TerrainChunk *child : children_)
            child->drawTerrain();
    }

    void drawWater() {
        water_->draw();

        for (TerrainChunk *child : children_)
            child->drawWater();
    }

private:
    int index_ = 0;
    TerrainChunk *parent_;
    std::array<TerrainChunk*, 4> children_;
    Drawable *terrain_;
    Drawable *water_;
}
#endif