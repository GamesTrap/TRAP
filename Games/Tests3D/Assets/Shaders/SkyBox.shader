#shader vertex
layout(location = 0) in vec3 Position;

layout(location = 1) out vec3 TexCoords;

layout(std140, UpdateFreqDynamic, binding = 0) uniform MatrixBuffer
{
	uniform mat4 sys_ProjectionMatrix;
	uniform mat4 sys_ViewMatrix;
} Matrices;

void main()
{
	gl_Position = Matrices.sys_ProjectionMatrix * mat4(mat3(Matrices.sys_ViewMatrix)) * vec4(Position, 1.0f);
	TexCoords = Position;
}

#shader fragment
layout(location = 1) in vec3 TexCoords;

layout(location = 0) out vec4 FragColor;

layout(UpdateFreqStatic, binding = 0) uniform textureCube CubeMap;
layout(UpdateFreqStatic, binding = 1) uniform sampler CubeSampler;

void main()
{
	FragColor = textureLod(samplerCube(CubeMap, CubeSampler), TexCoords, 0.0f);
}