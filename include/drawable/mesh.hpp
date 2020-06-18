#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "shader.hpp"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textureCoords;
    glm::vec4 color;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
private:
    std::vector<Vertex> vertices_;
    std::vector<unsigned int> indices_;
    unsigned int drawInstances_;
    unsigned int vao_, vbo_, ebo_, ibo_;
    void generateIndices();
    void initMesh();
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices = {});
    void draw();
    void updateMesh();
    void updateInstances(std::vector<glm::mat4> *instanceMatrices);
    void optimize();
    void addTexture(Texture tex);
    std::vector<Texture>& getTextures();
    void addColor(glm::vec4 color);
    void makeInstances(std::vector<glm::mat4> *instanceMatrices);
    std::vector<Vertex>& getVertices();
    std::vector<unsigned int>& getIndices(); 
};
#endif