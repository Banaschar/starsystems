#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 textureCoords;
layout (location = 3) in vec4 vertexColor;

out vec3 fragPos;
out vec4 color;
out vec3 normal;
out vec2 texCoords;

uniform mat4 MVP;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

void main() {
    gl_Position = MVP * vec4(vertexPosition, 1.0);
    texCoords = textureCoords;
    color = vertexColor;
    fragPos = vec3(modelMatrix * vec4(vertexPosition, 1.0));
    normal = normalMatrix * vertexNormal;
}