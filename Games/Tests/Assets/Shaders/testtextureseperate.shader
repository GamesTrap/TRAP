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

layout(UpdateFreqNone, binding = 0) uniform texture2D Texture;
//layout(UpdateFreqNone, binding = 1) uniform sampler Sampler;
layout(UpdateFreqNone, binding = 1) uniform sampler Samplers[MAX_TEXTURE_MIP_LEVELS];

layout(push_constant) uniform PushConstantBlock
{
	uniform uint SamplerIndex;
} SamplerRootConstant;

void main()
{
    FragColor = texture(sampler2D(Texture, Samplers[SamplerRootConstant.SamplerIndex]), vUV);
    //FragColor = vec4(vUV, 0.0f, 1.0f);
}