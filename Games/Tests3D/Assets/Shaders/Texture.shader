#shader vertex
layout(location = 0) in vec3 Position;
layout(location = 2) in vec2 UV;

layout(location = 0) out vec2 vUV;

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
	vUV = UV;
}

#shader fragment
layout(location = 0) in vec2 vUV;

layout(location = 0) out vec4 FragColor;

layout(UpdateFreqStatic, binding = 0) uniform texture2D Texture;
layout(UpdateFreqStatic, binding = 1) uniform sampler Sampler;

void main()
{
	FragColor = texture(sampler2D(Texture, Sampler), vUV);
}