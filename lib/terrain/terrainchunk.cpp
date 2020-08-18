#include "terrainchunk.hpp"

TerrainChunk::TerrainChunk(Drawable *terrain, Drawable *water) : terrain_(terrain), water_(water) {}

TerrainChunk::~TerrainChunk() {
    for (TerrainChunk *t : children_) {
        if (t)
            delete t;
    }

    if (terrain_)
        delete terrain_;

    if (water_)
        delete water_;
}

void TerrainChunk::setParent(TerrainChunk *parent) {
    parent_ = parent;
}

bool TerrainChunk::addChild(TerrainChunk *child) {
    if (index_ == 4)
        return false;

    child->setParent(this);
    children_[index_++] = child;
    return true;
}

void TerrainChunk::buildTerrainList(std::vector<Drawable *> *terrainList) {
    terrainList->push_back(terrain_);

    for (TerrainChunk *child : children_) {
        if (child)
            child->buildTerrainList(terrainList);
    }
}

void TerrainChunk::update() {
    /*
    terrain_->update();
    water_->update();
    for (TerrainChunk *child : children_)
        child->update();
    */
    ;
}