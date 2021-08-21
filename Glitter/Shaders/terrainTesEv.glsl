#version 410 core

layout(triangles, equal_spacing, ccw) in;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform float gDispFactor;
uniform sampler2D sand, grass1, grass, rock, snow, rockNormal;

//in vec4 passPosition_e[];
//in vec4 passNormal_e[];

struct OutputPatch
{
    vec3 WorldPos_B030;
    vec3 WorldPos_B021;
    vec3 WorldPos_B012;
    vec3 WorldPos_B003;
    vec3 WorldPos_B102;
    vec3 WorldPos_B201;
    vec3 WorldPos_B300;
    vec3 WorldPos_B210;
    vec3 WorldPos_B120;
    vec3 WorldPos_B111;
    vec3 Normal[3];
    vec2 TexCoord[3];
};

in patch OutputPatch oPatch;

out vec4 passPosition;
out vec4 passNormal;
out vec2 passTex;
out float height;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

void main()
{
    mat4 gVP = projMatrix * viewMatrix;
   // Interpolate the attributes of the output vertex using the barycentric coordinates
    passNormal.xyz = interpolate3D(oPatch.Normal[0], oPatch.Normal[1], oPatch.Normal[2]);
    passTex = interpolate2D(oPatch.TexCoord[0], oPatch.TexCoord[1], oPatch.TexCoord[2]);


    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    float w = gl_TessCoord.z;
    float uPow3 = pow(u, 3);
    float vPow3 = pow(v, 3);
    float wPow3 = pow(w, 3);
    float uPow2 = pow(u, 2);
    float vPow2 = pow(v, 2);
    float wPow2 = pow(w, 2);
    passPosition.xyz = oPatch.WorldPos_B300 * wPow3 + oPatch.WorldPos_B030 * uPow3 + oPatch.WorldPos_B003 * vPow3 +
    oPatch.WorldPos_B210 * 3.0 * wPow2 * u + oPatch.WorldPos_B120 * 3.0 * w * uPow2 + oPatch.WorldPos_B201 * 3.0 * wPow2 * v +
    oPatch.WorldPos_B021 * 3.0 * uPow2 * v + oPatch.WorldPos_B102 * 3.0 * w * vPow2 + oPatch.WorldPos_B012 * 3.0 * u * vPow2 +
    oPatch.WorldPos_B111 * 6.0 * w * u * v;
    height = passPosition.y;
    gl_Position = gVP * vec4(passPosition.xyz, 1.0);

}