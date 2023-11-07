#ifndef TRAP_VULKANSHADERREFLECTION_H
#define TRAP_VULKANSHADERREFLECTION_H

#include "Graphics/API/ShaderReflection.h"

namespace TRAP::Graphics::API
{
	/// @brief Create a shader reflection from Vulkan SPIRV source code.
	/// @param shaderCode SPIRV code.
	/// @param shaderStage Shader stage of SPIRV code.
	/// @return Shader reflection.
	[[nodiscard]] ShaderReflection::ShaderReflection VkCreateShaderReflection(const std::vector<u32>& shaderCode,
	                                                                          RendererAPI::ShaderStage shaderStage);
}

#endif /*TRAP_VULKANSHADERREFLECTION_H*/