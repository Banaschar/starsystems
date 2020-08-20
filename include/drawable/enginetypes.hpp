#ifndef ENGINETYPES_H
#define ENGINETYPES_H

#include <glm/glm.hpp>
#include <string>

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

#endif