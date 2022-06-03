#shader compute

//Edge Detection Compute Shader
//Source: https://github.com/SaschaWillems/Vulkan/blob/master/data/shaders/glsl/computeshader/edgedetect.comp

layout (local_size_x = 16, local_size_y = 16) in;
layout (set = 1, binding = 0, rgba8) uniform readonly image2D inputImage;
layout (set = 1, binding = 1, rgba8) uniform image2D resultImage;

layout(push_constant) uniform PushConstantBlock
{
	uniform float Brightness;
} BrughtnessRootConstant;

float conv(in float[9] kernel, in float[9] data, in float denom, in float offset)
{
    float res = 0.0;
    for (int i = 0; i < 9; ++i)
        res += kernel[i] * data[i];
    return clamp(res / denom + offset, 0.0, 1.0);
}

struct ImageData
{
    float avg[9];
} imageData;

void main()
{
    //Fetch neighbouring texels
    int n = -1;
    for (int i = -1; i < 2; ++i)
    {
        for(int j = -1; j < 2; ++j)
        {
            ++n;
            vec3 rgb = imageLoad(inputImage, ivec2(gl_GlobalInvocationID.x + i, gl_GlobalInvocationID.y + j)).rgb;
            imageData.avg[n] = (rgb.r + rgb.g + rgb.b) / 3.0;
        }
    }

    float[9] kernel;
    kernel[0] = -1.0 / 8.0; kernel[1] = -1.0 / 8.0; kernel[2] = -1.0 / 8.0;
    kernel[3] = -1.0 / 8.0; kernel[4] =        1.0; kernel[5] = -1.0 / 8.0;
    kernel[6] = -1.0 / 8.0; kernel[7] = -1.0 / 8.0; kernel[8] = -1.0 / 8.0;

    vec4 res = vec4(vec3(conv(kernel, imageData.avg, 0.1, 0.0)), 1.0);

    res.rgb *= BrughtnessRootConstant.Brightness;

    imageStore(resultImage, ivec2(gl_GlobalInvocationID.xy), res);
}