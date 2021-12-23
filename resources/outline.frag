#version 330 core
//#pragma debug(on)
out vec4 FragColor;

/* keep in sync with DRWManager.view_data */
//uniform viewBlock
//{
/* Same order as DRWViewportMatrixType */
    uniform mat4 ViewProjectionMatrix;
    uniform mat4 ViewProjectionMatrixInverse;
    uniform mat4 ViewMatrix;
    uniform mat4 ViewMatrixInverse;
    uniform mat4 ProjectionMatrix;
    uniform mat4 ProjectionMatrixInverse;

//    vec4 clipPlanes[6];

/**
 * If ortho : view_vecs[0] is the near-bottom-left corner of the frustum and
 *            view_vecs[1] is the vector going from the near-bottom-left corner to
 *            the far-top-right corner.
 * If Persp : view_vecs[0].xy and view_vecs[1].xy are respectively the bottom-left corner
 *            when Z = 1, and top-left corner if Z = 1.
 *            view_vecs[0].z the near clip distance and view_vecs[1].z is the (signed)
 *            distance from the near plane to the far clip plane.
 */

/* View frustum corners [NDC(-1.0, -1.0, -1.0) & NDC(1.0, 1.0, 1.0)].
 * Fourth components are near and far values. */
    uniform vec4 ViewVecs[2]; // TODO: 정체 알아보기 TODO TODO TODO TODO TODO TODO TODO TODO
//};

in vec3 fragPos;
in vec3 worldNormal;

uniform float width;
uniform float depthThreshold;
uniform sampler2D maxzBuffer; // 요거는... depthbuffer 그대로 넣으면 되려나?
//https://www.khronos.org/opengl/wiki/Array_Texture
//uniform sampler2DArray utilTex;

// source/blender/draw/intern/shaders/common_view_lib.glsl
#define normal_object_to_view(n) (mat3(ViewMatrix) * (NormalMatrix * n))
#define normal_object_to_world(n) (NormalMatrix * n)
#define normal_world_to_object(n) (NormalMatrixInverse * n)
#define normal_world_to_view(n) (mat3(ViewMatrix) * n)
#define normal_view_to_world(n) (mat3(ViewMatrixInverse) * n)

#define point_object_to_ndc(p) (ViewProjectionMatrix * vec4((ModelMatrix * vec4(p, 1.0)).xyz, 1.0))
#define point_object_to_view(p) ((ViewMatrix * vec4((ModelMatrix * vec4(p, 1.0)).xyz, 1.0)).xyz)
#define point_object_to_world(p) ((ModelMatrix * vec4(p, 1.0)).xyz)
#define point_view_to_ndc(p) (ProjectionMatrix * vec4(p, 1.0))
#define point_view_to_object(p) ((ModelMatrixInverse * (ViewMatrixInverse * vec4(p, 1.0))).xyz)
#define point_view_to_world(p) ((ViewProjectionMatrix * vec4(p, 1.0)).xyz)
#define point_world_to_ndc(p) (ViewProjectionMatrix * vec4(p, 1.0))
#define point_world_to_object(p) ((ModelMatrixInverse * vec4(p, 1.0)).xyz)
#define point_world_to_view(p) ((ViewMatrix * vec4(p, 1.0)).xyz)

float get_view_z_from_depth(float depth)
{
    // if perspective
    if (ProjectionMatrix[3][3] == 0.0) {
        float d = 2.0 * depth - 1.0;
        // 이게 linearize 한건가? -> 맞음 https://stackoverflow.com/a/45710371
        return -ProjectionMatrix[3][2] / (d + ProjectionMatrix[2][2]);
    }
    else {
        // if ortho
        return ViewVecs[0].z + depth * ViewVecs[1].z;
    }
}

