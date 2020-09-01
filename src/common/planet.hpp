#ifndef PLANET_H
#define PLANET_H

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "drawable.hpp"

class Planet : public Drawable {
  public:
    Planet(Mesh mesh, float orbitSpeed, glm::vec3 position, ShaderType type = ShaderType::SHADER_TYPE_DEFAULT);
    Planet(std::vector<Mesh> meshes, float orbitSpeed, glm::vec3 position, ShaderType type = ShaderType::SHADER_TYPE_DEFAULT);

    void update(Game *game) override;

  private:
    float orbitSpeed_;
    float orbitRadius_;
    float rotationVal_;
    void initPlanet(glm::vec3 position);
};
#endif