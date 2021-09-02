#shader vertex
layout(location = 0) in vec3 Position;
layout(location = 3) in vec3 Normal;

layout(location = 0) out vec3 LightIntensity;

layout(std140, UpdateFreqDynamic, binding = 0) uniform MatrixBuffer
{
    uniform mat4 sys_ProjectionMatrix;
    uniform mat4 sys_ViewMatrix;
} Matrices;

layout(UpdateFreqDynamic, binding = 1) uniform ModelBufferDynamic
{
	uniform mat4 sys_ModelMatrix;
} ModelMatrixDynamic;

layout(std140, UpdateFreqDynamic, binding = 2) uniform DataBuffer
{
    uniform vec4 LightPosition;
    uniform vec3 Ld;
    uniform vec3 Kd;
} Data;

void main()
{
    vec3 tnorm = normalize(mat3(Matrices.sys_ViewMatrix * ModelMatrixDynamic.sys_ModelMatrix) * Normal);
    vec4 cameraCoords = Matrices.sys_ViewMatrix * ModelMatrixDynamic.sys_ModelMatrix * vec4(Position, 1.0f);
    vec3 s = normalize(vec3(Data.LightPosition - cameraCoords));

    LightIntensity = Data.Ld * Data.Kd * max(dot(s, tnorm), 0.0f);

    gl_Position = Matrices.sys_ProjectionMatrix * Matrices.sys_ViewMatrix * ModelMatrixDynamic.sys_ModelMatrix * vec4(Position, 1.0f);
}

#shader fragment
layout(location = 0) in vec3 LightIntensity;

layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = vec4(LightIntensity, 1.0f);
}