#include "terraintile.hpp"
#include "perlinnoise.hpp"

TerrainTile::TerrainTile(Mesh *mesh) {
    Drawable::addMesh(mesh);
}

TerrainTile::TerrainTile(TerrainGenerator *terrainGen, int dimension, glm::vec2 position, int lod, glm::vec3 axis, GenerationType genType, ShaderType shaderType) {
    genAttr_.dimension = dimension;
    genAttr_.position = position;
    genAttr_.lod = lod;
    genAttr_.axis = axis;
    genAttr_.genType = genType;
    Drawable::setType(shaderType);
    initTerrain(terrainGen);
}

TerrainTile::TerrainTile(TerrainGenerator *terrainGen, int dimension, glm::vec2 position, int lod, GenerationType genType, ShaderType shaderType) {
    genAttr_.dimension = dimension;
    genAttr_.position = position;
    genAttr_.lod = lod;
    genAttr_.genType = genType;
    genAttr_.axis = glm::vec3(0, 1, 0);
    Drawable::setType(shaderType);
    initTerrain(terrainGen);
}

TerrainTile::TerrainTile(TerrainGenerator *terrainGen, GenerationAttributes attributes, ShaderType shaderType) : genAttr_(attributes) {
    Drawable::setType(shaderType);
    initTerrain(terrainGen);
}

float TerrainTile::getAmplitude() {
    return amplitude_;
}

int TerrainTile::getDimension() {
    return genAttr_.dimension;
}

glm::vec2 &TerrainTile::getPosition() {
    return genAttr_.position;
}

int TerrainTile::getLod() {
    return genAttr_.lod;
}

int TerrainTile::getSphereRadius() {
    return sphereRadius_;
}

glm::vec3 &TerrainTile::getSphereOrigin() {
    return sphereOrigin_;
}

glm::vec3 &TerrainTile::getAxis() {
    return genAttr_.axis;
}

/*
 * Create terrain of size dimension
 * and translate to specified position
 */
void TerrainTile::initTerrain(TerrainGenerator *terrainGen) {
    sphereRadius_ = terrainGen->getSphereRadius();
    sphereOrigin_ = terrainGen->getSphereOrigin();
    amplitude_ = terrainGen->getPerlinNoise().getAmplitude();

    Drawable::addMesh(terrainGen->generateTerrain(&genAttr_));
}

void TerrainTile::update(View *view) {
    ;
}