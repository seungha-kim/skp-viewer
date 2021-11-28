#version 330 core
in vec2 TexCoord;

uniform sampler2D tex;
uniform vec3 colorBalance;

out vec3 FragColor;

void main() {
    vec3 color = vec3(texture(tex, TexCoord));
    vec3 balanced = pow(color, vec3(1.0) / colorBalance);
    FragColor = balanced;
}
