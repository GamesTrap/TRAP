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

layout(UpdateFreqStatic, binding = 0) uniform texture2D Texture;
layout(UpdateFreqStatic, binding = 1) uniform sampler Sampler;

layout(push_constant) uniform PushConstantBlock
{
	uniform uint SamplerIndex;
} SamplerRootConstant;

void main()
{
    FragColor = textureLod(sampler2D(Texture, Sampler), vUV, SamplerRootConstant.SamplerIndex);
    //FragColor = vec4(vUV, 0.0f, 1.0f);
}