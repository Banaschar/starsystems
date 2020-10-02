#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "enginetypes.hpp"

class Mesh {
  private:
    std::vector<Texture> textures_;
    VertexData *vertexData_;
    bool isInstanced_ = false;
    bool incomplete_ = true;
    unsigned int drawInstances_ = 0;
    int drawMode_;
    unsigned int vao_, vbo_, ebo_, ibo_, abo_;
    int vertexAttributeIndex_ = 0;
    void initMesh();

  public:
    Mesh();
    Mesh(VertexData *vertexData, std::vector<Texture> textures = std::vector<Texture>(0));
    void updateMesh();
    void updateInstances(std::vector<glm::mat4> *instanceMatrices, VertexAttributeData *attribData = nullptr);
    void optimize();
    void addTexture(Texture tex);
    std::vector<Texture> &getTextures();
    void makeInstances(std::vector<glm::mat4> *instanceMatrices, VertexAttributeData *attribData = nullptr);
    unsigned int getVao();
    unsigned int getIndicesSize();
    unsigned int getInstanceSize();
    bool &isInstanced();
    bool incomplete();
    unsigned int getTriangleCount();
    int getDrawMode();
    void setDrawMode(MeshDrawMode mode);
};
#endif