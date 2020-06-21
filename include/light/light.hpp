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
    Light(glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 0.0f)) {
        Drawable::transform(NULL, &lightPos, NULL);
        Drawable::setType(SHADER_TYPE_LIGHT);
    }
    Light(Mesh mesh, std::string type = SHADER_TYPE_LIGHT) : Drawable(mesh, type) {}
    Light(std::vector<Mesh> meshes, std::string type = SHADER_TYPE_LIGHT) : Drawable(meshes, type) {}

    glm::vec3 setPosition(glm::vec3 pos) {
        glm::vec3 newpos = pos - Drawable::getPosition();
        Drawable::transform(NULL, &newpos, NULL);
    }

    bool hasModel() {
        return Drawable::meshes_.size(); 
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

    void update(Game *game) override {
        ;
    }

private:
    glm::vec3 ambient_ = DEFAULT_AMBIENT;
    glm::vec3 diffuse_ = DEFAULT_DIFFUSE;
    glm::vec3 specular_ = DEFAULT_SPECULAR;
    glm::vec3 color_ = DEFAULT_COLOR;
};
#endif