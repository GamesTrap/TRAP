#shader vertex
layout(location = 0) in vec3 Position;
layout(location = 1) in vec4 Color;

layout(location = 2) out vec4 vColor;

layout(std140, UpdateFreqDynamic, binding = 0) readonly buffer MatrixBuffer
{
	buffer mat4 sys_ProjectionMatrix;
	buffer mat4 sys_ViewMatrix;
} Matrices;

layout(std140, UpdateFreqDynamic, binding = 1) readonly buffer ModelBuffer
{
	buffer mat4 sys_ModelMatrix[];
} ModelMatrix;

void main()
{
	gl_Position = Matrices.sys_ProjectionMatrix * Matrices.sys_ViewMatrix * ModelMatrix.sys_ModelMatrix[gl_BaseInstance] * vec4(Position, 1.0f);
	vColor = Color;
}

#shader fragment
layout(location = 0) out vec4 FragColor;

layout(location = 2) in vec4 vColor;

void main()
{
	FragColor = vColor;
}