#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "enginetypes.hpp"

class Mesh {
  private:
    std::vector<Texture> textures_;
    std::vector<Vertex> vertices_;
    std::vector<unsigned int> indices_;
    bool isInstanced_ = false;
    bool incomplete_ = true;
    unsigned int drawInstances_ = 0;
    unsigned int vao_, vbo_, ebo_, ibo_;
    void initMesh();

  public:
    /*
     * TODO: Incomplete type. Handle this
     */
    Mesh();
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    Mesh(std::vector<Vertex> vertices, std::vector<Texture> textures, std::vector<unsigned int> indices);
    void updateMesh();
    void updateInstances(std::vector<glm::mat4> *instanceMatrices);
    void optimize();
    void addTexture(Texture tex);
    std::vector<Texture> &getTextures();
    void addColor(glm::vec4 color);
    void makeInstances(std::vector<glm::mat4> *instanceMatrices);
    unsigned int getVao();
    unsigned int getIndicesSize();
    unsigned int getInstanceSize();
    bool &isInstanced();
    bool incomplete();
    int getTriangleCount();
};
#endif