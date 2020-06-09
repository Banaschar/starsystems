#version 330 core
out vec4 SunColor;

uniform vec4 color;

void main()
{
    SunColor = color;
}