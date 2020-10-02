#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 3) in mat4 aInstanceMvps;

void main()
{
    gl_Position = aInstanceMvps * vec4(vertexPosition, 1.0f); 
}