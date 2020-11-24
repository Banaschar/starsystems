#include <glm/gtc/matrix_transform.hpp>
#include <utility>

#include "drawable.hpp"

Drawable::Drawable() {
    type_ = ShaderType::SHADER_TYPE_DEFAULT;
    initDrawable();
}
Drawable::Drawable(Mesh *mesh, ShaderType type) : type_(type) {
    meshes_.push_back(mesh);
    initDrawable();
}

Drawable::Drawable(std::vector<Mesh*> meshes, ShaderType type) : type_(type) {
    meshes_ = std::move(meshes);
    initDrawable();
}

Drawable::Drawable(Mesh *mesh, Texture texture, ShaderType type) : type_(type) {
    meshes_.push_back(mesh);
    addTexture(std::move(texture));
    initDrawable();
}

Drawable::~Drawable() {
    for (Mesh *mesh : meshes_) {
        delete mesh;
    }
}

ShaderType Drawable::type() {
    return type_;
}

void Drawable::setType(ShaderType type) {
    type_ = type;
}

void Drawable::addTexture(Texture tex, int index) {
    meshes_.at(index)->addTexture(std::move(tex));
}

void Drawable::addMesh(Mesh *mesh) {
    if (mesh)
        meshes_.push_back(mesh);
    else
        fprintf(stdout, "[DRAWABLE::addMesh] WARNING: Empty Mesh added\n");
}

void Drawable::setMeshDrawMode(MeshDrawMode mode, int index) {
    meshes_.at(index)->setDrawMode(mode);
}

std::vector<Texture> &Drawable::getTextures(int index) {
    return meshes_.at(index)->getTextures();
}

glm::vec3 Drawable::getPosition(int index) {
    return modelPositions_.at(index);
}

unsigned int Drawable::getTriangleCount(int index) {
    return meshes_.at(index)->getTriangleCount();
}

glm::vec3 Drawable::getScale(int index) {
    return scale_.at(index);
}

glm::mat4 Drawable::getModelMatrix(int index) {
    return modelMatrices_.at(index);
}

glm::mat3 Drawable::getNormalMatrix(int index) {
    return glm::mat3(glm::transpose(glm::inverse(modelMatrices_.at(index))));
}

void Drawable::transform(glm::vec3 *scaleVec, glm::vec3 *translateVec, glm::vec3 *rotationAxis, float degree) {
    transform(0, scaleVec, translateVec, rotationAxis, degree);
}

void Drawable::transform(int index, glm::vec3 *scaleVec, glm::vec3 *translateVec, glm::vec3 *rotationAxis,
                         float degree) {
    if (translateVec) {
        modelPositions_[index] = *translateVec;
    }

    if (scaleVec) {
        scale_[index] = *scaleVec;
    }

    if (rotationAxis) {
        if (degree != 0.0f) {
            rotationAxis_[index] = *rotationAxis;
            rotationDegree_[index] = degree;
        }
    }
    glm::mat4 tmp = glm::mat4(1.0f);
    tmp = glm::translate(tmp, modelPositions_[index]);
    tmp = glm::scale(tmp, scale_[index]);
    tmp = glm::rotate(tmp, glm::radians(rotationDegree_[index]), rotationAxis_[index]);
    modelMatrices_[index] = tmp;
}

void Drawable::setPosition(glm::vec3 pos, int index) {
    modelPositions_.at(index) = pos;
    transform(index, nullptr, nullptr, nullptr);
}

std::vector<Mesh*> &Drawable::getMeshes() {
    return meshes_;
}

void Drawable::updateMeshInstances(VertexAttributeData *attrData) {
    for (Mesh *mesh : meshes_) {
        mesh->updateInstances(&modelMatrices_, attrData);
    }
}

void Drawable::updateInstanceSize(int size) {
    if (size <= 0)
        size = 1;
    
    modelPositions_.resize(size, glm::vec3(0.0f, 0.0f, 0.0f));
    modelMatrices_.resize(size, glm::mat4(1.0f));
    scale_.resize(size, glm::vec3(1.0f));
    rotationAxis_.resize(size, glm::vec3(1.0f));
    rotationDegree_.resize(size, 0.0f);
}

void Drawable::initDrawable() {
    modelPositions_.emplace_back(0.0f, 0.0f, 0.0f);
    modelMatrices_.emplace_back(1.0f);
    scale_.emplace_back(1.0f);
    rotationAxis_.emplace_back(1.0f);
    rotationDegree_.emplace_back(0.0f);
}