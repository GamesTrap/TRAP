#ifndef _TRAP_VULKANSHADER_H_
#define _TRAP_VULKANSHADER_H_

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/ShaderReflection.h"
#include "Graphics/Shaders/Shader.h"

namespace TRAP::Graphics::API
{
	class VulkanDevice;
	
	class VulkanShader final : public Shader
	{
	public:
		VulkanShader(const std::string& name, const RendererAPI::BinaryShaderDesc& desc);
		~VulkanShader();
		
		const std::array<uint32_t, 3>& GetNumThreadsPerGroup() const;

		const std::vector<VkShaderModule>& GetVkShaderModules() const;
		TRAP::Ref<ShaderReflection::PipelineReflection> GetReflection() const;
		const std::vector<std::string>& GetEntryNames() const;

		void Use(Window* window = nullptr) override;
		
	private:
		TRAP::Ref<VulkanDevice> m_device;
		
		std::array<uint32_t, 3> m_numThreadsPerGroup;

		std::vector<VkShaderModule> m_shaderModules;
		TRAP::Ref<ShaderReflection::PipelineReflection> m_reflection;
		std::vector<std::string> m_entryNames;
	};
}

#endif /*_TRAP_VULKANSHADER_H_*/