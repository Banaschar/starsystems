#ifndef PLANET_H
#define PLANET_H

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "drawable.hpp"

const std::string DEFAULT_PLANET_TYPE = "planet";

class Planet : public Drawable {
  public:
    Planet(Mesh mesh, float orbitSpeed, glm::vec3 position, std::string type = DEFAULT_PLANET_TYPE);
    Planet(std::vector<Mesh> meshes, float orbitSpeed, glm::vec3 position, std::string type = DEFAULT_PLANET_TYPE);

    void update(Game *game) override;

  private:
    float orbitSpeed_;
    float orbitRadius_;
    float rotationVal_;
    void initPlanet(glm::vec3 position);
};
#endif