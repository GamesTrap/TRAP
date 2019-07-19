#version 460 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec4 Color;

layout(location = 2) out vec4 vColor;

uniform mat4 sys_ProjectionMatrix;
uniform mat4 sys_ViewMatrix;

void main()
{
	gl_Position = sys_ProjectionMatrix * sys_ViewMatrix * vec4(Position, 1.0f);
	vColor = Color;
}