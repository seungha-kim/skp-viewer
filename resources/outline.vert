#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

//uniform viewBlock
//{
///* Same order as DRWViewportMatrixType */
//    mat4 ViewProjectionMatrix;
//    mat4 ViewProjectionMatrixInverse;
    uniform mat4 ViewMatrix;
//    mat4 ViewMatrixInverse;
    uniform mat4 ProjectionMatrix;
//    mat4 ProjectionMatrixInverse;
//
////    vec4 clipPlanes[6];
//
///* View frustum corners [NDC(-1.0, -1.0, -1.0) & NDC(1.0, 1.0, 1.0)].
// * Fourth components are near and far values. */
//    vec4 ViewVecs[2];
//};


out vec3 worldNormal;
out vec3 fragPos;

void main() {
    worldNormal = normalMatrix * aNormal;
    gl_Position = ProjectionMatrix * ViewMatrix * modelMatrix * vec4(aPos, 1.0);
    fragPos = aPos;
}
