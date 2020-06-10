#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <stdio.h>
#include <iostream>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include "model.hpp"
#include "assetloader.hpp"

Model::Model(Mesh mesh, Shader shader, const callback_t cb) : 
                shader_(shader), drawCallback_(cb) {
    meshes_.push_back(mesh);
    initModel();
}

Model::Model(const char *path, Shader shader, const callback_t cb) : 
                shader_(shader), drawCallback_(cb) {
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

void Model::draw(Game *game) {
    drawCallback_(this, shader_, game);

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

void Model::update(Game *game) {
    mvp_ = game->getView().getProjectionMatrix() * game->getView().getCameraMatrix() * modelMatrix_;
    normalMatrix_ = glm::mat3(glm::transpose(glm::inverse(game->getView().getCameraMatrix() * modelMatrix_)));
}

glm::vec3 Model::getPosition() {
    return modelPosition_;
}

glm::mat3 Model::getNormalMatrix() {
    return normalMatrix_;
}

glm::mat4 Model::getModelMatrix() {
    return modelMatrix_;
}

glm::mat4 Model::getMvp() {
    return mvp_;
}