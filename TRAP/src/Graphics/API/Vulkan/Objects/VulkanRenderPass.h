#ifndef TRAP_VULKANRENDERPASS_H
#define TRAP_VULKANRENDERPASS_H

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

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanRenderPass(const VulkanRenderPass&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanRenderPass& operator=(const VulkanRenderPass&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanRenderPass(VulkanRenderPass&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanRenderPass& operator=(VulkanRenderPass&&) = default;

		VkRenderPass GetVkRenderPass() const;

		const std::vector<TRAP::Graphics::API::ImageFormat>& GetColorFormats() const;
		const std::vector<RendererAPI::LoadActionType>& GetLoadActionsColor() const;
		const std::vector<bool>& GetSRGBValues() const;
		uint32_t GetRenderTargetCount() const;
		RendererAPI::SampleCount GetSampleCount() const;
		TRAP::Graphics::API::ImageFormat GetDepthStencilFormat() const;
		RendererAPI::LoadActionType GetLoadActionTypeDepth() const;
		RendererAPI::LoadActionType GetLoadActionTypeStencil() const;

	private:
		VkRenderPass m_renderPass;

		std::vector<TRAP::Graphics::API::ImageFormat> m_colorFormats;
		std::vector<RendererAPI::LoadActionType> m_loadActionsColor;
		std::vector<bool> m_SRGBValues;
		uint32_t m_renderTargetCount;
		RendererAPI::SampleCount m_sampleCount;
		TRAP::Graphics::API::ImageFormat m_depthStencilFormat;
		RendererAPI::LoadActionType m_loadActionDepth;
		RendererAPI::LoadActionType m_loadActionStencil;

		TRAP::Ref<VulkanDevice> m_device;
	};
}

#endif /*TRAP_VULKANRENDERPASS_H*/