#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>

#include <iostream>

#include "mesh.hpp"
#include "view.hpp"


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) :
            vertices_(vertices), indices_(indices) {

    if (indices_.size() == 0)
        generateIndices();
    initMesh();
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Texture> textures, std::vector<unsigned int> indices) :
            vertices_(vertices), textures_(textures), indices_(indices) {
    if (indices_.size() == 0)
        generateIndices();
    initMesh();
}

void Mesh::generateIndices() {
    for (int i = 0; i < vertices_.size(); i++) {
        indices_.push_back(i);
    }
}

/*
 * TODO: Only use vertex attrib pointer of stuff we actually have
 * TODO: add flag if indices_ is empty, so we draw with glDrawArrays instead of glDrawElements
 */
void Mesh::initMesh() {

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    glGenBuffers(1, &ebo_);

    updateMesh();
    // break existing binding!
    glBindVertexArray(0);
}

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
}

void Mesh::draw(Shader shader) {
    glBindVertexArray(vao_);
    bool depthMask = false;

    // bind appropriate textures
    unsigned int diffuseNr  = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr   = 1;
    unsigned int heightNr   = 1;

    for (int i = 0; i < textures_.size(); i++) {
        std::string number;
        std::string type = textures_[i].type;
        if (type == "cubemap") {
            glDepthFunc(GL_LEQUAL);
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
        else {
            std::cout << "Texture with unknown type: " << type << std::endl;
            break;
        }

        shader.uniform((type + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures_[i].id);
    }
    

    glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    if (depthMask)
        glDepthFunc(GL_LESS);

    glActiveTexture(GL_TEXTURE0); // Set back to defaults
}

void Mesh::addTexture(Texture tex) {
    textures_.push_back(tex);
}

std::vector<Vertex>& Mesh::getVertices() {
    return vertices_;
}

std::vector<unsigned int>& Mesh::getIndices() {
    return indices_;
}
