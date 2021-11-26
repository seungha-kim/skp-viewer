#version 330 core
in vec2 TexCoord;

uniform sampler2D tex;

out vec3 FragColor;

void main() {
    // TODO: set blue as 1.0
    FragColor = vec3(vec2(texture(tex, TexCoord)), 1.0);
}
