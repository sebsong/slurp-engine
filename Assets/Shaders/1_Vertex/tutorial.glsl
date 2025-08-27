#version 330 core
layout (location = 0) in vec3 inPosition;
uniform float time;
out vec3 fragPosition;

void main() {
    float xOffset = sin(time) / 2 + 0.5f;
    float yOffset = cos(time * 4) / 4;
    vec3 position = vec3(inPosition.x + xOffset, -inPosition.y + yOffset, inPosition.z);
    gl_Position = vec4(position, 1.0);
    fragPosition = position;
}
