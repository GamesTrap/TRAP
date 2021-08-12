#language glsl
#shader vertex
layout(location = 0) in vec3 Position;
layout(location = 3) in vec3 Normal;

layout(location = 0) out vec3 LightIntensity;

layout(std140, UpdateFreqPerFrame, binding = 0) uniform MatrixBuffer
{
    uniform mat4 sys_ProjectionMatrix;
    uniform mat4 sys_ViewMatrix;
    //uniform mat4 sys_ModelMatrix;
} Matrices;

layout(push_constant) uniform PushConstantBlock
{
	uniform mat4 sys_ModelMatrix;
} ModelRootConstant;

layout(std140, UpdateFreqPerFrame, binding = 1) uniform DataBuffer
{
    uniform vec4 LightPosition;
    uniform vec3 Ld;
    uniform vec3 Kd;
} Data;

void main()
{
    vec3 tnorm = normalize(mat3(Matrices.sys_ViewMatrix * ModelRootConstant.sys_ModelMatrix) * Normal);
    vec4 cameraCoords = Matrices.sys_ViewMatrix * ModelRootConstant.sys_ModelMatrix * vec4(Position, 1.0f);
    vec3 s = normalize(vec3(Data.LightPosition - cameraCoords));

    LightIntensity = Data.Ld * Data.Kd * max(dot(s, tnorm), 0.0f);

    gl_Position = Matrices.sys_ProjectionMatrix * Matrices.sys_ViewMatrix * ModelRootConstant.sys_ModelMatrix * vec4(Position, 1.0f);
}

#shader fragment
layout(location = 0) in vec3 LightIntensity;

layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = vec4(LightIntensity, 1.0f);
}