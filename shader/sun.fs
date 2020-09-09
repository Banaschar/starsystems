#version 330 core
out vec4 SunColor;

in vec2 texCoords;

uniform vec3 color;
uniform sampler2D texture_diffuse1;

void main()
{
    vec4 c = texture(texture_diffuse1, texCoords);
    SunColor = c;
    //SunColor = vec4(color, 1.0);
}