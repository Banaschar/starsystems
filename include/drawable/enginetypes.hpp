#ifndef ENGINETYPES_H
#define ENGINETYPES_H

#include <glm/glm.hpp>
#include <string>

enum class MeshDrawMode {DRAW_MODE_DEFAULT, DRAW_MODE_TESSELLATION};

struct VertexAttributeData {
    size_t size;
    size_t sizeOfDataType;
    int numElements;
    void *data;
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textureCoords;
};

struct Vertex3dSingle {
    glm::vec3 position;
};

struct Vertex2d {
    glm::vec2 position;
    glm::vec2 textureCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};


enum class VertexType {VERTEX_DEFAULT, VERTEX_3D_SINGLE, VERTEX_2D};

class VertexData {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    VertexData(VertexType vType = VertexType::VERTEX_DEFAULT): type_(vType) {
        numAttributes_ = 3;
    }
    VertexData(int vertexSize, int indexSize, VertexType vType = VertexType::VERTEX_DEFAULT) : type_(vType) {
        setSize(vertexSize, indexSize);
    }

    void setSize(int vertexSize, int indexSize) {
        switch (type_) {
            case VertexType::VERTEX_2D:
                data = new std::vector<Vertex2d>(vertexSize);
                numAttributes_ = 2;
                break;
            case VertexType::VERTEX_3D_SINGLE:
                data = new std::vector<Vertex3dSingle>(vertexSize);
                numAttributes_ = 1;
                break;
            default:
                numAttributes_ = 3;
                break;
        }
        vertices.resize(vertexSize);
        indices.resize(indexSize);
    }

    void optimize() {
        switch (type_) {
            case VertexType::VERTEX_2D:
                alignVertex2d();
                break;
            case VertexType::VERTEX_3D_SINGLE:
                alignVertex3dSingle();
                break;
            default:
                break;
        }
    }

    void alignVertex3dSingle() {
        std::vector<Vertex3dSingle> *tmp = static_cast<std::vector<Vertex3dSingle> *>(data);
        for (int i = 0; i < vertices.size(); ++i) {
            (*tmp)[i].position = vertices[i].position;
        }
    }

    /* TODO: To use this, provide getArgumentSize function, so it can be used in attribPointer */
    void alignVertex2d() {
        ;
    }
    
    VertexType type() {
        return type_;
    }

    size_t getOffset1() {
        switch (type_) {
            case VertexType::VERTEX_2D:
                return offsetof(Vertex2d, textureCoords);
                break;
            case VertexType::VERTEX_DEFAULT:
                return offsetof(Vertex, normal);
                break;
            default:
                fprintf(stdout, "[VERTEXDATA::getOffset1] Error: Wrong VertexType\n");
                return 0;
                break;
        }
    }

    size_t getOffset2() {
        switch (type_) {
            case VertexType::VERTEX_DEFAULT:
                return offsetof(Vertex, textureCoords);
                break;
            default:
                fprintf(stdout, "[VERTEXDATA::getOffset2] Error: Wrong VertexType\n");
                return 0;
                break;
        }
    }

    size_t sizeOfData() {
        switch (type_) {
            case VertexType::VERTEX_2D:
                return static_cast<std::vector<Vertex2d> *>(data)->size();
                break;
            case VertexType::VERTEX_3D_SINGLE:
                return static_cast<std::vector<Vertex3dSingle> *>(data)->size();
                break;
            case VertexType::VERTEX_DEFAULT:
                return vertices.size();
                break;
            default:
                fprintf(stdout, "[VERTEXDATA::sizeOfData] Error: Wrong VertexType\n");
                return 0;
        }
    }

    size_t sizeOfVertexType() {
        switch (type_) {
            case VertexType::VERTEX_2D:
                return sizeof(Vertex2d);
                break;
            case VertexType::VERTEX_3D_SINGLE:
                return sizeof(Vertex3dSingle);
                break;
            case VertexType::VERTEX_DEFAULT:
                return sizeof(Vertex);
                break;
            default:
                fprintf(stdout, "[VERTEXDATA::sizeOfVertexData] Error: Wrong VertexType\n");
                return 0;
                break;
        }
    }

    void *getVertices() {
        switch (type_) {
            case VertexType::VERTEX_2D:
                return static_cast<void *>(static_cast<std::vector<Vertex2d> *>(data)->data());
                break;
            case VertexType::VERTEX_3D_SINGLE:
                return static_cast<void *>(static_cast<std::vector<Vertex3dSingle> *>(data)->data());
                break;
            case VertexType::VERTEX_DEFAULT:
                return static_cast<void *>(vertices.data());
                break;
            default:
                fprintf(stdout, "[VERTEXDATA::getVertices] Error: Wrong VertexType\n");
                return nullptr;
                break;
        }
    }

    int getNumAttributes() {
        return numAttributes_;
    }

private:
    int numAttributes_;
    VertexType type_;
    void *data = nullptr;
};
#endif