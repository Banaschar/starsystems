#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 3) in vec4 vertexColor;

flat out vec4 color;

uniform mat4 MVP;

void main()
{
    color = vertexColor;
    gl_Position = MVP * vec4(vertexPosition, 1.0);
} 