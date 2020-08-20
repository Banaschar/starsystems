#include "terraingenerator.hpp"

#include <cstdio>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "primitives.hpp"

TerrainGenerator::TerrainGenerator() : colorGen_(ColorGenerator()), pNoise_(PerlinNoise()) {}
TerrainGenerator::TerrainGenerator(PerlinNoise pNoise) : pNoise_(pNoise), colorGen_(ColorGenerator()) {}
TerrainGenerator::TerrainGenerator(ColorGenerator colorGen, PerlinNoise pNoise)
    : colorGen_(colorGen), pNoise_(pNoise) {}

/*
 * startX and startZ are used to create seamless terrain, so different chunks fit together
 * lod is the detail level of the terrain. 
 * dimension is the size
 */
Mesh TerrainGenerator::generateTerrain(int startX, int startZ, int dimension, int lod) {
    //std::vector<float> heights = generateHeights(dimension, startX, startZ, lod, pNoise_);
    //std::vector<glm::vec4> colors = colorGen_.genColors(heights, dimension, pNoise_.getAmplitude());
    return generateMesh(startX, startZ, dimension, lod);
}

ColorGenerator &TerrainGenerator::getColorGenerator() {
    return colorGen_;
}

PerlinNoise &TerrainGenerator::getPerlinNoise() {
    return pNoise_;
}

float TerrainGenerator::getHeightN(int x, int z, const std::vector<Vertex> &vertices, int dim, int dimLod) {
    x = x < 0 ? 0 : x;
    z = z < 0 ? 0 : z;
    x = x >= dimLod ? dimLod - 1 : x;
    z = z >= dimLod ? dimLod - 1 : z;
    return vertices[z * dimLod + x].position.y;
}

glm::vec3 TerrainGenerator::calcNormal(int x, int z, const std::vector<Vertex> &vertices, int dim, int dimLod) {
    float heightL = getHeightN(x - 1, z, vertices, dim, dimLod);
    float heightR = getHeightN(x + 1, z, vertices, dim, dimLod);
    float heightD = getHeightN(x, z - 1, vertices, dim, dimLod);
    float heightU = getHeightN(x, z + 1, vertices, dim, dimLod);

    return glm::normalize(glm::vec3(heightL - heightR, 2.0f, heightD - heightU));
}

/*
 * Switch out for the normal generator in primitives
 *
 */
void TerrainGenerator::generateNormalVector(std::vector<Vertex> &vertices, int dimension, int lod) {
    int dimensionLod = ((dimension - 1) / lod) + 1;
    for (int z = 0; z < dimensionLod; z++) {
        for (int x = 0; x < dimensionLod; x++) {
            vertices[z * dimensionLod + x].normal = calcNormal(x, z, vertices, dimension, dimensionLod);
        }
    }
}

/*
 * Generate height map.
 * TODO: Fix normal vectors, if I take only abs values as heights, the lighting sucks
 *
 * TODO: Improve averages and USE them
 */
float TerrainGenerator::generateHeights(int x, int z, int lod) {
    float tmp = 0.0;
    for (int i = 0; i < lod / 2; i++) {
        tmp += pNoise_.getNoise2d(x - i, z);
        tmp += pNoise_.getNoise2d(x + i, z);

        tmp += pNoise_.getNoise2d(x, z - i);
        tmp += pNoise_.getNoise2d(x, z + i);
    }

    return tmp / lod;
}

std::vector<unsigned int> TerrainGenerator::generateIndexVector(int dimension, int lod) {
    int dimensionLod = ((dimension - 1) / lod) + 1;
    std::vector<unsigned int> indices((dimensionLod - 1) * (dimensionLod - 1) * 6);
    int cnt = 0;
    for (int row = 0; row < dimensionLod - 1; row++) {
        for (int col = 0; col < dimensionLod - 1; col++) {
            indices[cnt++] = dimensionLod * row + col;
            indices[cnt++] = dimensionLod * row + col + dimensionLod;
            indices[cnt++] = dimensionLod * row + col + dimensionLod + 1;

            indices[cnt++] = dimensionLod * row + col;
            indices[cnt++] = dimensionLod * row + col + dimensionLod + 1;
            indices[cnt++] = dimensionLod * row + col + 1;
        }
    }
    
    return indices;
}

Mesh TerrainGenerator::generateMesh(int startX, int startZ, int dimension, int lod) {
    int dimensionLod = ((dimension - 1) / lod) + 1;
    std::vector<Vertex> vertices(dimensionLod * dimensionLod);
    int half = (dimension - 1) / 2;
    int index = 0, row = 0, col = 0;
    for (int z = startZ; z < dimension + startZ; z+=lod) {
        for (int x = startX; x < dimension + startX; x+=lod) {
            Vertex vertex;
            vertex.position.x = x - half;
            //vertex.position.y = lod == 16 ? generateHeights(x - half, z - half, lod) : pNoise_.getNoise2d(x - half, z - half);
            vertex.position.y = pNoise_.getNoise2d(x - half, z - half);
            vertex.position.z = z - half;
            vertex.textureCoords.x = (float)col / ((float)dimensionLod - 1);
            vertex.textureCoords.y = (float)row / ((float)dimensionLod - 1);
            //vertex.color = colors[z * dimension + x];
            vertices[index] = vertex;
            index++;
            col++;
        }
        col = 0;
        row++;
    }

    generateNormalVector(vertices, dimension, lod);

    return Mesh(vertices, generateIndexVector(dimension, lod));
}