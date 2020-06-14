#version 330 core
struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 cameraPos;
uniform Material material;
uniform Light light;

void main(){
	// ambient
	vec3 ambient = light.ambient * material.ambient;

	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDirection = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDirection), 0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);

	//specular
	vec3 viewDirection = normalize(cameraPos - FragPos);
	vec3 reflectDirection = reflect(-lightDirection, norm);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * material.specular);

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);
}