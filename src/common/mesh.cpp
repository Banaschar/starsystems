#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>

#include <iostream>

#include "mesh.hpp"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) :
            vertices_(vertices), indices_(indices) {
    initMesh();
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Texture> textures, std::vector<unsigned int> indices) :
            vertices_(vertices), textures_(textures), indices_(indices) {
    initMesh();
}
/*
Mesh::Mesh(std::vector<glm::vec2> positions, std::vector<unsigned int> indices) :
            indices_(indices) {
    initMesh2d(positions);
}
*/

void Mesh::generateIndices() {
    for (int i = 0; i < vertices_.size(); i++) {
        indices_.push_back(i);
        indices_.push_back(i+1);
        indices_.push_back(i+2);
    }
}

void Mesh::initMesh() {
    drawInstances_ = 0;
    if (indices_.size() == 0)
        generateIndices();
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
    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), &vertices_[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_STATIC_DRAW);

    // set vertex attrib pointer
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoords));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

    glBindVertexArray(0);
}

void Mesh::makeInstances(std::vector<glm::mat4> *instanceMatrices) {
    instanceMatrices_ = instanceMatrices;

    drawInstances_ = instanceMatrices_->size();

    glGenBuffers(1, &ibo_);
    glBindBuffer(GL_ARRAY_BUFFER, ibo_);
    glBufferData(GL_ARRAY_BUFFER, drawInstances_ * sizeof(glm::mat4), NULL, GL_STREAM_DRAW);

    glBindVertexArray(vao_);

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)sizeof(glm::vec4));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);

    glBindVertexArray(0);
}

void Mesh::draw(Shader *shader) {
    glBindVertexArray(vao_);
    bool depthMask = false;

    // bind appropriate textures
    unsigned int diffuseNr  = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr   = 1;
    unsigned int heightNr   = 1;
    unsigned int guiNr      = 1;

    for (int i = 0; i < textures_.size(); i++) {
        std::string number;
        std::string type = textures_[i].type;
        if (type == "cubemap") {
            glDepthFunc(GL_LEQUAL);
            glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_CUBE_MAP, textures_[i].id);
            depthMask = true;
            break;
        }
        glActiveTexture(GL_TEXTURE0 + i);

        if(type == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(type == "texture_specular")
            number = std::to_string(specularNr++);
        else if(type == "texture_normal")
            number = std::to_string(normalNr++);
        else if(type == "texture_height")
            number = std::to_string(heightNr++);
        else if(type == "texture_gui")
            number = std::to_string(guiNr++);
        else {
            std::cout << "Texture with unknown type: " << type << std::endl;
            break;
        }

        shader->uniform((type + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures_[i].id);
    }
    
    if (drawInstances_) {
        updateIbo();
        glDrawElementsInstanced(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0, drawInstances_);
    }
    else
        glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    if (depthMask) {
        glDepthFunc(GL_LESS);
        glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    }

    glActiveTexture(GL_TEXTURE0);
}

/*
 * Update the ibo attribute buffer that holds the matrices 
 * for instanced draw calls
 */
void Mesh::updateIbo() {
    drawInstances_ = instanceMatrices_->size();
    glBindBuffer(GL_ARRAY_BUFFER, ibo_);
    glBufferData(GL_ARRAY_BUFFER, drawInstances_ * sizeof(glm::mat4), NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, drawInstances_ * sizeof(glm::mat4), &instanceMatrices_->front());
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

std::vector<Vertex>& Mesh::getVertices() {
    return vertices_;
}

std::vector<unsigned int>& Mesh::getIndices() {
    return indices_;
}
