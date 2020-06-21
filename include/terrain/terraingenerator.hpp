#ifndef GENERATOR_H
#define GENERATOR_H

//#include <bits/stdc++.h>
#include "mesh.hpp"
#include "perlinnoise.hpp"
#include "colorgenerator.hpp"

class TerrainGenerator {
public:
    TerrainGenerator();
    TerrainGenerator(PerlinNoise pNoise);
    TerrainGenerator(ColorGenerator colorGen, PerlinNoise pNoise);

    Mesh generateTerrain(int dimension);

    ColorGenerator& getColorGenerator();

    PerlinNoise& getPerlinNoise();
private:
    ColorGenerator colorGen_;
    PerlinNoise pNoise_;

    float getHeightN(int x, int z, std::vector<float> &heights, int dim);

    glm::vec3 calcNormal(int x, int z, std::vector<float> &heights, int dim);

    /*
     * Switch out for the normal generator in primitives
     *
     */
    std::vector<glm::vec3> generateNormalVector(std::vector<float> &heights, int dimension);

    std::vector<float> generateHeights(int dimension, PerlinNoise pNoise);

    std::vector<unsigned int> generateIndexVector(int dimension);

    Mesh generateMesh(std::vector<float> heights, std::vector<glm::vec4> colors, int dimension);
};

#endif