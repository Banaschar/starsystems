#include "skyrenderer.hpp"
#include "oglheader.hpp"
#include "drawable.hpp"

SkyRenderer::SkyRenderer(Shader *shader, VaoRenderer *vaoRenderer) : shader_(shader), vaoRenderer_(vaoRenderer) {}

void SkyRenderer::render(std::vector<Drawable *> skies, SceneRenderData &sceneData) {
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    shader_->use();
    shader_->setSceneUniforms(sceneData, nullptr);
    
    for (Drawable *sky : skies) {
        shader_->setDrawableUniforms(sceneData, sky, nullptr);

        for (Mesh *mesh : sky->getMeshes()) {
            shader_->handleMeshTextures(mesh->getTextures());
            vaoRenderer_->draw(mesh);
        }
    }
    shader_->end();

    glDepthFunc(GL_LESS);
    glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}