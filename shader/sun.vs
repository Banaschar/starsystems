#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 2) in vec2 textureCoords;

out vec2 texCoords;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(vertexPosition, 1.0);
    texCoords = textureCoords;
} 