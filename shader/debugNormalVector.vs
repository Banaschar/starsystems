#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;

out VS_OUT {
    vec3 normal;
} vs_out;

uniform mat4 cameraMatrix;
uniform mat4 modelMatrix;

void main() {
    gl_Position = cameraMatrix * modelMatrix * vec4(vertexPosition, 1.0);
    mat3 normalMatrix = mat3(transpose(inverse(cameraMatrix * modelMatrix)));
    vs_out.normal = normalize(vec3(vec4(normalMatrix * vertexNormal, 1.0)));
}