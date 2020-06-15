#ifndef RENDERER_H
#define RENDERER_H

#include <map>
#include <vector>
#include <string>
#include <iostream>

#include "drawable.hpp"
#include "game.hpp"
#include "shader.hpp"
#include "global.hpp"
#include "waterframebuffer.hpp"
#include "gui.hpp"

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
    Renderer(std::vector<Shader*> shaders) {
        setupRenderer(shaders);
    }
    Renderer(std::vector<Shader*> shaders, WaterFrameBuffer *waterFb) : waterFrameBuffer_(waterFb) {
        setupRenderer(shaders);
    }
    ~Renderer() {
        if (lightShader_)
            delete lightShader_;

        if (terrainShader_)
            delete terrainShader_;

        if (skyShader_)
            delete skyShader_;

        if (waterShader_)
            delete waterShader_;

        if (waterFrameBuffer_)
            delete waterFrameBuffer_;

        for (const auto& kv : shaderMap_) {
            delete shaderMap_[kv.first];
        }

        if (guiRenderer_)
            delete guiRenderer_;
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
            waterFrameBuffer_->bindReflectionFrameBuffer();
            renderScene(lights, terrain, sky, game);
            renderList(waterShader_, water, game);
            waterFrameBuffer_->unbindActiveFrameBuffer();
        }

        renderScene(lights, terrain, sky, game);
        renderList(waterShader_, water, game);

        if (gui && guiRenderer_)
            guiRenderer_->render(gui, game);

        entityMap_.clear();
    }

    void setWaterFrameBuffer(WaterFrameBuffer *waterFb) {
        waterFrameBuffer_ = waterFb;
    }
private:
    ShaderMap shaderMap_;
    EntityMap entityMap_;
    Shader *lightShader_ = NULL;
    Shader *terrainShader_ = NULL;
    Shader *waterShader_ = NULL;
    Shader *skyShader_ = NULL;
    WaterFrameBuffer *waterFrameBuffer_ = NULL;
    GuiRenderer *guiRenderer_ = NULL;

    void setupRenderer(std::vector<Shader*> shaders) {
        
        for (Shader *shader : shaders) {
            if (shader->type() == SHADER_TYPE_LIGHT)
                lightShader_ = shader;
            else if (shader->type() == SHADER_TYPE_TERRAIN)
                terrainShader_ = shader;
            else if (shader->type() == SHADER_TYPE_SKY)
                skyShader_ = shader;
            else if (shader->type() == SHADER_TYPE_WATER)
                waterShader_ = shader;
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
                     DrawableList &sky, Game *game) {
        renderList(lightShader_, lights, game);         // render lights
        renderList(terrainShader_, terrain, game);      // render terrain
        renderEntities(game);                           // render models
        renderList(skyShader_, sky, game);              // render skybox
    }

    void renderList(Shader *shader, DrawableList &drawables, Game *game) {
        if (!drawables.empty() && shader) {
            shader->use();

            for (Drawable *drawable : drawables) {
                drawable->update(game);
                shader->prepare(drawable, game);
                drawable->draw(shader);
            }
        }
    }

    void renderEntities(Game *game) {
        for (auto& kv : entityMap_) {
            renderList(shaderMap_[kv.first], kv.second, game);
        }
    }

};
#endif