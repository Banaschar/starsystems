#include "guirenderer.hpp"
#include "oglheader.hpp"

#include "drawable.hpp"

GuiRenderer::GuiRenderer(Shader *shader, VaoRenderer *vaoRenderer) : shader_(shader), vaoRenderer_(vaoRenderer) {}

void GuiRenderer::render(std::vector<Drawable *> *guiElements, SceneRenderData &sceneData) {
    glDisable(GL_DEPTH_TEST);
    shader_->use();
    shader_->setSceneUniforms(sceneData, nullptr);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (Drawable *guiEle : *guiElements) {
        shader_->setDrawableUniforms(sceneData, guiEle, nullptr);

        for (Mesh *mesh : guiEle->getMeshes()) {
            shader_->handleMeshTextures(mesh->getTextures());
            vaoRenderer_->draw(mesh);
            shader_->resetTextureCount();
        }
    }
    // glDisable(GL_BLEND);
    shader_->end();
    glEnable(GL_DEPTH_TEST);
}