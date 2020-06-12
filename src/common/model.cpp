#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <stdio.h>
#include <iostream>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include "model.hpp"
#include "assetloader.hpp"

Model::Model(Mesh mesh, Shader shader, const callback_t cb, std::vector<glm::vec3> instancePositions) : 
                shader_(shader), drawCallback_(cb), modelPositions_(instancePositions) {
    meshes_.push_back(mesh);
    initModel();
}

Model::Model(const char *path, Shader shader, const callback_t cb, std::vector<glm::vec3> instancePositions) : 
                shader_(shader), drawCallback_(cb), modelPositions_(instancePositions) {
    loadModel(path);
    initModel();
}

/*
 * TODO: The check for uniforms here is ugly. Is calculating the normal matrix
 * really expensive?
 */
void Model::initModel() {
    if (modelPositions_.size()) {
        glm::mat4 baseModel = glm::mat4(1.0);
        mvps_.resize(modelPositions_.size());
        modelMatrices_.resize(modelPositions_.size());
        for (int i = 0; i < modelPositions_.size(); i++) {
            mvps_[i] = modelMatrices_[i] = glm::translate(baseModel, modelPositions_[i]);
        }

        for (Mesh &mesh : meshes_)
            mesh.makeInstances(&mvps_);
    } else {
        modelPositions_.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        modelMatrices_.push_back(glm::mat4(1.0));
        mvps_.resize(1);
    }

    GLint count, size;
    GLenum type;
    GLchar name[16];
    GLsizei len;
    glGetProgramiv(shader_.id(), GL_ACTIVE_UNIFORMS, &count);

    for (int i = 0; i < count; i++) {
        glGetActiveUniform(shader_.id(), (GLuint)i, 16, &len, &size, &type, name);
        if (name == "normalMatrix") {
            calcNormalMatrix_ = true;
            normalMatrices_.resize(modelPositions_.size());
        }
    }

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

    transform(0, scaleVec, translateVec, rotationAxis, degree);
}

void Model::transform(int index, glm::vec3 *scaleVec, glm::vec3 *translateVec, 
                    glm::vec3 *rotationAxis, float degree) {
    if (scaleVec)
        modelMatrices_[index] = glm::scale(modelMatrices_[index], *scaleVec);

    if (rotationAxis) {
        if (degree == 0.0f) {
            fprintf(stderr, "Rotation axis specified but no degree\n");
        } else {
            modelMatrices_[index] = glm::rotate(modelMatrices_[index], glm::radians(degree), *rotationAxis);
        }
    }

    if (translateVec) {
        modelPositions_[index] = modelPositions_[index] + *translateVec;
        modelMatrices_[index] = glm::translate(modelMatrices_[index], *translateVec);
    }
}

void Model::update(Game *game) {
    glm::mat4 projection = game->getView().getProjectionMatrix();
    glm::mat4 camera = game->getView().getCameraMatrix();
    for (int i = 0; i < modelMatrices_.size(); i++) {
        mvps_[i] = projection * camera * modelMatrices_[i];
        if (calcNormalMatrix_)
            normalMatrices_[i] = glm::mat3(glm::transpose(glm::inverse(modelMatrices_[i])));
    }

}

std::vector<glm::mat4>& Model::getInstanceModels() {
    return modelMatrices_;
}

glm::vec3 Model::getPosition(int index) {
    return modelPositions_[index];
}

glm::mat3 Model::getNormalMatrix(int index) {
    return normalMatrices_[index];
}

glm::mat4 Model::getModelMatrix(int index) {
    return modelMatrices_[index];
}

glm::mat4 Model::getMvp(int index) {
    return mvps_[index];
}