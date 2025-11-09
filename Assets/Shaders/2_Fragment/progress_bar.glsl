#version 330 core
in vec2 textureCoord;

uniform sampler2D inTexture;
uniform float alpha;
uniform float progress;

out vec4 color;

void main() {
    vec4 texColor = texture(inTexture, textureCoord);
    texColor.a *= alpha;
    if (texColor.a == 0 || textureCoord.y > progress) {
        discard;
    }
    color = texColor.bgra;
}