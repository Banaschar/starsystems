#include "terraingenerator.hpp"

#include <cstdio>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "primitives.hpp"

TerrainGenerator::TerrainGenerator() : colorGen_(ColorGenerator()), pNoise_(PerlinNoise()) {}
TerrainGenerator::TerrainGenerator(PerlinNoise pNoise) : pNoise_(pNoise), colorGen_(ColorGenerator()) {}
TerrainGenerator::TerrainGenerator(ColorGenerator colorGen, PerlinNoise pNoise)
    : colorGen_(colorGen), pNoise_(pNoise) {}

/*
 * startX and startZ are used to create seamless terrain, so different chunks fit together
 * lod is the detail level of the terrain. 
 * dimension is the size
 *
 * Normals from Noise:
 * https://www.scratchapixel.com/lessons/procedural-generation-virtual-worlds/perlin-noise-part-2/perlin-noise-computing-derivatives
 *
 * LOD transitions:
 * https://dexyfex.com/2016/07/14/voxels-and-seamless-lod-transitions/
 *
 * CDLOD (Best?)
 * Paper: Procedural_Terrain_Generator.pdf
 */
Mesh *TerrainGenerator::generateTerrain(GenerationAttributes *attr) {
    bool isFlat = false;
    switch (attr->genType) {
        case GenerationType::PLANE_FLAT:
            isFlat = true;
        case GenerationType::PLANE:
            return generateMeshNew(attr->position, attr->dimension, attr->lod, attr->axis, isFlat);
            break;
        case GenerationType::SPHERE_FLAT:
            isFlat = true;
        case GenerationType::SPHERE:
            return generateMeshNew(attr->position, attr->dimension, attr->lod, attr->axis, isFlat);
            break;
        default:
            fprintf(stderr, "[TERRAINGENERATOR::generateTerrain] CRITICAL ERROR: Generation Type unknown\n");
            return new Mesh();
    }
}

ColorGenerator &TerrainGenerator::getColorGenerator() {
    return colorGen_;
}

PerlinNoise &TerrainGenerator::getPerlinNoise() {
    return pNoise_;
}

void TerrainGenerator::setSphereRadius(int radius) {
    sphereRadius_ = radius;
}

int TerrainGenerator::getSphereRadius() {
    return sphereRadius_;
}

glm::vec3 TerrainGenerator::getSphereOrigin() {
    return sphereOrigin_;
}

void TerrainGenerator::setSphereOrigin(glm::vec3 origin) {
    sphereOrigin_ = origin;
}

glm::vec3 TerrainGenerator::calculateNormal(glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2) {
    glm::vec3 p1 = v1 - v0;
    glm::vec3 p2 = v2 - v0;
    return glm::normalize(glm::cross(p1, p2));
}

/*
 * Border vertices:
 * Index 0 to dimLod+1 are the top row
 * Afterwards first and last in next row -> 2 * (borderDim - 2) vertices
 * Until 
 *
 * v0     v0 v1
 * v1 v2     v2
 *
 * NORMAL DIRECTION: If a problem, reverse the order of calling calculateNormal to calculateNormal(v0, v2, v1)
 */
