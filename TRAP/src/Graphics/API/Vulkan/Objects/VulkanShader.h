#ifndef _TRAP_VULKANSHADER_H_
#define _TRAP_VULKANSHADER_H_

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/ShaderReflection.h"

namespace TRAP::Graphics::API
{
	class VulkanDevice;
	
	class VulkanShader
	{
	public:
		VulkanShader(TRAP::Ref<VulkanDevice> device, const RendererAPI::BinaryShaderDesc& desc);
		~VulkanShader();
		
	private:
		TRAP::Ref<VulkanDevice> m_device;
		
		RendererAPI::ShaderStage m_stages;
		std::array<uint32_t, 3> m_numThreadsPerGroup;

		std::vector<VkShaderModule> m_shaderModules;
		TRAP::Scope<ShaderReflection::PipelineReflection> m_reflection;
		std::vector<std::string> m_entryNames;
	};
}

#endif /*_TRAP_VULKANSHADER_H_*/