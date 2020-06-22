#include "waterrenderer.hpp"
#include "oglheader.hpp"
#include "textureloader.hpp"

const float WATER_WAVE_SPEED = 0.03f;

WaterRenderer::WaterRenderer(Shader *shader, VaoRenderer *vaoRenderer, unsigned int reflectionTexture,
                             unsigned int refractionTexture, unsigned int depthTexture)
    : shader_(shader), vaoRenderer_(vaoRenderer), reflectionTexture_(reflectionTexture),
      refractionTexture_(refractionTexture), depthTexture_(depthTexture) {

    dudvTexture_ = TextureLoader::loadTextureFromFile("assets/waterDUDV.png");
    normalTexture_ = TextureLoader::loadTextureFromFile("assets/waterNormal.png");
    renderPerformance_ = false;
}

WaterRenderer::WaterRenderer(Shader *shader, VaoRenderer *vaoRenderer) : shader_(shader), vaoRenderer_(vaoRenderer) {
    renderPerformance_ = true;
    waterTextureLow_ = TextureLoader::loadTextureFromFile("assets/waterLow.tga");
    dudvTexture_ = TextureLoader::loadTextureFromFile("assets/waterDUDV.png");
    normalTexture_ = TextureLoader::loadTextureFromFile("assets/waterNormal.png");
}

void WaterRenderer::render(std::vector<Drawable *> water, Game *game) {
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

        for (Mesh &mesh : drawable->getMeshes())
            vaoRenderer_->draw(mesh);
    }
    shader_->end();

    if (!renderPerformance_)
        glDisable(GL_BLEND);
}

void WaterRenderer::preparePerformance() {
    shader_->bindTexture("texture_waterLow", waterTextureLow_);
    shader_->bindTexture("texture_dudv", dudvTexture_);
    shader_->bindTexture("texture_normal", normalTexture_);
}

void WaterRenderer::prepareQuality() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    shader_->bindTexture("texture_reflection", reflectionTexture_);
    shader_->bindTexture("texture_refraction", refractionTexture_);
    shader_->bindTexture("texture_dudv", dudvTexture_);
    shader_->bindTexture("texture_normal", normalTexture_);
    shader_->bindTexture("texture_depth", depthTexture_);
}