#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 2) in vec2 textureCoords;

out vec2 texCoords;

void main()
{
    gl_Position = vec4(vertexPosition, 1.0);
    texCoords = textureCoords;
}