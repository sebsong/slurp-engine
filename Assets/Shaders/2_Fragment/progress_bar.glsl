#version 330 core
in vec2 textureCoord;

uniform sampler2D inTexture;
uniform float progress;

out vec4 color;

void main() {
    vec4 texColor = texture(inTexture, textureCoord);
    if (texColor.a == 0 || textureCoord.y > progress) {
        discard;
    }
    color = texColor.bgra;
}