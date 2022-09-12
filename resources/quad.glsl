#version 330 core

//PLACEHOLDER//

#if defined(VERT)
#define INOUT out
#elif defined(FRAG)
#define INOUT in
#endif

INOUT vec2 TexCoord;

////////////////////
// Uniform Blocks //
////////////////////

// MUST be synchronized with `Uniforms.h`

layout (std140) uniform ViewBlock {
    mat4 viewMatrix;
    mat4 viewMatrixInverse;
    mat4 projectionMatrix;
    mat4 projectionMatrixInverse;
    mat4 viewProjectionMatrix;
    mat4 viewProjectionMatrixInverse;
    vec3 cameraPosition;
};

//////////////////////
// Common Functions //
//////////////////////

float map(float value, float min1, float max1, float min2, float max2) {
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

float linearizeDepth(float depth, float near, float far) {
    // https://learnopengl.com/Advanced-OpenGL/Depth-testing
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

////////////
// Passes //
////////////

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


#if defined(OUTLINE) && defined(FRAG)
uniform sampler2D normalTexture;
uniform sampler2D depthTexture;
uniform float zNear;
uniform float zFar;
uniform int lineWidth;

out vec4 FragColor;

// https://stackoverflow.com/questions/32227283/getting-world-position-from-depth-buffer-value
vec3 ViewPosFromDepth(float depth) {
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(TexCoord * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = projectionMatrixInverse * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;
    return viewSpacePosition.xyz;
}

void main() {
    ivec2 size = textureSize(normalTexture, 0);
    float w = lineWidth / float(size.x);
    float h = lineWidth / float(size.y);

    vec3 normals[9];
    float dotVal = 1.0;
    float normalContrib = 0.0;
    vec2 coord = TexCoord.st;
    normals[0] = normalize(texture(normalTexture, coord).xyz);
    normals[1] = normalize(texture(normalTexture, coord + vec2(      w,     0.0)).xyz);
    normals[2] = normalize(texture(normalTexture, coord + vec2(2.0 * w,     0.0)).xyz);
    normals[3] = normalize(texture(normalTexture, coord + vec2(    0.0,       h)).xyz);
    normals[4] = normalize(texture(normalTexture, coord + vec2(      w,       h)).xyz);
    normals[5] = normalize(texture(normalTexture, coord + vec2(2.0 * w,       h)).xyz);
    normals[6] = normalize(texture(normalTexture, coord + vec2(    0.0, 2.0 * h)).xyz);
    normals[7] = normalize(texture(normalTexture, coord + vec2(      w, 2.0 * h)).xyz);
    normals[8] = normalize(texture(normalTexture, coord + vec2(2.0 * w, 2.0 * h)).xyz);

    for (int i = 1; i < 9; i++) {
        dotVal = dot(normals[i], normals[0]);
        normalContrib += pow(map(dotVal, -1.0, 1.0, 0.0, 1.0), 5.0);
    }
    normalContrib /= 9.0;

    float linDepths[9];
    linDepths[0] = linearizeDepth(texture(depthTexture, coord).r, zNear, zFar);
    linDepths[1] = linearizeDepth(texture(depthTexture, coord + vec2(      w,     0.0)).r, zNear, zFar);
    linDepths[2] = linearizeDepth(texture(depthTexture, coord + vec2(2.0 * w,     0.0)).r, zNear, zFar);
    linDepths[3] = linearizeDepth(texture(depthTexture, coord + vec2(    0.0,       h)).r, zNear, zFar);
    linDepths[4] = linearizeDepth(texture(depthTexture, coord + vec2(      w,       h)).r, zNear, zFar);
    linDepths[5] = linearizeDepth(texture(depthTexture, coord + vec2(2.0 * w,       h)).r, zNear, zFar);
    linDepths[6] = linearizeDepth(texture(depthTexture, coord + vec2(    0.0, 2.0 * h)).r, zNear, zFar);
    linDepths[7] = linearizeDepth(texture(depthTexture, coord + vec2(      w, 2.0 * h)).r, zNear, zFar);
    linDepths[8] = linearizeDepth(texture(depthTexture, coord + vec2(2.0 * w, 2.0 * h)).r, zNear, zFar);

    float linDepthDiff = 0.0;
    for (int i = 1; i < 9; i++) {
        linDepthDiff += abs(linDepths[i] - linDepths[0]);
    }
    linDepthDiff /= 9.0;

    vec3 vsNormal = mat3(viewMatrix) * normals[0];
    vec3 vsPos = ViewPosFromDepth(texture(depthTexture, coord).r);
    float frontFacing = dot(normalize(-vsPos), vsNormal);

    float depthContrib = clamp(map(linDepthDiff * frontFacing, 0.5, 1.0, 1.0, 0.0), 0.0, 1.0);
    FragColor = vec4(vec3(smoothstep(0.0, 1.0, normalContrib * depthContrib)), 1.0);
}
#endif