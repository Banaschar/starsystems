#include <iostream>

#include "scene.hpp"
#include "waterframebuffer.hpp"

Scene::Scene(Game *game, std::vector<Shader*> shaderList) : 
                            game_(game), lightSources_(game_->getLightSources()),
                            models_(game_->getDrawables()), skybox_(game_->getSkyBox()),
                            water_(game_->getWater()) {
    autoRotate_ = false;
    setupScene();
    setupShaderMap(shaderList);
}

Scene::~Scene() {
    delete game_;
    for (const auto& kv : shaderMap_) {
        delete shaderMap_[kv.first];
    }
    delete waterFrameBuffer_;
}

void Scene::setupScene() {
    int width, height;
    game_->getView().getWindowSize(&width, &height);
    waterFrameBuffer_ = new WaterFrameBuffer(width, height);
}

/*
 * Setup the hash map for batch drawing
 * TODO: Better handling and separation of light, terrain, water, skybox etc.
 */
void Scene::setupShaderMap(std::vector<Shader*> shaderList) {
    for (Shader *shader : shaderList) {

        if (shaderMap_.count(shader->type())) {
            continue;
        } else if (shader->type() == "water") {
            waterShader_ = shader;
        } else {
            shaderMap_[shader->type()] = shader;
            std::vector<Drawable*> tmp;
            for (Drawable *drawable : models_) {
                if (shader->type() == drawable->type())
                    tmp.push_back(drawable);
            }
            for (Drawable *drawable : lightSources_) {
                if (shader->type() == drawable->type())
                    tmp.push_back(drawable);
            }
            entityMap_[shader->type()] = tmp;
        }
    }
}

void Scene::insertDrawable(Drawable *drawable) {
    if (entityMap_.count(drawable->type()))
        entityMap_[drawable->type()].push_back(drawable);
}

/*
 * Delete from hash map. Need uuid for drawables
 * Even better if I used another map instead of vector,
 * mapping drawables to uuids
 */
/*
void deleteDrawable(std::string type, int uuid) {
    for (int i = 0; i < entityMap[type].size(); i++) {
        if (entityMap[type]->uuid() == uuid) {
            entityMap[type].erase(entityMap[type].begin() + i);
            break;
        }
    }
}
*/

void Scene::update() {
    game_->getView().update();
}

/*
 * TODO:
 * Each drawable needs a unique identifier, so it can be deleted from
 * the hash map.
 * Currently, shader type name need a integer qualifier, like
 * "0light, 1plane, 2water" to enforce the order of rendering.
 * --> Not very nice. Consider creating more hash maps,
 * one for each important type (light -> models -> skybox -> water)
 */
void Scene::render() {

    for (const auto& kv : entityMap_) {

        shaderMap_[kv.first]->use();

        for (Drawable *drawable : kv.second) {

            drawable->update(game_);
            shaderMap_[kv.first]->prepare(drawable, game_);
            drawable->draw(shaderMap_[kv.first]);
        }
    }
}

void Scene::setAutoRotate(bool value) {
    game_->getView().setAutoRotate(value);
}