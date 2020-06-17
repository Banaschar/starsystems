#ifndef TERRAINCHUNK_H
#define TERRAINCHUNK_H

/*
 * Draw water and terrain in one or two draw calls?
 * Iterate over tree twice, or siwtch shaders for every chunk?
 * I would guess switching shaders is more expensive, but need to do
 * performance tests.
 */
class TerrainChunk {
public:
    TerrainChunk() {}
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

    void addChild(TerrainChunk *child) {
        child->setParent(this);
        children_.push_back(child);
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
            child->draw();
    }

    void drawWater() {
        water_->draw();

        for (TerrainChunk *child : children_)
            child->draw();
    }

private:
    TerrainChunk *parent_;
    std::vector<TerrainChunk*> children_;
    Terrain *terrain_;
    Drawable *water_;
}
#endif