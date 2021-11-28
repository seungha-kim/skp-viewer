#version 330 core
in vec2 TexCoord;

uniform sampler2D tex;
uniform float threshold;

out vec4 FragColor;

void main() {
    FragColor = texture(tex, TexCoord);
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness < threshold) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
