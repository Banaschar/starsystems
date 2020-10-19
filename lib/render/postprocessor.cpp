#include "postprocessor.hpp"
#include "framebuffer.hpp"
#include "global.hpp"
#include "oglheader.hpp"
#include "scenerenderdata.hpp"

/*
 * TODO: Move shader specific code to a shader subClass (or the callback function), so I can use the postProcessor
 * class with different shaders.
 */
PostProcessor::PostProcessor(Shader *shader, VaoRenderer *vaoRenderer, int winWidth, int winHeight) : shader_(shader), vaoRenderer_(vaoRenderer), windowWidth_(winWidth), windowHeight_(winHeight) {
    init();
}

PostProcessor::~PostProcessor() {
    FrameBuffer::destroyFrameBuffer(&mainFrameBuffer_);
    FrameBuffer::destroyTexture(&mainTexture_);
    FrameBuffer::destroyRenderBuffer(&mainDepthBuffer_);

    delete screen_;

    if (shader_)
        delete shader_;
}

unsigned int PostProcessor::DEBUG_getTexture() {
    return mainTexture_;
}

void PostProcessor::start() {
    FrameBuffer::bindFrameBuffer(mainFrameBuffer_, windowWidth_, windowHeight_);
}

void PostProcessor::end() {
    FrameBuffer::unbindActiveFrameBuffer(windowWidth_, windowHeight_);
}

// TODO: Rebuild texture attachement with new resolution
void PostProcessor::resolutionChange(int width, int height) {
    windowWidth_ = width;
    windowHeight_ = height;
    //mainTexture_ = mainFrameBuffer_->resolutionChange(width, height);
}

void PostProcessor::render(TerrainRenderDataVector &renderDataVector, SceneRenderData &sceneData) {
    for (TerrainObjectRenderData &tObj : renderDataVector) {
        if (tObj.attributes->hasAtmosphere)
            render_(tObj, sceneData);
    }
}

void PostProcessor::render_(TerrainObjectRenderData &renderData, SceneRenderData &sceneData) {
    glDisable(GL_DEPTH_TEST);

    shader_->use();
    
    shader_->bindTexture("mainScreenTex", mainTexture_);
    shader_->bindTexture("mainDepthTexture", mainDepthBuffer_);
    shader_->setSceneUniforms(sceneData, renderData.attributes);
    shader_->setDrawableUniforms(sceneData, screen_, nullptr);
    shader_->uniform("scatterCoeffs", scatterCoeffs_);
    
    for (Mesh *mesh : screen_->getMeshes()) {
        vaoRenderer_->draw(mesh);
    }

    shader_->end();
    glEnable(GL_DEPTH_TEST);
}

void PostProcessor::init() {
    mainFrameBuffer_ = FrameBuffer::createFrameBuffer();
    mainTexture_ = FrameBuffer::createTextureAttachment(windowWidth_, windowHeight_);
    mainDepthBuffer_ = FrameBuffer::createDepthTextureAttachment(windowWidth_, windowHeight_);
    FrameBuffer::unbindActiveFrameBuffer(windowWidth_, windowHeight_);
    screen_ = DrawableFactory::createPrimitive(PrimitiveType::QUAD2D, ShaderType::SHADER_TYPE_POST_PROCESSOR);

    // Move to shader class
    float scatterStrength = 21.23;
    float scatterR = glm::pow(400 / waveLengths_.x, 4) * scatterStrength;
    float scatterG = glm::pow(400 / waveLengths_.y, 4) * scatterStrength;
    float scatterB = glm::pow(400 / waveLengths_.z, 4) * scatterStrength;
    scatterCoeffs_ = glm::vec3(scatterR, scatterG, scatterB);
}