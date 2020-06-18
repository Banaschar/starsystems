#ifndef SKYRENDERER_H
#define SKYRENDERER_H

#include "shader.hpp"
#include "drawable.hpp"

class SkyRenderer {
public:
    SkyRenderer(Shader *shader) : shader_(shader) {}

    void render(std::vector<Drawable*> skies, Game *game, glm::vec4 clipPlane) {
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        Texture tex;
        shader_->uniform("clipPlane", clipPlane);
        for (Drawable *sky : skies) {
            tex = sky->getTextures()[0];
            shader_->bindTexture(tex.type, tex.id);
            shader_->prepare(sky, game);
            sky->draw(shader_);
        }
        

        glDepthFunc(GL_LESS);
        glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    }
private:
    Shader *shader_;
}
#endif