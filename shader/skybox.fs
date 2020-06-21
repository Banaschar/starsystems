#version 330 core

in vec3 texCoords;

out vec4 FragColor;

uniform samplerCube texture_cubemap1;

void main() {
    FragColor = texture(texture_cubemap1, texCoords);
}