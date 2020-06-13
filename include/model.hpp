#ifndef MODEL_H
#define MODEL_H

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <stdlib.h>

#include <GLFW/glfw3.h>

#include "drawable.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "game.hpp"


class Model: public Drawable {
public:
    Model(const char *path, std::string type, std::vector<glm::vec3> instancePositions = {});
    Model(Mesh mesh, std::string type, std::vector<glm::vec3> instancePositions = {});
    void draw(Shader *shader);
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
    std::string type();
    //void register_callback(const callback_t &cb);
    //void test();
private:
    std::string type_;
    std::vector<Mesh> meshes_;
    std::vector<glm::vec3> modelPositions_;
    std::vector<glm::mat4> modelMatrices_;
    std::vector<glm::mat4> mvps_;
    std::vector<glm::mat3> normalMatrices_;

    void initModel();
    void loadModel(const char *path);
};
#endif