#shader vertex
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 col;

layout(location = 0) out vec3 vcol;

void main()
{
    gl_Position = vec4(pos, 1.0f);
    vcol = col;
}

#shader fragment
layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec3 vcol;

void main()
{
    FragColor = vec4(vcol, 1.0f);
}