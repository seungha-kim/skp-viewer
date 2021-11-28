#version 330 core
in vec2 TexCoord;

uniform sampler2D tex1;
uniform sampler2D tex2;
uniform float c1;
uniform float c2;

out vec4 FragColor;

void main() {
    FragColor = c1 * texture(tex1, TexCoord) + c2 * texture(tex2, TexCoord);
}
