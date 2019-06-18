#version 460 core

layout(location = 0) out vec4 FragColor;

layout(location = 2) in vec2 vTexCoord;
layout(binding = 0) uniform sampler2D Texture;

void main()
{
	vec4 TexColor = texture(Texture, vTexCoord);
	FragColor = TexColor;
}