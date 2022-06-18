#language glsl
#shader vertex
layout(location = 0) in vec3 Position;
layout(location = 2) in vec2 UV;

layout(location = 0) out vec2 vUV;

layout(std140, binding = 0) uniform MatrixBuffer
{
	mat4 sys_ProjectionMatrix;
	mat4 sys_ViewMatrix;
	mat4 sys_ModelMatrix;
} Matrices;

void main()
{
	gl_Position = Matrices.sys_ProjectionMatrix * Matrices.sys_ViewMatrix * Matrices.sys_ModelMatrix * vec4(Position, 1.0f);
	vUV = UV;
}

#shader fragment
layout(location = 0) in vec2 vUV;

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D Texture;

void main()
{
	FragColor = texture(Texture, vUV);
}