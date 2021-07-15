#ifndef _TRAP_VULKANSHADERREFLECTION_H_
#define _TRAP_VULKANSHADERREFLECTION_H_

#include "Graphics/API/ShaderReflection.h"

namespace TRAP::Graphics::API
{
	ShaderReflection::ShaderReflection VkCreateShaderReflection(const std::vector<uint32_t>& shaderCode,
	                                                            RendererAPI::ShaderStage shaderStage);
}

#endif /*_TRAP_VULKANSHADERREFLECTION_H_*/