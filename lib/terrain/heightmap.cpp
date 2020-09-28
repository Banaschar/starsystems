#include <cstdio>
#include "heightmap.hpp"
#include "textureloader.hpp"

HeightMap::HeightMap(TerrainGenerator *terrainGen, glm::vec3 cornerPos, glm::vec3 axis, int dimension, int index) : cornerPos_(cornerPos), axis_(axis), index_(index), dimension_(dimension) {
    heightData_ = new std::vector<unsigned char>(dimension_ * dimension_);
    normalData_ = new std::vector<unsigned char>(dimension_ * dimension_ * 3);
    terrainGen->generateTerrainHeightMap(cornerPos_, axis_, dimension_, heightData_, normalData_);
    heightTextureId_ = TextureLoader::createTextureFromArray(heightData_->data(), dimension_, dimension_, 1);
    normalTextureId_ = TextureLoader::createTextureFromArray(normalData_->data(), dimension_, dimension_, 3);
    delete normalData_;
}

int HeightMap::getIndex() {
    return index_;
}

unsigned int HeightMap::getHeightTexture() {
    return heightTextureId_;
}

unsigned int HeightMap::getNormalTexture() {
    return normalTextureId_;
}

/* Should be called after the root node tree for this height map is created */
void HeightMap::cleanUpMapData() {
    delete heightData_;
    heightData_ = nullptr;
}

void HeightMap::getMaxMinValuesFromArea(glm::vec3 &pos, int dimension, float *nodeMinHeight_, float *nodeMaxHeight_) {
    float min = 0.0f;
    float max = 0.0f;
    for (int y = pos.z; y < pos.z + dimension; ++y) {
        for (int x = pos.x; x < pos.x + dimension; ++x) {
            min = std::min(min, (float)(*heightData_)[y * dimension + x]);
            max = std::max(max, (float)(*heightData_)[y * dimension + x]);
        }
    }

    *nodeMinHeight_ = min;
    *nodeMaxHeight_ = max;
}