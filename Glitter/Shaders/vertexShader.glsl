#version 450
/*
 * 1 VertexShader
 */
in layout (location = 0) vec4 position;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec3 cameraPos;

out vec4 pos;

void main()
{
    vec4 viewPos = viewMatrix * vec4(position.xyz, 1.0);
    gl_Position = projMatrix * viewPos;
}