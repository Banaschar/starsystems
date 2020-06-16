#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

#include "global.hpp"
#include "drawable.hpp"

#define DEFAULT_AMBIENT glm::vec3(0.2f, 0.2f, 0.2f)
#define DEFAULT_DIFFUSE glm::vec3(1.0f, 1.0f, 1.0f)
#define DEFAULT_SPECULAR glm::vec3(0.5f, 0.5f, 0.5f)
#define DEFAULT_COLOR glm::vec3(1.0f, 1.0f, 1.0f)

class Light: public Drawable {
public:
    Light(glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 0.0f)) : position_(lightPos) {}
    Light(Drawable *model) : model_(model) {
        position_ = model_->getPosition();
    }

    glm::vec3 setPosition(glm::vec3 pos) {
        if (model_) {
            glm::vec3 newpos = pos - position_;
            transform(NULL, &newpos, NULL);
        }
        position_ = pos;
    }

    bool hasModel() {
        return model_; 
    }

    void setAmbient(glm::vec3 a) {
        ambient_ = a;
    }

    void setDiffuse(glm::vec3 d) {
        diffuse_ = d;
    }

    void setSpecular(glm::vec3 s) {
        specular_ = s;
    }

    void setColor(glm::vec3 c) {
        color_ = c;
    }

    glm::vec3 getAmbient() {
        return ambient_;
    }

    glm::vec3 getDiffuse() {
        return diffuse_;
    }

    glm::vec3 getSpecular() {
        return specular_;
    }

    glm::vec3 getColor() {
        return color_;
    }

    // Drawable functions
    glm::vec3 getPosition(int index = 0) {
        if (model_)
            return model_->getPosition(index);
        else
            return position_;
    }

    void transform(glm::vec3 *scale, glm::vec3 *translate, glm::vec3 *rotate, float degree = 0.0) {
        if (model_)
            model_->transform(scale, translate, rotate, degree);
    }

    void draw(Shader *shader) {
        if (model_)
            model_->draw(shader);
    }

    void update(Game *game) {
        if (model_)
            model_->update(game);
    }

    glm::vec3 getScale(int index = 0) {
        if (model_)
            return model_->getScale(index);
        else
            return glm::vec3(1.0f);
    }

    glm::mat4 getMvp(int index = 0) {
        if (model_)
            return model_->getMvp(index);
        else
            return glm::mat4(1.0f);
    }

    glm::mat4 getModelMatrix(int index = 0) {
        if (model_)
            return model_->getMvp(index);
        else
            return glm::mat4(1.0f);
    }

    glm::mat3 getNormalMatrix(int index = 0) {
        if (model_)
            return model_->getNormalMatrix(index);
        else
            return glm::mat3(1.0f);
    }

    std::string type() {
        if (model_)
            return model_->type();
        else
            return SHADER_TYPE_LIGHT;
    }
private:
    Drawable *model_ = NULL;
    glm::vec3 position_;
    glm::vec3 ambient_ = DEFAULT_AMBIENT;
    glm::vec3 diffuse_ = DEFAULT_DIFFUSE;
    glm::vec3 specular_ = DEFAULT_SPECULAR;
    glm::vec3 color_ = DEFAULT_COLOR;
};
#endif