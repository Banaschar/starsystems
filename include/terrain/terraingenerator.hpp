#ifndef GENERATOR_H
#define GENERATOR_H

//#include <bits/stdc++.h>
#include "colorgenerator.hpp"
#include "mesh.hpp"
#include "perlinnoise.hpp"

class TerrainGenerator {
  public:
    TerrainGenerator();
    TerrainGenerator(PerlinNoise pNoise);
    TerrainGenerator(ColorGenerator colorGen, PerlinNoise pNoise);
    /*
     * startX and startZ are the lower left corner of the terrain to generate
     * dimension is the size of the terrain: Has to be a power of 2 plus 1
     * so dimension = (2^X) + 1
     * lod is the level of detail: Has to be a power of 2, smaller than dimension - 1
     * lod = 2^X < (dimension - 1)
     */
    Mesh generateTerrain(int startX, int startZ, int dimension, int lod);

    ColorGenerator &getColorGenerator();

    PerlinNoise &getPerlinNoise();

  private:
    ColorGenerator colorGen_;
    PerlinNoise pNoise_;

    float getHeightN(int x, int z, const std::vector<Vertex> &vertices, int dim, int dimLod);

    glm::vec3 calcNormal(int x, int z, const std::vector<Vertex> &vertices, int dim, int dimLod);

    /*
     * Switch out for the normal generator in primitives
     *
     */
    void generateNormalVector(std::vector<Vertex> &vertices, int dimension, int lod);

    float generateHeights(int x, int z, int lod);

    std::vector<unsigned int> generateIndexVector(int dimension, int lod);

    Mesh generateMesh(int startX, int startZ, int dimension, int lod);
};

#endif