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
		VkFramebuffer GetVkFrameBuffer() const;

		/// <summary>
		/// Retrieve the width of the framebuffer.
		/// </summary>
		/// <returns>Framebuffer width.</returns>
		uint32_t GetWidth() const;
		/// <summary>
		/// Retrieve the height of the framebuffer.
		/// </summary>
		/// <returns>Framebuffer height.</returns>
		uint32_t GetHeight() const;
		/// <summary>
		/// Retrieve the array size of the framebuffer.
		/// </summary>
		/// <returns>Framebuffer array size.</returns>
		uint32_t GetArraySize() const;

	private:
		VkFramebuffer m_framebuffer;

		uint32_t      m_width;
		uint32_t      m_height;
		uint32_t      m_arraySize;

		TRAP::Ref<VulkanDevice> m_device;
	};
}

#endif /*TRAP_VULKANFRAMEBUFFER_H*/