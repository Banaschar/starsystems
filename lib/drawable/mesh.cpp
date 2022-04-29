#include <vector>

#include "mesh.hpp"
#include "oglheader.hpp"

Mesh::Mesh() = default;
Mesh::Mesh(VertexData *vertexData, std::vector<Texture> textures) : vertexData_(vertexData), textures_(textures) {
    drawMode_ = GL_TRIANGLES;
    vertexData_->optimize();
}

/*
 * initMesh() is not called on Mesh Object creation.
 * That's to ensure a mesh can be created by background Threads,
 * because the OpenGL functions to initialize the vertex array objects can
 * only be called by the main thread that owns the OpenGL context.
 */
void Mesh::initMesh() {
    if (incomplete_) {
        incomplete_ = false;
        drawInstances_ = 0;
        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);
        glGenBuffers(1, &ebo_);

        updateMesh();
    }
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

        glEnableVertexAttribArray(vertexAttributeIndex_);
        glVertexAttribPointer(vertexAttributeIndex_++, 3, GL_FLOAT, GL_FALSE, vertexData_->sizeOfVertexType(), (void *)0);

        if (vertexData_->getNumAttributes() > 1) {
            glEnableVertexAttribArray(vertexAttributeIndex_);
            glVertexAttribPointer(vertexAttributeIndex_++, 3, GL_FLOAT, GL_FALSE, vertexData_->sizeOfVertexType(), (void *)vertexData_->getOffset1());
        }

        if (vertexData_->getNumAttributes() > 2) {
            glEnableVertexAttribArray(vertexAttributeIndex_);
            glVertexAttribPointer(vertexAttributeIndex_++, 2, GL_FLOAT, GL_FALSE, vertexData_->sizeOfVertexType(), (void *)vertexData_->getOffset2());
        }

        glBindVertexArray(0);
    }
}

void Mesh::makeInstances(std::vector<glm::mat4> *instanceMatrices, VertexAttributeData *attribData) {
    if (incomplete_)
        initMesh();

    isInstanced_ = true;
    drawInstances_ = instanceMatrices->size();
    glGenBuffers(1, &ibo_);
    glBindBuffer(GL_ARRAY_BUFFER, ibo_);
    glBufferData(GL_ARRAY_BUFFER, drawInstances_ * sizeof(glm::mat4), nullptr, GL_STREAM_DRAW);

    glBindVertexArray(vao_);
    glEnableVertexAttribArray(vertexAttributeIndex_);
    glVertexAttribPointer(vertexAttributeIndex_++, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)0);
    glEnableVertexAttribArray(vertexAttributeIndex_);
    glVertexAttribPointer(vertexAttributeIndex_++, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)sizeof(glm::vec4));
    glEnableVertexAttribArray(vertexAttributeIndex_);
    glVertexAttribPointer(vertexAttributeIndex_++, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(vertexAttributeIndex_);
    glVertexAttribPointer(vertexAttributeIndex_++, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(3 * sizeof(glm::vec4)));

    glVertexAttribDivisor(vertexAttributeIndex_ - 4, 1);
    glVertexAttribDivisor(vertexAttributeIndex_ - 3, 1);
    glVertexAttribDivisor(vertexAttributeIndex_ - 2, 1);
    glVertexAttribDivisor(vertexAttributeIndex_ - 1, 1);

    if (attribData) {
        glGenBuffers(1, &abo_);
        glBindBuffer(GL_ARRAY_BUFFER, abo_);
        glBufferData(GL_ARRAY_BUFFER, attribData->size * attribData->sizeOfDataType, NULL, GL_STREAM_DRAW);
        glEnableVertexAttribArray(vertexAttributeIndex_);
        glVertexAttribPointer(vertexAttributeIndex_++, attribData->numElements, GL_FLOAT, GL_FALSE, attribData->sizeOfDataType, (void *) 0);
        glVertexAttribDivisor(vertexAttributeIndex_ - 1, 1);
    }

    glBindVertexArray(0);
}

/*
 * Update the ibo attribute buffer that holds the matrices
 * for instanced draw calls
 * TODO: Prevent usage of extra attrib data if buffer is not declared
 */
void Mesh::updateInstances(std::vector<glm::mat4> *instanceMatrices, VertexAttributeData *attribData) {
    if (!isInstanced_)
        makeInstances(instanceMatrices, attribData);

    drawInstances_ = instanceMatrices->size();
    glBindBuffer(GL_ARRAY_BUFFER, ibo_);
    glBufferData(GL_ARRAY_BUFFER, drawInstances_ * sizeof(glm::mat4), nullptr, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, drawInstances_ * sizeof(glm::mat4), &instanceMatrices->front());

    if (attribData) {
        glBindBuffer(GL_ARRAY_BUFFER, abo_);
        glBufferData(GL_ARRAY_BUFFER, drawInstances_ * attribData->sizeOfDataType, nullptr, GL_STREAM_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, drawInstances_ * attribData->sizeOfDataType, attribData->data);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::addTexture(Texture tex) {
    textures_.push_back(tex);
}

bool Mesh::incomplete() const {
    return incomplete_;
}

unsigned int Mesh::getVao() const {
    return vao_;
}

unsigned int Mesh::getIndicesSize() {
    return vertexData_->indices.size();
}

unsigned int Mesh::getInstanceSize() const {
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

int Mesh::getDrawMode() const {
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
