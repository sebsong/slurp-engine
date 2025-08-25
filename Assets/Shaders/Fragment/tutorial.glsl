#version 330 core
in vec3 fragColor;
out vec4 Color;

void main() {
    Color = vec4(fragColor, 1.f);
}
