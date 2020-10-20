#ifndef HEIGHTMAP_HPP
#define HEIGHTMAP_HPP

#include "terraingenerator.hpp"

class HeightMap {
public:
    HeightMap(TerrainGenerator *terrainGen, glm::vec2 cornerPos, glm::vec3 axis, int dimension, int index);
    int getIndex();
    unsigned int getHeightTexture();
    unsigned int getNormalTexture();
    glm::vec3 &getAxis();
    void cleanUpMapData();
    void getMaxMinValuesFromArea(glm::vec2 &pos, int dimension, float *nodeMinHeight_, float *nodeMaxHeight_);
private:
    glm::vec2 cornerPos_;
    glm::vec3 axis_;
    int index_;
    int dimension_;
    float lowerNoiseBound_;
    float upperNoiseBound_;
    unsigned int heightTextureId_;
    unsigned int normalTextureId_;
    std::vector<unsigned char> *heightData_ = nullptr;
    std::vector<unsigned char> *normalData_ = nullptr;

    float getOriginalHeight(float inVal);
};
#endif