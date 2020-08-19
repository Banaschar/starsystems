#ifndef TERRAIN_H
#define TERRAIN_H

#include "drawable.hpp"
#include "global.hpp"
#include "terraingenerator.hpp"

class Terrain : public Drawable {
  public:
    Terrain(TerrainGenerator *terrainGen, int dimension, int posX, int posZ, int lod);

    void update(Game *game) override;

    float getAmplitude();

    int getDimension();

    glm::vec3 &getPosition();

    int getLod();

  private:
    int dimension_;
    int lod_;
    glm::vec3 position_;
    float amplitude_;
    void initTerrain(TerrainGenerator *terrainGen);
};
#endif