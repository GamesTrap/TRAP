
#shader vertex
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 col;

layout(location = 2) out vec3 vCol;

layout(std140, UpdateFreqPerFrame, binding = 0) uniform CameraUBO
{
	uniform mat4 Projection;
	uniform mat4 View;
	uniform mat4 Model;
};

void main()
{
    gl_Position = Projection * View * Model * vec4(pos, 1.0f);
	vCol = col;
}

#shader fragment
layout(location = 0) out vec4 FragColor;

layout(location = 2) in vec3 vCol;

void main()
{
    FragColor = vec4(vCol, 1.0f);
}