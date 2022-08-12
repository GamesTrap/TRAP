#shader vertex
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;

layout(location = 0) out vec2 vUV;

void main()
{
    gl_Position = vec4(pos, 1.0f);
    vUV = uv;
}

#shader fragment
layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 vUV;

void main()
{
    FragColor = vec4(RED * HALF, 1.0f);
}