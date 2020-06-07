#include "planet.hpp"

Planet::Planet(Model model, float orbitSpeed): model_(model), orbitSpeed_(orbitSpeed) {
    orbitRadius_ = glm::distance(model_.getPosition(), glm::vec3(0.0f, 0.0f, 0.0f));
    rotationVal_ = 0.0;
}

void Planet::update(View view) {
    
    rotationVal_ += orbitSpeed_ * deltaTime;

    glm::vec3 trans = glm::vec3(0.0f);
    trans.x = sin(rotationVal_) * orbitRadius_;
    trans.z = cos(rotationVal_) * orbitRadius_;
    trans = trans - model_.getPosition();
    model_.transform(NULL, &trans, NULL);

    model_.update(view);
}

void Planet::draw(View view, glm::vec3 lightPos) {
    model_.draw(view, lightPos);
}