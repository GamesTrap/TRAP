#shader vertex
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 col;

layout(location = 0) out vec3 vCol;

layout(push_constant) uniform PushConstantBlock
{
	uniform vec3 Color;
} ColorRootConstant;

void main()
{
    gl_Position = vec4(pos, 1.0f);
    vCol = col;
}

#shader fragment
layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec3 vCol;

layout(push_constant) uniform PushConstantBlock
{
	uniform vec3 Color;
} ColorRootConstant;

void main()
{
    FragColor = vec4(ColorRootConstant.Color, 1.0f);
}