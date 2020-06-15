#version 330 core
out vec4 FragColor;

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec4 color;
in vec3 fragPos;
in vec3 normal;

uniform vec3 cameraPos;
uniform Light light;

vec4 calculateLighting(vec3 normal) {
    // ambient
    vec3 ambient = light.ambient * 0.1;

    // diffuse
    vec3 lightDir = normalize(-light.direction);
    vec3 norm = normalize(normal);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff;

    //specular
    vec3 viewDirection = normalize(cameraPos - fragPos);
    vec3 reflectDirection = reflect(-light.direction, norm);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32.0);
    vec3 specular = 0.5 * spec * light.specular;

    return vec4(ambient + diffuse + specular, 1.0);
}

void main()
{
    FragColor = calculateLighting(normal) * color;
}