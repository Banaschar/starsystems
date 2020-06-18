#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
using namespace std;

#include <GL/glew.h>
#include <stdlib.h>
#include <string.h>

#include "shader.hpp"

Shader::Shader(const char *vertexShaderPath, const char *fragmentShaderPath, 
				const std::string type, const callback_t cb) : type_(type),
				drawCallback_(cb) {
	if (!openShaders(vertexShaderPath, fragmentShaderPath))
		std::cout << "ERROR: SHADER COMPILATION FAILED!\n" << std::endl;
}

bool Shader::openShaders(const char *vertexShaderPath, const char *fragmentShaderPath){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertexShaderPath, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s.\n", vertexShaderPath);
		return false;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragmentShaderPath, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertexShaderPath);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragmentShaderPath);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	shaderProgramId_ = glCreateProgram();
	glAttachShader(shaderProgramId_, VertexShaderID);
	glAttachShader(shaderProgramId_, FragmentShaderID);
	glLinkProgram(shaderProgramId_);

	// Check the program
	glGetProgramiv(shaderProgramId_, GL_LINK_STATUS, &result);
	glGetProgramiv(shaderProgramId_, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(shaderProgramId_, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(shaderProgramId_, VertexShaderID);
	glDetachShader(shaderProgramId_, FragmentShaderID);
	
	glDeleteShader(shaderProgramId_);
	glDeleteShader(shaderProgramId_);

	return result ? true : false;
}

void Shader::use() {
	glUseProgram(shaderProgramId_);
	textureCounter_ = 0;
}

void Shader::end() {
	glActiveTexture(GL_TEXTURE0);
}

void Shader::prepare(Drawable *drawable, Game *game) {
	drawCallback_(this, drawable, game);
}

void Shader::bindTexture(const std::string &name, unsigned int texId) {
	glActiveTexture(GL_TEXTURE0 + textureCounter_);
	if (name == "cubemap") {
		glBindTexture(GL_TEXTURE_CUBE_MAP, texId);
	else
		glBindTexture(GL_TEXTURE_2D, texId);

	uniform(name, textureCounter_);
	++textureCounter_;
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

std::string Shader::type() {
	return type_;
}

unsigned int Shader::id() {
	return shaderProgramId_;
}