void TerrainGenerator::addBorderNormals(std::vector<Vertex> &vertices, std::vector<glm::vec3> &borderMap, int dimensionLod) {
    int borderDim = dimensionLod + 2;
    glm::vec3 v0;
    glm::vec3 v1;
    glm::vec3 v2;
    glm::vec3 normal;

    // Top row
    for (int x = 0; x < dimensionLod + 1; x++) {
        v0 = borderMap.at(x);

        if (x > 0)
            v1 = vertices.at(x-1).position;
        else
            v1 = borderMap.at(borderDim);

        if (x < dimensionLod)
            v2 = vertices.at(x).position;
        else
            v2 = borderMap.at(borderDim-1);

        normal = calculateNormal(v0, v1, v2);

        if (x > 0)
            vertices.at(x-1).normal += normal;
        if (x < dimensionLod)
            vertices.at(x).normal += normal;

        // second triangle
        v1 = borderMap.at(x+1);

        normal = calculateNormal(v0, v1, v2);

        if (x < dimensionLod)
            vertices.at(x).normal += normal;
    }

    // Bottom row
    // borderArray bottom row: borderDim + 2 * (borderDim - 2)
    int bArrayBottomRow = borderDim + 2 * (borderDim - 2);
    for (int x = 0; x < dimensionLod + 1; x++) {
        if (x > 0)
            v0 = vertices.at(dimensionLod * (dimensionLod - 1) + x - 1).position;
        else
            v0 = borderMap[bArrayBottomRow - 2];
        v1 = borderMap.at(bArrayBottomRow + x);
        v2 = borderMap.at(bArrayBottomRow + x + 1);

        normal = calculateNormal(v0, v1, v2);

        if (x > 0 && x < dimensionLod)
            vertices.at(dimensionLod * (dimensionLod - 1) + x - 1).normal += normal;

        // second triangle
        if (x < dimensionLod)
            v1 = vertices.at(dimensionLod * (dimensionLod - 1) + x).position; 
        else
            v1 = borderMap.at(bArrayBottomRow - 1);

        normal = calculateNormal(v0, v1, v2);

        if (x < dimensionLod)
            vertices.at(dimensionLod * (dimensionLod - 1) + x).normal += normal;
    }

    // Side rows
    for (int x = 0; x < dimensionLod - 1; x++) {
        // left side, first
        v0 = borderMap.at(borderDim + 2 * x);
        v1 = borderMap.at(borderDim + 2 * x + 2);
        v2 = vertices.at(dimensionLod * (x + 1)).position;

        normal = calculateNormal(v0, v1, v2);
        vertices.at(dimensionLod * (x + 1)).normal += normal;

        // left side, second
        v1 = vertices.at(dimensionLod * x).position;
        normal = calculateNormal(v0, v1, v2);
        vertices.at(dimensionLod * x).normal += normal;

        // right side, first
        v0 = vertices.at(dimensionLod * x + dimensionLod - 1).position;
        v1 = vertices.at(dimensionLod * (x + 1) + dimensionLod - 1).position;
        v2 = borderMap.at(borderDim + 2 * x + 3);

        normal= calculateNormal(v0, v1, v2);
        vertices.at(dimensionLod * x + dimensionLod - 1).normal += normal;
        vertices.at(dimensionLod * (x + 1) + dimensionLod - 1).normal += normal;

        //right side second
        v1 = borderMap[borderDim + 2 * x + 1];
        normal = calculateNormal(v0, v1, v2);
        vertices.at(dimensionLod * (x + 1) + dimensionLod - 1).normal += normal;
    }

}

void TerrainGenerator::calculateVertexNormalSphere(std::vector<Vertex> &vertices,
                                                                 std::vector<unsigned int> &indices,
                                                                 std::vector<glm::vec3> &borderMap, int dimensionLod) {
    
    for (int i = 0; i < indices.size() / 3; i++) {
        int index = i * 3;
        glm::vec3 v0 = vertices.at(indices[index]).position;
        glm::vec3 v1 = vertices.at(indices[index + 1]).position;
        glm::vec3 v2 = vertices.at(indices[index + 2]).position;

        glm::vec3 normal = calculateNormal(v0, v1, v2);

        vertices.at(indices[index]).normal += normal;
        vertices.at(indices[index + 1]).normal += normal;
        vertices.at(indices[index + 2]).normal += normal;
    }

    addBorderNormals(vertices, borderMap, dimensionLod);

    for (int i = 0; i < vertices.size(); i++) {
        vertices[i].normal = glm::normalize(vertices[i].normal);
    }
}

float TerrainGenerator::getHeightN(int x, int z, const std::vector<Vertex> &vertices, int dimLod) {
    x = x < 0 ? 0 : x;
    z = z < 0 ? 0 : z;
    x = x >= dimLod ? dimLod - 1 : x;
    z = z >= dimLod ? dimLod - 1 : z;
    return vertices[z * dimLod + x].position.y;
}

