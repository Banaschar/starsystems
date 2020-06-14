#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

out vec3 FragPos;
out vec3 Normal;
out vec3 lightPos;

uniform mat4 MVP;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

void main(){

	gl_Position =  MVP * vec4(vertexPosition, 1);
	FragPos = vec3(modelMatrix * vec4(vertexPosition, 1.0));
	Normal = normalMatrix * vertexNormal;
}

