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
    Model(const char *path);
    void draw(Shader shader);
    void update(View view);
    void transform(glm::vec3 *scaleVec, glm::vec3 *translateVec, 
                    glm::vec3 *rotationAxis, float degree = 0.0f);
private:
    std::vector<Mesh> meshes_;
    glm::mat4 modelMat_; // model matrix
    glm::mat4 mvp_; // model-view-projection matrix
    void loadModel(const char *path);
};
#endif