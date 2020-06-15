#ifndef GUI_H
#define GUI_H

#include <string>

#include "mesh.hpp"
#include "model.hpp"
#include "drawable.hpp"
#include "primitives.hpp"
#include "game.hpp"

class Gui {
public:
    Gui() {}
    Gui(unsigned int textureID, glm::vec2 position, glm::vec2 scale = glm::vec2(0.5f)) {
        addGuiElement(textureID, position, scale);
    }

    Gui(Texture tex, glm::vec2 position, glm::vec2 scale = glm::vec2(0.5f)) {
        addGuiElement(tex, position, scale);
    }

    ~Gui() {
        for (Drawable* m : guiElements_) {
            delete m;
        }
    }

    void addGuiElement(unsigned int textureID, glm::vec2 position, glm::vec2 scale = glm::vec2(0.5f)) {
        Texture t;
        t.id = textureID;
        t.type = "texture_gui";
        addGuiElement(t, position, scale);
    }

    void addGuiElement(Texture tex, glm::vec2 position, glm::vec2 scale = glm::vec2(0.5f)) {
        Mesh mesh = Primitives::createQuad2d();
        mesh.addTexture(tex);
        Model *tmp = new Model(mesh, "gui");
        glm::vec3 tmptrans = glm::vec3(position.x, position.y, 0.0f);
        glm::vec3 tmpscale = glm::vec3(scale.x, scale.y, 0.0f);
        tmp->transform(&tmpscale, &tmptrans, NULL);
        guiElements_.push_back(tmp);
    }

    void update(Game *game) {
        ;
    }

    std::vector<Drawable*>* getGuiElements() {
        return &guiElements_;
    }

private:
    std::vector<Drawable*> guiElements_;
};

class GuiRenderer {
public:

    GuiRenderer(Shader *shader) : shader_(shader) {}

    void render(std::vector<Drawable*> *guiElements, Game *game) {
        glDisable(GL_DEPTH_TEST);
        shader_->use();
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        for (Drawable* gui : *guiElements) {
            gui->update(game);
            shader_->prepare(gui, game);
            gui->draw(shader_);
        }
        //glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }

private:
    Shader *shader_;
};
#endif