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
    TerrainTile *t;
    if (!(t = dynamic_cast<TerrainTile*>(terrains[0]))) {
        fprintf(stderr, "[POSTPROCESSOR::render] CRITICAL ERROR: Drawable is not a TerrainTile\n");
        return;
    }
    glDisable(GL_DEPTH_TEST);

    shader_->use();
    shader_->bindTexture("mainScreenTex", mainTexture_);
    shader_->uniform("worldSpaceCamPos", game->getView().getCameraPosition());
    shader_->uniform("camDirection", game->getView().getCameraDirection());
    shader_->uniform("planetOrigin", t->getSphereOrigin());
    shader_->uniform("planetRadius", t->getSphereRadius());
    for (Mesh &mesh : screen_->getMeshes()) {
        vaoRenderer_->draw(mesh);
    }

    shader_->end();
    glEnable(GL_DEPTH_TEST);
}

void PostProcessor::init() {
    mainFrameBuffer_ = FrameBuffer::createFrameBuffer();
    mainTexture_ = FrameBuffer::createTextureAttachment(windowWidth_, windowHeight_);
    mainDepthBuffer_ = FrameBuffer::createDepthBufferAttachment(windowWidth_, windowHeight_);
    FrameBuffer::unbindActiveFrameBuffer(windowWidth_, windowHeight_);
    screen_ = DrawableFactory::createPrimitive(PrimitiveType::QUAD2D, ShaderType::SHADER_TYPE_POST_PROCESSOR);
    glm::vec3 scale = glm::vec3(1,1,0);
    glm::vec3 trans = glm::vec3(0,0,0);
    screen_->transform(&scale, &trans, NULL);
}