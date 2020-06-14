#version 330 core
out vec4 FragColor;

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec4 color;
//in vec3 vertPos_cameraSpace;
in vec3 normal;

//uniform vec3 viewPos;
uniform Light light;

vec4 calculateLighting(vec3 normal) {
    vec3 ambient = light.ambient * 0.1;

    vec3 lightDir = normalize(-light.direction);
    vec3 norm = normalize(normal);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff;

    return vec4(ambient + diffuse, 1.0);
}

void main()
{
    FragColor = calculateLighting(normal) * color;
}