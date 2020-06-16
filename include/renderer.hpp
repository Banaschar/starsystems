#ifndef RENDERER_H
#define RENDERER_H

#include <map>
#include <vector>
#include <string>
#include <iostream>

#include "global.hpp"

#include <GLFW/glfw3.h>

#include "drawable.hpp"
#include "game.hpp"
#include "shader.hpp"
#include "waterframebuffer.hpp"
#include "gui.hpp"
#include "terrain.hpp"
#include "water.hpp"

class Shader;

typedef std::map<std::string, Shader*> ShaderMap;
typedef std::map<std::string, std::vector<Drawable*>> EntityMap;
typedef std::vector<Drawable*> DrawableList;

/*
 * Current limitation:
 * All lights, terrains, water and skyboc are limited to one shader each
 * Do I need a hashmap for each of them?
 */
class Renderer {
public:
    Renderer(GLFWwindow *window, std::vector<Shader*> shaders) : window_(window) {
        setupRenderer(shaders);
    }

    ~Renderer() {
        if (lightShader_)
            delete lightShader_;

        //if (terrainShader_)
        //    delete terrainShader_;

        if (skyShader_)
            delete skyShader_;

        if (waterRenderer_)
            delete waterRenderer_;

        if (waterFrameBuffer_)
            delete waterFrameBuffer_;

        for (const auto& kv : shaderMap_) {
            delete shaderMap_[kv.first];
        }

        if (guiRenderer_)
            delete guiRenderer_;

        if (terrainRenderer_)
            delete terrainRenderer_;
    }

    void render(DrawableList &lights, DrawableList &terrain,
                    DrawableList &entities, DrawableList &sky, 
                    DrawableList &water, Game *game) {

        render(lights, terrain, entities, sky, water, NULL, game);
    }

    void render(DrawableList &lights, DrawableList &terrain,
                    DrawableList &entities, DrawableList &sky, 
                    DrawableList &water, DrawableList *gui, Game *game) {

        processEntities(entities);
        // Render to waterFrameBuffer
        if (waterFrameBuffer_) {
            glEnable(GL_CLIP_DISTANCE0);
            float distance = 2 * (game->getView().getCameraPosition().y - water[0]->getPosition().y);
            game->getView().getCameraPosition().y -= distance;
            game->getView().invertPitch();
            game->getView().updateForce();
            waterFrameBuffer_->bindReflectionFrameBuffer();
            renderScene(lights, terrain, sky, game, glm::vec4(0, 1, 0, -water[0]->getPosition().y));
            game->getView().getCameraPosition().y += distance;
            game->getView().invertPitch();
            game->getView().updateForce();

            waterFrameBuffer_->bindRefractionFrameBuffer();
            renderScene(lights, terrain, sky, game, glm::vec4(0, -1, 0, water[0]->getPosition().y));

            waterFrameBuffer_->unbindActiveFrameBuffer();
            glDisable(GL_CLIP_DISTANCE0);
        }

        
        renderScene(lights, terrain, sky, game, glm::vec4(0, -1, 0, 10000));
        
        if (waterRenderer_)
            waterRenderer_->render(water, game);
        
        if (gui && guiRenderer_)
            guiRenderer_->render(gui, game);

        entityMap_.clear();
    }

private:
    ShaderMap shaderMap_;
    EntityMap entityMap_;
    GLFWwindow *window_;
    Shader *lightShader_ = NULL;
    Shader *skyShader_ = NULL;
    WaterFrameBuffer *waterFrameBuffer_ = NULL;
    GuiRenderer *guiRenderer_ = NULL;
    TerrainRenderer *terrainRenderer_ = NULL;
    WaterRenderer *waterRenderer_ = NULL;

    void setupRenderer(std::vector<Shader*> shaders) {
        
        for (Shader *shader : shaders) {
            if (shader->type() == SHADER_TYPE_LIGHT)
                lightShader_ = shader;
            else if (shader->type() == SHADER_TYPE_TERRAIN)
                terrainRenderer_ = new TerrainRenderer(shader);
            else if (shader->type() == SHADER_TYPE_SKY)
                skyShader_ = shader;
            else if (shader->type() == SHADER_TYPE_WATER) {
                int width, height;
                glfwGetWindowSize(window_, &width, &height);
                waterFrameBuffer_ = new WaterFrameBuffer(width, height);
                waterRenderer_ = new WaterRenderer(shader, waterFrameBuffer_);
            }
            else if (shader->type() == SHADER_TYPE_GUI) {
                guiRenderer_ = new GuiRenderer(shader);
            }
            else {
                if (shaderMap_.count(shader->type()))
                    std::cout << "WARNING: Two shaders with same type" << std::endl;
                else
                    shaderMap_[shader->type()] = shader;
            }
        }
    }

    /*
     * Puts entities in a hash map for batched drawing
     */
    void processEntities(std::vector<Drawable*> &entities) {
        EntityMap::iterator it;
        
        for (Drawable *drawable : entities) {
            it = entityMap_.find(drawable->type());

            if (it != entityMap_.end())
                it->second.push_back(drawable);
            else {
                std::vector<Drawable*> tmp = {drawable};
                entityMap_.insert(it, EntityMap::value_type(drawable->type(), tmp));
            }
        }
    }

    void renderScene(DrawableList &lights, DrawableList &terrain,
                     DrawableList &sky, Game *game, glm::vec4 clipPlane) {
        renderList(lightShader_, lights, game, clipPlane);         // render lights
        if (terrainRenderer_)
            terrainRenderer_->render(terrain, game, clipPlane);      // render terrain
        renderEntities(game, clipPlane);                           // render models
        renderList(skyShader_, sky, game, clipPlane);              // render skybox
    }

    void renderList(Shader *shader, DrawableList &drawables, Game *game, glm::vec4 clipPlane) {
        if (!drawables.empty() && shader) {
            shader->use();
            shader->uniform("clipPlane", clipPlane);
            for (Drawable *drawable : drawables) {
                drawable->update(game);
                shader->prepare(drawable, game);
                drawable->draw(shader);
            }
        }
    }

    void renderEntities(Game *game, glm::vec4 clipPlane) {
        for (auto& kv : entityMap_) {
            renderList(shaderMap_[kv.first], kv.second, game, clipPlane);
        }
    }

};
#endif