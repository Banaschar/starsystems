#ifndef GENERATOR_H
#define GENERATOR_H

//#include <bits/stdc++.h>
#include "colorgenerator.hpp"
#include "mesh.hpp"
#include "perlinnoise.hpp"

enum class GenerationType {PLANE_FLAT, PLANE, SPHERE_FLAT, SPHERE};

struct GenerationAttributes {
    glm::vec3 position;
    glm::vec3 axis;
    int dimension;
    int lod;
    GenerationType genType;
};

class TerrainGenerator {
  public:
    TerrainGenerator();
    TerrainGenerator(PerlinNoise pNoise);
    TerrainGenerator(ColorGenerator colorGen, PerlinNoise pNoise);
    /*
     * startX and startZ are the center of the terrain to generate
     * dimension is the size of the terrain: Has to be a power of 2 plus 1
     * so dimension = (2^X) + 1
     * lod is the level of detail: Has to be a power of 2, smaller than dimension - 1
     * lod = 2^X < (dimension - 1)
     */
    Mesh *generateTerrain(GenerationAttributes *attr);

    ColorGenerator &getColorGenerator();

    PerlinNoise &getPerlinNoise();

    int getSphereRadius();
    void setSphereRadius(int radius);
    glm::vec3 getSphereOrigin();
    void setSphereOrigin(glm::vec3 origin);

  private:
    ColorGenerator colorGen_;
    PerlinNoise pNoise_;
    int sphereRadius_ = 0;
    glm::vec3 sphereOrigin_ = glm::vec3(0,0,0);

    float getHeightN(int x, int z, const std::vector<Vertex> &vertices, int dimLod);
    glm::vec3 getSpherePos(glm::vec3 &axis, int radius, int x, int z);

    glm::vec3 calcNormal(int x, int z, const std::vector<Vertex> &vertices, int dimLod);

    void calculateVertexNormalSphere(std::vector<Vertex> &vertices,
                                                                 std::vector<unsigned int> &indices,
                                                                 std::vector<glm::vec3> &borderMap, int dimensionLod);
    void addBorderNormals(std::vector<Vertex> &vertices, std::vector<glm::vec3> &borderMap, int dimensionLod);
    glm::vec3 calculateNormal(glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2);

    /*
     * Switch out for the normal generator in primitives
     *
     */
    void generateNormalVector(std::vector<Vertex> &vertices, int dimension, int lod);

    float generateHeights(int x, int z, int lod);

    std::vector<unsigned int> generateIndexVector(std::vector<unsigned int> &indices, int dimensionLod, bool inverted = false);

    Mesh *generateMesh(glm::vec3 &pos, int dimension, int lod, bool flat);
    Mesh *generateMeshNew(glm::vec3 &pos, int dimension, int lod, bool flat);
    Mesh *generateMeshSphere(glm::vec3 &pos, int dimension, int lod, glm::vec3 &axis, bool flat);
};

#endif