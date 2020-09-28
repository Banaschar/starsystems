#include "terrainnode.hpp"
#include "view.hpp"

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

TerrainNode_::TerrainNode_(HeightMap *heightMap, int nodeDimension, int lod, glm::vec3 pos) : nodeDimension_(nodeDimension), nodePos_(pos) {
    heightMapIndex_ = heightMap->getIndex();

    if (lod == 1) {
        heightMap->getMaxMinValuesFromArea(nodePos_, nodeDimension_, &nodeMinHeight_, &nodeMaxHeight_);
    } else {
        createChildren(heightMap, nodeDimension, lod);
        for (TerrainNode_ *child : children_) {
            nodeMaxHeight_ = std::max(child->getNodeMaxHeight(), nodeMaxHeight_);
            nodeMinHeight_ = std::min(child->getNodeMinHeight(), nodeMinHeight_);
        }
    }
}

TerrainNode_::~TerrainNode_() {
    for (TerrainNode_ *t : children_)
        delete t;
}

void TerrainNode_::createChildren(HeightMap *heightMap, int dim, int lod) {
    int half = dim / 2;
    --lod;

    children_[0] = new TerrainNode_(heightMap, half, lod, glm::vec3(nodePos_.x, 0, nodePos_.z));
    children_[1] = new TerrainNode_(heightMap, half, lod, glm::vec3(nodePos_.x + half, 0, nodePos_.z));
    children_[2] = new TerrainNode_(heightMap, half, lod, glm::vec3(nodePos_.x, 0, nodePos_.z + half));
    children_[3] = new TerrainNode_(heightMap, half, lod, glm::vec3(nodePos_.x + half, 0, nodePos_.z + half));
}

bool TerrainNode_::lodSelect(std::vector<int> &ranges, int lodLevel, View *view, std::vector<TerrainNode_ *> *tlist) {
    currentLodRange_ = ranges[lodLevel];

    if (!boundingBoxIntersectsSphere(ranges[lodLevel], view->getCameraPosition()))
        return false;

    if (!inFrustum(view))
        return true;

    if (lodLevel == 0) {
        tlist->push_back(this);
        return true;
    } else {
        if (!boundingBoxIntersectsSphere(ranges[lodLevel - 1], view->getCameraPosition())) {
            tlist->push_back(this);
        } else {
            for (TerrainNode_ *child : children_) {
                if (!child->lodSelect(ranges, lodLevel - 1, view, tlist)) {
                    child->currentLodRange_ = currentLodRange_;
                    tlist->push_back(child);
                }
            }
        }

        return true;
    }
}

bool TerrainNode_::boundingBoxIntersectsSphere(float radius, glm::vec3 position) {
    float r2 = radius * radius;
    glm::vec3 v1 = glm::vec3(nodePos_.x, nodeMinHeight_, nodePos_.z);
    glm::vec3 v2 = glm::vec3(nodePos_.x + nodeDimension_, nodeMaxHeight_, nodePos_.z + nodeDimension_);
    glm::vec3 dist;

    if (position.x < v1.x)
        dist.x = position.x - v1.x;
    else if (position.x > v2.x)
        dist.x = position.x - v2.x;

    if (position.y < v1.y)
        dist.y = position.y - v1.y;
    else if (position.y > v2.y)
        dist.y = position.y - v2.y;

    if (position.z < v1.z)
        dist.z = position.z - v1.z;
    else if (position.z > v2.z)
        dist.z = position.z - v2.z;

    return glm::dot(dist, dist) <= r2;
}

bool TerrainNode_::inFrustum(View *view) {
    glm::vec3 min = glm::vec3(nodePos_.x, nodeMinHeight_, nodePos_.z);
    glm::vec3 max = glm::vec3(nodePos_.x + nodeDimension_, nodeMaxHeight_, nodePos_.z + nodeDimension_);

    return view->isInsideFrustum(max, min);
}

float TerrainNode_::getNodeMaxHeight() {
    return nodeMaxHeight_;
}

float TerrainNode_::getNodeMinHeight() {
    return nodeMinHeight_;
}