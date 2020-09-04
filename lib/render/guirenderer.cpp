#include "guirenderer.hpp"
#include "oglheader.hpp"

GuiRenderer::GuiRenderer(Shader *shader, VaoRenderer *vaoRenderer) : shader_(shader), vaoRenderer_(vaoRenderer) {}

void GuiRenderer::render(std::vector<Drawable *> *guiElements, Game *game) {
    glDisable(GL_DEPTH_TEST);
    shader_->use();
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (Drawable *gui : *guiElements) {
        gui->update(game);
        shader_->prepare(gui, game);

        for (Mesh *mesh : gui->getMeshes()) {
            shader_->handleMeshTextures(mesh->getTextures());
            vaoRenderer_->draw(mesh);
            shader_->resetTextureCount();
        }
    }
    // glDisable(GL_BLEND);
    shader_->end();
    glEnable(GL_DEPTH_TEST);
}