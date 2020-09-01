#include "terrainnode.hpp"

TerrainNode::TerrainNode(TerrainTile *terrain, TerrainTile *water) : terrain_(terrain), water_(water) {}

TerrainNode::~TerrainNode() {
    if (terrain_)
        delete terrain_;

    if (water_)
        delete water_;

    for (TerrainNode *t : children_) {
        if (t)
            delete t;
    }
}

void TerrainNode::setParent(TerrainNode *parent) {
    parent_ = parent;
}

bool TerrainNode::addChild(TerrainNode *child) {
    if (index_ == 4)
        return false;

    child->setParent(this);
    children_[index_++] = child;
    return true;
}

int TerrainNode::getDimension() {
    return terrain_->getDimension();
}

glm::vec3 &TerrainNode::getPosition() {
    return terrain_->getPosition();
}

int TerrainNode::getLod() {
    return terrain_->getLod();
}

std::array<TerrainNode *, 4> &TerrainNode::getChildren() {
    return children_;
}

TerrainTile *TerrainNode::getTerrain() {
    return terrain_;
}

TerrainTile *TerrainNode::getWater() {
    return water_;
}

int TerrainNode::getIndex() {
    return index_;
}

bool TerrainNode::isScheduled() {
    return childrenScheduled_;
}

void TerrainNode::setChildrenScheduled() {
    childrenScheduled_ = !childrenScheduled_;
}

void TerrainNode::destroyChildren() {
    for (TerrainNode *t : children_) {
        if (t) {
            delete t;
            t = NULL;
        }
    }
}

void TerrainNode::update() {
    /*
    terrain_->update();
    water_->update();
    for (TerrainNode *child : children_)
        child->update();
    */
    ;
}