#include "terrainnode.hpp"
#include "view.hpp"
#include "mathutils.hpp"
#include <glm/gtx/string_cast.hpp>

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

/* TODO: getMaxMinValuesFromArea -> call when reached lowest level of current heightmap, instead of global 0 
 *          -> that means a heightMap needs a range of lod levels it covers.
 */
TerrainNode_::TerrainNode_(HeightMap *heightMap, int nodeDimension, int lod, glm::vec2 pos) : nodeDimension_(nodeDimension), nodePos_(pos), lodLevel_(lod) {
    heightMapIndex_ = heightMap->getIndex();
    
    if (lod == 0) {
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

    children_[0] = new TerrainNode_(heightMap, half, lod, glm::vec3(nodePos_.x, 0, nodePos_.y));
    children_[1] = new TerrainNode_(heightMap, half, lod, glm::vec3(nodePos_.x + half, 0, nodePos_.y));
    children_[2] = new TerrainNode_(heightMap, half, lod, glm::vec3(nodePos_.x, 0, nodePos_.y + half));
    children_[3] = new TerrainNode_(heightMap, half, lod, glm::vec3(nodePos_.x + half, 0, nodePos_.y + half));

    childrenCreated_ = true;
}

/* Add argument that represents a list of heightmaps that need to be created 
 * Needed parameters: heightmapindex, parentPosition (as heightmap Origin), axis
 * !! In that case, terrainNode needs a new variable, the heightmapdimension...
 */
bool TerrainNode_::lodSelect(std::vector<float> &ranges, int lodLevel, View *view, std::unordered_map<int, std::vector<TerrainNode_ *>> *nodeMap, ) {
    currentLodRange_ = ranges[lodLevel];

    BoundingBox bBox(glm::vec3(nodePos_.x, nodeMinHeight_, nodePos_.y), glm::vec3(nodePos_.x + nodeDimension_, nodeMaxHeight_, nodePos_.y + nodeDimension_));

    if (!bBox.intersectSphereSq(view->getCameraPosition(), currentLodRange_*currentLodRange_))
        return false;

    /*
    if (!view->isInsideFrustum(bBox)) {
        fprintf(stdout, "NotInFrustum. LodLevel: %i\n", lodLevel);
        return true;
    }
    */

    if (lodLevel == 0) {
        (*nodeMap)[heightMapIndex_].push_back(this);
        return true;
    } else {
        if (!bBox.intersectSphereSq(view->getCameraPosition(), ranges[lodLevel-1] * ranges[lodLevel-1])) {
            tlist->push_back(this);
        } else {
            if (childrenCreated_) {
                for (TerrainNode_ *child : children_) {
                    if (!child->lodSelect(ranges, lodLevel - 1, view, tlist)) {
                        //child->currentLodRange_ = currentLodRange_;
                        (*nodeMap)[heightMapIndex_].push_back(child); // this should actually be the area covered by the child represented in the current lodlevel
                    }
                }
            } else {
                if (!childrenCreationScheduled_) {
                    heightMapCreationList.push_back(this);
                    childrenCreationScheduled_ = true;
                }

                /* Cover the are with ourself, while we wait for children to be created */
                (*nodeMap)[heightMapIndex_].push_back(this);
            }
        }

        return true;
    }
}

float TerrainNode_::getNodeMaxHeight() {
    return nodeMaxHeight_;
}

float TerrainNode_::getNodeMinHeight() {
    return nodeMinHeight_;
}

float TerrainNode_::getRange() {
    return currentLodRange_;
}

glm::vec2 &TerrainNode_::getPosition() {
    return nodePos_;
}

int TerrainNode_::getSize() {
    return nodeDimension_;
}

int TerrainNode_::getLodLevel() {
    return lodLevel_;
}