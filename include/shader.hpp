#ifndef SHADER_HPP
#define SHADER_HPP

#include <glm/glm.hpp>
#include <string>

class Shader {
public:
    Shader(const char *vertexShaderPath, const char *fragmentShaderPath);
    void use();
    void uniform(const std::string &name, glm::mat4 value);
    void uniform(const std::string &name, glm::vec3 value);
    void uniform(const std::string &name, int value);
private:
    unsigned int shaderProgramId_;
    bool openShaders(const char *vertexShaderPath, const char *fragmentShaderPath);
};

#endif
