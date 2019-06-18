#version 460 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TexCoord;

layout(location = 2) out vec2 vTexCoord;

void main()
{
	gl_Position = vec4(Position, 1.0f);
	vTexCoord = TexCoord;
}