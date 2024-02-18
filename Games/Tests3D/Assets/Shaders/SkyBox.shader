#shader vertex
layout(location = 0) in vec3 Position;

layout(location = 1) out vec3 TexCoords;

layout(std140, UpdateFreqDynamic, binding = 0) readonly buffer MatrixBuffer
{
	buffer mat4 sys_ProjectionMatrix;
	buffer mat4 sys_ViewMatrix;
} Matrices;

void main()
{
	vec4 mvpPos = Matrices.sys_ProjectionMatrix * mat4(mat3(Matrices.sys_ViewMatrix)) * vec4(Position, 1.0f);
	gl_Position = vec4(mvpPos.xy, 0.0f, mvpPos.w); //Make z = 0.0f so depth testing will always fail (note: 0.0f instead of 1.0f due to reverse Z in projection matrix)
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