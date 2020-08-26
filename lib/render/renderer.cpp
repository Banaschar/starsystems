#include "renderer.hpp"
#include "guirenderer.hpp"
#include "oglheader.hpp"
#include "skyrenderer.hpp"
#include "terrainrenderer.hpp"
#include "vaorenderer.hpp"
#include "waterframebuffer.hpp"
#include "waterrenderer.hpp"

Renderer::Renderer(std::vector<Shader *> shaders) {
    vaoRenderer_ = new VaoRenderer();
    setupRenderer(shaders);
}

Renderer::~Renderer() {
    if (lightShader_)
        delete lightShader_;

    if (skyRenderer_)
        delete skyRenderer_;

    if (waterRenderer_)
        delete waterRenderer_;

    if (waterFrameBuffer_)
        delete waterFrameBuffer_;

    for (const auto &kv : shaderMap_) {
        delete shaderMap_[kv.first];
    }

    if (guiRenderer_)
        delete guiRenderer_;

    if (terrainRenderer_)
        delete terrainRenderer_;

    if (vaoRenderer_)
        delete vaoRenderer_;
}

void Renderer::render(DrawableList &lights, DrawableList &terrain, DrawableList &entities, DrawableList &sky,
                      DrawableList &water, DrawableList *gui, Game *game) {
    
    processEntities(entities);
    /* 
     * Render to waterFrameBuffer only if there is water to render
     * and waterTypeQuiality is true 
     */
    if (waterTypeQuality_ && !water.empty()) {
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

    if (waterRenderer_ && !water.empty())
        waterRenderer_->render(water, game);

    if (gui && guiRenderer_)
        guiRenderer_->render(gui, game);

    entityMap_.clear();
}

void Renderer::resolutionChange(int width, int height) {
    windowWidth_ = width;
    windowHeight_ = height;

    if (waterFrameBuffer_)
        waterFrameBuffer_->resolutionChange(windowWidth_, windowHeight_);
}

void Renderer::setupRenderer(std::vector<Shader *> shaders) {

    for (Shader *shader : shaders) {
        if (shader->type() == SHADER_TYPE_LIGHT)
            lightShader_ = shader;
        else if (shader->type() == SHADER_TYPE_TERRAIN)
            terrainRenderer_ = new TerrainRenderer(shader, vaoRenderer_);
        else if (shader->type() == SHADER_TYPE_SKY)
            skyRenderer_ = new SkyRenderer(shader, vaoRenderer_);
        else if (shader->type() == SHADER_TYPE_WATER) {
            waterTypeQuality_ = true;
            waterFrameBuffer_ = new WaterFrameBuffer(windowWidth_, windowHeight_);
            waterRenderer_ = new WaterRenderer(shader, vaoRenderer_, waterFrameBuffer_->getReflectionTexture(),
                                               waterFrameBuffer_->getRefractionTexture(),
                                               waterFrameBuffer_->getRefractionDepthTexture());
        } else if (shader->type() == SHADER_TYPE_WATER_PERFORMANCE) {
            waterRenderer_ = new WaterRenderer(shader, vaoRenderer_);
        } else if (shader->type() == SHADER_TYPE_GUI) {
            guiRenderer_ = new GuiRenderer(shader, vaoRenderer_);
        } else {
            if (shaderMap_.count(shader->type()))
                std::cout << "WARNING: Two shaders with same type" << std::endl;
            else
                shaderMap_[shader->type()] = shader;
        }
    }
}

/*
 * Puts entities not in the base types (terrain, water, sky, lights)
 * in a hash map for batched drawing
 */
void Renderer::processEntities(std::vector<Drawable *> &entities) {
    EntityMap::iterator it;

    for (Drawable *drawable : entities) {
        it = entityMap_.find(drawable->type());

        if (it != entityMap_.end())
            it->second.push_back(drawable);
        else {
            std::vector<Drawable *> tmp = {drawable};
            entityMap_.insert(it, EntityMap::value_type(drawable->type(), tmp));
        }
    }
}

void Renderer::renderScene(DrawableList &lights, DrawableList &terrain, DrawableList &sky, Game *game,
                           glm::vec4 clipPlane) {
    renderList(lightShader_, lights, game, clipPlane); // render lights

    if (terrainRenderer_)
        terrainRenderer_->render(terrain, game, clipPlane); // render terrain

    renderEntities(game, clipPlane);                        // render models
    
    if (skyRenderer_)
        skyRenderer_->render(sky, game, clipPlane); // render skybox
}

void Renderer::renderList(Shader *shader, DrawableList &drawables, Game *game, glm::vec4 clipPlane) {
    if (!drawables.empty() && shader) {
        shader->use();
        shader->uniform("clipPlane", clipPlane);
        for (Drawable *drawable : drawables) {
            drawable->update(game);
            shader->prepare(drawable, game);

            for (Mesh &mesh : drawable->getMeshes()) {
                shader->handleMeshTextures(mesh.getTextures());
                vaoRenderer_->draw(mesh);
            }
        }
        shader->end();
    }
}

void Renderer::renderEntities(Game *game, glm::vec4 clipPlane) {
    for (auto &kv : entityMap_) {
        renderList(shaderMap_[kv.first], kv.second, game, clipPlane);
    }
}