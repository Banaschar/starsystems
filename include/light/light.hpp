#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

#include "drawable.hpp"
#include "global.hpp"
#include "assetloader.hpp"

const glm::vec3 DEFAULT_AMBIENT = glm::vec3(0.2f, 0.2f, 0.2f);
const glm::vec3 DEFAULT_DIFFUSE = glm::vec3(1.0f, 1.0f, 1.0f);
const glm::vec3 DEFAULT_SPECULAR = glm::vec3(0.5f, 0.5f, 0.5f);
const glm::vec3 DEFAULT_COLOR = glm::vec3(0.98f, 0.83f, 0.25f);

class Light : public Drawable {
  public:
    Light(glm::vec3 lightPos, ShaderType type = ShaderType::SHADER_TYPE_LIGHT) {
        Drawable::transform(NULL, &lightPos, NULL);
        Drawable::setType(type);
    }
    Light(Mesh *mesh, ShaderType type = ShaderType::SHADER_TYPE_LIGHT) : Drawable(mesh, type) {}
    Light(std::vector<Mesh*> &meshes, ShaderType type = ShaderType::SHADER_TYPE_LIGHT) : Drawable(meshes, type) {}
    Light(const std::string &path, ShaderType type = ShaderType::SHADER_TYPE_LIGHT) {
        std::vector<Mesh*> meshes;
        AssetLoader::loadModel(path, &meshes);

        for (Mesh *mesh : meshes)
            Drawable::addMesh(mesh);
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