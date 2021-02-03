#shader vertex
layout(location = 0) in vec3 pos;

void main()
{
    gl_Position = vec4(pos, 1.0f);
}

#shader fragment
layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0f);
}