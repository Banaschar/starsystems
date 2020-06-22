#include "primitives.hpp"

Mesh Primitives::createPlane(int dimension) {
    int numVertices = dimension * dimension;
    int half = dimension / 2;
    std::vector<Vertex> vertices(numVertices);
    int numIndices = (dimension - 1) * (dimension - 1) * 2 * 3;
    std::vector<unsigned int> indices(numIndices);

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

Mesh Primitives::createQuad() {
    std::vector<Vertex> vertices(4);
    std::vector<glm::vec3> pos = {glm::vec3(-1, 0, -1), glm::vec3(-1, 0, 1), glm::vec3(1, 0, -1), glm::vec3(1, 0, 1)};
    std::vector<glm::vec2> texCoords = {glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 0), glm::vec2(1, 1)};
    std::vector<unsigned int> indices = {0, 1, 2, 2, 1, 3};

    std::vector<glm::vec3> normals = calculateVertexNormalAverages(pos, indices);

    for (int i = 0; i < 4; i++) {
        vertices[i].position = pos[i];
        vertices[i].normal = normals[i];
        vertices[i].textureCoords = texCoords[i];
    }

    return Mesh(vertices, indices);
}

Mesh Primitives::createQuad2d() {
    std::vector<Vertex> vertices(4);
    std::vector<glm::vec3> pos = {glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0), glm::vec3(1, 1, 0)};
    std::vector<glm::vec2> texCoords = {glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 0), glm::vec2(1, 1)};
    std::vector<unsigned int> indices = {2, 1, 0, 3, 1, 2};

    for (int i = 0; i < 4; i++) {
        vertices[i].position = pos[i];
        vertices[i].textureCoords = texCoords[i];
        vertices[i].normal = glm::vec3(0, 1, 0);
    }

    return Mesh(vertices, indices);
}

Mesh Primitives::createCube(int side) {

    glm::vec3 vertList[] = {glm::vec3(-1, -1, -1), glm::vec3(1, -1, -1), glm::vec3(1, 1, -1), glm::vec3(-1, 1, -1),
                            glm::vec3(-1, -1, 1),  glm::vec3(1, -1, 1),  glm::vec3(1, 1, 1),  glm::vec3(-1, 1, 1)};

    glm::vec2 texCoords[] = {glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1), glm::vec2(0, 1)};

    glm::vec3 normals[] = {glm::vec3(0, 0, 1),  glm::vec3(1, 0, 0), glm::vec3(0, 0, -1),
                           glm::vec3(-1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, -1, 0)};

    std::vector<unsigned int> indices = {0, 1, 3, 3, 1, 2, 1, 5, 2, 2, 5, 6, 5, 4, 6, 6, 4, 7,
                                         4, 0, 7, 7, 0, 3, 3, 2, 7, 7, 2, 6, 4, 5, 0, 0, 5, 1};

    int texInds[6] = {0, 1, 3, 3, 1, 2};

    std::vector<Vertex> vertices(8);
    for (int i = 0; i < 8; i++) {
        vertices[i].position = vertList[i];
        // vertices[i].normal = normals[indices[i / 6]];
        // vertices[i].textureCoords = texCoords[texInds[i % 4]];
    }

    return Mesh(vertices, indices);
}

std::vector<glm::vec3> Primitives::calculateVertexNormalAverages(std::vector<glm::vec3> &pos,
                                                                 std::vector<unsigned int> &indices) {
    std::vector<glm::vec3> normals(pos.size());

    for (int i = 0; i < indices.size() / 3; i++) {
        glm::vec3 v0 = pos[indices[i]];
        glm::vec3 v1 = pos[indices[i + 1]];
        glm::vec3 v2 = pos[indices[i + 2]];

        glm::vec3 p1 = v1 - v0;
        glm::vec3 p2 = v2 - v0;
        glm::vec3 normal = glm::cross(p1, p2);
        normal = glm::normalize(normal);

        normals[indices[i]] += normal;
        normals[indices[i + 1]] += normal;
        normals[indices[i + 2]] += normal;
    }

    for (int i = 0; i < normals.size(); i++) {
        normals[i] = glm::normalize(normals[i]);
    }

    return normals;
}