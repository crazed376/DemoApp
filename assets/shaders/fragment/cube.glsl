// Cube fragment shader
#version 460 core

in vec2 texCoords;

// NOTE: These are not required for the challenge
in vec3 normal;
in mat3 TBN;

out vec4 fragColor;

uniform sampler2D u_texture;

// NOTE: These are not required for the challenge
uniform sampler2D u_normalTexture;

const vec3 sunlightDir = normalize(vec3(-0.3, -0.5, -0.2));
const float ambientOcclusion = 0.3f;

void main() {
    // NOTE: For the challenge all you need here is something like
    // fragColor = texture(u_texture, texCoords);
    // Everything in this shader is just to enable normal surface lighting

    vec4 defaultColor = vec4(0.6, 0.1, 0.04, 1.0);
    vec4 albedo = texture(u_texture, texCoords);
    float lightStrength = clamp(-dot(normal, sunlightDir), 0, 1);
    lightStrength = max(ambientOcclusion, lightStrength);

    vec3 textureNormal = normalize(texture(u_normalTexture, texCoords).xyz * 2.0 - 1.0);
    textureNormal = normalize(TBN * textureNormal);
    float incidentAngle = -dot(textureNormal, sunlightDir);
    float normalLightStrength = clamp(incidentAngle, 0.0, 1.0);
    normalLightStrength = (normalLightStrength * 0.7) + 0.3;

    if(incidentAngle < 0) {
        normalLightStrength -= ((incidentAngle * 0.7) + 0.3);
        normalLightStrength = clamp(normalLightStrength, 0, 1);
    }

    vec4 finalColor = albedo * normalLightStrength;

    fragColor = finalColor;
}
