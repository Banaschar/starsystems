#ifndef POSTPROCESSOR_HPP
#define POSTPROCESSOR_HPP

#include "shader.hpp"
#include "vaorenderer.hpp"
#include "drawablefactory.hpp"
#include "terraintile.hpp"

class PostProcessor {
public:
    PostProcessor(Shader *shader, VaoRenderer *vaoRenderer_, int winHeight, int winWidth);
    ~PostProcessor();
    void start();
    void end();
    void render(Game *game, std::vector<Drawable *> &terrains);
    void resolutionChange(int width, int height);

    unsigned int DEBUG_getTexture();

private:
    int windowWidth_;
    int windowHeight_;
    unsigned int mainFrameBuffer_;
    unsigned int mainTexture_;
    unsigned int mainDepthBuffer_;
    Shader *shader_;
    Drawable *screen_;
    VaoRenderer *vaoRenderer_;

    // move this to a shader object inheriting from shader
    glm::vec3 waveLengths_ = glm::vec3(700, 530, 460);
    glm::vec3 scatterCoeffs_;

    void init();
};
#endif