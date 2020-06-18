#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>
#include <string>
#include <functional>

class Drawable;
class Game;

typedef std::function<void(Shader*, Drawable*, Game*)> callback_t;

enum TextureType {
    TEXTURE_TYPE_DIFFUSE,
    TEXTURE_TYPE_SPECULAR,
    TEXTURE_TYPE_NORMAL,
    TEXTURE_TYPE_HEIGHT,
    TEXTURE_TYPE_GUI
}

class Shader {
public:
    Shader(const char *vertexShaderPath, const char *fragmentShaderPath, const std::string type,
                const callback_t cb);
    void use();
    void end();
    void prepare(Drawable *drawable, Game *game);
    std::string type();
    unsigned int id();
    void bindTexture(const std::string &name, unsigned int texId);
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
    int textureCounter_;
    bool openShaders(const char *vertexShaderPath, const char *fragmentShaderPath);
};

#endif
