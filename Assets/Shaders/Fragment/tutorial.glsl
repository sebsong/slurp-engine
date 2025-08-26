#version 330 core
uniform float time;
out vec4 Color;

void main() {
    //    Color = vec4(fragColor, 1.f);
    float redValue = 0.5f * (1.f + cos(time));
    float blueValue = 0.5f * (1.f + sin(time));
    Color = vec4(redValue, 0.2f, blueValue, 1.f);
}
