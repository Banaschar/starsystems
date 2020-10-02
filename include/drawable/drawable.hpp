#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <glm/glm.hpp>
#include <string>

#include "mesh.hpp"
#include "global.hpp"

class Game;

/*
 *
 */
class Drawable {
  public:
    Drawable();
    Drawable(Mesh *mesh, ShaderType type);
    Drawable(std::vector<Mesh*> meshes, ShaderType type);
    Drawable(Mesh *mesh, Texture texture, ShaderType type);

    ~Drawable();

    virtual void update(Game *game);
    ShaderType type();
    void setType(ShaderType type);
    void addTexture(Texture tex, int index = 0);
    void addMesh(Mesh *mesh);
    std::vector<Texture> &getTextures(int index = 0);
    /*
     * Returns the position of the Drawable in WorldSpace
     */
    glm::vec3 getPosition(int index = 0);
    glm::vec3 getScale(int index = 0);
    glm::mat4 getModelMatrix(int index = 0);
    glm::mat3 getNormalMatrix(int index = 0);
    void transform(glm::vec3 *scale, glm::vec3 *translate, glm::vec3 *rotate, float degree = 0.0f);
    void transform(int index, glm::vec3 *scale, glm::vec3 *translate, glm::vec3 *rotate, float degree = 0.0f);
    void setPosition(glm::vec3 pos, int index = 0);
    std::vector<Mesh*> &getMeshes();
    int getTriangleCount(int index = 0);
    void setMeshDrawMode(MeshDrawMode mode, int index = 0);
    void updateMeshInstances(VertexAttributeData *attrData = nullptr);
    void updateInstanceSize(int size);

  protected:
    ShaderType type_;
    std::vector<Mesh*> meshes_;
    std::vector<glm::vec3> modelPositions_;
    std::vector<glm::mat4> modelMatrices_;
    std::vector<glm::vec3> scale_;
    std::vector<glm::vec3> rotationAxis_;
    std::vector<float> rotationDegree_;

  private:
    void initDrawable();
};
#endif