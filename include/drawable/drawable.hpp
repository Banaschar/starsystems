#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <string>
#include <glm/glm.hpp>

#include "mesh.hpp"

class Shader;

const std::string DEFAULT_TYPE = "type_default";

/*
 * 
 */
class Drawable {
public:
        Drawable() {
            type_ = DEFAULT_TYPE;
            initDrawable();
        }
        Drawable(Mesh mesh, std::string type, std::vector<glm::vec3> instancePositions = {})
                     : type_(type), drawablePositions_(instancePositions) {
            meshes_.push_back(mesh);
            if (instancePositions.empty())
                initDrawable();
            else
                initInstances();
        }
        Drawable(std::vector<Mesh> meshes, std::string type, std::vector<glm::vec3> instancePositions = {})
                     : type_(type), drawablePositions_(instancePositions) {
            meshes_ = meshes;
            if (instancePositions.empty())
                initDrawable();
            else
                initInstances();
        }
        Drawable(Mesh mesh, Texture texture, std::string type, std::vector<glm::vec3> instancePositions = {}) 
                    : type_(type), drawablePositions_(instancePositions) {
            meshes_.push_back(mesh);
            addTexture(texture);
            if (instancePositions.empty())
                initDrawable();
            else
                initInstances();
        }

        virtual void update() {

        }

        virtual void draw(Shader *shader) {
            for (Mesh &mesh : meshes_) {
                mesh.draw(shader);
            }
        }

        virtual std::string type() {
            return type_;
        }

        void addTexture(Texture tex, int index = 0) {
            meshes_.at(index).addTexture(tex);
        }

        std::vector<Texture>& getTextures(int index = 0) {
            return meshes_.at(index).getTextures();
        }
        
        glm::vec3 getPosition(int index = 0) {
            return modelPositions_.at(index);
        }

        glm::vec3 getScale(int index = 0) {
            return scale_.at(index);
        }

        glm::mat4 getModelMatrix(int index = 0) {
            return modelMatrices_.at(index);
        }

        glm::mat3 getNormalMatrix(int index = 0) {
            return glm::mat3(glm::transpose(glm::inverse(modelMatrices_.at(index))));
        }

        void transform(glm::vec3 *scale, glm::vec3 *translate, glm::vec3 *rotate, float *degree, index = 0) {
            if (translateVec) {
                modelPositions_[index] += *translateVec;
            }
            
            if (scaleVec) {
                scale_[index].x *= scaleVec->x;
                scale_[index].y *= scaleVec->y;
                scale_[index].z *= scaleVec->z;
            }

            if (rotationAxis) {
                if (!degree) {
                    fprintf(stderr, "Rotation axis specified but no degree\n");
                } else {
                    rotationAxis_[index] = *rotationAxis;
                    rotationDegree_[index] = *degree;
                }
            }
            glm::mat4 tmp = glm::mat4(1.0f);
            tmp = glm::scale(tmp, scale_[index]);
            tmp = glm::rotate(tmp, glm::radians(rotationDegree_[index]), rotationAxis_[index]);
            tmp = glm::translate(tmp, modelPositions_[index]);
            modelMatrices_[index] = tmp; 
        }

protected:
    std::string type_;
    std::vector<Mesh> meshes_;
    std::vector<glm::vec3> drawablePositions_;
    std::vector<glm::mat4> drawableMatrices_;
    std::vector<glm::vec3> scale_;
    std::vector<glm::vec3> rotationAxis_;
    std::vector<float> rotationDegree_;

    void submitInstanceBuffer(std::vector<mat4> *instanceMatrices) {
        for (Mesh &mesh : meshes_) {
            mesh.updateInstances(instanceMatrices);
        }
    }
private:
    void initDrawable() {
        drawablePositions_.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        drawableMatrices_.push_back(glm::mat4(1.0f));
        scale_.push_back(glm::vec3(1.0f));
        rotationAxis_.push_back(glm::vec3(1.0f));
        rotationDegree_.push_back(0.0f);
    }

    void initInstances() {
        int size = drawablePositions_.size();
        drawableMatrices_.resize(size);
        scale_.resize(size);
        rotationAxis_.resize(size);
        rotationDegree_.resize(size);

        glm::mat4 base = glm::mat4(1.0f);
        for (int i = 0; i < drawablePositions_.size(); i++) {
            drawableMatrices_[i] = glm::translate(base, drawablePositions_[i]);
        }

        for (Mesh &mesh : meshes_) {
            mesh.makeInstances(&drawablePositions_);
        }
    }
};
#endif