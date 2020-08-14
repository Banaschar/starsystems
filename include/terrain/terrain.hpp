#ifndef TERRAIN_H
#define TERRAIN_H

#include "drawable.hpp"
#include "global.hpp"
#include "terraingenerator.hpp"

class Terrain : public Drawable {
  public:
    Terrain(TerrainGenerator *terrainGen, int dimension, int posX, int posZ);

    void update(Game *game) override;

    float getAmplitude();

    int getDimension();

  private:
    int dimension_;
    int posX_, posZ_;
    float amplitude_;
    void initTerrain(TerrainGenerator *terrainGen);
};
#endif