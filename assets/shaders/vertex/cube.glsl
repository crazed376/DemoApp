// Cube vertex shader
#version 460 core

layout(location=0) in vec3 aPosition;
layout(location=1) in vec2 aTexCoords;

// NOTE: These attributes are not required for the challenge
layout(location=2) in vec3 aNormal;
layout(location=3) in vec3 aTangent;
layout(location=4) in vec3 aBitangent;

out vec2 texCoords;

// NOTE: These variables are not required for the challenge
out vec3 normal;
out mat3 TBN;

uniform mat4 u_transform;
uniform mat4 u_projection;
uniform mat4 u_view;

void main() {
    texCoords = aTexCoords;

    // NOTE: This block is not required for the challenge
    normal = aNormal;
    vec3 T = normalize(vec3(vec4(aTangent,   0.0)));
    vec3 B = normalize(vec3(vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(vec4(aNormal,    0.0)));
    TBN = mat3(T, B, N);

    gl_Position = u_projection * u_view * (u_transform * vec4(aPosition, 1.0));
}
