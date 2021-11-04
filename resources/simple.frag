#version 330 core
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixRatio;

out vec4 FragColor;

void main() {
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), mixRatio);
}