#version 330 core
in vec2 textureCoord;

uniform sampler2D inTexture;
uniform float alpha;

uniform vec4 srcColor;
uniform vec4 dstColor;
uniform vec4 colorOverride;

out vec4 outColor;

void main() {
    vec4 texColor = texture(inTexture, textureCoord).bgra;
    if (texColor.a == 0) {
        discard;
    }

    vec4 color;
    if (length(colorOverride) > 0) {
        color = colorOverride;
    } else if (texColor == srcColor) {
        color = dstColor;
    } else {
        color = texColor;
    }

    color.a *= alpha;
    if (color.a == 0) {
        discard;
    }

    outColor = color;
}