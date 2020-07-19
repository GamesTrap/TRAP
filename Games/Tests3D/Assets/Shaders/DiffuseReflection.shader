#language glsl
#shader vertex
layout(location = 0) in vec3 Position;
layout(location = 3) in vec3 Normal;

layout(location = 0) out vec3 LightIntensity;

layout(std140, binding = 0) uniform MatrixBuffer
{
    mat4 sys_ProjectionMatrix;
    mat4 sys_ViewMatrix;
    mat4 sys_ModelMatrix;
} Matrices;

layout(std140, binding = 1) uniform DataBuffer
{
    vec4 LightPosition;
    vec3 Ld;
    vec3 Kd;
} Data;

void main()
{
    vec3 tnorm = normalize(mat3(Matrices.sys_ViewMatrix * Matrices.sys_ModelMatrix) * Normal);
    vec4 cameraCoords = Matrices.sys_ViewMatrix * Matrices.sys_ModelMatrix * vec4(Position, 1.0f);
    vec3 s = normalize(vec3(Data.LightPosition - cameraCoords));

    LightIntensity = Data.Ld * Data.Kd * max(dot(s, tnorm), 0.0f);

    gl_Position = Matrices.sys_ProjectionMatrix * Matrices.sys_ViewMatrix * Matrices.sys_ModelMatrix * vec4(Position, 1.0f);
}

#shader fragment
layout(location = 0) in vec3 LightIntensity;

layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = vec4(LightIntensity, 1.0f);
}