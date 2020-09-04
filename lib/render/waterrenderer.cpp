#include "waterrenderer.hpp"
#include "oglheader.hpp"
#include "textureloader.hpp"

const float WATER_WAVE_SPEED = 0.03f;

WaterRenderer::WaterRenderer(VaoRenderer *vaoRenderer, int width, int height) : vaoRenderer_(vaoRenderer), windowWidth_(width), windowHeight_(height) {}

WaterRenderer::~WaterRenderer() {
    if (waterFrameBuffer_)
        delete waterFrameBuffer_;
}

void WaterRenderer::resolutionChange(int width, int height) {
    windowWidth_ = width;
    windowHeight_ = height;
    if (waterFrameBuffer_)
        waterFrameBuffer_->resolutionChange(width, height);
}

void WaterRenderer::addShader(Shader *shader) {
    if (shader->type() == ShaderType::SHADER_TYPE_WATER) {
        shaderQ_ = shader;
        initQualityShader();
    } else {
        shaderP_ = shader;
        initPerformanceShader();
    }
}

void WaterRenderer::bindReflectionFrameBuffer() {
    waterFrameBuffer_->bindReflectionFrameBuffer();
}

void WaterRenderer::bindRefractionFrameBuffer() {
    waterFrameBuffer_->bindRefractionFrameBuffer();
}

void WaterRenderer::unbindActiveFrameBuffer() {
    waterFrameBuffer_->unbindActiveFrameBuffer();
}

void WaterRenderer::initQualityShader() {
    waterFrameBuffer_ = new WaterFrameBuffer(windowWidth_, windowHeight_);
    reflectionTexture_ = waterFrameBuffer_->getReflectionTexture();
    refractionTexture_ = waterFrameBuffer_->getRefractionTexture();
    depthTexture_ = waterFrameBuffer_->getRefractionDepthTexture();
    dudvTexture_ = TextureLoader::loadTextureFromFile("assets/waterDUDV.png");
    normalTexture_ = TextureLoader::loadTextureFromFile("assets/waterNormal.png");
}

void WaterRenderer::initPerformanceShader() {
    waterTexturePerformance_ = TextureLoader::loadTextureFromFile("assets/waterLow.tga");
    dudvTexturePerformance_ = TextureLoader::loadTextureFromFile("assets/waterDUDV.png");
    normalTexturePerformance_ = TextureLoader::loadTextureFromFile("assets/waterNormal.png");
}

void WaterRenderer::render(std::vector<Drawable *> water, Game *game) {
    moveFactor_ += WATER_WAVE_SPEED * g_deltaTime;
    moveFactor_ = moveFactor_ >= 1.0 ? 0.0 : moveFactor_;

    bool quality = water[0]->type() == ShaderType::SHADER_TYPE_WATER ? true : false;
    Shader *shaderTmp = quality ? shaderQ_ : shaderP_;

    if (!shaderTmp) {
        fprintf(stderr, "[WATERRENDERER::render] Error: No matching shader\n");
        return;
    }

    shaderTmp->use();
    shaderTmp->uniform("moveFactor", moveFactor_);

    if (quality)
        prepareQuality();
    else
        preparePerformance();
        
    for (Drawable *drawable : water) {
        if (drawable) {
            drawable->update(game);
            shaderTmp->prepare(drawable, game);

            for (Mesh *mesh : drawable->getMeshes())
                vaoRenderer_->draw(mesh);
        }
    }
    shaderTmp->end();

    if (quality)
        glDisable(GL_BLEND);
}

void WaterRenderer::preparePerformance() {
    shaderP_->bindTexture("texture_waterLow", waterTexturePerformance_);
    shaderP_->bindTexture("texture_dudv", dudvTexturePerformance_);
    shaderP_->bindTexture("texture_normal", normalTexturePerformance_);
}

void WaterRenderer::prepareQuality() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    shaderQ_->bindTexture("texture_reflection", reflectionTexture_);
    shaderQ_->bindTexture("texture_refraction", refractionTexture_);
    shaderQ_->bindTexture("texture_dudv", dudvTexture_);
    shaderQ_->bindTexture("texture_normal", normalTexture_);
    shaderQ_->bindTexture("texture_depth", depthTexture_);
}