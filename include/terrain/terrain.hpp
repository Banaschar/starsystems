#ifndef TERRAIN_H
#define TERRAIN_H

#include "drawable.hpp"
#include "global.hpp"
#include "terraingenerator.hpp"

class Terrain : public Drawable {
  public:
    Terrain(TerrainGenerator *terrainGen, int dimension, glm::vec3 position, int lod, glm::vec3 axis = glm::vec3(0,1,0));
    Terrain(TerrainGenerator *terrainGen, int dimension, int startX, int startZ, int lod, glm::vec3 axis = glm::vec3(0,1,0));

    void update(Game *game) override;
    float getAmplitude();
    int getDimension();
    glm::vec3 &getPosition();
    int getLod();
    int getSphereRadius();
    glm::vec3 &getSphereOrigin();
    glm::vec3 &getAxis();

  private:
    int dimension_;
    int lod_;
    glm::vec3 axis_;
    glm::vec3 position_;
    float amplitude_;
    int sphereRadius_;
    glm::vec3 sphereOrigin_;
    void initTerrain(TerrainGenerator *terrainGen, bool sphere3d);
};

class WaterTile : public Drawable {
public:
    WaterTile(TerrainGenerator *terrainGen, int dimension, glm::vec3 position, int lod, glm::vec3 axis, int waterLevel_ = 0);

    glm::vec3 &getPosition();
    int getDimension();

private:
    glm::vec3 position_;
    int dimension_;
    int lod_;
    int waterLevel_;
    void initWaterTile(TerrainGenerator *terrainGen, glm::vec3 axis);
};
#endif