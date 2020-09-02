#include "postprocessor.hpp"
#include "framebuffer.hpp"
#include "global.hpp"
#include "oglheader.hpp"


PostProcessor::PostProcessor(Shader *shader, VaoRenderer *vaoRenderer, int winWidth, int winHeight) : shader_(shader), vaoRenderer_(vaoRenderer), windowWidth_(winWidth), windowHeight_(winHeight) {
    init();
}

PostProcessor::~PostProcessor() {
    FrameBuffer::destroyFrameBuffer(&mainFrameBuffer_);
    FrameBuffer::destroyTexture(&mainTexture_);
    FrameBuffer::destroyRenderBuffer(&mainDepthBuffer_);

    delete screen_;
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
}

void PostProcessor::render(Game *game, std::vector<Drawable *> &terrains) {
    // calculate the cross product -> vector perpendiculat to the normal of the plane
    // and the normal of the camera (camera direction)
    // that gives the rotation axis

    TerrainTile *t;
    if (!terrains.empty() && !(t = dynamic_cast<TerrainTile*>(terrains[0]))) {
        fprintf(stdout, "[POSTPROCESSOR::render] CRITICAL ERROR: Drawable is not a TerrainTile\n");
        return;
    }

    //glm::vec3 newPos = game->getView().getCameraPosition() + 100.0f * glm::normalize(game->getView().getCameraDirection());
    //screen_->setPosition(newPos);
    screen_->setPosition(game->getView().getCameraPosition());

    //glDisable(GL_DEPTH_TEST);

    shader_->use();
    shader_->prepare(screen_, game);
    shader_->bindTexture("mainScreenTex", mainTexture_);
    shader_->bindTexture("mainDepthTexture", mainDepthBuffer_);
    //shader_->uniform("size", glm::vec3(1.7778, 1, 0));
    shader_->uniform("size", glm::vec3(20, 20, 0));
    shader_->uniform("worldSpaceCamPos", game->getView().getCameraPosition());
    shader_->uniform("camDirection", game->getView().getCameraDirection());
    shader_->uniform("planetOrigin", t->getSphereOrigin());
    shader_->uniform("planetRadius", t->getSphereRadius());
    for (Mesh &mesh : screen_->getMeshes()) {
        vaoRenderer_->draw(mesh);
    }

    shader_->end();
    //glEnable(GL_DEPTH_TEST);
}

void PostProcessor::init() {
    mainFrameBuffer_ = FrameBuffer::createFrameBuffer();
    mainTexture_ = FrameBuffer::createTextureAttachment(windowWidth_, windowHeight_);
    mainDepthBuffer_ = FrameBuffer::createDepthTextureAttachment(windowWidth_, windowHeight_);
    FrameBuffer::unbindActiveFrameBuffer(windowWidth_, windowHeight_);
    screen_ = DrawableFactory::createPrimitive(PrimitiveType::QUAD2D, ShaderType::SHADER_TYPE_POST_PROCESSOR);
}