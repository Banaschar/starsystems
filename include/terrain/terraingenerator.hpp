#ifndef GENERATOR_H
#define GENERATOR_H

//#include <bits/stdc++.h>
#include "colorgenerator.hpp"
#include "mesh.hpp"
#include "perlinnoise.hpp"

class TerrainMeshData;
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
     */
    Mesh *generateTerrain(GenerationAttributes *attr);
    Mesh *generateTerrainMesh(GenerationAttributes *attr, bool isFlat);
    void generateTerrainHeightMap(glm::vec3 &origin, glm::vec3 &axis, int dimension, std::vector<unsigned char> *heightValues, std::vector<unsigned char> *normalValues);
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

    void generateTerrainData(TerrainMeshData &meshData, glm::vec3 &pos, int dimension, int lod, glm::vec3 &axis, bool flat);
    glm::vec3 getVertexPosition(glm::vec3 vertPos, float height);
    glm::vec3 getAxisPos(glm::vec3 &axis, int x, int y);
    float getHeightValue(glm::vec3 &pos);
};
#endif