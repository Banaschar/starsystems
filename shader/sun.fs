#version 330 core
out vec4 SunColor;

uniform vec3 color;

void main()
{
    SunColor = vec4(color, 1.0);
}