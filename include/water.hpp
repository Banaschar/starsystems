#ifndef WATER_H
#define WATER_H

#include <iostream>

#include "global.hpp"
#include "shader.hpp"
#include "drawable.hpp"
#include "waterframebuffer.hpp"
#include "assetloader.hpp"

#define WATER_WAVE_SPEED 0.03f

/*
 * Water Renderer
 * TODO: Load dudv texture somewhere else and pass it to the renderer
 */
class WaterRenderer {
public:
    WaterRenderer(Shader *shader, WaterFrameBuffer *waterFb) : shader_(shader) {
        reflectionTexture_ = waterFb->getReflectionTexture();
        refractionTexture_ = waterFb->getRefractionTexture();
        dudvTexture_ = loadTextureFromFile("waterDUDV.png");
        normalTexture_ = loadTextureFromFile("waterNormal.png");
        depthTexture_ = waterFb->getRefractionDepthTexture();
        renderPerformance_ = false;
    }
    WaterRenderer(Shader *shader) : shader_(shader) {
        renderPerformance_ = true;
        waterTextureLow_ = loadTextureFromFile("waterLow.tga");
        dudvTexture_ = loadTextureFromFile("waterDUDV.png");
        normalTexture_ = loadTextureFromFile("waterNormal.png");
    }
    void render(std::vector<Drawable*> water, Game *game) {
        moveFactor_ += WATER_WAVE_SPEED * g_deltaTime;
        moveFactor_ = moveFactor_ >= 1.0 ? 0.0 : moveFactor_;
        shader_->use();
        shader_->uniform("moveFactor", moveFactor_);

        if (renderPerformance_)
            preparePerformance();
        else
            prepareQuality();

        for (Drawable *drawable : water) {
            drawable->update(game);
            shader_->prepare(drawable, game);
            drawable->draw(shader_);
        }
        shader_->end();

        if (!renderPerformance_)
            glDisable(GL_BLEND);
    }
private:
    Shader *shader_;
    unsigned int waterTextureLow_;
    unsigned int reflectionTexture_;
    unsigned int refractionTexture_;
    unsigned int dudvTexture_;
    unsigned int normalTexture_;
    unsigned int depthTexture_;
    float moveFactor_ = 0.0;
    bool renderPerformance_;

    void preparePerformance() {
        shader_->bindTexture("texture_waterLow", waterTextureLow_);
        shader_->bindTexture("texture_dudv", dudvTexture_);
        shader_->bindTexture("texture_normal", normalTexture_);
    }

    void prepareQuality() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        shader_->bindTexture("texture_reflection", reflectionTexture_);
        shader_->bindTexture("texture_refraction", refractionTexture_);
        shader_->bindTexture("texture_dudv", dudvTexture_);
        shader_->bindTexture("texture_normal", normalTexture_);
        shader_->bindTexture("texture_depth", depthTexture_);
    }
};
#endif