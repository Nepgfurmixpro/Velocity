#version 450

layout(location = 0) in vec3 out_FragColor;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(out_FragColor, 1.0);
}