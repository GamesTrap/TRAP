#version 460 core

layout(location = 0) in vec3 Position;

layout(location = 1) out vec3 vPos;

void main()
{
	vPos = Position;
	gl_Position = vec4(Position, 1.0f);
}