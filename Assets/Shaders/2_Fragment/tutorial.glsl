#version 330 core
in vec3 fragPosition;
in vec2 textureCoord;

uniform float time;
uniform sampler2D inTexture;

out vec4 color;

void main() {
    //    float redValue = 0.5f * (1.f + cos(time));
    //    float blueValue = 0.5f * (1.f + sin(time));
    //    color = vec4(redValue, 0.2f, blueValue, 1.f);
//    vec3 colorOffset = vec3(0.5, 0.5, 0.5);
//    color = vec4(inFragPosition / 2 + colorOffset, 1);
    color = texture(inTexture, textureCoord);
}
