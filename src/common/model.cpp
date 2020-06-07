#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <stdio.h>
#include <iostream>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include "model.hpp"
#include "assetloader.hpp"

Model::Model(Mesh mesh, Shader shader) : shader_(shader) {
    meshes_.push_back(mesh);
    initModel();
}

Model::Model(const char *path, Shader shader) : shader_(shader) {
    loadModel(path);
    initModel();
}

void Model::initModel() {
    modelMatrix_ = glm::mat4(1.0);
    modelPosition_ = glm::vec3(0.0f, 0.0f, 0.0f);
    needsUpdate_ = true;
}

void Model::loadModel(const char *path) {
    if (!loadObj(path, &meshes_))
        fprintf(stdout, "Could not load model.\n");
}

void Model::draw(View view, glm::vec3 lightPos) {
    shader_.use();
    shader_.uniform("MVP", mvp_);

    if (shader_.type() != "light") {
        shader_.uniform("lightPosition", lightPos);
        shader_.uniform("modelMatrix", modelMatrix_);
        shader_.uniform("normalMatrix", normalMatrix_);
        shader_.uniform("cameraMatrix", view.getCameraMatrix());

        //shader_.uniform("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
        shader_.uniform("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
        shader_.uniform("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
        shader_.uniform("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        shader_.uniform("material.shininess", 32.0f);
        shader_.uniform("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
        shader_.uniform("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));  
        shader_.uniform("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));     
    } else {
        shader_.uniform("color", glm::vec4(1.0f, 1.0f, 0.2f, 0.7f));
    }
    for (unsigned int i = 0; i < meshes_.size(); i++) {
        meshes_[i].draw(shader_);
    }
}

void Model::transform(glm::vec3 *scaleVec, glm::vec3 *translateVec, 
                    glm::vec3 *rotationAxis, float degree){

    if (scaleVec != NULL)
        modelMatrix_ = glm::scale(modelMatrix_, *scaleVec);

    if (rotationAxis != NULL) {
        if (degree == 0.0f) {
            fprintf(stderr, "Rotation axis specified but no degree\n");
        } else {
            modelMatrix_ = glm::rotate(modelMatrix_, glm::radians(degree), *rotationAxis);
        }
    }

    if (translateVec != NULL) {
        modelPosition_ = modelPosition_ + *translateVec;
        modelMatrix_ = glm::translate(modelMatrix_, *translateVec);
    }

    needsUpdate_ = true;
}

void Model::update(View view) {
    mvp_ = view.getProjectionMatrix() * view.getCameraMatrix() * modelMatrix_;
    normalMatrix_ = glm::mat3(glm::transpose(glm::inverse(view.getCameraMatrix() * modelMatrix_)));
}

glm::vec3 Model::getPosition() {
    return modelPosition_;
}