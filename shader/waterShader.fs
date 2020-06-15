#version 330 core

out vec4 fragColor;

struct Light {
    vec3 direction;
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec4 color;
in vec3 fragPos;
in vec3 normal;
in vec2 texCoords;

uniform Light light;
uniform sampler2D texture_normal1;
uniform samplerCube cubemap;
uniform vec3 worldNormal;
uniform vec3 cameraPos;

vec4 calculateLighting() {
    // ambient
    vec3 ambient = light.ambient * 0.1;

    // diffuse
    vec3 lightDir = normalize(-light.direction);
    vec3 norm = normalize(normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff;

    //specular
    vec3 viewDirection = normalize(cameraPos - fragPos);
    vec3 reflectDirection = reflect(-light.direction, norm);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32.0);
    vec3 specular = 0.5 * spec * light.specular;

    return vec4(ambient + diffuse + specular, 1.0);
}

vec4 calculateCubeReflect() {
    vec3 lightDir = normalize(-light.direction);
    vec3 r = reflect(lightDir, worldNormal);
    vec4 skyBoxColor = texture(cubemap, r);
    //skyBoxColor.a = 0.5;
    return skyBoxColor;
}

void main() {
    //vec3 normalt = texture(texture_normal1, texCoords).rgb;
    //normalt = normalize(normalt * 2.0 - 1.0);

    fragColor = calculateLighting() * color;
    //fragColor = calculateLighting(normalt) * calculateCubeReflect() * color;
}