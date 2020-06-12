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

typedef std::function<void(Drawable*, Shader&, Game*)> callback_t;

class Model: public Drawable {
public:
    Model(const char *path, Shader shader, const callback_t cb, std::vector<glm::vec3> instancePositions = {});
    Model(Mesh mesh, Shader shader, const callback_t cb, std::vector<glm::vec3> instancePositions = {});
    void draw(Game *game);
    void update(Game *game);
    void transform(int index, glm::vec3 *scaleVec, glm::vec3 *translateVec, 
                    glm::vec3 *rotationAxis, float degree = 0.0f);
    void transform(glm::vec3 *scaleVec, glm::vec3 *translateVec, 
                    glm::vec3 *rotationAxis, float degree = 0.0f);
    glm::vec3 getPosition(int index = 0);
    glm::mat3 getNormalMatrix(int index = 0);
    glm::mat4 getModelMatrix(int index = 0);
    std::vector<glm::mat4>& getInstanceModels();
    glm::mat4 getMvp(int index = 0);
    //void register_callback(const callback_t &cb);
    //void test();
private:
    Shader shader_;
    std::vector<Mesh> meshes_;
    std::vector<glm::vec3> modelPositions_;
    std::vector<glm::mat4> modelMatrices_;
    std::vector<glm::mat4> mvps_;
    std::vector<glm::mat3> normalMatrices_;
    bool calcNormalMatrix_ = false;

    void initModel();
    void loadModel(const char *path);
    callback_t drawCallback_;
};
#endif