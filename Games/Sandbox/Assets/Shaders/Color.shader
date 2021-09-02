#shader vertex
layout(location = 0) in vec3 Position;
layout(location = 1) in vec4 Color;

layout(location = 2) out vec4 vColor;

layout(std140, UpdateFreqDynamic, binding = 0) uniform MatrixBuffer
{
	uniform mat4 sys_ProjectionMatrix;
	uniform mat4 sys_ViewMatrix;
} Matrices;

layout(UpdateFreqDynamic, binding = 1) uniform ModelBufferDynamic
{
	uniform mat4 sys_ModelMatrix;
} ModelMatrixDynamic;

void main()
{
	gl_Position = Matrices.sys_ProjectionMatrix * Matrices.sys_ViewMatrix * ModelMatrixDynamic.sys_ModelMatrix * vec4(Position, 1.0f);
	vColor = Color;
}

#shader fragment
layout(location = 0) out vec4 FragColor;

layout(location = 2) in vec4 vColor;

void main()
{
	FragColor = vColor;
}