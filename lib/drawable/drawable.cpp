#include <glm/gtc/matrix_transform.hpp>

#include "drawable.hpp"

Drawable::Drawable() {
    type_ = ShaderType::SHADER_TYPE_DEFAULT;
    initDrawable();
}
Drawable::Drawable(Mesh *mesh, ShaderType type, std::vector<glm::vec3> instancePositions)
    : type_(type), modelPositions_(instancePositions) {
    meshes_.push_back(mesh);
    if (instancePositions.empty())
        initDrawable();
    else
        initInstances();
}
Drawable::Drawable(std::vector<Mesh*> meshes, ShaderType type, std::vector<glm::vec3> instancePositions)
    : type_(type), modelPositions_(instancePositions) {
    meshes_ = meshes;
    if (instancePositions.empty())
        initDrawable();
    else
        initInstances();
}
Drawable::Drawable(Mesh *mesh, Texture texture, ShaderType type, std::vector<glm::vec3> instancePositions)
    : type_(type), modelPositions_(instancePositions) {
    meshes_.push_back(mesh);
    addTexture(texture);
    if (instancePositions.empty())
        initDrawable();
    else
        initInstances();
}

Drawable::~Drawable() {
    for (Mesh *mesh : meshes_) {
        if (mesh)
            delete mesh;
    }
}

void Drawable::update(Game *game) {
    ;
}

ShaderType Drawable::type() {
    return type_;
}

void Drawable::setType(ShaderType type) {
    type_ = type;
}

void Drawable::addTexture(Texture tex, int index) {
    meshes_.at(index)->addTexture(tex);
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

int Drawable::getTriangleCount(int index) {
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
        modelPositions_[index] += *translateVec;
    }

    if (scaleVec) {
        scale_[index] = *scaleVec;
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
    tmp = glm::translate(tmp, modelPositions_[index]);
    tmp = glm::scale(tmp, scale_[index]);
    tmp = glm::rotate(tmp, glm::radians(rotationDegree_[index]), rotationAxis_[index]);
    modelMatrices_[index] = tmp;
}

void Drawable::setPosition(glm::vec3 pos, int index) {
    modelPositions_.at(index) = pos;
    transform(index, NULL, NULL, NULL);
}

std::vector<Mesh*> &Drawable::getMeshes() {
    return meshes_;
}

void Drawable::submitInstanceBuffer(std::vector<glm::mat4> *instanceMatrices) {
    for (Mesh *mesh : meshes_) {
        mesh->updateInstances(instanceMatrices);
    }
}

void Drawable::initDrawable() {
    modelPositions_.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    modelMatrices_.push_back(glm::mat4(1.0f));
    scale_.push_back(glm::vec3(1.0f));
    rotationAxis_.push_back(glm::vec3(1.0f));
    rotationDegree_.push_back(0.0f);
}

void Drawable::initInstances() {
    int size = modelPositions_.size();
    modelMatrices_.resize(size);
    scale_.resize(size);
    rotationAxis_.resize(size);
    rotationDegree_.resize(size);

    glm::mat4 base = glm::mat4(1.0f);
    for (int i = 0; i < modelPositions_.size(); i++) {
        modelMatrices_[i] = glm::translate(base, modelPositions_[i]);
    }

    for (Mesh *mesh : meshes_) {
        mesh->makeInstances(&modelMatrices_);
    }
}