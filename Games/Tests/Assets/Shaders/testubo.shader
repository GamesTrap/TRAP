#shader vertex
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 col;

layout(location = 0) out vec3 vCol;

//layout(std140, UpdateFreqPerFrame, binding = 1) uniform Matrices
//{
//    uniform mat4 sys_ProjectionMatrix;
//    uniform mat4 sys_ViewMatrix;
//    uniform mat4 sys_ModelMatrix;
//};

//layout(std140, UpdateFreqPerFrame, binding = 0) uniform ColorV
//{
    //uniform vec3 Multiplier;
//};

void main()
{
    //gl_Position = vec4(pos * Multiplier, 1.0f);
    //gl_Position = sys_ProjectionMatrix * sys_ViewMatrix * sys_ModelMatrix * vec4(pos, 1.0f);
    gl_Position = vec4(pos, 1.0f);
    vCol = col;
}

#shader fragment
layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec3 vCol;

//layout(std140, UpdateFreqPerFrame, binding = 1) uniform ColorF
//{
//    uniform vec4 color;
//};

void main()
{
    //FragColor = color;
    FragColor = vec4(1.0f);
}