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

struct Vertex2D {
    glm::vec2 position;
    glm::vec2 textureCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

/* // FUTURE IMPROVEMENT
enum class VertexType {VERTEX_DEFAULT, VERTEX_2D};

class VertexData {
public:
    VertexData(VertexType vType, int size) : type_(vType) {
        switch (type_) {
            case VertexType::VERTEX_DEFAULT:
                data = new std::array<Vertex>(size);
                break;
            case VertexType::VERTEX_2D:
                data = new std::array<Vertex2D>(size);
        }
    }

    void *data;

    VertexType type() {
        return type_;
    }
private:
    VertexType type_;
};
*/
#endif