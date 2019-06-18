#version 460 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec4 Color;

layout(location = 2) out vec4 vColor;

void main()
{
	gl_Position = vec4(Position, 1.0f);
	vColor = Color;
}