vec3 get_view_space_from_depth(vec2 uvcoords, float depth)
{
    // if perspectivee
    if (ProjectionMatrix[3][3] == 0.0) {
        return vec3(ViewVecs[0].xy + uvcoords * ViewVecs[1].xy, 1.0) * get_view_z_from_depth(depth);
    }
    else {
        // if orhot
        return ViewVecs[0].xyz + vec3(uvcoords, depth) * ViewVecs[1].xyz;
    }
}

void calc_outline(vec3 normal,
float width,
out float depth,
out vec3 depth_hit_position,
out float negative_depth,
out vec3 negative_depth_hit_position,
out float object,
out float width_ws_size,
out float debug1,
out float debug2,
out float debug3
)
{
    // TODO: normal_world_to_view - source/blender/draw/intern/shaders/common_view_lib.glsl
    vec3 viewNormal = normalize(normal_world_to_view(normal));
    depth_hit_position = vec3(0,0,0);
    negative_depth_hit_position = vec3(0, 0, 0);

    ivec2 texel = ivec2(gl_FragCoord.xy);
    // TODO: maxzBuffer - source/blender/draw/engines/eevee/shaders/raytrace_lib.glsl 에서 쓰고 있다? 이건 어디서 넣어주는 건가
    // TODO: texelFetch - 이건 GLSL 내장 https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/texelFetch.xhtml
    float texel_depth = texelFetch(maxzBuffer, texel, 0).r;
    // TODO: get_view_z_from_depth - source/blender/draw/intern/shaders/common_view_lib.glsl
    float texel_z = get_view_z_from_depth(texel_depth);

    vec2 texel_uv = vec2(texel) / textureSize(maxzBuffer, 0).xy;
    vec3 texel_vs = get_view_space_from_depth(texel_uv, texel_depth);

    //ivec2 offsets[4] = ivec2[4](ivec2(-1, -1), ivec2(-1, 1), ivec2(1, -1), ivec2(1, 1));
    ivec2 offsets[4] = ivec2[4](ivec2(-1, 0), ivec2(1, 0), ivec2(0, -1), ivec2(0, 1));

    // TODO: viewPosition - ? 몇개 나오는데 어딘지 모르겠다 view space 상의 position 인가?
//    float camera_dot = dot(viewNormal, normalize(-viewPosition));
    // 면이 카메라를 바라보고 있으면 1, 수직이면 0, 반대를 바라보고 있으면 -1

    /*
    vec2 camera_jitter = vec2(ProjectionMatrix[2][0], ProjectionMatrix[2][1]);

    // If orthographic projection
    if (ProjectionMatrix[3][3] != 0.0)
    {
      camera_jitter = vec2(ProjectionMatrix[3][0], ProjectionMatrix[3][1]);
    }
    */

    float max_depth = 0.0;
    float min_depth = 0.0;

    // TODO: utilTex - source/blender/draw/engines/eevee/shaders/common_utiltex_lib.glsl
//    float noise = texelFetch(utilTex, ivec3(0, 0, 2.0), 0).r;
//    if (noise < fract(width))
//    {
//        width = ceil(width);
//    }
//    else
//    {
//        width = floor(width);
//    }

    for (int w = 1; w <= int(round(width)); w++) {
        for (int i = 0; i < 4; i++)
        {
            ivec2 sample_offset = offsets[i] * w;

            ivec2 offset = texel + sample_offset;
            float offset_depth = texelFetch(maxzBuffer, offset, 0).r;
            float offset_z = get_view_z_from_depth(offset_depth);

            // TODO: textureSize - GLSL 내장
            vec2 offset_uv = vec2(offset) / textureSize(maxzBuffer, 0).xy;
            // TODO: get_view_space_from_depth - source/blender/draw/intern/shaders/common_view_lib.glsl
            vec3 offset_vs = get_view_space_from_depth(offset_uv, texel_depth);
            vec3 actual_offset_vs = get_view_space_from_depth(offset_uv, offset_depth);

            width_ws_size = length(offset_vs - texel_vs);

            vec3 ray_origin = vec3(0,0,0);
            // If orthographic projection
            if (ProjectionMatrix[3][3] != 0.0)
            {
                ray_origin = get_view_space_from_depth(offset_uv, 0);
            }

            vec3 ray_direction = normalize(offset_vs - ray_origin);

            // ray-plane intersection
            // P = P1 + v_d * t
            // dot(v_n, P - P2) = 0
            // => t = dot(v_n, P2 - P1) / dot(v_n, v_d)
            // 여기서 len(v_d) == 1 이면 t가 곧 거리
            float expected_distance = dot(texel_vs - ray_origin, viewNormal) /
            dot(ray_direction, viewNormal);

            //        if (i == 4) {
            debug1 = 0.0;//max(debug1, dot(texel_vs - ray_origin, viewNormal));
            debug2 = 0.0;//max(debug2, dot(ray_direction, viewNormal));
            debug3 = max(debug3, pow(length(offset_vs - actual_offset_vs), 10.0));
            //        }

            // TODO: miguel pozo 랑 다르게 미리 나눠버림.
            float delta_depth = (length(actual_offset_vs - ray_origin) - expected_distance) / width_ws_size;

            // 더 들어간 경우 (볼록하거나, 뒤에 있음)
            max_depth = max(max_depth, delta_depth);
            if (max_depth == delta_depth)
            {
                // TODO: point_view_to_world - source/blender/draw/intern/shaders/common_view_lib.glsl
                depth_hit_position = point_view_to_world(actual_offset_vs) - point_view_to_world(texel_vs);
                //            depth_hit_position = actual_offset_vs - texel_vs;
            }
            // 덜 들어간 경우 (오목하거나, 앞에 있음)
            min_depth = min(min_depth, delta_depth);
            if (min_depth == delta_depth)
            {
                //            negative_depth_hit_position = actual_offset_vs - texel_vs;
                negative_depth_hit_position = point_view_to_world(actual_offset_vs) - point_view_to_world(texel_vs);
            }
        }

    }

    depth = max_depth;
    negative_depth = -min_depth;
    object = 0;

//    debug1 = ViewVecs[0][0] / -2.0;
//    debug2 = ViewVecs[0][1] / -2.0;
//    debug3 = ViewVecs[0][2] / 2.0;
//    debug1 = texel_vs.x;
//    debug2 = texel_vs.y;
//    debug3 = texel_vs.z;
}