glm::vec3 TerrainGenerator::calcNormal(int x, int z, const std::vector<Vertex> &vertices, int dimLod) {
    float heightL = getHeightN(x - 1, z, vertices, dimLod);
    float heightR = getHeightN(x + 1, z, vertices, dimLod);
    float heightD = getHeightN(x, z - 1, vertices, dimLod);
    float heightU = getHeightN(x, z + 1, vertices, dimLod);

    return glm::normalize(glm::vec3(heightL - heightR, 2.0f, heightD - heightU));
}

/*
 * Switch out for the normal generator in primitives?
 * TODO: Normal generation does not work right for spheres
 */
void TerrainGenerator::generateNormalVector(std::vector<Vertex> &vertices, int dimension, int lod) {
    int dimensionLod = ((dimension - 1) / lod) + 1;
    for (int z = 0; z < dimensionLod; z++) {
        for (int x = 0; x < dimensionLod; x++) {
            vertices[z * dimensionLod + x].normal = calcNormal(x, z, vertices, dimensionLod);
        }
    }
}

/*
 * Generate height points. Averages between neighbouring points. SLOW
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

/*
 * TODO: Seems unresonable large and wasteful=? Think about it a bit
 * -------> Put this into the generateMesh loop
 */
std::vector<unsigned int> TerrainGenerator::generateIndexVector(std::vector<unsigned int> &indices, int dimensionLod, bool inverted) {
    int cnt = 0;
    for (int row = 0; row < dimensionLod - 1; row++) {
        for (int col = 0; col < dimensionLod - 1; col++) {
            if (inverted) {
                indices[cnt++] = dimensionLod * row + col;
                indices[cnt++] = dimensionLod * row + col + dimensionLod + 1;
                indices[cnt++] = dimensionLod * row + col + dimensionLod;
                
                indices[cnt++] = dimensionLod * row + col;
                indices[cnt++] = dimensionLod * row + col + 1;
                indices[cnt++] = dimensionLod * row + col + dimensionLod + 1;
            } else {
                indices[cnt++] = dimensionLod * row + col;
                indices[cnt++] = dimensionLod * row + col + dimensionLod;
                indices[cnt++] = dimensionLod * row + col + dimensionLod + 1;

                indices[cnt++] = dimensionLod * row + col;
                indices[cnt++] = dimensionLod * row + col + dimensionLod + 1;
                indices[cnt++] = dimensionLod * row + col + 1;
            }
        }
    }

    return indices;
}

glm::vec3 TerrainGenerator::getSpherePos(glm::vec3 &axis, int radius, int x, int z) {
    if (axis.x) {
        return glm::vec3(radius, z, x); //was x, z
    } else if (axis.y) {
        return glm::vec3(x, radius, z); // was z, rad, x
    } else {
        return glm::vec3(x, z, radius);
    }
}

/*
 * Slight problems with seams between terrain chunks.
 */
