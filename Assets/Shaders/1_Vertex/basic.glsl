#version 330 core
layout (location = 0) in vec2 inPosition;

uniform float zOrder;

void main() {
    gl_Position = vec4(inPosition, zOrder, 1);
}