#version 330 core
in vec2 textureCoord;

uniform sampler2D inTexture;

out vec4 color;

void main() {
    vec4 texColor = texture(inTexture, textureCoord);
    color = texColor.bgra;
}