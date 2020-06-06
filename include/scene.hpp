#ifndef SCENE_H
#define SCENE_H

#include <glm/glm.hpp>

#include "view.hpp"
#include "model.hpp"
#include "shader.hpp"

class Scene {
public:
    Scene(View view, Shader shader);
    void update();
    void render();
    void addModel(Model model);
private:
    View view_;
    std::vector<Model> models_;
    Shader shader_;
    glm::vec3 lightPos_;
};
#endif