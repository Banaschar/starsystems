#include "primitives.hpp"

Mesh *Primitives::createPlane(int dimension) {
    VertexData *vertexData = new VertexData(dimension * dimension, (dimension - 1) * (dimension - 1) * 6);

    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            Vertex &tmp = vertexData->vertices[i * dimension + j];
            tmp.position.x = j;
            tmp.position.y = 0;
            tmp.position.z = i;
            tmp.textureCoords.x = (float)j / ((float)dimension - 1);
            tmp.textureCoords.y = (float)i / ((float)dimension - 1);
        }
    }

    int cnt = 0;
    for (int row = 0; row < dimension - 1; row++) {
        for (int col = 0; col < dimension - 1; col++) {
            vertexData->indices[cnt++] = dimension * row + col;
            vertexData->indices[cnt++] = dimension * row + col + dimension;
            vertexData->indices[cnt++] = dimension * row + col + dimension + 1;

            vertexData->indices[cnt++] = dimension * row + col;
            vertexData->indices[cnt++] = dimension * row + col + dimension + 1;
            vertexData->indices[cnt++] = dimension * row + col + 1;
        }
    }

    return new Mesh(vertexData);
}

Mesh *Primitives::createQuad() {
    std::vector<Vertex> vertices(4);
    std::vector<glm::vec3> pos = {glm::vec3(-1, 0, -1), glm::vec3(-1, 0, 1), glm::vec3(1, 0, -1), glm::vec3(1, 0, 1)};
    std::vector<glm::vec2> texCoords = {glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 0), glm::vec2(1, 1)};
    std::vector<unsigned int> indices = {0, 1, 2, 2, 1, 3};

    VertexData *vertexData = new VertexData(4, 6);

    std::vector<glm::vec3> normals = calculateVertexNormalAverages(pos, indices);

    for (int i = 0; i < 4; i++) {
        vertexData->vertices[i].position = pos[i];
        vertexData->vertices[i].normal = normals[i];
        vertexData->vertices[i].textureCoords = texCoords[i];
    }

    for (int i = 0; i < 6; ++i) {
        vertexData->indices[i] = indices[i];
    }

    return new Mesh(vertexData);
}

Mesh *Primitives::createQuad2d() {
    std::vector<Vertex> vertices(4);
    std::vector<glm::vec3> pos = {glm::vec3(-1, -1, 0), glm::vec3(1, 1, 0), glm::vec3(-1, 1, 0), glm::vec3(1, -1, 0)};
    std::vector<glm::vec2> texCoords = {glm::vec2(0, 0), glm::vec2(1, 1), glm::vec2(0, 1), glm::vec2(1, 0)};
    std::vector<unsigned int> indices = {0, 1, 2, 0, 3, 1};

    VertexData *vertexData = new VertexData(4, 6);

    for (int i = 0; i < 4; i++) {
        vertexData->vertices[i].position = pos[i];
        vertexData->vertices[i].textureCoords = texCoords[i];
        vertexData->vertices[i].normal = glm::vec3(0, 0, -1);
    }

    for (int i = 0; i < 6; ++i) {
        vertexData->indices[i] = indices[i];
    }

    return new Mesh(vertexData);
}

Mesh *Primitives::createCube(int side) {

    glm::vec3 vertList[] = {glm::vec3(-1, -1, -1), glm::vec3(1, -1, -1), glm::vec3(1, 1, -1), glm::vec3(-1, 1, -1),
                            glm::vec3(-1, -1, 1),  glm::vec3(1, -1, 1),  glm::vec3(1, 1, 1),  glm::vec3(-1, 1, 1)};
    glm::vec2 texCoords[] = {glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1), glm::vec2(0, 1)};

    std::vector<unsigned int> indices = {0, 1, 3, 3, 1, 2, 1, 5, 2, 2, 5, 6, 5, 4, 6, 6, 4, 7,
                                         4, 0, 7, 7, 0, 3, 3, 2, 7, 7, 2, 6, 4, 5, 0, 0, 5, 1};

    VertexData *vertexData = new VertexData(8, indices.size());

    int texInds[6] = {0, 1, 3, 3, 1, 2};

    std::vector<Vertex> vertices(8);
    for (int i = 0; i < 8; i++) {
        vertexData->vertices[i].position = vertList[i];
        vertexData->vertices[i].textureCoords = texCoords[texInds[i % 4]];
    }

    for (int i = 0; i < indices.size(); ++i) {
        vertexData->indices[i] = indices[i];
    }

    return new Mesh(vertexData);
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