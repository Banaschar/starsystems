#ifndef MODEL_H
#define MODEL_H

#include <glm/glm.hpp>
#include <vector>
#include <stdlib.h>
#include <functional>

#include <GLFW/glfw3.h>

#include "drawable.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "game.hpp"

typedef std::function<void(Drawable*, Shader&, Game&)> callback_t;

class Model: public Drawable {
public:
    Model(const char *path, Shader shader, const callback_t cb);
    Model(Mesh mesh, Shader shader, const callback_t cb);
    void draw(Game &game);
    void update(Game &game);
    void transform(glm::vec3 *scaleVec, glm::vec3 *translateVec, 
                    glm::vec3 *rotationAxis, float degree = 0.0f);
    glm::vec3 getPosition();
    glm::mat3 getNormalMatrix();
    glm::mat4 getModelMatrix();
    glm::mat4 getMvp();
    //void register_callback(const callback_t &cb);
    //void test();
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
    callback_t drawCallback_;
};
#endif