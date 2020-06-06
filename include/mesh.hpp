#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <vector>

#include "drawable.hpp"
#include "shader.hpp"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textureCoords;
};

struct Texture {
    unsigned int id;
    char *type;
    char *path;
};

class Mesh: public Drawable {
private:
    std::vector<Vertex> vertices_;
    std::vector<unsigned int> indices_;
    std::vector<Texture> textures_;
    unsigned int vao_, vbo_, ebo_;

    void initMesh();
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    void draw(Shader shader);
};
#endif