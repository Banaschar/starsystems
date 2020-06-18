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

void Model::initModel() {

}



void Model::draw(Shader *shader) {
    for (unsigned int i = 0; i < meshes_.size(); i++) {
        for (Texture *tex : texturePerMesh_[i])
            shader->bindTexture(tex.type, tex.id);
        meshes_[i].draw(shader);
    }
}

void Model::transform(glm::vec3 *scaleVec, glm::vec3 *translateVec, 
                    glm::vec3 *rotationAxis, float degree){

    transform(0, scaleVec, translateVec, rotationAxis, degree);
}

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