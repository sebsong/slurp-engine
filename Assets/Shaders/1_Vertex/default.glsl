#version 330 core
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTextureCoord;

uniform vec2 positionTransform;

out vec2 textureCoord;

void main() {
    vec3 position = inPosition + vec3(positionTransform, 0);
    gl_Position = vec4(position, 1.0);

    textureCoord = inTextureCoord;
}