Mesh *TerrainGenerator::generateMeshSphere(glm::vec3 &pos, int dimension, int lod, glm::vec3 &axis, bool flat) {
    int dimensionLod = (dimension / lod) + 1;
    std::vector<Vertex> vertices(dimensionLod * dimensionLod);
    int half = dimension / 2;
    dimension += 1;
    int index = 0;
    int borderIndex = 0;
    std::vector<glm::vec3> borderMap(2 * (dimensionLod + 2) + 2 * dimensionLod);

    int direction;
    glm::vec2 offset;
    if (axis.x) {
        direction = axis.x;
        offset.x = pos.z;
        offset.y = pos.y;
    }
    else if (axis.y) {
        direction = axis.y;
        offset.x = pos.x;
        offset.y = pos.z;
    }
    else {
        direction = axis.z;
        offset.x = pos.x;
        offset.y = pos.y;
    }

    // Inverted vertex triangle winding order for terrain meshes on the other side of the cube.
    bool inverted = false;
    if (axis.x == -1 || axis.z == 1 || axis.y == -1)
        inverted = true;

    for (int y = -lod; y < dimension + lod; y+=lod) {
        for (int x = -lod; x < dimension + lod; x+=lod) {

            glm::vec3 tmpPos = getSpherePos(axis, direction * sphereRadius_, x + offset.x - half, y + offset.y - half);
            // modify point so it has distance radius from origin 
            tmpPos = sphereOrigin_ + (float)sphereRadius_ * glm::normalize(tmpPos - sphereOrigin_);

            if (!flat) {
                // Get height based on sphere point
                float height = pNoise_.getNoise3d(tmpPos.x, tmpPos.y, tmpPos.z); 
                // modify point so it has distance radius+heigh from origin
                tmpPos = sphereOrigin_ + ((float)sphereRadius_ + height) * glm::normalize(tmpPos - sphereOrigin_); 
            }

            if (x == -lod || x >= dimension || y == -lod || y >= dimension) {
                borderMap[borderIndex++] = tmpPos;
            } else {

                Vertex vertex;
                vertex.position = tmpPos;
                vertex.textureCoords.x = (float)x / ((float)dimensionLod - 1);
                vertex.textureCoords.y = (float)y / ((float)dimensionLod - 1);
                vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);
                vertices[index++] = vertex;
            }
        }
    }

    //generateNormalVector(vertices, dimension, lod);
    std::vector<unsigned int> indices((dimensionLod - 1) * (dimensionLod - 1) * 6);
    generateIndexVector(indices, dimensionLod, inverted);
    calculateVertexNormalSphere(vertices, indices, borderMap, dimensionLod);

    return new Mesh(vertices, indices);    
}

/*
 * Plane: Only take pos.x and pos.z
 */
