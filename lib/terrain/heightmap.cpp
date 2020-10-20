#include <cstdio>
#include "heightmap.hpp"
#include "textureloader.hpp"

HeightMap::HeightMap(TerrainGenerator *terrainGen, glm::vec2 cornerPos, glm::vec3 axis, int dimension, int index) : cornerPos_(cornerPos), axis_(axis), index_(index), dimension_(dimension) {
    heightData_ = new std::vector<unsigned char>(dimension_ * dimension_);
    normalData_ = new std::vector<unsigned char>(dimension_ * dimension_ * 3);
    lowerNoiseBound_ = terrainGen->getPerlinNoise().getLowerBound();
    upperNoiseBound_ = terrainGen->getPerlinNoise().getUpperBound();
    GenerationAttributes attribs;
    attribs.position = cornerPos_;
    attribs.axis = axis_;
    attribs.dimension = dimension_;
    attribs.heightData = heightData_;
    attribs.normalData = normalData_;
    terrainGen->generateTerrainHeightMap(&attribs);
    heightTextureId_ = TextureLoader::createTextureFromArray(heightData_->data(), dimension_, dimension_, 1);
    normalTextureId_ = TextureLoader::createTextureFromArray(normalData_->data(), dimension_, dimension_, 3);
    delete normalData_;
}

int HeightMap::getIndex() {
    return index_;
}

glm::vec3 &HeightMap::getAxis() {
    return axis_;
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

float HeightMap::getOriginalHeight(float inVal) {
    float s = 1.0 * (upperNoiseBound_ - lowerNoiseBound_) / (255 - 0);
    return lowerNoiseBound_ + std::floor((s * (inVal - 0)) + 0.5);
}

void HeightMap::getMaxMinValuesFromArea(glm::vec2 &pos, int dimension, float *nodeMinHeight_, float *nodeMaxHeight_) {
    float min = 0.0f;
    float max = 0.0f;
    for (int y = pos.y; y < pos.y + dimension; ++y) {
        for (int x = pos.x; x < pos.x + dimension; ++x) {
            min = std::min(min, getOriginalHeight((float)(*heightData_)[y * dimension + x]));
            max = std::max(max, getOriginalHeight((float)(*heightData_)[y * dimension + x]));
        }
    }

    *nodeMinHeight_ = min;
    *nodeMaxHeight_ = max;
}