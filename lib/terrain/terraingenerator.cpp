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
 */
Mesh *TerrainGenerator::generateTerrain(GenerationAttributes *attr) {
    bool isFlat = false;
    switch (attr->genType) {
        case GenerationType::PLANE_FLAT:
            isFlat = true;
        case GenerationType::PLANE:
            return generateMesh(attr->position, attr->dimension, attr->lod, isFlat);
            break;
        case GenerationType::SPHERE_FLAT:
            isFlat = true;
        case GenerationType::SPHERE:
            return generateMeshSphere(attr->position, attr->dimension, attr->lod, attr->axis, isFlat);
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
            vertices.at(dimensionLod * (dimensionLod - 1) + x - 1).normal = normal;

        // second triangle
        if (x < dimensionLod)
            v1 = vertices.at(dimensionLod * (dimensionLod - 1) + x).position; 
        else
            v1 = borderMap.at(bArrayBottomRow - 1);

        normal = calculateNormal(v0, v1, v2);

        if (x < dimensionLod)
            vertices.at(dimensionLod * (dimensionLod - 1) + x).normal = normal;
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
        vertices.at(dimensionLod * (x + 1) + dimensionLod - 1).normal = normal;
    }

}

void TerrainGenerator::calculateVertexNormalSphere(std::vector<Vertex> &vertices,
                                                                 std::vector<unsigned int> &indices,
                                                                 std::vector<glm::vec3> &borderMap, int dimensionLod) {

    for (int i = 0; i < indices.size() / 3; i++) {
        glm::vec3 v0 = vertices.at(indices[i]).position;
        glm::vec3 v1 = vertices.at(indices[i + 1]).position;
        glm::vec3 v2 = vertices.at(indices[i + 2]).position;

        glm::vec3 normal = calculateNormal(v0, v2, v1);

        vertices.at(indices[i]).normal += normal;
        vertices.at(indices[i + 1]).normal += normal;
        vertices.at(indices[i + 2]).normal += normal;
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
 * Every chunk (except for the lowest lod level) should have a border of high resolution vertexes.
 * https://www.youtube.com/watch?v=c2BUgXdjZkg&list=PLFt_AvWsXl0eBW2EiBtl_sxmDtSgZBxB3&index=21
 * Better solution:
 * Get the border vertex positions of surrounding chunks.
 * Align the vertex positions.
 * Lod = 2 | Lod = 1
 *  x1          y1
 *              y2
 *  x2          y3
 *
 * So y1 = x1; y3 = x2;
 * y2 = y1 + 1 * normalize(x2-x1) // distance(y1,y2) is always 1? otherwise distance(y1,y2)
 * --> so the border vertices in the higher res chunk line up with the vertices in the lower res chunk
 * CAVEAT: Slower performance, as I have to access very different memory locations (cache)
 */
Mesh *TerrainGenerator::generateMeshSphere(glm::vec3 &pos, int dimension, int lod, glm::vec3 &axis, bool flat) {
    int dimensionLod = ((dimension - 1) / lod) + 1;
    std::vector<Vertex> vertices(dimensionLod * dimensionLod);
    int half = (dimension - 1) / 2;
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

    // TODO: FIX THIS VERY UGLY HACK TO CHANGE WINDING ORDER FOR THESE AXIS
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
    int dimensionLod = ((dimension - 1) / lod) + 1;
    std::vector<Vertex> vertices(dimensionLod * dimensionLod);
    int half = (dimension - 1) / 2;
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


/*
 * IMPLEMENTATION LIKE SEB LAGUE
 *
 * UFF
 */

/*
Mesh *TerrainGenerator::generateMesh() {
    int numVertsPerLine = dimension + 5;
}
*/