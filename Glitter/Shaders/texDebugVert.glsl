#version 450

in layout (location = 0) vec4 position;

out vec2 uv;

void main()
{
    uv = vec2((position.x + 1) * 0.5, (position.y + 1) * 0.5);
    gl_Position = position;
}