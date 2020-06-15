#version 330 core

in vec2 texCoords;

out vec4 color;

uniform sampler2D texture_gui;

void main() {
    color = texture(texture_gui, texCoords);
}