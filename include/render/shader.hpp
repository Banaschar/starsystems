#ifndef SHADER_H
#define SHADER_H

#include <functional>
#include <glm/glm.hpp>
#include <string>

#include "drawable.hpp"

enum TextureType {
    TEXTURE_TYPE_DIFFUSE,
    TEXTURE_TYPE_SPECULAR,
    TEXTURE_TYPE_NORMAL,
    TEXTURE_TYPE_HEIGHT,
    TEXTURE_TYPE_GUI
};

class Shader {
  public:
    Shader(const char *vertexShaderPath, const char *fragmentShaderPath, const char *tessControlShaderPath, 
            const char *tessEvalShaderPath, const char *geometryShaderPath, ShaderType type);
    Shader(const char *vertexShaderPath, const char *fragmentShaderPath, ShaderType type);
    void use();
    void end();
    void resetTextureCount();
    ShaderType type();
    unsigned int id();
    void bindTexture(const std::string &name, unsigned int texId);
    void handleMeshTextures(std::vector<Texture> &textures);
    virtual void setGlobalUniforms();
    virtual void setLocalUniforms();
    void uniform(const std::string &name, glm::mat4 value);
    void uniform(const std::string &name, glm::mat3 value);
    void uniform(const std::string &name, glm::vec3 value);
    void uniform(const std::string &name, glm::vec4 value);
    void uniform(const std::string &name, int value);
    void uniform(const std::string &name, float value);

  private:
    unsigned int shaderProgramId_;
    ShaderType type_;
    callback_t drawCallback_;
    int textureCounter_;
    void openShaders(const char *vertexShaderPath, const char *fragmentShaderPath, const char *tessControlShaderPath, 
                            const char *tessEvalShaderPath, const char *geometryShaderPath);
    std::string *readShaderFile(const char *path);
    bool attachShader(const char *path, int shaderType, std::vector<unsigned int> &shaderIds);
    bool linkProgram(std::vector<unsigned int> &shaderIds);
};

class TerrainInstanceShader : public Shader {
    void setGlobalUniforms() override;
    void setLocalUniforms() override;
}
#endif