Mesh *TerrainGenerator::generateMesh(glm::vec3 &pos, int dimension, int lod, bool flat) {
    int dimensionLod = (dimension / lod) + 1;
    std::vector<Vertex> vertices(dimensionLod * dimensionLod);
    int half = dimension / 2;
    dimension += 1;

    int index = 0, row = 0, col = 0;
    for (int z = pos.z; z < dimension + pos.z; z+=lod) {
        for (int x = pos.x; x < dimension + pos.x; x+=lod) {
            Vertex vertex;
            vertex.position.x = x - half;
            //vertex.position.y = lod == 16 ? generateHeights(x - half, z - half, lod) : pNoise_.getNoise2d(x - half, z - half);
            if (flat)
                vertex.position.y = 0;
            else
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

    std::vector<unsigned int> indices((dimensionLod - 1) * (dimensionLod - 1) * 6);
    generateIndexVector(indices, dimensionLod);
    generateNormalVector(vertices, dimension, lod);

    return new Mesh(vertices, indices);
}

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    std::vector<glm::vec3> outOfMeshVertices;
    std::vector<int> outOfMeshTriangles;
    int outOfMeshTriangleIndex = 0;
    int triangleIndex = 0;

    MeshData(int numVertsPerLine, int skipIncrement) {
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

    void addVertex(glm::vec3 pos, glm::vec2 uv, int vertexIndex) {
        if (vertexIndex < 0)
            outOfMeshVertices[-vertexIndex - 1] = pos; // Because index starts with -1 for outOfMesh vertices
        else {
            vertices[vertexIndex].position = pos;
            vertices[vertexIndex].textureCoords = uv;
        }
    }

    void addTriangle(int a, int b, int c) {
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

    glm::vec3 calculateNormalFromIndices(int index0, int index1, int index2) {
        glm::vec3 v0 = (index0 < 0) ? outOfMeshVertices[-index0 - 1] : vertices[index0].position;
        glm::vec3 v1 = (index1 < 0) ? outOfMeshVertices[-index1 - 1] : vertices[index1].position;
        glm::vec3 v2 = (index2 < 0) ? outOfMeshVertices[-index2 - 1] : vertices[index2].position;

        glm::vec3 p0 = v1 - v0;
        glm::vec3 p1 = v2 - v0;

        return glm::normalize(glm::cross(p0, p1));
    }

    void calculateNormals() {
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
    }
};

/*
 * Computes the vertex position in the terrain
 * height is optional, in case it's precomputed
 */
glm::vec3 TerrainGenerator::getVertexPosition(glm::vec3 &axis, int x, int y, bool isFlat, float height) {
    glm::vec3 vertPos;
    if (axis.x) {
        vertPos = glm::vec3(sphereOrigin_.x + axis.x * sphereRadius_, y, x);
    } else if (axis.y) {
        vertPos = glm::vec3(x, sphereOrigin_.y + axis.y * sphereRadius_, y);
    } else {
        vertPos = glm::vec3(x, y, sphereOrigin_.z + axis.z * sphereRadius_);
    }

    if (sphereRadius_) {
        // SHOULD USE THE CUBESIDE POSITION AS NOISE INPUTS -> Easier for biome calculation? AND faster calc. BUT HORRIBLE SEAMS AT CUBE SIDE BORDERS
        vertPos = sphereOrigin_ + (float)sphereRadius_ * glm::normalize(vertPos - sphereOrigin_);
        if (!height && !isFlat)
            height = pNoise_.getNoise3d(vertPos.x, vertPos.y, vertPos.z); 
        vertPos = sphereOrigin_ + ((float)sphereRadius_ + height) * glm::normalize(vertPos - sphereOrigin_);
    } else {
        if (!height && !isFlat)
            height = pNoise_.getNoise2d(x, y);
        vertPos.y = height;
    }

    return vertPos;
}

/*
 * Remaining problems: Normal vectors at the border of cubesides do not match up -> creates visible seams
 *
 */
Mesh *TerrainGenerator::generateMeshNew(glm::vec3 &pos, int dimension, int lod, glm::vec3 &axis, bool isFlat) {
    if (dimension % 60 != 0) {
        fprintf(stdout, "[TERRAINGENERATOR::generateMesh] Error: Terrain Dimension has to be divisible by 60\n");
        return nullptr;
    }

    int half = (dimension / 2) + 1;
    int numVertsPerLine = dimension + 3;
    int skipIncrement = lod;

    int offsetX, offsetY;
    if (axis.x) {
        offsetX = pos.z;
        offsetY = pos.y;
    }
    else if (axis.y) {
        offsetX = pos.x;
        offsetY = pos.z;
    }
    else {
        offsetX = pos.x;
        offsetY = pos.y;
    }

    /* Triangles on the opposite side of the sphere need to have a clock-wise winding order */
    bool inverted = false;
    if (axis.x == -1 || axis.z == 1 || axis.y == -1)
        inverted = true;

    MeshData meshData(numVertsPerLine, skipIncrement);

    int vertexIndicesMap[numVertsPerLine][numVertsPerLine];
    int meshVertexIndex = 0;
    int outOfMeshVertexIndex = -1;

    for (int y = 0; y < numVertsPerLine; ++y) {
        for (int x = 0; x < numVertsPerLine; ++x) {
            bool isOutOfMeshVertex = y == 0 || y == numVertsPerLine - 1 || x == 0 || x == numVertsPerLine - 1;
            bool isSkippedVertex = x > 1 && x < numVertsPerLine - 2 && y > 1 && y < numVertsPerLine - 2 && ((x - 1) % skipIncrement != 0 || (y - 1) % skipIncrement != 0);

            if (isOutOfMeshVertex)
                vertexIndicesMap[x][y] = outOfMeshVertexIndex--;
            else if (!isSkippedVertex)
                vertexIndicesMap[x][y] = meshVertexIndex++;
            else
                vertexIndicesMap[x][y] = 200000000;
        }
    }

    for (int y = 0; y < numVertsPerLine; ++y) {
        for (int x = 0; x < numVertsPerLine; ++x) {
            bool isSkippedVertex = x > 1 && x < numVertsPerLine - 2 && y > 1 && y < numVertsPerLine - 2 && ((x - 1) % skipIncrement != 0 || (y - 1) % skipIncrement != 0);

            if (!isSkippedVertex) {
                bool isOutOfMeshVertex = y == 0 || y == numVertsPerLine - 1 || x == 0 || x == numVertsPerLine - 1;
                bool isMeshEdgeVertex = !isOutOfMeshVertex && (y == 1 || y == numVertsPerLine - 2 || x == 1 || x == numVertsPerLine - 2);
                bool isMainVertex = !isOutOfMeshVertex && !isMeshEdgeVertex && (x - 1) % skipIncrement == 0 && (y - 1) % skipIncrement == 0;

                int vertexIndex = vertexIndicesMap[x][y];
                float height = 0.0;
                
                glm::vec3 sPos = getVertexPosition(axis, x + offsetX - half, y + offsetY - half, isFlat, height);
                glm::vec2 uv = glm::vec2(x - 1, y - 1) / (float)(numVertsPerLine - 1);
                meshData.addVertex(sPos, uv, vertexIndex);
                //if ((y > numVertsPerLine - 3 && x > numVertsPerLine - 3) || (y < 2 && x < 2))
                    //fprintf(stdout, "Chunk: %s, Index(y,x): (%i,%i), VertIndex: %i, VertPos: %s\n", glm::to_string(pos).c_str(), y,x, vertexIndex, glm::to_string(sPos).c_str());

                bool createTriangle = x < numVertsPerLine - 1 && y < numVertsPerLine - 1;

                if (createTriangle) {

                    if (skipIncrement != 1 && isMeshEdgeVertex && !(x == numVertsPerLine - 2 && y == numVertsPerLine - 2)) { // bottom right corner only draws the normal helper triangles
                        bool isCorner = x == 1 && y == 1 || x == numVertsPerLine - 2 && y == 1 || x == 1 && y == numVertsPerLine - 2 || x == numVertsPerLine - 2 && y == numVertsPerLine - 2;
                        bool isVertical = x == 1 || x == numVertsPerLine - 2;
                        bool isEdgeMainVertex = (isVertical ? y - 1 : x - 1) % skipIncrement == 0;
                        int lastMainVert = (isVertical ? y : x) - ((isVertical ? y - 1 : x - 1) % skipIncrement);
                        int nextMainVert = lastMainVert + skipIncrement;
                        bool nextMainIsCorner = nextMainVert == numVertsPerLine - 2;

                        int a = vertexIndicesMap[x][y];
                        int b, c, d;

                        if (x != numVertsPerLine - 2 && y != numVertsPerLine - 2) {
                            if (!nextMainIsCorner) {
                                c = isVertical ? vertexIndicesMap[x][y + 1] : vertexIndicesMap[nextMainVert][y + skipIncrement];
                                d = isVertical ? vertexIndicesMap[x + skipIncrement][nextMainVert] : vertexIndicesMap[x + 1][y];

                                meshData.addTriangle(a, inverted ? d : c, inverted ? c : d);
                                if (isEdgeMainVertex) {
                                    b = isVertical ? vertexIndicesMap[x + (isCorner ? 1 : skipIncrement)][y] : vertexIndicesMap[x][y + skipIncrement];
                                    isVertical ? meshData.addTriangle(a, inverted ? b : d, inverted ? d : b) : meshData.addTriangle(a, inverted ? c : b, inverted ? b : c);
                                }
                            } else {
                                c = isVertical ? vertexIndicesMap[x][y + 1] : vertexIndicesMap[lastMainVert][y + skipIncrement];
                                d = isVertical ? vertexIndicesMap[x + skipIncrement][lastMainVert] : vertexIndicesMap[x + 1][y];
                                meshData.addTriangle(a, inverted ? d : c, inverted ? c : d);
                            }
                        } else {
                            if (x == 1) { // Edge case: bottom left corner is treated as horizontal
                                isVertical = false;
                                nextMainVert = x + skipIncrement;
                            }
                            if (!nextMainIsCorner) {
                                c = isVertical ? vertexIndicesMap[x - skipIncrement][nextMainVert] : vertexIndicesMap[x + 1][y];
                                d = isVertical ? vertexIndicesMap[x][y + 1] : vertexIndicesMap[nextMainVert][y - skipIncrement];
                                meshData.addTriangle(a, inverted ? d : c, inverted ? c : d);
                                if (isEdgeMainVertex && !isCorner) {
                                    b = isVertical ? vertexIndicesMap[x - skipIncrement][y] : vertexIndicesMap[x][y - skipIncrement];
                                    isVertical ? meshData.addTriangle(a, inverted ? c : b, inverted ? b : c) : meshData.addTriangle(a, inverted ? b : d, inverted ? d : b);
                                }
                            } else {
                                c = isVertical ? vertexIndicesMap[x - skipIncrement][lastMainVert] : vertexIndicesMap[x + 1][y];
                                d = isVertical ? vertexIndicesMap[x][y + 1] : vertexIndicesMap[lastMainVert][y - skipIncrement];
                                meshData.addTriangle(a, inverted ? d : c, inverted ? c : d);
                            }
                        }
                    }
                    bool isEdgeAndCreatesOutOfMeshTriangle = x == numVertsPerLine - 2 || y == numVertsPerLine - 2;
                    bool isMainAndCreatesEdge = x == numVertsPerLine - 2 - skipIncrement || y == numVertsPerLine - 2 - skipIncrement;

                    if (skipIncrement == 1 || isOutOfMeshVertex || (isMainVertex && !isMainAndCreatesEdge) || isEdgeAndCreatesOutOfMeshTriangle) {
                        int currentIncrement = (isMainVertex && x != numVertsPerLine - 2 && y != numVertsPerLine - 2) ? skipIncrement : 1;
                        int a = vertexIndicesMap[x][y];
                        int b = vertexIndicesMap[x + currentIncrement][y];
                        int c = vertexIndicesMap[x][y + currentIncrement];
                        int d = vertexIndicesMap[x + currentIncrement][y + currentIncrement];
                        
                        meshData.addTriangle(a, inverted ? d : c, inverted ? c : d);
                        meshData.addTriangle(a, inverted ? b : d, inverted ? d : b);
                    }
                }
            }
        }
    }
    meshData.calculateNormals();
    return new Mesh(meshData.vertices, meshData.indices);
}

/*
Explanation (for dimension 13 with lod 4):
M: main vertex, e: edge Vertex, c: edge connection Vertex, x: outOfMesh vertex (just for normals)
  0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
0 x x x x x x x x x x x  x  x  x  x  x  x
1 x e e e e e e e e e e  e  e  e  e  e  x 
2 x e M c c c M c c c M  c  c  c  M  e  x
3 x e c                           c  e  x
4 x e c                           c  e  x
5 x e c                           c  e  x
6 x e M       M       M           M  e  x
7 x e c                           c  e  x
8 x e c                           c  e  x
9 x e c                           c  e  x
10x e M       M       M           M  e  x
11x e c                           c  e  x
12x e c                           c  e  x
13x e c                           c  e  x
14x e M c c c M c c c M  c  c  c  M  e  x
15x e e e e e e e e e e  e  e  e  e  e  x
16x x x x x x x x x x x  x  x  x  x  x  x

  0 1 2 3 4 5 6 7 8 9
0 e e e e e e e e e e
1
2
3
4
5
6
7
8
9
*/

/*
 * BIOMES
 * 
 */

/*
 * Creates a biome map for each cube side
 * Creating specific values for each possible coordinate would be far too large for big planets
 * -> Create a hash map which matches a range of coordinates to biome functions
 * For example: Everything from (15, -radius, 12) to (63, -radius, 23) -> ocean function.
 * -> Complexity: Soft transitions between biomes
 *
 * For biome assignment -> Divide cubeSide into squares -> (0,0), (1,0) etc. Like QuadTree. 
 * To get square from cubeside coordinates, divide by dimension or something.
 *
 *
 * Randomized input variables: WaterPercentage, HeatLevel (Desertworld, ArcticWorld), etc.
 */

/*
 * Represents a side of the cube
 */
/*
class PlanetCubeSide {
public:
    PlanetCubeSide();
private:
    bool side;
    bool pole; 
    glm::vec3 axis;
    PlanetCubeSide *up;
    PlanetCubeSide *right;
    PlanetCubeSide *left;
    PlanetCubeSide *down;
}
*/
/*
void TerrainGenerator::buildPlanetBiomeMap() {

}
*/