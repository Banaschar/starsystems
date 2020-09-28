#ifndef HEIGHTMAP_HPP
#define HEIGHTMAP_HPP

#include "terraingenerator.hpp"

class HeightMap {
public:
    HeightMap(TerrainGenerator *terrainGen, glm::vec3 cornerPos, glm::vec3 axis, int dimension, int index);
    int getIndex();
    unsigned int getHeightTexture();
    unsigned int getNormalTexture();
    void cleanUpMapData();
    void getMaxMinValuesFromArea(glm::vec3 &pos, int dimension, float *nodeMinHeight_, float *nodeMaxHeight_);
private:
    glm::vec3 cornerPos_;
    glm::vec3 axis_;
    int index_;
    int dimension_;
    unsigned int heightTextureId_;
    unsigned int normalTextureId_;
    std::vector<unsigned char> *heightData_ = nullptr;
    std::vector<unsigned char> *normalData_ = nullptr;
};
#endif