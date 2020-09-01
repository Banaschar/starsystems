#include "planet.hpp"
#include "global.hpp"

Planet::Planet(Mesh mesh, float orbitSpeed, glm::vec3 position, ShaderType type)
    : Drawable(mesh, type), orbitSpeed_(orbitSpeed) {
    initPlanet(position);
}

Planet::Planet(std::vector<Mesh> meshes, float orbitSpeed, glm::vec3 position, ShaderType type)
    : Drawable(meshes, type), orbitSpeed_(orbitSpeed) {
    initPlanet(position);
}

void Planet::initPlanet(glm::vec3 position) {
    Drawable::transform(NULL, &position, NULL);
    orbitRadius_ = glm::distance(Drawable::getPosition(), glm::vec3(0.0f, 0.0f, 0.0f));
    rotationVal_ = 0.0;
}

void Planet::update(Game *game) {

    rotationVal_ += orbitSpeed_ * g_deltaTime;

    glm::vec3 trans = glm::vec3(0.0f);
    trans.x = sin(rotationVal_) * orbitRadius_;
    trans.z = cos(rotationVal_) * orbitRadius_;
    trans = trans - Drawable::getPosition();
    transform(NULL, &trans, NULL);
}