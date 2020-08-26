#include "terrain.hpp"
#include "perlinnoise.hpp"

Terrain::Terrain(TerrainGenerator *terrainGen, int dimension, glm::vec3 position, int lod, glm::vec3 axis) : dimension_(dimension), position_(position), lod_(lod), axis_(axis) {
    initTerrain(terrainGen);
}

Terrain::Terrain(TerrainGenerator *terrainGen, int dimension, int startX, int startZ, int lod, glm::vec3 axis) : dimension_(dimension), position_(glm::vec3(startX, 0, startZ)), lod_(lod), axis_(axis) {
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

int Terrain::getSphereRadius() {
    return sphereRadius_;
}

glm::vec3 &Terrain::getSphereOrigin() {
    return sphereOrigin_;
}
/*
 * Create terrain of size dimension
 * and translate to specified position
 */
void Terrain::initTerrain(TerrainGenerator *terrainGen) {
    sphereRadius_ = terrainGen->getSphereRadius();
    sphereOrigin_ = terrainGen->getSphereOrigin();

    if (sphereRadius_)
        Drawable::addMesh(terrainGen->generateTerrain(position_, dimension_, sphereRadius_, lod_, axis_));
    else
        Drawable::addMesh(terrainGen->generateTerrain(position_.x, position_.z, dimension_, lod_));
    
    Drawable::setType(SHADER_TYPE_TERRAIN);
    amplitude_ = terrainGen->getPerlinNoise().getAmplitude();
}