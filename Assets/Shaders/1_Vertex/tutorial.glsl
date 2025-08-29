#version 330 core
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTextureCoord;

uniform float time;

out vec3 fragPosition;
out vec2 textureCoord;

void main() {
    float xOffset = sin(time) / 2;
    float yOffset = cos(time * 4) / 4;
    vec3 position = vec3(inPosition.x + xOffset, inPosition.y + yOffset, inPosition.z);
    gl_Position = vec4(position, 1.0);
    fragPosition = position;
    textureCoord = inTextureCoord;
}
