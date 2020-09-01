#include "renderer.hpp"
#include "guirenderer.hpp"
#include "oglheader.hpp"
#include "skyrenderer.hpp"
#include "terrainrenderer.hpp"
#include "vaorenderer.hpp"
#include "waterframebuffer.hpp"
#include "waterrenderer.hpp"
#include "terraintile.hpp"
#include "postprocessor.hpp"

Renderer::Renderer(std::vector<Shader *> shaders, int winWidth, int winHeight) : windowWidth_(winWidth), windowHeight_(winHeight) {
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
     * AND don't do this if we are rendering a whole planet and we are far away
     */
    
    if (waterTypeQuality_ && !water.empty()) {
        glEnable(GL_CLIP_DISTANCE0);
        
        glm::vec3 normal;
        glm::vec3 saveCamPos = game->getView().getCameraPosition();
        glm::vec3 tmpCamPos;
        float distancePlane;
        float distanceCam;
        TerrainTile *t;
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
        waterFrameBuffer_->bindReflectionFrameBuffer();
        renderScene(lights, terrain, sky, game, glm::vec4(normal, -distancePlane));
        
        game->getView().getCameraPosition() = saveCamPos;
        game->getView().invertPitch();
        game->getView().updateForce();
        waterFrameBuffer_->bindRefractionFrameBuffer();
        renderScene(lights, terrain, sky, game, glm::vec4(-1.0f * normal, distancePlane));

        waterFrameBuffer_->unbindActiveFrameBuffer();
        glDisable(GL_CLIP_DISTANCE0);
    }

    if (postProcessorAtmosphere_ && !terrain.empty())
        postProcessorAtmosphere_->start(); // render to postProcess texture

    renderScene(lights, terrain, sky, game, glm::vec4(0, -1, 0, 10000));

    if (waterRenderer_ && !water.empty())
        waterRenderer_->render(water, game);

    if (postProcessorAtmosphere_ && !terrain.empty()) {
        postProcessorAtmosphere_->end();
        postProcessorAtmosphere_->render(game, terrain);
    }
    
    
    if (gui && guiRenderer_)
        guiRenderer_->render(gui, game);
    
    entityMap_.clear();
}

void Renderer::resolutionChange(int width, int height) {
    fprintf(stdout, "[RENDERER::resolutionChange] Resolution changed to %ix%i\n", width, height);
    windowWidth_ = width;
    windowHeight_ = height;

    if (waterFrameBuffer_)
        waterFrameBuffer_->resolutionChange(windowWidth_, windowHeight_);

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
                waterTypeQuality_ = true;
                waterFrameBuffer_ = new WaterFrameBuffer(windowWidth_, windowHeight_);
                waterRenderer_ = new WaterRenderer(shader, vaoRenderer_, 
                                                   waterFrameBuffer_->getReflectionTexture(),
                                                   waterFrameBuffer_->getRefractionTexture(),
                                                   waterFrameBuffer_->getRefractionDepthTexture());
                break;
            case ShaderType::SHADER_TYPE_WATER_PERFORMANCE:
                waterRenderer_ = new WaterRenderer(shader, vaoRenderer_);
                break;
            case ShaderType::SHADER_TYPE_GUI:
                guiRenderer_ = new GuiRenderer(shader, vaoRenderer_);
                break;
            case ShaderType::SHADER_TYPE_POST_PROCESSOR:
                postProcessorAtmosphere_ = new PostProcessor(shader, vaoRenderer_, windowWidth_, windowHeight_);
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