#language glsl
#shader vertex
layout(location = 0) in vec3 Position;
layout(location = 3) in vec3 Normal;

layout(location = 0) out vec3 LightIntensity;

// layout(std140, UpdateFreqPerFrame, binding = 0) uniform MatrixBuffer
// {
// 	uniform mat4 sys_ProjectionMatrix;
// 	uniform mat4 sys_ViewMatrix;
// 	uniform mat4 sys_ModelMatrix;
// } Matrices;

layout(std140, UpdateFreqPerFrame, binding = 1) uniform DataBuffer
{
    uniform vec4 LightPosition;
    uniform vec3 LightLa;
    uniform vec3 LightLd;
    uniform vec3 LightLs;

    uniform vec3 MaterialKa;
    uniform vec3 MaterialKd;
    uniform vec3 MaterialKs;
    uniform float MaterialShininess;
} Data;

void GetCameraSpace(out vec3 normal, out vec3 position)
{
    normal = normalize(mat3(Matrices.sys_ViewMatrix * Matrices.sys_ModelMatrix) * Normal);
    position = (Matrices.sys_ViewMatrix * Matrices.sys_ModelMatrix * vec4(Position, 1.0f)).xyz;
}

vec3 PhongModel(vec3 position, vec3 normal)
{
    vec3 ambient = Data.LightLa * Data.MaterialKa;
    vec3 s = normalize(Data.LightPosition.xyz - position);
    float sDotN = max(dot(s, normal), 0.0f);
    vec3 diffuse = Data.LightLd * Data.MaterialKd * sDotN;
    vec3 specular = vec3(0.0f);
    if(sDotN > 0.0f)
    {
        vec3 v = normalize(-position);
        vec3 r = reflect(-s, normal);
        specular = Data.LightLs * Data.MaterialKs * pow(max(dot(r, v), 0.0f), Data.MaterialShininess);
    }

    return ambient + diffuse + specular;
}

void main()
{
    //Get the position and normal in camera space
    // vec3 cameraNormal, cameraPosition;
    // GetCameraSpace(cameraNormal, cameraPosition);

    //Evaluate the reflection model
    //LightIntensity = PhongModel(cameraPosition, cameraNormal);

	//gl_Position = Matrices.sys_ProjectionMatrix * Matrices.sys_ViewMatrix * Matrices.sys_ModelMatrix * vec4(Position, 1.0f);
	gl_Position = vec4(Position, 1.0f);
}

#shader fragment
layout(location = 0) in vec3 LightIntensity;

layout(location = 0) out vec4 FragColor;

void main()
{
	FragColor = vec4(LightIntensity, 1.0f);
}