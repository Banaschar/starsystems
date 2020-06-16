#include "planet.hpp"

Planet::Planet(Model model, float orbitSpeed): model_(model), orbitSpeed_(orbitSpeed) {
    orbitRadius_ = glm::distance(model_.getPosition(), glm::vec3(0.0f, 0.0f, 0.0f));
    rotationVal_ = 0.0;
}

void Planet::update(Game *game) {
    
    rotationVal_ += orbitSpeed_ * g_deltaTime;

    glm::vec3 trans = glm::vec3(0.0f);
    trans.x = sin(rotationVal_) * orbitRadius_;
    trans.z = cos(rotationVal_) * orbitRadius_;
    trans = trans - model_.getPosition();
    model_.transform(NULL, &trans, NULL);
    
    model_.update(game);
}

void Planet::draw(Shader *shader) {
    model_.draw(shader);
}

glm::vec3 Planet::getPosition(int index) {
    return model_.getPosition(index);
}

glm::vec3 Planet::getScale(int index) {
    return model_.getScale(index);
}

glm::mat4 Planet::getMvp(int index) {
    return model_.getMvp(index);
}

glm::mat4 Planet::getModelMatrix(int index) {
    return model_.getModelMatrix(index);
}

glm::mat3 Planet::getNormalMatrix(int index) {
    return model_.getNormalMatrix(index);
}

std::string Planet::type() {
    return model_.type();
}

void Planet::transform(glm::vec3 *scale, glm::vec3 *translate, glm::vec3 *rotate, float degree) {
    model_.transform(scale, translate, rotate, degree);
}