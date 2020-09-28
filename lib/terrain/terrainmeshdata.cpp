#include "terrainmeshdata.hpp"

TerrainMeshData::TerrainMeshData(int numVertsPerLine, int skipIncrement) {
    initMeshData(numVertsPerLine, skipIncrement);
}

TerrainMeshData::TerrainMeshData(int numVertsPerLine, int skipIncrement, PerlinNoise &noiseGen, std::vector<unsigned char> *heightValues, std::vector<unsigned char> *normalValues) {
    initMeshData(numVertsPerLine, 1);
    heightMapHeightValues = heightValues;
    heightMapNormalValues = normalValues;
    heightLowerBound = noiseGen.getLowerBound();
    heightUpperBound = noiseGen.getUpperBound();
    calcHeightMap_ = true;;
}

void TerrainMeshData::initMeshData(int numVertsPerLine, int skipIncrement) {
    int numMeshEdgeVertices = (numVertsPerLine - 2) * 4 - 4;
    int numMainVerticesPerLine = ((numVertsPerLine - 3) / skipIncrement + 1) - 2;
    int numMainVertices = numMainVerticesPerLine * numMainVerticesPerLine;

    vertices.resize(numMainVertices + numMeshEdgeVertices);

    int numMeshEdgeTriangles;
    if (skipIncrement != 1) {
        int numQuadsPerLine = (numVertsPerLine - 3) / skipIncrement;
        numMeshEdgeTriangles = (numQuadsPerLine * 4 - 4 - 4) * (skipIncrement + 1) + 4 * 2 * skipIncrement;
    } else
        numMeshEdgeTriangles = 8 * (numVertsPerLine - 4);
    int numMainTriangles = (numMainVerticesPerLine - 1) * (numMainVerticesPerLine - 1) * 2;
    indices.resize((numMeshEdgeTriangles + numMainTriangles) * 3);

    outOfMeshVertices.resize(numVertsPerLine * 4 - 4);
    outOfMeshTriangles.resize(3 * (4 * (2 * (numVertsPerLine - 1)) - 8));
}

void TerrainMeshData::addHeight(float height, int vertexIndex) {
    if (calcHeightMap_ && vertexIndex >= 0)
        (*heightMapHeightValues)[vertexIndex] = mapRange(height, heightLowerBound, heightUpperBound);
}

void TerrainMeshData::addVertex(glm::vec3 pos, glm::vec2 uv, int vertexIndex) {
    if (vertexIndex < 0)
        outOfMeshVertices[-vertexIndex - 1] = pos; // Because index starts with -1 for outOfMesh vertices
    else {
        vertices[vertexIndex].position = pos;
        vertices[vertexIndex].textureCoords = uv;
    }
}

void TerrainMeshData::addTriangle(int a, int b, int c) {
    if (a < 0 || b < 0 || c < 0) {
        outOfMeshTriangles[outOfMeshTriangleIndex] = a;
        outOfMeshTriangles[outOfMeshTriangleIndex + 1] = b;
        outOfMeshTriangles[outOfMeshTriangleIndex + 2] = c;
        outOfMeshTriangleIndex += 3;
    } else {
        indices[triangleIndex] = a;
        indices[triangleIndex + 1] = b;
        indices[triangleIndex + 2] = c;
        triangleIndex += 3;
    }
}

glm::vec3 TerrainMeshData::calculateNormalFromIndices(int index0, int index1, int index2) {
    glm::vec3 v0 = (index0 < 0) ? outOfMeshVertices[-index0 - 1] : vertices[index0].position;
    glm::vec3 v1 = (index1 < 0) ? outOfMeshVertices[-index1 - 1] : vertices[index1].position;
    glm::vec3 v2 = (index2 < 0) ? outOfMeshVertices[-index2 - 1] : vertices[index2].position;

    glm::vec3 p0 = v1 - v0;
    glm::vec3 p1 = v2 - v0;

    return glm::normalize(glm::cross(p0, p1));
}

void TerrainMeshData::calculateNormals(bool createHeightMap) {
    int triangleCount = indices.size() / 3;

    for (int i = 0; i < triangleCount; ++i) {
        int normalIndex = i * 3;
        int index0 = indices[normalIndex];
        int index1 = indices[normalIndex + 1];
        int index2 = indices[normalIndex + 2];

        glm::vec3 normal = calculateNormalFromIndices(index0, index1, index2);

        vertices[index0].normal += normal;
        vertices[index1].normal += normal;
        vertices[index2].normal += normal;
    }

    int borderTriangleCount = outOfMeshTriangles.size() / 3;
    for (int i = 0; i < borderTriangleCount; ++i) {
        int normalIndex = i * 3;
        int index0 = outOfMeshTriangles[normalIndex];
        int index1 = outOfMeshTriangles[normalIndex + 1];
        int index2 = outOfMeshTriangles[normalIndex + 2];

        glm::vec3 normal = calculateNormalFromIndices(index0, index1, index2);

        if (index0 >= 0)
            vertices[index0].normal += normal;
        if (index1 >= 0)
            vertices[index1].normal += normal;
        if (index2 >= 0)
            vertices[index2].normal += normal;
    }

    for (int i = 0; i < vertices.size(); ++i) {
        vertices[i].normal = glm::normalize(vertices[i].normal);
    }

    if (createHeightMap) {
        float lower = -1.0f;
        float upper = 1.0f;
        for (int i = 0; i < vertices.size(); ++i) {
            (*heightMapNormalValues)[i * 3] = mapRange(vertices[i].normal.x, lower, upper);
            (*heightMapNormalValues)[i * 3 + 1] = mapRange(vertices[i].normal.y, lower, upper);
            (*heightMapNormalValues)[i * 3 + 2] = mapRange(vertices[i].normal.z, lower, upper);
        }
    }
}

unsigned char TerrainMeshData::mapRange(float inVal, float lowerBound, float upperBound) {
    float in = clamp(inVal, lowerBound, upperBound);
    float s = 1.0 * (255 - 0) / (upperBound - lowerBound);
    return 0 + std::floor((s * (inVal - lowerBound)) + 0.5);
}

float TerrainMeshData::clamp(float i, float l, float u) {
    if (i < l)
        return l;
    else if (i > u)
        return u;
    else
        return i;
}