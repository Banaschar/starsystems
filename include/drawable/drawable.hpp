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
    Drawable(Mesh *mesh, ShaderType type, std::vector<glm::vec3> instancePositions = {});
    Drawable(std::vector<Mesh*> meshes, ShaderType type, std::vector<glm::vec3> instancePositions = {});
    Drawable(Mesh *mesh, Texture texture, ShaderType type, std::vector<glm::vec3> instancePositions = {});

    ~Drawable();

    virtual void update(Game *game);

    ShaderType type();

    void setType(ShaderType type);

    void addTexture(Texture tex, int index = 0);

    void addColor(glm::vec4 color, int index = 0);

    void addMesh(Mesh *mesh);

    std::vector<Texture> &getTextures(int index = 0);

    glm::vec3 getPosition(int index = 0);

    glm::vec3 getScale(int index = 0);

    glm::mat4 getModelMatrix(int index = 0);

    glm::mat3 getNormalMatrix(int index = 0);

    void transform(glm::vec3 *scale, glm::vec3 *translate, glm::vec3 *rotate, float degree = 0.0f);

    void transform(int index, glm::vec3 *scale, glm::vec3 *translate, glm::vec3 *rotate, float degree = 0.0f);

    void setPosition(glm::vec3 pos, int index = 0);

    std::vector<Mesh*> &getMeshes();

    int getTriangleCount(int index = 0);

  protected:
    ShaderType type_;
    std::vector<Mesh*> meshes_;
    std::vector<glm::vec3> modelPositions_;
    std::vector<glm::mat4> modelMatrices_;
    std::vector<glm::vec3> scale_;
    std::vector<glm::vec3> rotationAxis_;
    std::vector<float> rotationDegree_;
    void submitInstanceBuffer(std::vector<glm::mat4> *instanceMatrices);

  private:
    void initDrawable();
    void initInstances();
};
#endif