#shader vertex
    void main()
    {
        gl_Position = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }

#shader fragment
	layout(location = 0) out vec4 FragColor;

    void main()
    {
        FragColor = vec4(0.75f, 0.5f, 1.0f, 1.0f);
    }