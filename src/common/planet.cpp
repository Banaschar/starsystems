#include "planet.hpp"

Planet::Planet(Model model, float orbitSpeed): model_(model), orbitSpeed_(orbitSpeed) {
    orbitRadius_ = glm::distance(model_.getPosition(), glm::vec3(0.0f, 0.0f, 0.0f));
    rotationVal_ = 0.0;
}

void Planet::update(Game *game) {
    
    rotationVal_ += orbitSpeed_ * deltaTime;

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

std::string Planet::type() {
    return model_.type();
}