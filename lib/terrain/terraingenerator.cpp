#include "terraingenerator.hpp"

#include <cstdio>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "primitives.hpp"
#include "textureloader.hpp"
#include "terrainmeshdata.hpp"

TerrainGenerator::TerrainGenerator() : colorGen_(ColorGenerator()), pNoise_(PerlinNoise()) {}
TerrainGenerator::TerrainGenerator(PerlinNoise pNoise) : pNoise_(pNoise), colorGen_(ColorGenerator()) {}
TerrainGenerator::TerrainGenerator(ColorGenerator colorGen, PerlinNoise pNoise)
    : colorGen_(colorGen), pNoise_(pNoise) {}

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

Mesh *TerrainGenerator::generateTerrain(GenerationAttributes *attr) {
    bool isFlat = false;
    switch (attr->genType) {
        case GenerationType::PLANE_FLAT:
            attr->isFlat = true;
        case GenerationType::PLANE:
            return generateTerrainMesh(attr);
            break;
        case GenerationType::SPHERE_FLAT:
            attr->isFlat = true;
        case GenerationType::SPHERE:
            return generateTerrainMesh(attr);
            break;
        default:
            fprintf(stderr, "[TERRAINGENERATOR::generateTerrain] CRITICAL ERROR: Generation Type unknown\n");
            return new Mesh();
    }
}

Mesh *TerrainGenerator::generateTerrainMesh(GenerationAttributes *attr) {
    int dimension = attr->dimension + 3;
    TerrainMeshData meshData(dimension, attr->lod, attr->vertexType);
    generateTerrainData(meshData, attr->position, dimension, attr->lod, attr->axis, attr->isFlat);
    meshData.calculateNormals(false);
    return new Mesh(meshData.vertexData);
}

void TerrainGenerator::generateTerrainHeightMap(GenerationAttributes *attr) {
    int dimension = attr->dimension + 2;
    TerrainMeshData meshData(dimension, 1, pNoise_, attr->heightData, attr->normalData, attr->vertexType);
    generateTerrainData(meshData, attr->position, dimension, 1, attr->axis, false);
    meshData.calculateNormals(true);
    meshData.destroy();
}

glm::vec3 TerrainGenerator::getAxisPos(glm::vec3 &axis, int x, int y) {
    glm::vec3 outPos;
    if (axis.x) {
        outPos = glm::vec3(sphereOrigin_.x + axis.x * sphereRadius_, y, x);
    } else if (axis.y) {
        outPos = glm::vec3(x, sphereOrigin_.y + axis.y * sphereRadius_, y);
    } else {
        outPos = glm::vec3(x, y, sphereOrigin_.z + axis.z * sphereRadius_);
    }

    return outPos;
}

float TerrainGenerator::getHeightValue(glm::vec3 &pos) {
    float height;
    if (sphereRadius_) {
        pos = sphereOrigin_ + (float)sphereRadius_ * glm::normalize(pos - sphereOrigin_);
        height = pNoise_.getNoise3d(pos.x, pos.y, pos.z);
    } else {
        height = pNoise_.getNoise2d(pos.x, pos.z);
    }

    return height;
}

glm::vec3 TerrainGenerator::getVertexPosition(glm::vec3 vertPos, float height) {
    if (sphereRadius_) {
        vertPos = sphereOrigin_ + ((float)sphereRadius_ + height) * glm::normalize(vertPos - sphereOrigin_);
    } else {
        vertPos.y = height;
    }

    return vertPos;
}

void TerrainGenerator::generateTerrainData(TerrainMeshData &meshData, glm::vec3 &pos, int numVertsPerLine, int skipIncrement, glm::vec3 &axis, bool isFlat) {
    if (skipIncrement != 1 && (numVertsPerLine - 3) % 60 != 0) {
        fprintf(stdout, "[TERRAINGENERATOR::generateMesh] Error: Terrain Dimension has to be divisible by 60 if lod > 1\n");
    }

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
                
                glm::vec3 vertPos = getAxisPos(axis, offsetX + x - 1, offsetY + y - 1);
                float height = isFlat ? 0.0f : getHeightValue(vertPos); // vertPos is modified to sphere Position if sphereRadius > 0
                glm::vec3 sPos = getVertexPosition(vertPos, height);
                glm::vec2 uv = glm::vec2(x - 1, y - 1) / (float)(numVertsPerLine - 1);
                meshData.addVertex(sPos, uv, vertexIndex);
                meshData.addHeight(height, vertexIndex);
                //if (vertexIndex == 0 || vertexIndex > 4092)
                    //fprintf(stdout, "Pos(y,x): (%i,%i). VertexPos(x,y,z): %s, VertexIndex: %i\n", y,x,glm::to_string(sPos).c_str(),vertexIndex);

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
}
/*
void TerrainGenerator::generateHeightMap(glm::vec3 startPos, int dimension, unsigned char *data) {
    float lowerBound = pNoise_.getLowerBound();
    float upperBound = pNoise_.getUpperBound();
    //fprintf(stdout, "lowerBound: %f, upperBound: %f\n", lowerBound, upperBound);
    for (int y = 0; y < dimension; ++y) {
        for (int x = 0; x < dimension; ++x) {
            float height = pNoise_.getNoise2d(startPos.x + x, startPos.y + y);
            data[y * dimension + x] = mapRange(height, lowerBound, upperBound);
            //fprintf(stdout, "Pos: (%i, %i). Height: %u\n", x,y,data[y * dimension + x]);
        }
    }
}

unsigned char TerrainGenerator::mapRange(float inVal, float lowerBound, float upperBound) {
    float in = clamp(inVal, lowerBound, upperBound);
    float s = 1.0 * (255 - 0) / (upperBound - lowerBound);
    return 0 + std::floor((s * (inVal - lowerBound)) + 0.5);
}

float TerrainGenerator::clamp(float i, float l, float u) {
    if (i < l)
        return l;
    else if (i > u)
        return u;
    else
        return i;
}
*/

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