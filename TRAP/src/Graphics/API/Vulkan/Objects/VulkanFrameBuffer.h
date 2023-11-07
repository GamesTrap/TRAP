#ifndef TRAP_VULKANFRAMEBUFFER_H
#define TRAP_VULKANFRAMEBUFFER_H

#include "Graphics/API/Vulkan/VulkanRenderer.h"

namespace TRAP::Graphics::API
{
	class VulkanRenderTarget;

	class VulkanFrameBuffer
	{
	public:
		/// @brief Constructor.
		/// @param device Vulkan device.
		/// @param desc Framebuffer description.
		VulkanFrameBuffer(TRAP::Ref<VulkanDevice> device, const VulkanRenderer::FrameBufferDesc& desc);
		/// @brief Destructor.
		~VulkanFrameBuffer();

		/// @brief Copy constructor.
		VulkanFrameBuffer(const VulkanFrameBuffer&) noexcept = default;
		/// @brief Copy assignment operator.
		VulkanFrameBuffer& operator=(const VulkanFrameBuffer&) noexcept = default;
		/// @brief Move constructor.
		VulkanFrameBuffer(VulkanFrameBuffer&&) noexcept = default;
		/// @brief Move assignment operator.
		VulkanFrameBuffer& operator=(VulkanFrameBuffer&&) noexcept = default;

		/// @brief Retrieve the VkFramebuffer handle.
		/// @return VkFramebuffer handle.
		[[nodiscard]] constexpr VkFramebuffer GetVkFrameBuffer() const noexcept;

		/// @brief Retrieve the width of the framebuffer.
		/// @return Framebuffer width.
		[[nodiscard]] constexpr u32 GetWidth() const noexcept;
		/// @brief Retrieve the height of the framebuffer.
		/// @return Framebuffer height.
		[[nodiscard]] constexpr u32 GetHeight() const noexcept;
		/// @brief Retrieve the array size of the framebuffer.
		/// @return Framebuffer array size.
		[[nodiscard]] constexpr u32 GetArraySize() const noexcept;

	private:
		VkFramebuffer m_framebuffer = VK_NULL_HANDLE;

		u32      m_width = 0;
		u32      m_height = 0;
		u32      m_arraySize = 0;

		TRAP::Ref<VulkanDevice> m_device;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkFramebuffer TRAP::Graphics::API::VulkanFrameBuffer::GetVkFrameBuffer() const noexcept
{
	return m_framebuffer;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::API::VulkanFrameBuffer::GetWidth() const noexcept
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::API::VulkanFrameBuffer::GetHeight() const noexcept
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::API::VulkanFrameBuffer::GetArraySize() const noexcept
{
	return m_arraySize;
}

#endif /*TRAP_VULKANFRAMEBUFFER_H*/