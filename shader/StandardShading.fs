#version 330 core
struct Material {
	vec3 ambient;
	vec3 diffuse;
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

out vec4 FragColor;

//uniform vec3 objectColor;
uniform Material material;
uniform Light light;

void main(){
	// ambient
	//float ambientStrength = 0.1;
	vec3 ambient = light.ambient * material.ambient;

	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDirection = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDirection), 0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);

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