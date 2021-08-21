#version 450
/*
 * 1 VertexShader
 */
in layout (location = 0) vec4 position;
in layout (location = 1) vec4 normal;

uniform vec3 cameraPos;
uniform vec3 lightPos;

out vec4 passPosition_c;
out vec4 passNormal_c;

void main()
{
    passPosition_c = position;
    passNormal_c = normal;
}