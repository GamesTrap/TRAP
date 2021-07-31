#language glsl
#shader vertex
layout(location = 0) in vec3 Position;
layout(location = 2) in vec2 UV;

layout(location = 0) out vec2 vUV;

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
	vUV = UV;
}

#shader fragment
layout(location = 0) in vec2 vUV;

layout(location = 0) out vec4 FragColor;

layout(UpdateFreqNone, binding = 0) uniform texture2D Texture;
layout(UpdateFreqNone, binding = 1) uniform sampler Sampler;

void main()
{
	FragColor = texture(sampler2D(Texture, Sampler), vUV);
}