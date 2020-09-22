#version 330 core
out vec4 FragColor;

struct Light {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec4 color;
in vec3 fragPos_worldspace;
in vec3 normal;
in vec2 texCoords;

uniform vec3 cameraPos;
uniform Light light;
uniform float amplitude;
uniform float waterLevel;
uniform int sphereRadius;
uniform vec3 sphereOrigin;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_diffuse4;
uniform sampler2D texture_diffuse5;
uniform sampler2D texture_diffuse6;

vec4 calculateLighting() {
    // ambient
    vec3 ambient = light.ambient * 0.1; // TODO: Make ambientStrength a variable

    // diffuse
    vec3 lightDir = normalize(light.position - fragPos_worldspace);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff;
    if (normal == 0.0)
        return vec4(1.0,0.0,0.0,1.0);
    //specular
    /*
    vec3 viewDirection = normalize(cameraPos - fragPos_worldspace);
    vec3 reflectDirection = reflect(lightDir, normal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32.0);
    vec3 specular = 0.5 * spec * light.specular;

    return vec4(ambient + diffuse + specular, 1.0);
    */
    return vec4(ambient + diffuse, 1.0);
}

/*
 * TODO: Use a different blending strategy.
 * Blend grass and rock based on normal vector for example, e.g.
 * the more the normal is in y direction, the more grass, the more horizonal, e.g. x/z
 * direction, the more rock.
 * Same with beach. Only sand if it's flat, otherwise rock (or maybe brown rock)
 *
 */
vec4 blendTextures() {
    float height = fragPos_worldspace.y;

    if (sphereRadius > 0)
        height = distance(fragPos_worldspace, sphereOrigin) - sphereRadius; // Subtract radius to get height relative to zero

    float spread = 0.45;
    float part = 1.0 / 4.0;
    vec4 seaGround = texture(texture_diffuse1, texCoords);
    vec4 sand = texture(texture_diffuse2, texCoords);
    if (height < waterLevel-1.0)
        return seaGround;

    if (height < waterLevel)
        return mix(seaGround, sand, 1 - abs(height));
    
    vec4 grass = texture(texture_diffuse3, texCoords);
    vec4 ground = texture(texture_diffuse4, texCoords);
    vec4 rock = texture(texture_diffuse5, texCoords);
    vec4 snow = texture(texture_diffuse6, texCoords);

    //float value = (fragPos_worldspace.y + amplitude) / (amplitude * 2);
    float amp = amplitude + (amplitude / 2.0f);
    float value = min(height + 2.0f, amp) / amp;
    value = clamp((value - spread/2.0) * (1.0 / spread), 0.0, 0.9999);
    int firstPalette = int(floor(value / part));
    float blend = (value - (firstPalette * part)) / part;
    vec4 result;

    switch (firstPalette) {
    case 0: 
        result = mix(sand, grass, blend);
        break;
    case 1: 
        result = mix(grass, ground, blend);
        break;
    case 2: 
        result = mix(ground, rock, blend);
        break;
    case 3: 
        result = mix(rock, snow, blend);
        break;
    }
    return result;
}

/*
float calculateFog(float distCameraToPoint, vec3 dirCamToPoint) {
    float fogFalloff = 2.0; // uniform value
    float fogStrength = 18.0 * exp(-cameraHeightAboveSurfave * fogFalloff) *
                            (1.0 - exp(-distCameraToPoint * dirCamToPoint.y * fogFalloff)) / dirCamToPoint.y;
    vec3 fogColor = vec3(0.5, 0.6, 0.7);
    return clamp(fogStrength, 0.0, 1.0);
}
*/
/*
float fog2(vec3 inCol) {
    vec3 fogColor = vec3(0.6, 0.7, 0.8);
    float fogStart = 120;
    float fogEnd = 200;
    float dist = length(cameraPos - fragPos_worldspace);
    float fogFac = clamp(((fogEnd - dist) / (fogEnd - fogStart)), 0.125, 1.0);
    return mix(fogColor, inCol, fogFac);
}
*/

void main()
{
    vec4 mix = calculateLighting() * blendTextures();
    //vec4 mix = vec4(0.8,0.8,0.8, 1.0) * blendTextures();

    //float fog = calculateFog(distance(cameraPos, fragPos_worldspace), normalize(fragPos_worldspace - cameraPos));
    FragColor = mix;
}