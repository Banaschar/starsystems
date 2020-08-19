#include "terrain.hpp"
#include "perlinnoise.hpp"

Terrain::Terrain(TerrainGenerator *terrainGen, int dimension, int posX, int posZ, int lod) : dimension_(dimension), position_(glm::vec3(posX, 0, posZ)), lod_(lod) {
    initTerrain(terrainGen);
}

void Terrain::update(Game *game) {
    ;
}

float Terrain::getAmplitude() {
    return amplitude_;
}

int Terrain::getDimension() {
    return dimension_;
}

glm::vec3 &Terrain::getPosition() {
    return position_;
}

int Terrain::getLod() {
    return lod_;
}
/*
 * Create terrain of size dimension
 * and translate to specified position
 */
void Terrain::initTerrain(TerrainGenerator *terrainGen) {
    Drawable::addMesh(terrainGen->generateTerrain(position_.x, position_.z, dimension_, lod_));
    Drawable::setType(SHADER_TYPE_TERRAIN);
    amplitude_ = terrainGen->getPerlinNoise().getAmplitude();
    //glm::vec3 trans = glm::vec3(posX_, 0, posZ_);
    //Drawable::transform(NULL, &trans, NULL);
}