#version 330 core

struct Material {
    sampler2D diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec3 lightPos;
in vec2 TexCoords;

uniform Material material;
uniform Light light;

void main() {
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

    //specular
    //float specularStrength = 0.5;
    vec3 viewDirection = normalize(-FragPos);
    vec3 reflectDirection = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    //vec3 result = (ambient + diffuse + specular) * objectColor;
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}