#ifndef _TRAP_VULKANPIPELINE_H_
#define _TRAP_VULKANPIPELINE_H_

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/Pipeline.h"

namespace TRAP::Graphics::API
{
	class VulkanDevice;

	class VulkanPipeline final : public Pipeline
	{
	public:
		explicit VulkanPipeline(const RendererAPI::PipelineDesc& desc);
		~VulkanPipeline();

		VkPipeline& GetVkPipeline();
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

#endif /*_TRAP_VULKANPIPELINE_H_*/