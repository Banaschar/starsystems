#include "terraintile.hpp"
#include "perlinnoise.hpp"

TerrainTile::TerrainTile(Mesh *mesh) {
    Drawable::addMesh(mesh);
}

TerrainTile::TerrainTile(TerrainGenerator *terrainGen, int dimension, glm::vec3 position, int lod, glm::vec3 axis, GenerationType genType, ShaderType shaderType) {
    genAttr_.dimension = dimension;
    genAttr_.position = position;
    genAttr_.lod = lod;
    genAttr_.axis = axis;
    genAttr_.genType = genType;
    Drawable::setType(shaderType);
    initTerrain(terrainGen);
}

TerrainTile::TerrainTile(TerrainGenerator *terrainGen, int dimension, glm::vec3 position, int lod, GenerationType genType, ShaderType shaderType) {
    genAttr_.dimension = dimension;
    genAttr_.position = position;
    genAttr_.lod = lod;
    genAttr_.genType = genType;
    Drawable::setType(shaderType);
    initTerrain(terrainGen);
}

void TerrainTile::update(Game *game) {
    ;
}

float TerrainTile::getAmplitude() {
    return amplitude_;
}

int TerrainTile::getDimension() {
    return genAttr_.dimension;
}

glm::vec3 &TerrainTile::getPosition() {
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

    //Drawable::setType(SHADER_TYPE_WATER);
    //Drawable::addColor(glm::vec4(0, 0, 1, 0.6));
}