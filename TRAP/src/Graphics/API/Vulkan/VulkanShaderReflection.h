#ifndef TRAP_VULKANSHADERREFLECTION_H
#define TRAP_VULKANSHADERREFLECTION_H

#include "Graphics/API/ShaderReflection.h"

namespace TRAP::Graphics::API
{
	ShaderReflection::ShaderReflection VkCreateShaderReflection(const std::vector<uint32_t>& shaderCode,
	                                                            RendererAPI::ShaderStage shaderStage);
}

#endif /*TRAP_VULKANSHADERREFLECTION_H*/