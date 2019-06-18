#version 460 core

layout(location = 0) out vec4 FragColor;

layout(location = 2) in vec4 vColor;

void main()
{
	FragColor = vColor;
}