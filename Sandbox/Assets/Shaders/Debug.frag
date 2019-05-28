#version 460 core

layout(location = 0) out vec4 FragColor;

in vec3 vPos;

void main()
{
	FragColor = vec4(vPos * 1.0f + 0.5f, 1.0f);
}