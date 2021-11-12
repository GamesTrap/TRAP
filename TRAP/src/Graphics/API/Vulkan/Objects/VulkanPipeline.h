#ifndef TRAP_VULKANPIPELINE_H
#define TRAP_VULKANPIPELINE_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/Pipeline.h"

namespace TRAP::Graphics::API
{
	class VulkanDevice;

	class VulkanPipeline final : public Pipeline
	{
	public:
		explicit VulkanPipeline(const RendererAPI::PipelineDesc& desc);
		~VulkanPipeline() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanPipeline(const VulkanPipeline&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanPipeline& operator=(const VulkanPipeline&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanPipeline(VulkanPipeline&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanPipeline& operator=(VulkanPipeline&&) = default;

		VkPipeline GetVkPipeline() const;
		RendererAPI::PipelineType GetPipelineType() const;

	private:
		void AddComputePipeline(const RendererAPI::PipelineDesc& desc);
		void AddGraphicsPipeline(const RendererAPI::PipelineDesc& desc);

		VkPipeline m_vkPipeline;
		RendererAPI::PipelineType m_type;
		//In DX12 this information is stored in ID3D12StateObject.
		//But for Vulkan we need to store it manually.
		std::vector<const char*> m_shaderStageNames;

		TRAP::Ref<VulkanDevice> m_device;
	};
}

#endif /*TRAP_VULKANPIPELINE_H*/