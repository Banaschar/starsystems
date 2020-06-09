#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 texCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec3 lightPos;

uniform mat4 MVP;
uniform mat4 modelMatrix;
uniform mat4 cameraMatrix;
uniform mat3 normalMatrix;
uniform vec3 lightPosition;

void main(){

    gl_Position =  MVP * vec4(vertexPosition, 1);
    FragPos = vec3(cameraMatrix * modelMatrix * vec4(vertexPosition, 1.0));
    Normal = normalMatrix * vertexNormal;
    lightPos = vec3(cameraMatrix * vec4(lightPosition, 1.0)); // world space light position to camera(view) space
    TexCoords = texCoords;
}