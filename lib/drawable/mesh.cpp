#include <string>
#include <vector>

#include "mesh.hpp"
#include "oglheader.hpp"

Mesh::Mesh() {}

Mesh::Mesh(VertexData *vertexData, std::vector<Texture> textures) : vertexData_(vertexData), textures_(textures) {
    drawMode_ = GL_TRIANGLES;
    vertexData_->optimize();
}

void Mesh::initMesh() {
    if (incomplete_)
        incomplete_ = false;

    drawInstances_ = 0;
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    updateMesh();
}

void Mesh::updateMesh() {
    if (incomplete_)
        initMesh();
    else {
        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, vertexData_->sizeOfData() * vertexData_->sizeOfVertexType(), vertexData_->getVertices(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexData_->indices.size() * sizeof(unsigned int), vertexData_->indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexData_->sizeOfVertexType(), (void *)0);

        if (vertexData_->getNumAttributes() > 1) {
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexData_->sizeOfVertexType(), (void *)vertexData_->getOffset1());
        }

        if (vertexData_->getNumAttributes() > 2) {
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertexData_->sizeOfVertexType(), (void *)vertexData_->getOffset2());
        }

        glBindVertexArray(0);
    }
}

void Mesh::makeInstances(std::vector<glm::mat4> *instanceMatrices) {
    if (incomplete_)
        initMesh();

    isInstanced_ = true;
    drawInstances_ = instanceMatrices->size();

    glGenBuffers(1, &ibo_);
    glBindBuffer(GL_ARRAY_BUFFER, ibo_);
    glBufferData(GL_ARRAY_BUFFER, drawInstances_ * sizeof(glm::mat4), NULL, GL_STREAM_DRAW);

    glBindVertexArray(vao_);

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)0);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)sizeof(glm::vec4));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(3 * sizeof(glm::vec4)));

    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);

    glBindVertexArray(0);
}

/*
 * Update the ibo attribute buffer that holds the matrices
 * for instanced draw calls
 */
void Mesh::updateInstances(std::vector<glm::mat4> *instanceMatrices) {
    if (incomplete_)
        initMesh();

    if (!isInstanced_)
        makeInstances(instanceMatrices);

    drawInstances_ = instanceMatrices->size();
    glBindBuffer(GL_ARRAY_BUFFER, ibo_);
    glBufferData(GL_ARRAY_BUFFER, drawInstances_ * sizeof(glm::mat4), NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, drawInstances_ * sizeof(glm::mat4), &instanceMatrices->front());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::addTexture(Texture tex) {
    textures_.push_back(tex);
}

bool Mesh::incomplete() {
    return incomplete_;
}

unsigned int Mesh::getVao() {
    return vao_;
}

unsigned int Mesh::getIndicesSize() {
    return vertexData_->indices.size();
}

unsigned int Mesh::getInstanceSize() {
    return drawInstances_;
}

bool &Mesh::isInstanced() {
    return isInstanced_;
}

std::vector<Texture> &Mesh::getTextures() {
    return textures_;
}

unsigned int Mesh::getTriangleCount() {
    return vertexData_->indices.size() / 3;
}

int Mesh::getDrawMode() {
    return drawMode_;
}

void Mesh::setDrawMode(MeshDrawMode mode) {
    switch (mode) {
        case MeshDrawMode::DRAW_MODE_TESSELLATION:
            drawMode_ = GL_PATCHES;
            break;
        default:
            drawMode_ = GL_TRIANGLES;
            break;
    }
}
