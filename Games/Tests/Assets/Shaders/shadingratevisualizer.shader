#shader vertex
layout(location = 0) out vec2 vUV;

void main()
{
	vUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	gl_Position = vec4(vUV * 2.0f + -1.0f, 0.0f, 1.0f);
}

#shader fragment

#extension GL_EXT_fragment_shading_rate : require

layout(location = 0) in vec2 vUV;

layout(location = 0) out vec4 FragColor;

void main()
{
	vec4 overlay = vec4(0.0f);

	//Fragment shading rate visualizer
	int v = 1;
	int h = 1;

	if((gl_ShadingRateEXT & gl_ShadingRateFlag2VerticalPixelsEXT) == gl_ShadingRateFlag2VerticalPixelsEXT)
		v = 2;
	if((gl_ShadingRateEXT & gl_ShadingRateFlag4VerticalPixelsEXT) == gl_ShadingRateFlag4VerticalPixelsEXT)
		v = 4;
	if((gl_ShadingRateEXT & gl_ShadingRateFlag2HorizontalPixelsEXT) == gl_ShadingRateFlag2HorizontalPixelsEXT)
		h = 2;
	if((gl_ShadingRateEXT & gl_ShadingRateFlag4HorizontalPixelsEXT) == gl_ShadingRateFlag4HorizontalPixelsEXT)
		h = 4;

	if((v == 1 && h == 2) || (v == 2 && h == 1))
		overlay = vec4(0.0f, 0.0f, 1.0f, 0.3f);
	else if(v == 2 && h == 2)
		overlay = vec4(0.0f, 1.0f, 0.0f, 0.2f);
	else if((v == 2 && h == 4) || (v == 4 && h == 2))
		overlay = vec4(0.8f, 0.8f, 0.0f, 0.3f);
	else if(v == 4 && h == 4)
		overlay = vec4(0.8f, 0.0f, 0.0f, 0.3f);

	FragColor = overlay;
}