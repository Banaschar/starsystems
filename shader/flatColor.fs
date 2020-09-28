#version 330 core
out vec4 FragColor;

in vec4 fColor;
in vec3 normal_frag_in;
in vec3 wPos_frag_in;

uniform vec3 sunPos;

vec4 calculateLighting() {
    // ambient
    vec3 ambient = vec3(0.2,0.2,0.2); // TODO: Make ambientStrength a variable

    // diffuse
    vec3 lightDir = normalize(sunPos - wPos_frag_in);
    float diff = max(dot(normal_frag_in, lightDir), 0.0);
    vec3 diffuse = vec3(0.8,0.8,0.8) * diff;

    return vec4(ambient + diffuse, 1.0);
}

void main()
{
    FragColor = vec4(0.2,0.2,0.8,1.0) * calculateLighting();
    //FragColor = vec4(normal_frag_in, 1.0);
    //FragColor = fColor;
}