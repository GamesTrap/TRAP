#ifndef TRAP_VULKANSHADER_H
#define TRAP_VULKANSHADER_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/ShaderReflection.h"
#include "Graphics/Shaders/Shader.h"

namespace TRAP::Graphics::API
{
	class VulkanDevice;

	class VulkanShader final : public Shader
	{
	public:
		VulkanShader(std::string name, const RendererAPI::BinaryShaderDesc& desc);
		~VulkanShader() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanShader(const VulkanShader&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanShader& operator=(const VulkanShader&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanShader(VulkanShader&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanShader& operator=(VulkanShader&&) = default;

		const std::array<uint32_t, 3>& GetNumThreadsPerGroup() const;

		const std::vector<VkShaderModule>& GetVkShaderModules() const;
		TRAP::Ref<ShaderReflection::PipelineReflection> GetReflection() const;
		const std::vector<std::string>& GetEntryNames() const;

		void Use(Window* window) override;
		void UseTexture(uint32_t set, uint32_t binding, TRAP::Graphics::Texture* texture,
		                Window* window) override;
		void UseTextures(uint32_t set, uint32_t binding,
						 const std::vector<TRAP::Graphics::Texture*>& textures,
						 Window* window) override;
		void UseSampler(uint32_t set, uint32_t binding, TRAP::Graphics::Sampler* sampler,
		                Window* window) override;
		void UseSamplers(uint32_t set, uint32_t binding,
		                 const std::vector<TRAP::Graphics::Sampler*>& samplers,
						 Window*) override;
		void UseUBO(uint32_t set, uint32_t binding, TRAP::Graphics::UniformBuffer* uniformBuffer,
		            uint64_t size, uint64_t offset, Window* window) override;
		void UseSSBO(uint32_t set, uint32_t binding, TRAP::Graphics::StorageBuffer* storageBuffer,
		             uint64_t size, Window* window) override;

	private:
		void UseBuffer(uint32_t set, uint32_t binding, TRAP::Graphics::Buffer* buffer,
		               uint64_t size, uint64_t offset, Window* window);

		std::string RetrieveDescriptorName(uint32_t set, uint32_t binding, RendererAPI::DescriptorType type, uint64_t size = 1) const;

		TRAP::Ref<VulkanDevice> m_device;

		std::array<uint32_t, 3> m_numThreadsPerGroup;

		std::vector<VkShaderModule> m_shaderModules;
		TRAP::Ref<ShaderReflection::PipelineReflection> m_reflection;
		std::vector<std::string> m_entryNames;
	};
}

#endif /*TRAP_VULKANSHADER_H*/