#version 330 core
in vec2 textureCoord;

uniform sampler2D inTexture;
uniform float alpha;
uniform float progress;
uniform bool isVertical;

out vec4 color;

void main() {
    vec4 texColor = texture(inTexture, textureCoord);
    texColor.a *= alpha;
    float fillValue = isVertical ? textureCoord.y : textureCoord.x;
    if (texColor.a == 0 || fillValue > progress) {
        discard;
    }
    color = texColor.bgra;
}