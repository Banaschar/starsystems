#version 330 core
out vec4 FragColor;

flat in vec4 color;

void main()
{
    FragColor = color;
}