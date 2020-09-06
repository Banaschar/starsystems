#ifndef TERRAINTILE_H
#define TERRAINTILE_H

#include "drawable.hpp"
#include "global.hpp"
#include "terraingenerator.hpp"

class TerrainTile : public Drawable {
  public:
    TerrainTile(Mesh *mesh);
    TerrainTile(TerrainGenerator *terrainGen, int dimension, glm::vec3 position, int lod, glm::vec3 axis, GenerationType genType, ShaderType shaderType);
    TerrainTile(TerrainGenerator *terrainGen, int dimension, glm::vec3 position, int lod, GenerationType genType, ShaderType shaderType);

    void update(Game *game) override;
    float getAmplitude();
    int getDimension();
    glm::vec3 &getPosition();
    int getLod();
    int getSphereRadius();
    glm::vec3 &getSphereOrigin();
    glm::vec3 &getAxis();
    std::array<glm::vec3, 4> &getCorners();

  private:
    GenerationAttributes genAttr_;
    float amplitude_;
    int sphereRadius_;
    glm::vec3 sphereOrigin_;
    std::array<glm::vec3, 4> corners_;
    void setCorners(glm::vec3 axis);
    void initTerrain(TerrainGenerator *terrainGen);
};
#endif