#language glsl
#shader vertex
layout(location = 0) in vec3 Position;

layout(std140, UpdateFreqPerFrame, binding = 0) uniform MatrixBuffer
{
	uniform mat4 sys_ProjectionMatrix;
	uniform mat4 sys_ViewMatrix;
	uniform mat4 sys_ModelMatrix;
} Matrices;

void main()
{
	gl_Position = Matrices.sys_ProjectionMatrix * Matrices.sys_ViewMatrix * Matrices.sys_ModelMatrix * vec4(Position, 1.0f);
}

#shader fragment
layout(location = 0) out vec4 FragColor;

void main()
{
	FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}