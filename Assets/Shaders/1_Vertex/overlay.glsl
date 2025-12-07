#version 330 core
layout (location = 0) in vec2 inPosition;

uniform vec2 positionTransform;
uniform float zOrder;

out vec2 position;

void main() {
    position = inPosition + positionTransform;
    gl_Position = vec4(position, zOrder, 1);
}