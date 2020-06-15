#ifndef LIGHT_H
#define LIGHT_H

#define DEFAULT_AMBIENT glm::vec3(0.2f, 0.2f, 0.2f)
#define DEFAULT_DIFFUSE glm::vec3(1.0f, 1.0f, 1.0f)
#define DEFAULT_SPECULAR glm::Vec3(0.5f, 0.5f, 0.5f);

class Light {
public:
    Light(glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 0.0f)) : position_(lightPos) {}
    Light(Model *model) : model_(model) {
        position_ = model_->getPosition();
    }

    glm::vec3 getPosition() {
        if (model_)
            return model_->getPosition();
        else
            return position_;
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

    glm::vec3 getAmbient() {
        return ambient_;
    }

    glm::vec3 getDiffuse() {
        return diffuse_;
    }

    glm::vec3 getSpecular() {
        return specular_;
    }
private:
    Model *model_ = NULL;
    glm::vec3 position_;
    glm::vec3 ambient_ = DEFAULT_AMBIENT;
    glm::vec3 diffuse_ = DEFAULT_DIFFUSE;
    glm::vec3 specular_ = DEFAULT_SPECULAR;
}
#endif