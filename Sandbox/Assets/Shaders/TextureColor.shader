#language glsl
#shader vertex

layout(location = 0) in vec3 Position;
layout(location = 1) in vec4 Color;
layout(location = 2) in vec2 UV;
layout(location = 4) out vec2 vUV;

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

layout(location = 0) out vec4 FragColor;

layout(location = 4) in vec2 vUV;

layout(binding = 0) uniform sampler2D Texture;

layout(std140, binding = 1) uniform DataBuffer
{
    float time;
} Data;

void main()
{
    vec3 color = 0.5f + 0.5f * cos(Data.time + vUV.xyx + vec3(0.0f, 2.0f, 4.0f));

	FragColor = texture(Texture, vUV) * vec4(color, 1.0f);
}