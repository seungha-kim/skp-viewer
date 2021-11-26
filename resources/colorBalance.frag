#version 330 core
in vec2 TexCoord;

uniform sampler2D tex;
uniform vec3 colorBalance;

out vec3 FragColor;

void main() {
    // TODO: set blue as 1.0
    FragColor = vec3(texture(tex, TexCoord)) + colorBalance;
}
