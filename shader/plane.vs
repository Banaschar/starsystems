#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 3) in vec4 vertexColor;

out vec4 color;
out vec3 fragPos;
out vec3 normal;

uniform mat4 modelMat;
uniform mat3 normalMatrix;
uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(vertexPosition, 1.0);
    color = vertexColor;
    normal = normalMatrix * vertexNormal;
    fragPos = vec3(modelMat * vec4(vertexPosition, 1.0));
} 