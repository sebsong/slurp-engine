#version 330 core
in vec3 fragPosition;
in vec2 textureCoord;

uniform float time;
uniform sampler2D inTexture;

out vec4 color;

void main() {
    vec4 texColor = texture(inTexture, textureCoord);
    color = texColor.bgra;
}