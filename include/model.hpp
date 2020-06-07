#ifndef MODEL_H
#define MODEL_H

#include <glm/glm.hpp>
#include <vector>

#include <GLFW/glfw3.h>

#include "drawable.hpp"
#include "mesh.hpp"
#include "view.hpp"
#include "shader.hpp"

class Model: public Drawable {
public:
    Model(const char *path, Shader shader);
    Model(Mesh mesh, Shader shader);
    void draw(View view, glm::vec3 lightPos);
    void update(View view);
    void transform(glm::vec3 *scaleVec, glm::vec3 *translateVec, 
                    glm::vec3 *rotationAxis, float degree = 0.0f);
    glm::vec3 getPosition();
private:
    Shader shader_;
    std::vector<Mesh> meshes_;
    glm::vec3 modelPosition_;
    glm::mat4 modelMatrix_; 
    glm::mat3 normalMatrix_;
    glm::mat4 mvp_; // model-view-projection matrix
    void initModel();
    void loadModel(const char *path);
    bool needsUpdate_;
};
#endif