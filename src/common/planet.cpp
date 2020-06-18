#include "planet.hpp"

Planet::Planet(Mesh mesh, float orbitSpeed, std::string type) 
            : Drawable(mesh, type), orbitSpeed_(orbitSpeed) {
    orbitRadius_ = glm::distance(model_.getPosition(), glm::vec3(0.0f, 0.0f, 0.0f));
    rotationVal_ = 0.0;
}

void Planet::update(Game *game) {
    
    rotationVal_ += orbitSpeed_ * g_deltaTime;

    glm::vec3 trans = glm::vec3(0.0f);
    trans.x = sin(rotationVal_) * orbitRadius_;
    trans.z = cos(rotationVal_) * orbitRadius_;
    trans = trans - model_.getPosition();
    transform(NULL, &trans, NULL);
    
    model_.update(game);
}