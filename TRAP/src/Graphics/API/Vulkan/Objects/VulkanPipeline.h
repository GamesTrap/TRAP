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
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="desc">Pipeline description</param>
		explicit VulkanPipeline(const RendererAPI::PipelineDesc& desc);
		/// <summary>
		/// Destructor.
		/// </summary>
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

		/// <summary>
		/// Retrieve the Vulkan pipeline handle.
		/// </summary>
		/// <returns>Vulkan pipeline handle</returns>
		VkPipeline GetVkPipeline() const;
		/// <summary>
		/// Retrieve the pipeline type.
		/// </summary>
		/// <returns>Pipeline type</returns>
		RendererAPI::PipelineType GetPipelineType() const;

	private:
		/// <summary>
		/// Initialize compute pipeline.
		/// </summary>
		/// <param name="desc">Pipeline description</param>
		void InitComputePipeline(const RendererAPI::PipelineDesc& desc);
		/// <summary>
		/// Initialize graphics pipeline.
		/// </summary>
		/// <param name="desc">Pipeline description</param>
		void InitGraphicsPipeline(const RendererAPI::PipelineDesc& desc);

		VkPipeline m_vkPipeline;
		RendererAPI::PipelineType m_type;
		//In DX12 this information is stored in ID3D12StateObject.
		//But for Vulkan we need to store it manually.
		std::vector<const char*> m_shaderStageNames;

		TRAP::Ref<VulkanDevice> m_device;
	};
}

#endif /*TRAP_VULKANPIPELINE_H*/