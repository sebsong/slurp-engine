#version 330 core
uniform float time;
in vec3 fragPosition;
out vec4 color;

void main() {
    //    float redValue = 0.5f * (1.f + cos(time));
    //    float blueValue = 0.5f * (1.f + sin(time));
    //    color = vec4(redValue, 0.2f, blueValue, 1.f);
    vec3 colorOffset = vec3(0.5, 0.5, 0.5);
    color = vec4(fragPosition / 2 + colorOffset, 1);
}
