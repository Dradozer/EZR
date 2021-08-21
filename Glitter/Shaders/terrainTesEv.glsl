#version 410 core

layout(triangles, equal_spacing, ccw) in;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform float gDispFactor;

in vec4 passPosition_e[];
in vec4 passNormal_e[];

out vec4 passPosition;
out vec4 passNormal;

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
    passNormal.xyz = interpolate3D(passNormal_e[0].xyz, passNormal_e[1].xyz, passNormal_e[2].xyz);
    passNormal.xyz = normalize(passNormal.xyz);
    passPosition.xyz = interpolate3D(passPosition_e[0].xyz, passPosition_e[1].xyz, passPosition_e[2].xyz);

    // Displace the vertex along the normal
    passPosition += passNormal * gDispFactor;
    gl_Position = gVP * vec4(passPosition.xyz, 1.0);
}