#include "planet.hpp"

Planet::Planet(Model model, float orbitSpeed): model_(model), orbitSpeed_(orbitSpeed) {
    orbitRadius_ = glm::distance(model_.getPosition(), glm::vec3(0.0f, 0.0f, 0.0f));
    rotationVal_ = 0.0;
}

void Planet::update(Game &game) {
    
    rotationVal_ += orbitSpeed_ * deltaTime;

    glm::vec3 trans = glm::vec3(0.0f);
    trans.x = sin(rotationVal_) * orbitRadius_;
    trans.z = cos(rotationVal_) * orbitRadius_;
    trans = trans - model_.getPosition();
    model_.transform(NULL, &trans, NULL);

    model_.update(game);
}

void Planet::draw(Game &game) {
    model_.draw(game);
}

glm::vec3 Planet::getPosition() {
    return model_.getPosition();
}