#include "terrainchunk.hpp"

TerrainChunk::TerrainChunk(Terrain *terrain, Drawable *water) : terrain_(terrain), water_(water) {}

TerrainChunk::~TerrainChunk() {
    if (terrain_)
        delete terrain_;

    if (water_)
        delete water_;

    for (TerrainChunk *t : children_) {
        if (t)
            delete t;
    }
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

int TerrainChunk::getDimension() {
    return terrain_->getDimension();
}

glm::vec3 &TerrainChunk::getPosition() {
    return terrain_->getPosition();
}

int TerrainChunk::getLod() {
    return terrain_->getLod();
}

std::array<TerrainChunk *, 4> &TerrainChunk::getChildren() {
    return children_;
}

Terrain *TerrainChunk::getTerrain() {
    return terrain_;
}

int TerrainChunk::getIndex() {
    return index_;
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