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
    setCorners(axis);
}

TerrainTile::TerrainTile(TerrainGenerator *terrainGen, int dimension, glm::vec3 position, int lod, GenerationType genType, ShaderType shaderType) {
    genAttr_.dimension = dimension;
    genAttr_.position = position;
    genAttr_.lod = lod;
    genAttr_.genType = genType;
    Drawable::setType(shaderType);
    setCorners(glm::vec3(0,1,0));
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

std::array<glm::vec3, 4> &TerrainTile::getCorners() {
    return corners_;
}

void TerrainTile::setCorners(glm::vec3 axis) {
    int half = (genAttr_.dimension - 1) / 2;
    int index = 0;
    glm::vec3 pos = genAttr_.position;
    for (int y = -1; y < 2; y+=2) {
        for (int x = -1; x < 2; x+=2) {
            glm::vec3 c;
            if (axis.x)
                c = glm::vec3(pos.x, pos.y + y * half, pos.z + x * half);
            else if (axis.y)
                c = glm::vec3(pos.x + x * half, pos.y, pos.z + y * half);
            else
                c = glm::vec3(pos.x + x * half, pos.y + y * half, pos.z);

            if (sphereRadius_)
                corners_[index++] = sphereOrigin_ + (float) sphereRadius_ * glm::normalize(c - sphereOrigin_);
            else
                corners_[index++] = c;
        }
    }
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