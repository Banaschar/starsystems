#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <string>
#include <glm/glm.hpp>

class Shader;
class Game;

/*
 * TODO: refactor -> only provide a getModel function
 * and the model class simply returns this.
 * That way all drawables need to have a model class...hmmmmm
 */
class Drawable {
    public:
        virtual void draw(Shader *shader) = 0;
        virtual void update(Game *game) = 0;
        virtual glm::vec3 getPosition(int index = 0) = 0;
        virtual glm::mat4 getMvp(int index = 0) = 0;
        virtual glm::mat4 getModelMatrix(int index = 0) = 0;
        virtual glm::mat3 getNormalMatrix(int index = 0) = 0;
        virtual std::string type() = 0;
};
#endif