#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <stdio.h>
#include <iostream>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include "model.hpp"
#include "assetloader.hpp"

Model::Model(const char *path) {
    loadModel(path);
    modelMat_ = glm::mat4(1.0);
}

void Model::loadModel(const char *path) {
    if (!loadObj(path, &meshes_))
        fprintf(stdout, "Could not load model.\n");
}

void Model::draw(Shader shader) {
    shader.uniform("MVP", mvp_);
    shader.uniform("M", modelMat_);
    for (unsigned int i = 0; i < meshes_.size(); i++) {
        meshes_[i].draw(shader);
    }
}

void Model::transform(glm::vec3 *scaleVec, glm::vec3 *translateVec, 
                    glm::vec3 *rotationAxis, float degree){

    if (scaleVec != NULL)
        modelMat_ = glm::scale(modelMat_, *scaleVec);

    if (rotationAxis != NULL) {
        if (degree == 0.0f) {
            fprintf(stderr, "Rotation axis specified but no degree\n");
        } else {
            modelMat_ = glm::rotate(modelMat_, degree, *rotationAxis);
        }
    }

    if (translateVec != NULL)
        modelMat_ = glm::translate(modelMat_, *translateVec);

}

void Model::update(View view) {
    mvp_ = view.getProjectionMatrix() * view.getCameraMatrix() * modelMat_;
}