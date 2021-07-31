#language glsl
#shader vertex
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Color;

layout(location = 0) out vec3 vColor;

// layout(std140, UpdateFreqPerFrame, binding = 0) uniform MatrixBuffer
// {
// 	uniform mat4 sys_ProjectionMatrix;
// 	uniform mat4 sys_ViewMatrix;
// 	uniform mat4 sys_ModelMatrix;
// } Matrices;

void main()
{
	//gl_Position = Matrices.sys_ProjectionMatrix * Matrices.sys_ViewMatrix * Matrices.sys_ModelMatrix * vec4(Position, 1.0f);
	gl_Position = vec4(Position, 1.0f);
	vColor = Color;
}

#shader fragment
layout(location = 0) in vec3 vColor;

layout(location = 0) out vec4 FragColor;

void main()
{
	FragColor = vec4(vColor, 1.0f);
}