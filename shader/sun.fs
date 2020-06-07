#version 330 core
out vec4 SunColor;

uniform vec4 color;

void main()
{
    SunColor = color; // set all 4 vector values to 1.0
}