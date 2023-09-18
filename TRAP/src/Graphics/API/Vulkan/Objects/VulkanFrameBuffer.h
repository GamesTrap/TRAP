#ifndef TRAP_VULKANFRAMEBUFFER_H
#define TRAP_VULKANFRAMEBUFFER_H

#include "Graphics/API/Vulkan/VulkanRenderer.h"

namespace TRAP::Graphics::API
{
	class VulkanRenderTarget;

	class VulkanFrameBuffer
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="device">Vulkan device.</param>
		/// <param name="desc">Framebuffer description.</param>
		VulkanFrameBuffer(TRAP::Ref<VulkanDevice> device, const VulkanRenderer::FrameBufferDesc& desc);
		/// <summary>
		/// Destructor.
		/// </summary>
		~VulkanFrameBuffer();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanFrameBuffer(const VulkanFrameBuffer&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanFrameBuffer& operator=(const VulkanFrameBuffer&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanFrameBuffer(VulkanFrameBuffer&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanFrameBuffer& operator=(VulkanFrameBuffer&&) noexcept = default;

		/// <summary>
		/// Retrieve the VkFramebuffer handle.
		/// </summary>
		/// <returns>VkFramebuffer handle.</returns>
		[[nodiscard]] constexpr VkFramebuffer GetVkFrameBuffer() const noexcept;

		/// <summary>
		/// Retrieve the width of the framebuffer.
		/// </summary>
		/// <returns>Framebuffer width.</returns>
		[[nodiscard]] constexpr uint32_t GetWidth() const noexcept;
		/// <summary>
		/// Retrieve the height of the framebuffer.
		/// </summary>
		/// <returns>Framebuffer height.</returns>
		[[nodiscard]] constexpr uint32_t GetHeight() const noexcept;
		/// <summary>
		/// Retrieve the array size of the framebuffer.
		/// </summary>
		/// <returns>Framebuffer array size.</returns>
		[[nodiscard]] constexpr uint32_t GetArraySize() const noexcept;

	private:
		VkFramebuffer m_framebuffer = VK_NULL_HANDLE;

		uint32_t      m_width = 0;
		uint32_t      m_height = 0;
		uint32_t      m_arraySize = 0;

		TRAP::Ref<VulkanDevice> m_device;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkFramebuffer TRAP::Graphics::API::VulkanFrameBuffer::GetVkFrameBuffer() const noexcept
{
	return m_framebuffer;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t TRAP::Graphics::API::VulkanFrameBuffer::GetWidth() const noexcept
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t TRAP::Graphics::API::VulkanFrameBuffer::GetHeight() const noexcept
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t TRAP::Graphics::API::VulkanFrameBuffer::GetArraySize() const noexcept
{
	return m_arraySize;
}

#endif /*TRAP_VULKANFRAMEBUFFER_H*/