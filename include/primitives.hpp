#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <vector>
#include "mesh.hpp"

Mesh createPlane(int dimension) {
    int numVertices = dimension * dimension;
    int half = dimension / 2;
    std::vector<Vertex> vertices (numVertices);
    int numIndices = (dimension - 1) * (dimension - 1) * 2 * 3;
    std::vector<unsigned int> indices (numIndices);

    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            Vertex &tmp = vertices[i * dimension + j];
            tmp.position.x = j - half;
            tmp.position.y = 0;
            tmp.position.z = i - half;
        }
    }

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

    return Mesh(vertices, indices);
}

Mesh createCube(int side) {
    
    glm::vec3 vertList[] = {
        glm::vec3(-1, -1, -1),
        glm::vec3(1, -1, -1),
        glm::vec3(1, 1, -1),
        glm::vec3(-1, 1, -1),
        glm::vec3(-1, -1, 1),
        glm::vec3(1, -1, 1),
        glm::vec3(1, 1, 1),
        glm::vec3(-1, 1, 1)
    };

    glm::vec2 texCoords[] = {
        glm::vec2(0, 0),
        glm::vec2(1, 0),
        glm::vec2(1, 1),
        glm::vec2(0, 1)
    };

    glm::vec3 normals[] = {
        glm::vec3(0, 0, 1),
        glm::vec3(1, 0, 0),
        glm::vec3(0, 0, -1),
        glm::vec3(-1, 0, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(0, -1, 0)
    };

    std::vector<unsigned int> indices = {
        0, 1, 3, 3, 1, 2,
        1, 5, 2, 2, 5, 6,
        5, 4, 6, 6, 4, 7,
        4, 0, 7, 7, 0, 3,
        3, 2, 7, 7, 2, 6,
        4, 5, 0, 0, 5, 1
    };

    int texInds[6] = { 0, 1, 3, 3, 1, 2 };

    std::vector<Vertex> vertices(8);
    for (int i = 0; i < 8; i++) {
        vertices[i].position = vertList[i];
        //vertices[i].normal = normals[indices[i / 6]];
        //vertices[i].textureCoords = texCoords[texInds[i % 4]];
    }
    
    // ##########

    /*
    glm::vec3 vertList[] = {
        glm::vec3(-1.0, -1.0,  1.0),
        glm::vec3(1.0, -1.0,  1.0),
        glm::vec3(1.0,  1.0,  1.0),
        glm::vec3(-1.0,  1.0,  1.0),

        glm::vec3(-1.0, -1.0, -1.0),
        glm::vec3(1.0, -1.0, -1.0),
        glm::vec3(1.0,  1.0, -1.0),
        glm::vec3(-1.0,  1.0, -1.0)
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,
        2, 3, 0,
        1, 5, 6,
        6, 2, 1,
        7, 6, 5,
        5, 4, 7,
        4, 0, 3,
        3, 7, 4,
        4, 5, 1,
        1, 0, 4,
        3, 2, 6,
        6, 7, 3
    };
    */
    // ##################

    /*
    glm::vec3 vertList[] = {
        glm::vec3(-1.0f, 1.0f, 1.0f),
        glm::vec3(-1.0f, -1.0f, 1.0f),
        glm::vec3(1.0f, -1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, -1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, 1.0f, -1.0f),
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,
        0, 2, 3,
        7, 6, 1,
        7, 1, 0,
        4, 5, 6,
        4, 6, 7,
        3, 2, 5,
        3, 5, 4
    };
    */
    //####################
    /*
    glm::vec3 vertList[] =
    {
        vec3(-1.0f, +1.0f, +1.0f), // 0
        vec3(+1.0f, +1.0f, +1.0f), // 1
        vec3(+1.0f, +1.0f, -1.0f), // 2
        vec3(-1.0f, +1.0f, -1.0f), // 3
        vec3(-1.0f, +1.0f, -1.0f), // 4
        vec3(+1.0f, +1.0f, -1.0f), // 5
        vec3(+1.0f, -1.0f, -1.0f), // 6
        vec3(-1.0f, -1.0f, -1.0f), // 7
        vec3(+1.0f, +1.0f, -1.0f), // 8
        vec3(+1.0f, +1.0f, +1.0f), // 9
        vec3(+1.0f, -1.0f, +1.0f), // 10
        vec3(+1.0f, -1.0f, -1.0f), // 11
        vec3(-1.0f, +1.0f, +1.0f), // 12
        vec3(-1.0f, +1.0f, -1.0f), // 13
        vec3(-1.0f, -1.0f, -1.0f), // 14
        vec3(-1.0f, -1.0f, +1.0f), // 15
        vec3(+1.0f, +1.0f, +1.0f), // 16
        vec3(-1.0f, +1.0f, +1.0f), // 17
        vec3(-1.0f, -1.0f, +1.0f), // 18
        vec3(+1.0f, -1.0f, +1.0f), // 19
        vec3(+1.0f, -1.0f, -1.0f), // 20
        vec3(-1.0f, -1.0f, -1.0f), // 21
        vec3(-1.0f, -1.0f, +1.0f), // 22
        vec3(+1.0f, -1.0f, +1.0f), // 23
    };

    std::vector<unsigned int> indices = {
        0,   1,  2,  0,  2,  3, // Top
        4,   5,  6,  4,  6,  7, // Front
        8,   9, 10,  8, 10, 11, // Right
        12, 13, 14, 12, 14, 15, // Left
        16, 17, 18, 16, 18, 19, // Back
        20, 22, 21, 20, 23, 22, // Bottom
    };
    */

    // ################ WORKING ##########
    /*
    float vertList[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
    };

    std::vector<Vertex> vertices(36);
    std::vector<unsigned int> indices(36);
    for (int i = 0; i < 36; i++) {
        //vertices[i].position = vertList[indices[i]];
        //vertices[i].normal = normals[indices[i / 6]];
        //vertices[i].textureCoords = texCoords[texInds[i % 4]];
        vertices[i].position.x = vertList[i * 3];
        vertices[i].position.y = vertList[i * 3 + 1];
        vertices[i].position.z = vertList[i * 3 + 2];
        indices[i] = i;
    }
    */
    return Mesh(vertices, indices);
}

/*
Mesh createCube(float xPos, float yPos, float zPos, float edgeLen) {
    float halfEdge = edgeLen * 0.5;

    float vertList[] = {
        xPos - halfEdge, yPos + halfEdge, zPos + halfEdge,
    }
}
*/

#endif