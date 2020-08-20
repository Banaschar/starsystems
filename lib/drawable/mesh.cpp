#include <string>
#include <vector>

#include "mesh.hpp"
#include "oglheader.hpp"

Mesh::Mesh() {}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) 
                        : vertices_(vertices), indices_(indices) {
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Texture> textures, std::vector<unsigned int> indices)
    : vertices_(vertices), textures_(textures), indices_(indices) {
}
/*
Mesh::Mesh(std::vector<glm::vec2> positions, std::vector<unsigned int> indices) :
            indices_(indices) {
    initMesh2d(positions);
}
*/

void Mesh::initMesh() {
    if (incomplete_)
        incomplete_ = false;

    drawInstances_ = 0;
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    updateMesh();
}
/*
void Mesh::initMesh2d(std::vector<glm::vec2d> positions) {
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    if (!indices_.empty())
        glGenBuffers(1, &vbo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), &vertices_[0], GL_STATIC_DRAW);

    if (!indices_.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_STATIC_DRAW);
    }

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoords));

}
*/

/*
 * TODO: Internally, create a Vertex struct that only holds the data types we actually
 * have and use
 * Also check if the vertices array contains any entries that are overlapping, e.g. any
 * vertices no index in the indices array is pointing to. Remove these and shrink the array
 * appropriately.
 * --> Can't create a struct dynamically. So the best possibilty would be
 * to create one char buffer and put everything inside interleaved
 *
 */
void optimize() {
    /*
    char buffer[] = new char[sizeOfEverything]
    for (int i = 0; i < vertices_.size(); i++) {
        memcpy(buffer + (i * sizeOfOneTriangleData), vertices_[i].position, sizeof(glm::vec3));
        memcpy(buffer + (i * sizeOfOneTriangleData) + sizeof(glm::vec3), vertices_[i].normal, sizeof(glm::vec3));
        etc.
    }
    */
    ;
}

/*
 * TODO: Either implement the optimize solution,
 * or simply use a different buffer object for each attribute
 *
 */
void Mesh::updateMesh() {
    if (incomplete_)
        initMesh();

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), &vertices_[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_STATIC_DRAW);

    // set vertex attrib pointer
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, textureCoords));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, color));

    glBindVertexArray(0);
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

void Mesh::addColor(glm::vec4 color) {
    for (Vertex &vert : vertices_) {
        vert.color = color;
    }
    updateMesh();
}

bool Mesh::incomplete() {
    return incomplete_;
}

unsigned int Mesh::getVao() {
    return vao_;
}

unsigned int Mesh::getIndicesSize() {
    return indices_.size();
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

int Mesh::getTriangleCount() {
    return indices_.size() / 3;
}