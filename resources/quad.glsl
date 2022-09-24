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
    vec3 cameraFront;
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

    vec3 normalAdj[3];
    float angleContrib = 1.0;
    vec2 coord = TexCoord.st;
    vec3 normal = texture(normalTexture, coord).xyz;

    normalAdj[0] = normalize(texture(normalTexture, coord + vec2( -w, -h)).xyz);
    normalAdj[1] = normalize(texture(normalTexture, coord + vec2(0.0, -h)).xyz);
    normalAdj[2] = normalize(texture(normalTexture, coord + vec2(  w, -h)).xyz);
    for (int i = 0; i < 3; i++) {
        angleContrib = min(angleContrib, dot(normalAdj[i], normal));
    }
    angleContrib = pow(clamp(angleContrib, 0.0, 1.0), 2.0);
    float depth = texture(depthTexture, coord).r;
    float linDepthAdj[3];
    float linDepth = linearizeDepth(depth, zNear, zFar);
    float linDepthDiff = 0.0;
    linDepthAdj[0] = linearizeDepth(texture(depthTexture, coord + vec2( -w, -h)).r, zNear, zFar);
    linDepthAdj[1] = linearizeDepth(texture(depthTexture, coord + vec2(0.0, -h)).r, zNear, zFar);
    linDepthAdj[2] = linearizeDepth(texture(depthTexture, coord + vec2(  w, -h)).r, zNear, zFar);
    for (int i = 0; i < 3; i++) {
        linDepthDiff = max(linDepthDiff, abs(linDepthAdj[i] - linDepth));
    }

    vec3 vsNormal = mat3(viewMatrix) * normal;
    vec3 vsPos = ViewPosFromDepth(texture(depthTexture, coord).r);

    float frontFacing = dot(normalize(-vsPos), vsNormal);
    float depthContrib = clamp(map(linDepthDiff * frontFacing, 0.0, 1.0, 1.0, 0.0), 0.0, 1.0);
    float outColor = angleContrib * depthContrib;
    if (outColor > 0.3) {
        outColor = 1.0;
    }
    FragColor = vec4(vec3(outColor), 1.0);
}
#endif