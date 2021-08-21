#version 450 core

in vec2 uv;
out vec4 FragColor;

uniform bool is3D;
uniform int layer;
uniform int channel;
uniform int dimension;

layout(binding=0) uniform sampler2D texture_2D;
layout(binding=1) uniform sampler3D texture_3D;

void main()
{
    float value;
    if (!is3D) {
            if (channel == 0) {
                FragColor = texture(texture_2D, uv);
            } else {
                if (channel == 1) value = texture(texture_2D, uv).x;
                if (channel == 2) value = texture(texture_2D, uv).y;
                if (channel == 3) value = texture(texture_2D, uv).z;
                if (channel == 4) value = texture(texture_2D, uv).w;
                FragColor = vec4(value, value, value, 1);
            }
    } else {
        float fDim = float(dimension-1);
        if (channel == 0) {
            FragColor = texture(texture_3D, vec3(uv.x, uv.y, layer / fDim));
        } else {
            if (channel == 1) value = texture(texture_3D, vec3(uv.x, uv.y, layer / fDim)).x;
            if (channel == 2) value = texture(texture_3D, vec3(uv.x, uv.y, layer / fDim)).y;
            if (channel == 3) value = texture(texture_3D, vec3(uv.x, uv.y, layer / fDim)).z;
            if (channel == 4) value = texture(texture_3D, vec3(uv.x, uv.y, layer / fDim)).w;
            FragColor = vec4(value, value, value, 1);
        }
    }
}