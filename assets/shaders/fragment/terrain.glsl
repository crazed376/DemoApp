// Terrain fragment shader
#version 460 core

layout(location=0) in vec3 fragCoord;
layout(location=1) in vec3 normal;

out vec4 fragColor;

vec3 lightColor = vec3(0.9);
vec3 lightPos = vec3(3.0, 30.0, 3.0);

void main() {
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragCoord);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    fragColor = vec4(diffuse, 1.0);
}
