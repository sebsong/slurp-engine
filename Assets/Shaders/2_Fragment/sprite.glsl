#version 330 core
in vec2 textureCoord;

uniform sampler2D inTexture;
uniform float alpha;

uniform vec4 srcColor;
uniform vec4 dstColor;

out vec4 outColor;

void main() {
    vec4 texColor = texture(inTexture, textureCoord).bgra;
    if (texColor == srcColor) {
        texColor = dstColor;
    }
    texColor.a *= alpha;
    if (texColor.a == 0) {
        discard;
    }
    outColor = texColor;
}