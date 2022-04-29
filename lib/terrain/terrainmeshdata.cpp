#include "terrainmeshdata.hpp"

TerrainMeshData::TerrainMeshData(int numVertsPerLine, int skipIncrement, VertexType type) {
    initMeshData(numVertsPerLine, skipIncrement, type);
}

TerrainMeshData::TerrainMeshData(int numVertsPerLine, int skipIncrement, PerlinNoise &noiseGen, 
                                std::vector<unsigned char> *heightValues, std::vector<unsigned char> *normalValues, VertexType type) {
    initMeshData(numVertsPerLine, 1, type);
    heightMapHeightValues = heightValues;
    heightMapNormalValues = normalValues;
    heightLowerBound = noiseGen.getLowerBound();
    heightUpperBound = noiseGen.getUpperBound();
    calcHeightMap_ = true;
}

void TerrainMeshData::initMeshData(int numVertsPerLine, int skipIncrement, VertexType type) {
    int numMeshEdgeVertices = (numVertsPerLine - 2) * 4 - 4;
    int numMainVerticesPerLine = ((numVertsPerLine - 3) / skipIncrement + 1) - 2;
    int numMainVertices = numMainVerticesPerLine * numMainVerticesPerLine;

    int numMeshEdgeTriangles;
    if (skipIncrement != 1) {
        int numQuadsPerLine = (numVertsPerLine - 3) / skipIncrement;
        numMeshEdgeTriangles = (numQuadsPerLine * 4 - 4 - 4) * (skipIncrement + 1) + 4 * 2 * skipIncrement;
    } else
        numMeshEdgeTriangles = 8 * (numVertsPerLine - 4);
    int numMainTriangles = (numMainVerticesPerLine - 1) * (numMainVerticesPerLine - 1) * 2;

    vertexData = new VertexData(numMainVertices + numMeshEdgeVertices, (numMeshEdgeTriangles + numMainTriangles) * 3, type);

    outOfMeshVertices.resize(numVertsPerLine * 4 - 4);
    outOfMeshTriangles.resize(3 * (4 * (2 * (numVertsPerLine - 1)) - 8));
}

void TerrainMeshData::addHeight(float height, int vertexIndex) {
    if (calcHeightMap_ && vertexIndex >= 0)
        (*heightMapHeightValues)[vertexIndex] = mapRangeToUnsignedByte(height, heightLowerBound, heightUpperBound);
}

void TerrainMeshData::addVertex(glm::vec3 pos, glm::vec2 uv, int vertexIndex) {
    if (vertexIndex < 0)
        outOfMeshVertices[-vertexIndex - 1] = pos; // Because index starts with -1 for outOfMesh vertices
    else {
        vertexData->vertices[vertexIndex].position = pos;
        vertexData->vertices[vertexIndex].textureCoords = uv;
    }
}

void TerrainMeshData::addTriangle(int a, int b, int c) {
    if (a < 0 || b < 0 || c < 0) {
        outOfMeshTriangles[outOfMeshTriangleIndex] = a;
        outOfMeshTriangles[outOfMeshTriangleIndex + 1] = b;
        outOfMeshTriangles[outOfMeshTriangleIndex + 2] = c;
        outOfMeshTriangleIndex += 3;
    } else {
        vertexData->indices[triangleIndex] = a;
        vertexData->indices[triangleIndex + 1] = b;
        vertexData->indices[triangleIndex + 2] = c;
        triangleIndex += 3;
    }
}

glm::vec3 TerrainMeshData::calculateNormalFromIndices(int index0, int index1, int index2) {
    glm::vec3 v0 = (index0 < 0) ? outOfMeshVertices[-index0 - 1] : vertexData->vertices[index0].position;
    glm::vec3 v1 = (index1 < 0) ? outOfMeshVertices[-index1 - 1] : vertexData->vertices[index1].position;
    glm::vec3 v2 = (index2 < 0) ? outOfMeshVertices[-index2 - 1] : vertexData->vertices[index2].position;

    glm::vec3 p0 = v1 - v0;
    glm::vec3 p1 = v2 - v0;

    return glm::normalize(glm::cross(p0, p1));
}

void TerrainMeshData::calculateNormals(bool createHeightMap) {
    unsigned int triangleCount = vertexData->indices.size() / 3;

    for (int i = 0; i < triangleCount; ++i) {
        int normalIndex = i * 3;
        int index0 = vertexData->indices[normalIndex];
        int index1 = vertexData->indices[normalIndex + 1];
        int index2 = vertexData->indices[normalIndex + 2];

        glm::vec3 normal = calculateNormalFromIndices(index0, index1, index2);

        vertexData->vertices[index0].normal += normal;
        vertexData->vertices[index1].normal += normal;
        vertexData->vertices[index2].normal += normal;
    }

    unsigned int borderTriangleCount = outOfMeshTriangles.size() / 3;
    for (int i = 0; i < borderTriangleCount; ++i) {
        int normalIndex = i * 3;
        int index0 = outOfMeshTriangles[normalIndex];
        int index1 = outOfMeshTriangles[normalIndex + 1];
        int index2 = outOfMeshTriangles[normalIndex + 2];

        glm::vec3 normal = calculateNormalFromIndices(index0, index1, index2);

        if (index0 >= 0)
            vertexData->vertices[index0].normal += normal;
        if (index1 >= 0)
            vertexData->vertices[index1].normal += normal;
        if (index2 >= 0)
            vertexData->vertices[index2].normal += normal;
    }

    for (auto& vertex : vertexData->vertices) {
        vertex.normal = glm::normalize(vertex.normal);
    }

    if (createHeightMap) {
        float lower = -1.0f;
        float upper = 1.0f;
        for (int i = 0; i < vertexData->vertices.size(); ++i) {
            (*heightMapNormalValues)[i * 3] = mapRangeToUnsignedByte(vertexData->vertices[i].normal.x, lower, upper);
            (*heightMapNormalValues)[i * 3 + 1] = mapRangeToUnsignedByte(vertexData->vertices[i].normal.y, lower, upper);
            (*heightMapNormalValues)[i * 3 + 2] = mapRangeToUnsignedByte(vertexData->vertices[i].normal.z, lower, upper);
        }
    }
}

unsigned char TerrainMeshData::mapRangeToUnsignedByte(float inVal, float lowerBound, float upperBound) {
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

void TerrainMeshData::destroy() {
    delete vertexData;
}