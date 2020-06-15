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
in vec3 fragPosOriginal;
in vec3 normal;
in vec2 texCoords;

uniform vec3 cameraPos;
uniform Light light;
uniform float amplitude;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_diffuse4;
uniform sampler2D texture_diffuse5;

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

vec4 interpolateColors(vec4 tex1, vec4 tex2, float blend) {
    float color1weight = 1 - blend;
    float r = (color1weight * tex1.x) + (blend * tex2.y);
    float g = (color1weight * tex1.y) + (blend * tex2.y);
    float b = (color1weight * tex1.z) + (blend * tex2.z);

    return vec4(r,g,b,1.0f);
}

vec4 blendTextures() {
    float spread = 0.45;
    float part = 1.0 / 4.0;
    vec4 sand = texture(texture_diffuse1, texCoords);
    vec4 grass = texture(texture_diffuse2, texCoords);
    vec4 ground = texture(texture_diffuse3, texCoords);
    vec4 rock = texture(texture_diffuse4, texCoords);
    vec4 snow = texture(texture_diffuse5, texCoords);

    float value = (fragPosOriginal.y + amplitude / (amplitude * 2));
    value = clamp((value - spread/2.0) * (1.0 / spread), 0.0, 0.9999);
    int firstPalette = int(floor(value / part));
    float blend = (value - (firstPalette * part)) / part;
    vec4 result;

    switch (firstPalette) {
    case 0: 
        result = interpolateColors(sand, grass, blend);
        break;
    case 1: 
        result = interpolateColors(grass, ground, blend);
        break;
    case 2: 
        result = interpolateColors(ground, rock, blend);
        break;
    case 3: 
        result = interpolateColors(rock, snow, blend);
        break;
    }
    return result;
}

void main()
{
    //FragColor = calculateLighting(normal) * color;
    FragColor = calculateLighting(normal) * blendTextures();
    //FragColor = blendTextures();
    //if (fragPosOriginal.y == 0) {
    //    FragColor = vec4(0.0,0.0,1.0,1.0);
    //} else {
    //    FragColor = color;
    //}
}