#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "basetypes.hpp"

enum TextureType {
    TEXTURE_TYPE_DIFFUSE,
    TEXTURE_TYPE_SPECULAR,
    TEXTURE_TYPE_NORMAL,
    TEXTURE_TYPE_HEIGHT,
    TEXTURE_TYPE_GUI
};

struct SceneRenderData;
class Drawable;
struct Texture;

class Shader {
  public:
    /* ShaderList order: vertex, fragment, tessellationControl, tessellationEval, geometry */
    Shader(std::vector<const char *> &shaderFiles, ShaderType type);
    void use();
    void end();
    void resetTextureCount();
    ShaderType type();
    unsigned int id();
    void bindTexture(const std::string &name, unsigned int texId);
    void handleMeshTextures(std::vector<Texture> &textures);
    void uniform(const std::string &name, glm::mat4 value);
    void uniform(const std::string &name, glm::mat3 value);
    void uniform(const std::string &name, glm::vec3 value);
    void uniform(const std::string &name, glm::vec4 value);
    void uniform(const std::string &name, int value);
    void uniform(const std::string &name, float value);
    virtual void setSceneUniforms(SceneRenderData &sceneData, void *data);
    virtual void setDrawableUniforms(SceneRenderData &sceneData, Drawable *drawable, void *data);

  private:
    unsigned int shaderProgramId_;
    ShaderType type_;
    int textureCounter_;
    void openShaders(std::vector<const char *> &shaderFiles);
    std::string *readShaderFile(const char *path);
    bool attachShader(const char *path, int shaderType, std::vector<unsigned int> &shaderIds);
    bool linkProgram(std::vector<unsigned int> &shaderIds);
};
#endif