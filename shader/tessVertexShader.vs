#version 410 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 textureCoords;

out vec3 wPos_tControl_in;
out vec3 normal_tControl_in;

uniform mat4 modelMatrix;

void main() {
    wPos_tControl_in = (modelMatrix * vec4(vertexPosition, 1.0)).xyz;
    normal_tControl_in = (modelMatrix * vec4(vertexNormal, 1.0)).xyz;
}