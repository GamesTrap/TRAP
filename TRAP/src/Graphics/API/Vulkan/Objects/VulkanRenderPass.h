#ifndef _TRAP_VULKANRENDERPASS_H_
#define _TRAP_VULKANRENDERPASS_H_

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

namespace TRAP::Graphics::API
{
	class VulkanDevice;
	
	class VulkanRenderPass
	{
	public:
		VulkanRenderPass(TRAP::Ref<VulkanDevice> device, const VulkanRenderer::RenderPassDesc& desc);
		~VulkanRenderPass();

		VkRenderPass& GetVkRenderPass();

		const std::vector<RendererAPI::ImageFormat>& GetColorFormats() const;
		const std::vector<RendererAPI::LoadActionType>& GetLoadActionsColor() const;
		const std::vector<bool>& GetSRGBValues() const;
		uint32_t GetRenderTargetCount() const;
		RendererAPI::SampleCount GetSampleCount() const;
		RendererAPI::ImageFormat GetDepthStencilFormat() const;
		RendererAPI::LoadActionType GetLoadActionTypeDepth() const;
		RendererAPI::LoadActionType GetLoadActionTypeStencil() const;

	private:
		VkRenderPass m_renderPass;

		std::vector<RendererAPI::ImageFormat> m_colorFormats;
		std::vector<RendererAPI::LoadActionType> m_loadActionsColor;
		std::vector<bool> m_SRGBValues;
		uint32_t m_renderTargetCount;
		RendererAPI::SampleCount m_sampleCount;
		RendererAPI::ImageFormat m_depthStencilFormat;
		RendererAPI::LoadActionType m_loadActionDepth;
		RendererAPI::LoadActionType m_loadActionStencil;

		TRAP::Ref<VulkanDevice> m_device;
	};
}

#endif /*_TRAP_VULKANRENDERPASS_H_*/