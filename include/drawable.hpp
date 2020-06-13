#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <string>
#include <glm/glm.hpp>

class Shader;
class Game;

class Drawable {
    public:
        virtual void draw(Shader *shader) = 0;
        virtual void update(Game *game) = 0;
        virtual glm::vec3 getPosition(int index = 0) = 0;
        virtual std::string type() = 0;
};
#endif