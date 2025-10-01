#version 330 core
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTextureCoord;

out vec3 fragPosition;
out vec2 textureCoord;

void main() {
    fragPosition = inPosition;
    textureCoord = inTextureCoord;
    gl_Position = vec4(inPosition, 1.0);
}