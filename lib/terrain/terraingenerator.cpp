#include "terraingenerator.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "primitives.hpp"

TerrainGenerator::TerrainGenerator() : colorGen_(ColorGenerator()), pNoise_(PerlinNoise()) {}
TerrainGenerator::TerrainGenerator(PerlinNoise pNoise) : pNoise_(pNoise), colorGen_(ColorGenerator()) {}
TerrainGenerator::TerrainGenerator(ColorGenerator colorGen, PerlinNoise pNoise)
    : colorGen_(colorGen), pNoise_(pNoise) {}

Mesh TerrainGenerator::generateTerrain(int dimension) {
    std::vector<float> heights = generateHeights(dimension, pNoise_);
    std::vector<glm::vec4> colors = colorGen_.genColors(heights, dimension, pNoise_.getAmplitude());
    return generateMesh(heights, colors, dimension);
}

ColorGenerator &TerrainGenerator::getColorGenerator() {
    return colorGen_;
}

PerlinNoise &TerrainGenerator::getPerlinNoise() {
    return pNoise_;
}

float TerrainGenerator::getHeightN(int x, int z, std::vector<float> &heights, int dim) {
    x = x < 0 ? 0 : x;
    z = z < 0 ? 0 : z;
    x = x >= dim ? dim - 1 : x;
    z = z >= dim ? dim - 1 : z;
    return heights[z * dim + x];
}

glm::vec3 TerrainGenerator::calcNormal(int x, int z, std::vector<float> &heights, int dim) {
    float heightL = getHeightN(x - 1, z, heights, dim);
    float heightR = getHeightN(x + 1, z, heights, dim);
    float heightD = getHeightN(x, z - 1, heights, dim);
    float heightU = getHeightN(x, z + 1, heights, dim);

    return glm::normalize(glm::vec3(heightL - heightR, 2.0f, heightD - heightU));
}

/*
 * Switch out for the normal generator in primitives
 *
 */
std::vector<glm::vec3> TerrainGenerator::generateNormalVector(std::vector<float> &heights, int dimension) {
    std::vector<glm::vec3> normals(dimension * dimension);
    for (int z = 0; z < dimension; z++) {
        for (int x = 0; x < dimension; x++) {
            normals[z * dimension + x] = calcNormal(x, z, heights, dimension);
        }
    }
    return normals;
}

std::vector<float> TerrainGenerator::generateHeights(int dimension, PerlinNoise pNoise) {
    std::vector<float> heights(dimension * dimension);
    for (int z = 0; z < dimension; z++) {
        for (int x = 0; x < dimension; x++) {
            heights[z * dimension + x] = pNoise.getNoise2d(x, z);
        }
    }
    return heights;
}

std::vector<unsigned int> TerrainGenerator::generateIndexVector(int dimension) {
    std::vector<unsigned int> indices((dimension - 1) * (dimension - 1) * 6);
    int cnt = 0;
    for (int row = 0; row < dimension - 1; row++) {
        for (int col = 0; col < dimension - 1; col++) {
            indices[cnt++] = dimension * row + col;
            indices[cnt++] = dimension * row + col + dimension;
            indices[cnt++] = dimension * row + col + dimension + 1;

            indices[cnt++] = dimension * row + col;
            indices[cnt++] = dimension * row + col + dimension + 1;
            indices[cnt++] = dimension * row + col + 1;
        }
    }

    return indices;
}

Mesh TerrainGenerator::generateMesh(std::vector<float> heights, std::vector<glm::vec4> colors, int dimension) {
    std::vector<Vertex> vertices(dimension * dimension);
    std::vector<glm::vec3> normals = generateNormalVector(heights, dimension);
    int half = dimension / 2;
    for (int z = 0; z < dimension; z++) {
        for (int x = 0; x < dimension; x++) {
            Vertex vertex;
            vertex.position.x = x - half;
            vertex.position.y = heights[z * dimension + x];
            vertex.position.z = z - half;
            vertex.normal = normals[z * dimension + x];
            vertex.textureCoords.x = (float)x / ((float)dimension - 1);
            vertex.textureCoords.y = (float)z / ((float)dimension - 1);
            vertex.color = colors[z * dimension + x];
            vertices[z * dimension + x] = vertex;
        }
    }
    return Mesh(vertices, generateIndexVector(dimension));
}