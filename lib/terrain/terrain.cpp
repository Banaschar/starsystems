#include "terrain.hpp"
#include "perlinnoise.hpp"

Terrain::Terrain(int dimension) : dimension_(dimension) {
    PerlinNoise pNoise = PerlinNoise(4, 10.0f, 0.01f);
    TerrainGenerator terrainGen = TerrainGenerator(pNoise);

    initTerrain(terrainGen);
}

Terrain::Terrain(TerrainGenerator &terrainGen, int dimension) : dimension_(dimension) {
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

void Terrain::initTerrain(TerrainGenerator &terrainGen) {
    Drawable::addMesh(terrainGen.generateTerrain(dimension_));
    Drawable::setType(SHADER_TYPE_TERRAIN);
    amplitude_ = terrainGen.getPerlinNoise().getAmplitude();
}