void main() {
    float depth;
    vec3 depth_hit_position;
    float negative_depth;
    vec3 negative_depth_hit_position;
    float object;
    float width_ws_size;
    float debug1 = 0.0, debug2 = 0.0, debug3 = 0.0;

//    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
//    return;
    calc_outline(worldNormal, width, depth, depth_hit_position, negative_depth, negative_depth_hit_position, object, width_ws_size, debug1, debug2, debug3);
    float value = max(depth, negative_depth);

    // ### 왜 중간에 그림자같은 것이 생기는가
    // [x] depth 계산 문제 없음
    // [ ] texel_uv 문제 없음 -> 방향 맞나? -> 오오 이거 이상했음
    // [x] texel_z
    // [ ] texel_vs (ray_direction과 관련) 어 이거 이상하네..
    // [ ] ViewVecs - 이게 정의랑 달랐구만
    // [x] viewMatrix 등 - 이건 이미 모델이 잘 보인다는 데서부터..
//     FragColor = vec4(debug1, debug2, debug3, 1.0);

    // [x] depth_hit_position 말은 되는 것 같은데 좀 이상함
//     FragColor = vec4(depth_hit_position, 1.0);

    ivec2 texel = ivec2(gl_FragCoord.xy);
    // TODO: maxzBuffer - source/blender/draw/engines/eevee/shaders/raytrace_lib.glsl 에서 쓰고 있다? 이건 어디서 넣어주는 건가
    // TODO: texelFetch - 이건 GLSL 내장 https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/texelFetch.xhtml
    float texel_depth = texelFetch(maxzBuffer, texel, 0).r;
    // TODO: get_view_z_from_depth - source/blender/draw/intern/shaders/common_view_lib.glsl
    float texel_z = get_view_z_from_depth(texel_depth);

    vec2 texel_uv = vec2(texel) / textureSize(maxzBuffer, 0).xy;
    vec3 texel_vs = get_view_space_from_depth(texel_uv, texel_depth);


    vec3 incoming_vs = normalize(-texel_vs);
    vec3 viewNormal = normalize(normal_world_to_view(worldNormal));
    float perp = dot(incoming_vs, viewNormal);
    float perp2 = pow(clamp(perp, 0.0, 1.0), 2.0);

//    FragColor = vec4(debug1, debug2, debug3, 1.0);

    // NOTE: ViewVecs 고쳤음.
//    FragColor = vec4(vec3(-texel_z / 10.0), 1.0); // [ ]
// 이거 uv랑 왜 비슷해보이냐. 그러면 안되는데. viewspace 니까 앞뒤로 움직이면 달라져야 되는데
// *** 왠지 texel_vs 원점 계산이 잘못되고 있는 느낌. *********************
/// 화면 중앙이 0이어야 하는데 아니고, 부호도 이상한 느낌
//    FragColor = vec4(normalize(-texel_vs), 1.0); // [ ] incoming 이 이상하다는건 texel_vs 가 이상하다는 것
//    FragColor = vec4(texel_vs, 1.0); // [ ] incoming 이 이상하다는건 texel_vs 가 이상하다는 것
//    FragColor = vec4(abs(incoming_vs.x * 2), abs(incoming_vs.y * 2), incoming_vs.z, 1.0); // [ ] 이거 이상해
//    FragColor = vec4(depth, negative_depth, 0.0, 1.0); // [ ] 궁극! 가로줄!
//    FragColor = vec4(vec3(pow(texel_depth, 100)), 1.0); // [x]
//    FragColor = vec4(fragPos, 1.0); // [x] 비교용
//    FragColor = vec4(abs(ViewVecs[0].xy + ViewVecs[1].xy * texel_uv) * 2.0, 1.0, 1.0); // [x] 아 ViewVecs 가 잘못된 거였음
//    FragColor = vec4(-ViewVecs[0].xy, 1.0, 1.0); // [x] 아래꺼랑 같음
//    FragColor = vec4(ViewVecs[1].xy, 1.0, 1.0); // [x] 위에꺼랑 같음
//    FragColor = vec4(texel_uv, 0.0, 1.0); // [x]
//    FragColor = vec4(vec3(perp2), 1.0); // [x] clamp 하니까 괜찮
//    FragColor = vec4(vec3(perp), 1.0); // [x]
//    FragColor = vec4(viewNormal.x, viewNormal.y, viewNormal.z, 1.0); // [x]
    // TODO: 뭐지 uv 가 내가 생각한 거랑 달라...
    // TODO: 아 outline 코드의 버그인가... depth 나 uv가 그 의미가 아닐수도
    // TODO: plane 크기를 키워서 아주 멀리 있는 표면일 떄 어떻게 되는지 실험 (a.k.a 씽크홀 현상)

    // 의도
    value = value - perp2;
    if (value > depthThreshold) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    } else {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }


// TODO: [x] view frustum 과 관련되어 보이는 줄 나타나는 현상
// 면이 시점방향과 특정 각도를 이룰 때 생기는 문제인데...
// normal 과 관계가 있고 일단
// view 계산 방식과도 관계가 있는데
// [ ] viewvecs 문제일까? <- 맞았음 ㅠㅠ TODO: 기록하기
// [x] viewNormal?

// 이거 그려보면 debug1이 0이 되려고 하는 중간에 depth가 빡 생기는 것 같은 느낌인데
//debug1 = dot(texel_vs - ray_origin, viewNormal);
//debug2 = dot(ray_direction, viewNormal);
//debug3 = 0.0;


//    FragColor = vec4(vec3(1 - exp(-value)), 1.0);
}
