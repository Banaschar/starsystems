#ifndef TERRAINMESHDATA_HPP
#define TERRAINMESHDATA_HPP

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>
#include "perlinnoise.hpp"
#include "enginetypes.hpp"

class TerrainMeshData {
public:
    TerrainMeshData(int numVertsPerLine, int skipIncrement, VertexType type);
    TerrainMeshData(int numVertsPerLine, int skipIncrement, PerlinNoise &noiseGen, 
        std::vector<unsigned char> *heightValues, std::vector<unsigned char> *normalValues, VertexType type);
    
    void addHeight(float height, int vertexIndex);
    void addVertex(glm::vec3 pos, glm::vec2 uv, int vertexIndex);
    void addTriangle(int a, int b, int c);
    void calculateNormals(bool createHeightMap);
    /* 
     * Needs to be called if no mesh is created from the TerrainMeshData
     * TODO: Use unique Pointer for vertex Data to avoid this
     */
    void destroy();

    VertexData *vertexData;

private:
    std::vector<unsigned char> *heightMapHeightValues;
    std::vector<unsigned char> *heightMapNormalValues;
    bool calcHeightMap_ = false;
    float heightLowerBound;
    float heightUpperBound;

    std::vector<glm::vec3> outOfMeshVertices;
    std::vector<int> outOfMeshTriangles;
    int outOfMeshTriangleIndex = 0;
    int triangleIndex = 0;

    void initMeshData(int numVertsPerLine, int skipIncrement, VertexType type);
    glm::vec3 calculateNormalFromIndices(int index0, int index1, int index2);
    unsigned char mapRangeToUnsignedByte(float inVal, float lowerBound, float upperBound);
    float clamp(float i, float l, float u);
};
#endif