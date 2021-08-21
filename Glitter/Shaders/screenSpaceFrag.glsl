#version 450 core

in vec2 uv;
out vec4 FragColor;

layout (binding = 0) uniform sampler2D textureSampler;

void main()
{
	FragColor = texture(textureSampler, uv);
}