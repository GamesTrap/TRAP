#ifndef TRAP_VULKANSHADERREFLECTION_H
#define TRAP_VULKANSHADERREFLECTION_H

#include "Graphics/API/ShaderReflection.h"

namespace TRAP::Graphics::API
{
	/// <summary>
	/// Create a shader reflection from Vulkan SPIRV source code.
	/// </summary>
	/// <param name="shaderCode">SPIRV code.</param>
	/// <param name="shaderStage">Shader stage of SPIRV code.</param>
	/// <returns>Shader reflection.</returns>
	[[nodiscard]] ShaderReflection::ShaderReflection VkCreateShaderReflection(const std::vector<u32>& shaderCode,
	                                                                          RendererAPI::ShaderStage shaderStage);
}

#endif /*TRAP_VULKANSHADERREFLECTION_H*/