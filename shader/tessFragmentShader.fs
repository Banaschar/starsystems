#version 410 core

out vec4 fragColor;

struct Light {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 wPos_frag_in;
in vec3 normal_frag_in;

uniform Light light;
uniform vec3 cameraPos;

vec4 calculateLighting() {
    // diffuse
    vec3 lightDir = normalize(light.position - wPos_frag_in);
    float diff = max(dot(normal_frag_in, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff;
    if (normal_frag_in == 0.0)
        return vec4(1.0,0.0,0.0,1.0);

    //specular
    vec3 viewDirection = normalize(cameraPos - wPos_frag_in);
    vec3 reflectDirection = reflect(-lightDir, normal_frag_in);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32.0);
    vec3 specular = 0.9 * spec * light.specular;

    return vec4(light.ambient + diffuse + specular, 1.0);
}

void main() {
    fragColor = vec4(0.0, 0.3, 0.5, 1.0) * calculateLighting();
}