// Vertex Shader
#version 460 core

layout(location=0) in vec3 aPosition;

layout(location=0) out vec2 fragCoord;

uniform mat4 u_transform;
uniform mat4 u_projection;
uniform mat4 u_view;

void main() {
    fragCoord = aPosition.xy + vec2(0.5);
    gl_Position = u_projection * u_view * (u_transform * vec4(aPosition, 1.0));
}
