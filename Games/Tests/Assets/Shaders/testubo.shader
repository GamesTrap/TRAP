#shader vertex
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 col;

layout(location = 0) out vec3 vCol;

layout(std140, UpdateFreqPerFrame, binding = 0) uniform SizeMultiplicator
{
	uniform vec3 Multiplier;
};

void main()
{
    gl_Position = vec4(pos * Multiplier, 1.0f);
    vCol = col;
}

#shader fragment
layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec3 vCol;

layout(std140, UpdateFreqPerFrame, binding = 1) uniform Color
{
	uniform vec3 color;
};

void main()
{
    FragColor = vec4(color, 1.0f);
}