#ifndef SHADER_HPP
#define SHADER_HPP

#include <glm/glm.hpp>
#include <string>

class Shader {
public:
    Shader(const char *vertexShaderPath, const char *fragmentShaderPath, const std::string type);
    void use();
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
    bool openShaders(const char *vertexShaderPath, const char *fragmentShaderPath);
};

#endif
