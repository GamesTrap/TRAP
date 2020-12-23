#ifndef _TRAP_VULKANPIPELINE_H_
#define _TRAP_VULKANPIPELINE_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics::API
{
	class VulkanDevice;

	class VulkanPipeline
	{
	public:
		VulkanPipeline(TRAP::Ref<VulkanDevice> device, const RendererAPI::PipelineDesc& desc);
		~VulkanPipeline();

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