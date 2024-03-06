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
		/// @brief Constructor.
		/// @param device Vulkan device.
		/// @param desc Render pass description.
		VulkanRenderPass(TRAP::Ref<VulkanDevice> device, const VulkanRenderer::RenderPassDesc& desc);
		/// @brief Destructor.
		~VulkanRenderPass();

		/// @brief Copy constructor.
		consteval VulkanRenderPass(const VulkanRenderPass&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval VulkanRenderPass& operator=(const VulkanRenderPass&) noexcept = delete;
		/// @brief Move constructor.
		VulkanRenderPass(VulkanRenderPass&&) noexcept = default;
		/// @brief Move assignment operator.
		VulkanRenderPass& operator=(VulkanRenderPass&&) noexcept = default;

		/// @brief Retrieve the Vulkan render pass handle.
		/// @return Vulkan render pass handle.
		[[nodiscard]] constexpr VkRenderPass GetVkRenderPass() const noexcept;

		/// @brief Retrieve the used color formats.
		/// @return Used color formats.
		[[nodiscard]] constexpr const std::vector<TRAP::Graphics::API::ImageFormat>& GetColorFormats() const noexcept;
		/// @brief Retrieve the used color load action types.
		/// @return Used color load action types.
		[[nodiscard]] constexpr const std::vector<RendererAPI::LoadActionType>& GetLoadActionsColor() const noexcept;
		/// @brief Retrieve the number of used render targets.
		/// @return Number of used render targets.
		[[nodiscard]] constexpr u32 GetRenderTargetCount() const noexcept;
		/// @brief Retrieve the used sample count.
		/// @return Used sample count.
		[[nodiscard]] constexpr RendererAPI::SampleCount GetSampleCount() const noexcept;
		/// @brief Retrieve the used depth stencil format.
		/// @return Used depth stencil format.
		[[nodiscard]] constexpr TRAP::Graphics::API::ImageFormat GetDepthStencilFormat() const noexcept;
		/// @brief Retrieve the used depth load action type.
		/// @return Used depth load action type.
		[[nodiscard]] constexpr RendererAPI::LoadActionType GetLoadActionTypeDepth() const noexcept;
		/// @brief Retrieve the used stencil load action type.
		/// @return Used stencil load action type.
		[[nodiscard]] constexpr RendererAPI::LoadActionType GetLoadActionTypeStencil() const noexcept;

	private:
		VkRenderPass m_renderPass = VK_NULL_HANDLE;

		std::vector<TRAP::Graphics::API::ImageFormat> m_colorFormats;
		std::vector<RendererAPI::LoadActionType> m_loadActionsColor;
		u32 m_renderTargetCount;
		RendererAPI::SampleCount m_sampleCount;
		TRAP::Graphics::API::ImageFormat m_depthStencilFormat;
		RendererAPI::LoadActionType m_loadActionDepth;
		RendererAPI::LoadActionType m_loadActionStencil;

		TRAP::Ref<VulkanDevice> m_device = nullptr;
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

[[nodiscard]] constexpr u32 TRAP::Graphics::API::VulkanRenderPass::GetRenderTargetCount() const noexcept
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
