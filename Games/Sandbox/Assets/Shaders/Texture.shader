#language glsl
#shader vertex
layout(location = 0) in vec3 Position;
layout(location = 1) in vec4 Color;
layout(location = 2) in vec2 UV;

layout(location = 3) out vec4 vColor;
layout(location = 4) out vec2 vUV;

layout(std140, UpdateFreqPerFrame, binding = 0) uniform MatrixBuffer
{
	uniform mat4 sys_ProjectionMatrix;
	uniform mat4 sys_ViewMatrix;
} Matrices;

layout(UpdateFreqPerDraw, binding = 0) uniform ModelBufferDynamic
{
	uniform mat4 sys_ModelMatrix;
} ModelMatrixDynamic;

void main()
{
	gl_Position = Matrices.sys_ProjectionMatrix * Matrices.sys_ViewMatrix * ModelMatrixDynamic.sys_ModelMatrix * vec4(Position, 1.0f);
	vColor = Color;
	vUV = UV;
}

#shader fragment
layout(location = 0) out vec4 FragColor;

layout(location = 3) in vec4 vColor;
layout(location = 4) in vec2 vUV;

layout(UpdateFreqNone, binding = 0) uniform texture2D Texture;
layout(UpdateFreqNone, binding = 1) uniform sampler Sampler;

void main()
{
	FragColor = texture(sampler2D(Texture, Sampler), vUV);
}