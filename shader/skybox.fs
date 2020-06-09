#version 330 core

in vec3 texCoords;

out vec4 FragColor;

uniform samplerCube cubemap;

void main() {
    FragColor = texture(cubemap, texCoords);
}