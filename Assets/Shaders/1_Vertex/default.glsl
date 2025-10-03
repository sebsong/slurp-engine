#version 330 core
layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec2 inTextureCoord;

uniform vec2 positionTransform;

out vec2 textureCoord;

void main() {
    vec2 position = inPosition + positionTransform;
    gl_Position = vec4(position, 0, 1);

    textureCoord = inTextureCoord;
}