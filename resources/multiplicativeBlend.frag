#version 330 core
in vec2 TexCoord;

uniform sampler2D tex1;
uniform sampler2D tex2;

out vec4 FragColor;

void main() {
    FragColor = texture(tex1, TexCoord) * texture(tex2, TexCoord);
}
