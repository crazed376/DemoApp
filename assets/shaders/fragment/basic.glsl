// Fragment shader
#version 460 core

layout(location=0) in vec2 fragCoord;

out vec4 fragColor;

uniform vec3 u_color;

void main() {
    fragColor = vec4(u_color, 1.0);
}
