#include "postprocessor.hpp"
#include "framebuffer.hpp"
#include "global.hpp"
#include "oglheader.hpp"

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

void PostProcessor::render(Game *game, std::vector<Drawable *> &terrains) {
    TerrainTile *t;
    if (!terrains.empty() && !(t = dynamic_cast<TerrainTile*>(terrains[0]))) {
        fprintf(stdout, "[POSTPROCESSOR::render] CRITICAL ERROR: Drawable is not a TerrainTile\n");
        return;
    }
    
    //glm::vec3 newPos = game->getView().getCameraPosition() + 25.0f * glm::normalize(game->getView().getCameraDirection());
    //screen_->setPosition(newPos);
    screen_->setPosition(game->getView().getCameraPosition());

    glDisable(GL_DEPTH_TEST);

    shader_->use();
    shader_->prepare(screen_, game);
    shader_->bindTexture("mainScreenTex", mainTexture_);
    shader_->bindTexture("mainDepthTexture", mainDepthBuffer_);
    //shader_->uniform("size", glm::vec3(1, 1, 0));
    //shader_->uniform("size", glm::vec3(windowWidth_, windowHeight_, 0));
    shader_->uniform("size", glm::vec3(1280, 720, 0));
    shader_->uniform("worldSpaceCamPos", game->getView().getCameraPosition());
    shader_->uniform("camDirection", game->getView().getCameraDirection());
    shader_->uniform("planetOrigin", t->getSphereOrigin());
    shader_->uniform("planetRadius", t->getSphereRadius());
    shader_->uniform("nearPlane", game->getView().getNearPlane());
    shader_->uniform("farPlane", game->getView().getFarPlane());
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
    glm::vec3 scale = glm::vec3(1280, 720, 0);
    //screen_->transform(&scale, NULL, NULL);
}