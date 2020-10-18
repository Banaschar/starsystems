#include "renderer.hpp"
#include "guirenderer.hpp"
#include "oglheader.hpp"
#include "skyrenderer.hpp"
#include "terrainrenderer.hpp"
#include "vaorenderer.hpp"
#include "waterrenderer.hpp"
#include "terraintile.hpp"
#include "postprocessor.hpp"

Renderer::Renderer(std::vector<Shader *> shaders, int winWidth, int winHeight) : windowWidth_(winWidth), windowHeight_(winHeight) {
    vaoRenderer_ = new VaoRenderer();
    setupRenderer(shaders);
}

Renderer::~Renderer() {
    delete lightShader_;
    delete skyRenderer_;
    delete waterRenderer_;
    delete guiRenderer_;
    delete terrainRenderer_;
    delete vaoRenderer_;
    delete postProcessorAtmosphere_;

    for (const auto &kv : shaderMap_) {
        delete shaderMap_[kv.first];
    }
}

unsigned int Renderer::DEBUG_getPostProcessingTexture() {
    if (postProcessorAtmosphere_)
        return postProcessorAtmosphere_->DEBUG_getTexture();
    else
        fprintf(stdout, "DEBUG: No post processor\n");
    return 0;
}

void Renderer::setPolygonRenderModeWireFrame(bool set) {
        if (set) {
            g_debugPolygonMode = true;
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            g_debugPolygonMode = false;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
}

void Renderer::render(TerrainDrawDataContainer &terrainDrawDataContainer, DrawableList &lights, DrawableList &entities, DrawableList &sky,
                      DrawableList *gui, SceneRenderData &sceneRenderData) {
    
    processEntities(entities);
    
    /* 
     * Render to waterFrameBuffer only if there is water to render
     * and waterTypeQuiality is true 
     * AND don't do this if we are rendering a whole planet and we are far away
     */
    /*
    if (terrainDrawData.water && terrainDrawData.water->size && terrainDrawData.water->getDrawableListAtIndex(0)[0]->type() == ShaderType::SHADER_TYPE_WATER) {
        glEnable(GL_CLIP_DISTANCE0);
        
        glm::vec3 normal;
        glm::vec3 saveCamPos = game->getView().getCameraPosition();
        glm::vec3 tmpCamPos;
        float distancePlane;
        float distanceCam;
        TerrainTile *t;
        DrawableList &water = terrainDrawData.water->getDrawableListAtIndex(0);
        if ((t = dynamic_cast<TerrainTile *>(water[0])) && t->getSphereRadius()) {
            // clipPlane normal:
            glm::vec3 normal = glm::normalize(game->getView().getCameraPosition() - t->getSphereOrigin());
            // water level pos below camera
            glm::vec3 waterPos = t->getSphereOrigin() + (float)t->getSphereRadius() * normal; // Assuming the water lies exactly at radius!
            // distance origin to plane
            distancePlane = t->getSphereRadius();
            // distance water to camPos
            distanceCam = glm::distance(game->getView().getCameraPosition(), waterPos);
            // move cam pos by distanceCam in direction of origin (so it's under the water level looking up)
            tmpCamPos = t->getSphereOrigin() + (t->getSphereRadius() - distanceCam) * normal;
            
        } else {
            glm::vec3 normal = glm::vec3(0, 1, 0);
            distancePlane = water[0]->getPosition().y;
            distanceCam = 2 * (game->getView().getCameraPosition().y - water[0]->getPosition().y);
            tmpCamPos = saveCamPos;
            tmpCamPos.y -= distanceCam;
        }
        
        game->getView().getCameraPosition() = tmpCamPos;
        game->getView().invertPitch();
        game->getView().updateForce();
        waterRenderer_->bindReflectionFrameBuffer();
        renderScene(lights, terrain, sky, game, glm::vec4(normal, -distancePlane));
        
        game->getView().getCameraPosition() = saveCamPos;
        game->getView().invertPitch();
        game->getView().updateForce();
        waterRenderer_->bindRefractionFrameBuffer();
        renderScene(lights, terrain, sky, game, glm::vec4(-1.0f * normal, distancePlane));

        waterRenderer_->unbindActiveFrameBuffer();
        glDisable(GL_CLIP_DISTANCE0);
    }
    */

    if (postProcessorAtmosphere_ && !terrainDrawDataContainer.land.empty())
        postProcessorAtmosphere_->start(); // render to postProcess texture

    if (waterRenderer_ && !terrainDrawDataContainer.water.empty())
        waterRenderer_->render(terrainDrawDataContainer.water, game);

    renderScene(lights, terrainDrawData.land, sky, game, glm::vec4(0, -1, 0, 10000));

    if (postProcessorAtmosphere_ && !terrainDrawDataContainer.land.empty()) {
        postProcessorAtmosphere_->end();
        postProcessorAtmosphere_->render(terrainDrawDataContainer.land, game);
    }

    if (debugShader_)
        renderDebug(terrainDrawDataContainer, game);
    
    if (gui && guiRenderer_)
        guiRenderer_->render(gui, game);
    
    entityMap_.clear();
}

void Renderer::renderDebug(terrainDrawDataContainer &terrainDrawDataContainer, Game *game) {
    debugShader_->use();

    for (Drawable *drawable : terrainDrawDataContainer.land[0]->getDrawableListAtIndex(0)) {
        debugShader_->prepare(drawable, game);
        for (Mesh *m : drawable->getMeshes())
            vaoRenderer_->draw(m);
    }

    for (Drawable *drawable : terrainDrawDataContainer.water[0]->getDrawableListAtIndex(0)) {
        debugShader_->prepare(drawable, game);
        for (Mesh *m : drawable->getMeshes())
            vaoRenderer_->draw(m);
    }

    debugShader_->end();
}

void Renderer::resolutionChange(int width, int height) {
    fprintf(stdout, "[RENDERER::resolutionChange] Resolution changed to %ix%i\n", width, height);
    windowWidth_ = width;
    windowHeight_ = height;

    if (waterRenderer_)
        waterRenderer_->resolutionChange(windowWidth_, windowHeight_);

    if (postProcessorAtmosphere_)
        postProcessorAtmosphere_->resolutionChange(windowWidth_, windowHeight_);
}

void Renderer::setupRenderer(std::vector<Shader *> shaders) {

    for (Shader *shader : shaders) {
        switch (shader->type()) {
            case ShaderType::SHADER_TYPE_LIGHT:
                lightShader_ = shader;
                break;
            case ShaderType::SHADER_TYPE_TERRAIN:
                terrainRenderer_ = new TerrainRenderer(shader, vaoRenderer_);
                break;
            case ShaderType::SHADER_TYPE_SKY:
                skyRenderer_ = new SkyRenderer(shader, vaoRenderer_);
                break;
            case ShaderType::SHADER_TYPE_WATER:
                if (!waterRenderer_)
                    waterRenderer_ = new WaterRenderer(vaoRenderer_, windowWidth_, windowHeight_);
                waterRenderer_->addShader(shader);
                break;
            case ShaderType::SHADER_TYPE_WATER_PERFORMANCE:
                if (!waterRenderer_)
                    waterRenderer_ = new WaterRenderer(vaoRenderer_, windowWidth_, windowHeight_);
                waterRenderer_->addShader(shader);
                break;
            case ShaderType::SHADER_TYPE_GUI:
                guiRenderer_ = new GuiRenderer(shader, vaoRenderer_);
                break;
            case ShaderType::SHADER_TYPE_POST_PROCESSOR:
                postProcessorAtmosphere_ = new PostProcessor(shader, vaoRenderer_, windowWidth_, windowHeight_);
                break;
            case ShaderType::SHADER_TYPE_DEBUG:
                debugShader_ = shader;
                break;
            default:
                if (shaderMap_.count(shader->type()))
                    fprintf(stdout, "[RENDERER::setupRenderer] WARNING: Two shaders with same type\n");
                else
                    shaderMap_[shader->type()] = shader;
                break;
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

/*
 * TODO: Should call shader->resetTextureCount() after each mesh is drawn if every mesh has it's own textures.
 * How likely is that? 
 */
void Renderer::renderList(Shader *shader, DrawableList &drawables, Game *game, glm::vec4 clipPlane) {
    if (!drawables.empty() && shader) {
        shader->use();
        shader->uniform("clipPlane", clipPlane);
        for (Drawable *drawable : drawables) {
            drawable->update(game);
            shader->prepare(drawable, game);

            for (Mesh *mesh : drawable->getMeshes()) {
                shader->handleMeshTextures(mesh->getTextures());
                vaoRenderer_->draw(mesh);
                shader->resetTextureCount();
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