#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <string.h>
#include <string>
#include <vector>
#include <memory>

#include "drawable.hpp"
#include "oglheader.hpp"
#include "shader.hpp"

Shader::Shader(const char *vertexShaderPath, const char *fragmentShaderPath, const char *tessControlShaderPath, const char *tessEvalShaderPath, 
                const char *geometryShaderPath, ShaderType type, const callback_t cb) : type_(type), drawCallback_(cb) {
    openShaders(vertexShaderPath, fragmentShaderPath, tessControlShaderPath, tessEvalShaderPath, geometryShaderPath);
}

Shader::Shader(const char *vertexShaderPath, const char *fragmentShaderPath, ShaderType type,
               const callback_t cb) : type_(type), drawCallback_(cb) {
    openShaders(vertexShaderPath, fragmentShaderPath, nullptr, nullptr, nullptr);

}

void Shader::openShaders(const char *vertexShaderPath, const char *fragmentShaderPath, const char *tessControlShaderPath, 
                            const char *tessEvalShaderPath, const char *geometryShaderPath) {

    std::vector<GLuint> shaderIds;
    bool compiled = true;

    compiled = attachShader(vertexShaderPath, GL_VERTEX_SHADER, shaderIds);
    compiled = attachShader(fragmentShaderPath, GL_FRAGMENT_SHADER, shaderIds);
    if (tessControlShaderPath)
        compiled = attachShader(tessControlShaderPath, GL_TESS_CONTROL_SHADER, shaderIds);
    if (tessEvalShaderPath)
        compiled = attachShader(tessEvalShaderPath, GL_TESS_EVALUATION_SHADER, shaderIds);
    if (geometryShaderPath)
        compiled = attachShader(geometryShaderPath, GL_GEOMETRY_SHADER, shaderIds);

    if (compiled)
        compiled = linkProgram(shaderIds);

    if (!compiled)
        fprintf(stdout, "[SHADER::Shader] Error: Shader compilation failed!\n");
}

bool Shader::linkProgram(std::vector<GLuint> &shaderIds) {
    GLint result = GL_FALSE;
    int infoLogLength;
    shaderProgramId_ = glCreateProgram();

    for (GLuint sid : shaderIds)
        glAttachShader(shaderProgramId_, sid);

    glLinkProgram(shaderProgramId_);

    /* Check for errors */
    glGetProgramiv(shaderProgramId_, GL_LINK_STATUS, &result);
    glGetProgramiv(shaderProgramId_, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(infoLogLength + 1);
        glGetProgramInfoLog(shaderProgramId_, infoLogLength, NULL, &ProgramErrorMessage[0]);
        fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
    }

    for (GLuint sid : shaderIds) {
        glDetachShader(shaderProgramId_, sid);
        glDeleteShader(shaderProgramId_);
    }

    return result;
}

bool Shader::attachShader(const char *path, int shaderType, std::vector<GLuint> &shaderIds) {
    std::unique_ptr<std::string> shaderCode(readShaderFile(path));

    if (!shaderCode)
        return false;

    GLuint shaderId = glCreateShader(shaderType);
    GLint result = GL_FALSE;
    int InfoLogLength;

    fprintf(stdout, "Compiling shader : %s\n", path);
    char const *sourcePointer = shaderCode->c_str();
    glShaderSource(shaderId, 1, &sourcePointer, NULL);
    glCompileShader(shaderId);

    /* Check for errors */
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> shaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(shaderId, InfoLogLength, NULL, &shaderErrorMessage[0]);
        fprintf(stdout, "%s\n", &shaderErrorMessage[0]);
    }

    shaderIds.push_back(shaderId);
    return true;
}

std::string *Shader::readShaderFile(const char *path) {
    std::string *outString = new std::string;
    std::ifstream fileStream(path, std::ios::in);
    if (fileStream.is_open()) {
        std::stringstream sstr;
        sstr << fileStream.rdbuf();
        *outString = sstr.str();
        fileStream.close();
    } else {
        fprintf(stdout, "Impossible to open %s.\n", path);
        outString = nullptr;
    }

    return outString;
}

void Shader::use() {
    glUseProgram(shaderProgramId_);
    textureCounter_ = 0;
}

void Shader::end() {
    glActiveTexture(GL_TEXTURE0);
}

void Shader::resetTextureCount() {
    textureCounter_ = 0;
}

void Shader::prepare(Drawable *drawable, Game *game) {
    drawCallback_(this, drawable, game);
}

void Shader::bindTexture(const std::string &name, unsigned int texId) {
    glActiveTexture(GL_TEXTURE0 + textureCounter_);
    if (name.substr(0, name.size() - 1) == "texture_cubemap")
        glBindTexture(GL_TEXTURE_CUBE_MAP, texId);
    else
        glBindTexture(GL_TEXTURE_2D, texId);

    uniform(name, textureCounter_);
    ++textureCounter_;
}

void Shader::handleMeshTextures(std::vector<Texture> &textures) {
    // bind textures if any
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    unsigned int guiNr = 1;
    unsigned int cubeNr = 1;
    unsigned int def = 1;

    for (Texture &tex : textures) {
        std::string number;
        std::string type = tex.type;

        if (type == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (type == "texture_specular")
            number = std::to_string(specularNr++);
        else if (type == "texture_normal")
            number = std::to_string(normalNr++);
        else if (type == "texture_height")
            number = std::to_string(heightNr++);
        else if (type == "texture_gui")
            number = std::to_string(guiNr++);
        else if (type == "texture_cubemap")
            number = std::to_string(cubeNr++);
        else {
            number = std::to_string(def++);
        }
        bindTexture((type + number).c_str(), tex.id);
    }
}

void Shader::uniform(const std::string &name, glm::mat4 value) {
    glUniformMatrix4fv(glGetUniformLocation(shaderProgramId_, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void Shader::uniform(const std::string &name, glm::mat3 value) {
    glUniformMatrix3fv(glGetUniformLocation(shaderProgramId_, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void Shader::uniform(const std::string &name, glm::vec3 value) {
    glUniform3f(glGetUniformLocation(shaderProgramId_, name.c_str()), value.x, value.y, value.z);
}

void Shader::uniform(const std::string &name, glm::vec4 value) {
    glUniform4f(glGetUniformLocation(shaderProgramId_, name.c_str()), value.x, value.y, value.z, value.w);
}

void Shader::uniform(const std::string &name, int value) {
    glUniform1i(glGetUniformLocation(shaderProgramId_, name.c_str()), value);
}

void Shader::uniform(const std::string &name, float value) {
    glUniform1f(glGetUniformLocation(shaderProgramId_, name.c_str()), value);
}

ShaderType Shader::type() {
    return type_;
}

unsigned int Shader::id() {
    return shaderProgramId_;
}