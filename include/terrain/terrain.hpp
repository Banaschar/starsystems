#ifndef TERRAIN_H
#define TERRAIN_H

#include "global.hpp"
#include "drawable.hpp"
#include "terraingenerator.hpp"

const int DEFAULT_DIMENSION = 200;

class Terrain : public Drawable {
public:
    Terrain(int dimension = DEFAULT_DIMENSION);
    Terrain(TerrainGenerator &terrainGen, int dimension = DEFAULT_DIMENSION);

    void update(Game *game) override;

    float getAmplitude();

    int getDimension();
private:
    int dimension_;
    float amplitude_;
    void initTerrain(TerrainGenerator &terrainGen);
};
#endif