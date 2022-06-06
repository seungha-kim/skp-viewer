#version 330 core

//PLACEHOLDER//

#if defined(VERT)
#define INOUT out
#elif defined(FRAG)
#define INOUT in
#endif

INOUT vec2 TexCoord;

#ifdef VERT
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

void main() {
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
#endif

#if defined(SIMPLE) && defined(FRAG)
uniform sampler2D tex;

out vec4 FragColor;

void main() {
    FragColor = texture(tex, TexCoord);
}
#endif

#if defined(TONE_MAP) && defined(FRAG)
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
#endif

#if defined(MULTIPLICATIVE_BLEND) && defined(FRAG)
uniform sampler2D tex1;
uniform sampler2D tex2;

out vec4 FragColor;

void main() {
    FragColor = texture(tex1, TexCoord) * texture(tex2, TexCoord);
}
#endif

#if defined(GAUSSIAN_BLUR) && defined(FRAG)
uniform sampler2D tex;
uniform bool horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

out vec4 FragColor;

void main()
{
    vec2 tex_offset = 1.0 / textureSize(tex, 0); // gets size of single texel
    vec3 result = texture(tex, TexCoord).rgb * weight[0]; // current fragment's contribution
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(tex, TexCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(tex, TexCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(tex, TexCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(tex, TexCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    FragColor = vec4(result, 1.0);
}
#endif

#if defined(COLOR_BALANCE) && defined(FRAG)
uniform sampler2D tex;
uniform vec3 colorBalance;

out vec3 FragColor;

void main() {
    vec3 color = vec3(texture(tex, TexCoord));
    vec3 balanced = pow(color, vec3(1.0) / colorBalance);
    FragColor = balanced;
}
#endif

#if defined(BRIGHT_FILTER) && defined(FRAG)
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
#endif

#if defined(ADDITIVE_BLEND) && defined(FRAG)
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform float c1;
uniform float c2;

out vec4 FragColor;

void main() {
    FragColor = c1 * texture(tex1, TexCoord) + c2 * texture(tex2, TexCoord);
}
#endif
