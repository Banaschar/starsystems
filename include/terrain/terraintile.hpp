#ifndef TERRAINTILE_H
#define TERRAINTILE_H

#include "drawable.hpp"
#include "global.hpp"
#include "terraingenerator.hpp"

class TerrainTile : public Drawable {
public:
    TerrainTile(Mesh *mesh);
    TerrainTile(TerrainGenerator *terrainGen, GenerationAttributes attributes, ShaderType shaderType);
    TerrainTile(TerrainGenerator *terrainGen, int dimension, glm::vec2 position, int lod, glm::vec3 axis, GenerationType genType, ShaderType shaderType);
    TerrainTile(TerrainGenerator *terrainGen, int dimension, glm::vec2 position, int lod, GenerationType genType, ShaderType shaderType);

    float getAmplitude();
    int getDimension();
    glm::vec2 &getPosition();
    int getLod();
    int getSphereRadius();
    glm::vec3 &getSphereOrigin();
    glm::vec3 &getAxis();

  private:
    GenerationAttributes genAttr_;
    float amplitude_;
    int sphereRadius_;
    glm::vec3 sphereOrigin_;
    void initTerrain(TerrainGenerator *terrainGen);
};
#endif