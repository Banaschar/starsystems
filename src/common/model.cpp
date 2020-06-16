#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <stdio.h>
#include <iostream>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include "model.hpp"
#include "assetloader.hpp"

Model::Model(Mesh mesh, std::string type, std::vector<glm::vec3> instancePositions) : 
                type_(type), modelPositions_(instancePositions) {
    meshes_.push_back(mesh);
    initModel();
}

Model::Model(const char *path, std::string type, std::vector<glm::vec3> instancePositions) : 
                type_(type), modelPositions_(instancePositions) {
    loadModel(path);
    initModel();
}

/*
 * TODO:
 * Split in multiple classes:
 * Entitiy (main class, only contains the transformation code)
 * model -> base sub class
 * instance -> instance sub class
 */
void Model::initModel() {
    if (modelPositions_.size()) {
        glm::mat4 baseModel = glm::mat4(1.0);
        mvps_.resize(modelPositions_.size());
        modelMatrices_.resize(modelPositions_.size());
        normalMatrices_.resize(modelPositions_.size());
        scale_.resize(modelPositions_.size());
        rotationAxis_.resize(modelPositions_.size());
        rotationDegree_.resize(modelPositions_.size());
        for (int i = 0; i < modelPositions_.size(); i++) {
            mvps_[i] = modelMatrices_[i] = glm::translate(baseModel, modelPositions_[i]);
            scale_[i] = glm::vec3(1.0f);
            rotationAxis_[i] = glm::vec3(1.0f);
            rotationDegree_[i] = 0.0f;
        }

        for (Mesh &mesh : meshes_)
            mesh.makeInstances(&mvps_);
    } else {
        modelPositions_.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        modelMatrices_.push_back(glm::mat4(1.0));
        mvps_.resize(1);
        normalMatrices_.resize(1);
        scale_.push_back(glm::vec3(1.0f));
        rotationAxis_.push_back(glm::vec3(1.0f));
        rotationDegree_.push_back(0.0f);
    }
}

void Model::loadModel(const char *path) {
    if (!loadObj(path, &meshes_))
        fprintf(stdout, "Could not load model.\n");
}

void Model::draw(Shader *shader) {

    for (unsigned int i = 0; i < meshes_.size(); i++) {
        meshes_[i].draw(shader);
    }
}

void Model::transform(glm::vec3 *scaleVec, glm::vec3 *translateVec, 
                    glm::vec3 *rotationAxis, float degree){

    transform(0, scaleVec, translateVec, rotationAxis, degree);
}

/*
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
*/

void Model::transform(int index, glm::vec3 *scaleVec, glm::vec3 *translateVec, 
                    glm::vec3 *rotationAxis, float degree) {

    if (translateVec) {
        modelPositions_[index] += *translateVec;
    }
    
    if (scaleVec) {
        scale_[index].x *= scaleVec->x;
        scale_[index].y *= scaleVec->y;
        scale_[index].z *= scaleVec->z;
    }

    if (rotationAxis) {
        if (degree == 0.0f) {
            fprintf(stderr, "Rotation axis specified but no degree\n");
        } else {
            rotationAxis_[index] = *rotationAxis;
            rotationDegree_[index] = degree;
        }
    }
    glm::mat4 tmp = glm::mat4(1.0f);
    tmp = glm::scale(tmp, scale_[index]);
    tmp = glm::rotate(tmp, glm::radians(rotationDegree_[index]), rotationAxis_[index]);
    tmp = glm::translate(tmp, modelPositions_[index]);
    modelMatrices_[index] = tmp;
}

void Model::update(Game *game) {
    glm::mat4 projection = game->getView().getProjectionMatrix();
    glm::mat4 camera = game->getView().getCameraMatrix();

    for (int i = 0; i < modelMatrices_.size(); i++) {
        mvps_[i] = projection * camera * modelMatrices_[i];
        normalMatrices_[i] = glm::mat3(glm::transpose(glm::inverse(modelMatrices_[i])));
    }

}

std::vector<glm::mat4>& Model::getInstanceModels() {
    return modelMatrices_;
}

glm::vec3 Model::getPosition(int index) {
    return modelPositions_.at(index);
}

glm::vec3 Model::getScale(int index) {
    return scale_.at(index);
}

glm::mat3 Model::getNormalMatrix(int index) {
    return normalMatrices_.at(index);
}

glm::mat4 Model::getModelMatrix(int index) {
    return modelMatrices_.at(index);
}

glm::mat4 Model::getMvp(int index) {
    return mvps_.at(index);
}

std::string Model::type() {
    return type_;
}

Mesh& Model::getMesh(int index) {
    return meshes_.at(index);
}