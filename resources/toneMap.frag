#version 330 core
in vec2 TexCoord;

uniform sampler2D tex;
uniform float exposure;
uniform float gamma;

out vec4 FragColor;

void main() {
    vec3 color = texture(tex, TexCoord).rgb;
    vec3 mapped = vec3(1.0) - exp(-color * exposure);
    vec3 corrected = pow(mapped, vec3(1.0 / gamma));
    FragColor = vec4(corrected, 1.0);
}
