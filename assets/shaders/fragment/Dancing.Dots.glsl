#version 460 core

layout(location=0) in vec2 fragCoord;

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

out vec4 fragColor;


void mainImage(out vec4 O, vec2 F) {
    vec2 r = u_resolution.xy, u = (F+F-r)/r.y/.6;
    float i, t = u_time+26.;
         
    for (O *= i = 0.; (i+=.1) < 5.; O += pow(
        .01/length(u+vec2(sin(t*2.1+i),cos(t+i)))
        *(cos(i*6.+vec4(0,1,2,0))+1.),vec4(1.3))
    ) u *= mat2(cos(t*.02+vec4(0,33,11,0)));
}

void main() {
    mainImage(fragColor, fragCoord);
}
