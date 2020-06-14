#ifndef PLANET_H
#define PLANET_H

#include <glm/glm.hpp>
#include <string>

#include "model.hpp"
#include "game.hpp"
#include "drawable.hpp"
#include "global.hpp"
#include "shader.hpp"

class Planet : public Drawable {
public:
    Planet(Model model, float orbitSpeed);
    void draw(Shader *shader);
    void update(Game *game);
    std::string type();
    glm::vec3 getPosition(int index = 0);
    glm::mat4 getMvp(int index = 0);
    glm::mat4 getModelMatrix(int index = 0);
    glm::mat3 getNormalMatrix(int index = 0);
private:
    Model model_;
    float orbitSpeed_;
    float orbitRadius_;
    float rotationVal_;
};
#endif