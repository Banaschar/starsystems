#version 330 core
layout (location = 0) in vec3 vertexPosition;
//layout (location = 1) in vec3 vertexNormal;
layout (location = 3) in vec4 vertexColor;

out vec4 color;

//uniform vec3 lightDirection;
//uniform vec3 lightColor;
//uniform vec2 lightBias;
uniform mat4 MVP;

// diffuse light
//vec3 lighting() {
//    vec3 normal = in_normal.xyz * 2.0 - 1.0;
//    float brightness = max(dot(-lightDirection, normal), 0.0);
//    return (lightColor * lightBias.x) + (brightess * lightColor * lightBias.y);
//}

void main()
{
    gl_Position = MVP * vec4(vertexPosition, 1.0);
    color = vertexColor;
} 