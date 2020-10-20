#ifndef GENERATOR_H
#define GENERATOR_H

//#include <bits/stdc++.h>
#include "colorgenerator.hpp"
#include "mesh.hpp"
#include "perlinnoise.hpp"

class TerrainMeshData;
enum class GenerationType {PLANE_FLAT, PLANE, SPHERE_FLAT, SPHERE};

struct GenerationAttributes {
    glm::vec2 position;
    glm::vec3 axis;
    int dimension;
    int lod;
    GenerationType genType;
    bool isFlat = false;
    VertexType vertexType = VertexType::VERTEX_DEFAULT;
    std::vector<unsigned char> *heightData = nullptr;
    std::vector<unsigned char> *normalData = nullptr;
};

class TerrainGenerator {
  public:
    TerrainGenerator();
    TerrainGenerator(PerlinNoise pNoise);
    TerrainGenerator(ColorGenerator colorGen, PerlinNoise pNoise);
    /*
     */
    Mesh *generateTerrain(GenerationAttributes *attr);
    Mesh *generateTerrainMesh(GenerationAttributes *attr);
    void generateTerrainHeightMap(GenerationAttributes *attr);
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

    void generateTerrainHeightMapData(TerrainMeshData &meshData, glm::vec2 &pos, int numVertsPerLine, int skipIncrement, glm::vec3 &axis, bool isFlat);
    void generateTerrainMeshData(TerrainMeshData &meshData, glm::vec2 &pos, int dimension, int lod, glm::vec3 &axis, bool flat);
    glm::vec3 getVertexPosition(glm::vec3 vertPos, float height);
    glm::vec3 getAxisPos(glm::vec3 &axis, int x, int y);
    float getHeightValue(glm::vec3 &pos);
};
#endif