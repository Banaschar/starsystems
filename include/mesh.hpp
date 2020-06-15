#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "drawable.hpp"
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
    std::vector<Texture> textures_;
    std::vector<glm::mat4> *instanceMatrices_;
    unsigned int drawInstances_;
    unsigned int vao_, vbo_, ebo_, ibo_;
    void generateIndices();
    void initMesh();
    //void initMesh2d(std::vector<glm::vec2> positions);
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices = {});
    Mesh(std::vector<Vertex> vertices, std::vector<Texture> textures, 
        std::vector<unsigned int> indices = {});
    // 2D mesh constructor
    //Mesh(std::vector<glm::vec2> positions, std::vector<unsigned int> indices = {});
    void draw(Shader *shader);
    void updateMesh();
    void updateIbo();
    void optimize();
    void addTexture(Texture tex);
    void addColor(glm::vec4 color);
    void makeInstances(std::vector<glm::mat4> *instanceMatrices);
    std::vector<Vertex>& getVertices();
    std::vector<unsigned int>& getIndices(); 
};
#endif