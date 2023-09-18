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
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="device">Vulkan device.</param>
		/// <param name="desc">Render pass description.</param>
		VulkanRenderPass(TRAP::Ref<VulkanDevice> device, const VulkanRenderer::RenderPassDesc& desc);
		/// <summary>
		/// Destructor.
		/// </summary>
		~VulkanRenderPass();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanRenderPass(const VulkanRenderPass&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanRenderPass& operator=(const VulkanRenderPass&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanRenderPass(VulkanRenderPass&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanRenderPass& operator=(VulkanRenderPass&&) noexcept = default;

		/// <summary>
		/// Retrieve the Vulkan render pass handle.
		/// </summary>
		/// <returns>Vulkan render pass handle.</returns>
		[[nodiscard]] constexpr VkRenderPass GetVkRenderPass() const noexcept;

		/// <summary>
		/// Retrieve the used color formats.
		/// </summary>
		/// <returns>Used color formats.</returns>
		[[nodiscard]] constexpr const std::vector<TRAP::Graphics::API::ImageFormat>& GetColorFormats() const noexcept;
		/// <summary>
		/// Retrieve the used color load action types.
		/// </summary>
		/// <returns>Used color load action types.</returns>
		[[nodiscard]] constexpr const std::vector<RendererAPI::LoadActionType>& GetLoadActionsColor() const noexcept;
		/// <summary>
		/// Retrieve the number of used render targets.
		/// </summary>
		/// <returns>Number of used render targets.</returns>
		[[nodiscard]] constexpr uint32_t GetRenderTargetCount() const noexcept;
		/// <summary>
		/// Retrieve the used sample count.
		/// </summary>
		/// <returns>Used sample count.</returns>
		[[nodiscard]] constexpr RendererAPI::SampleCount GetSampleCount() const noexcept;
		/// <summary>
		/// Retrieve the used depth stencil format.
		/// </summary>
		/// <returns>Used depth stencil format.</returns>
		[[nodiscard]] constexpr TRAP::Graphics::API::ImageFormat GetDepthStencilFormat() const noexcept;
		/// <summary>
		/// Retrieve the used depth load action type.
		/// </summary>
		/// <returns>Used depth load action type.</returns>
		[[nodiscard]] constexpr RendererAPI::LoadActionType GetLoadActionTypeDepth() const noexcept;
		/// <summary>
		/// Retrieve the used stencil load action type.
		/// </summary>
		/// <returns>Used stencil load action type.</returns>
		[[nodiscard]] constexpr RendererAPI::LoadActionType GetLoadActionTypeStencil() const noexcept;

	private:
		[[nodiscard]] static VkRenderPass CreateRenderPass(const TRAP::Ref<VulkanDevice>& device, const VulkanRenderer::RenderPassDesc& desc);
		[[nodiscard]] static VkRenderPass CreateRenderPass2(const TRAP::Ref<VulkanDevice>& device, const VulkanRenderer::RenderPassDesc& desc);

		VkRenderPass m_renderPass = VK_NULL_HANDLE;

		std::vector<TRAP::Graphics::API::ImageFormat> m_colorFormats;
		std::vector<RendererAPI::LoadActionType> m_loadActionsColor;
		uint32_t m_renderTargetCount;
		RendererAPI::SampleCount m_sampleCount;
		TRAP::Graphics::API::ImageFormat m_depthStencilFormat;
		RendererAPI::LoadActionType m_loadActionDepth;
		RendererAPI::LoadActionType m_loadActionStencil;

		TRAP::Ref<VulkanDevice> m_device;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<TRAP::Graphics::API::ImageFormat>& TRAP::Graphics::API::VulkanRenderPass::GetColorFormats() const noexcept
{
	return m_colorFormats;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<TRAP::Graphics::RendererAPI::LoadActionType>& TRAP::Graphics::API::VulkanRenderPass::GetLoadActionsColor() const noexcept
{
	return m_loadActionsColor;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkRenderPass TRAP::Graphics::API::VulkanRenderPass::GetVkRenderPass() const noexcept
{
	return m_renderPass;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t TRAP::Graphics::API::VulkanRenderPass::GetRenderTargetCount() const noexcept
{
	return m_renderTargetCount;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::SampleCount TRAP::Graphics::API::VulkanRenderPass::GetSampleCount() const noexcept
{
	return m_sampleCount;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::API::ImageFormat TRAP::Graphics::API::VulkanRenderPass::GetDepthStencilFormat() const noexcept
{
	return m_depthStencilFormat;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::LoadActionType TRAP::Graphics::API::VulkanRenderPass::GetLoadActionTypeDepth() const noexcept
{
	return m_loadActionDepth;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::LoadActionType TRAP::Graphics::API::VulkanRenderPass::GetLoadActionTypeStencil() const noexcept
{
	return m_loadActionStencil;
}

#endif /*TRAP_VULKANRENDERPASS_H*/