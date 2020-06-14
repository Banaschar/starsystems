#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>
#include <string>
#include <functional>

#include "drawable.hpp"
#include "game.hpp"

typedef std::function<void(Shader*, Drawable*, Game*)> callback_t;

class Shader {
public:
    Shader(const char *vertexShaderPath, const char *fragmentShaderPath, const std::string type,
                const callback_t cb);
    void use();
    void prepare(Drawable *drawable, Game *game);
    std::string type();
    unsigned int id();
    void uniform(const std::string &name, glm::mat4 value);
    void uniform(const std::string &name, glm::mat3 value);
    void uniform(const std::string &name, glm::vec3 value);
    void uniform(const std::string &name, glm::vec4 value);
    void uniform(const std::string &name, int value);
    void uniform(const std::string &name, float value);

private:
    unsigned int shaderProgramId_;
    const std::string type_;
    callback_t drawCallback_;
    bool openShaders(const char *vertexShaderPath, const char *fragmentShaderPath);
};

#endif
