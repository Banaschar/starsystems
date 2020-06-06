#include "scene.hpp"

Scene::Scene(View view, Shader shader) : view_(view), shader_(shader) {
    lightPos_ = glm::vec3(4,4,4);
}

void Scene::addModel(Model model) {
    models_.push_back(model);
}

void Scene::update() {
    view_.rotateCamera();
}

void Scene::render() {

    shader_.use();
    shader_.uniform("V", view_.getCameraMatrix());
    shader_.uniform("LightPosition_worldspace", lightPos_);

    for (int i = 0; i < models_.size(); i++) {
        models_[i].update(view_);
        models_[i].draw(shader_);
    }
}