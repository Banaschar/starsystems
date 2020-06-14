#version 330 core

out vec4 fragColor;

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec4 color;
//in vec3 normal;
in vec2 texCoords;

uniform Light light;
uniform sampler2D texture_normal1;
//uniform sampler2D texture_diffuse1;

vec4 calculateLighting(vec3 normal) {
    vec3 ambient = light.ambient * 0.1;

    vec3 lightDir = normalize(-light.direction);
    vec3 norm = normalize(normal);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff;

    return vec4(ambient + diffuse, 1.0);
}

void main() {
    vec3 normalt = texture(texture_normal1, texCoords).rgb;
    normalt = normalize(normalt * 2.0 - 1.0);

    fragColor = calculateLighting(normalt) * color;
    //fragColor = color;
}