#ifndef TRAP_VULKANFRAMEBUFFER_H
#define TRAP_VULKANFRAMEBUFFER_H

#include "VulkanRenderPass.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

namespace TRAP::Graphics::API
{
	class VulkanRenderTarget;

	class VulkanFrameBuffer
	{
	public:
		VulkanFrameBuffer(TRAP::Ref<VulkanDevice> device, const VulkanRenderer::FrameBufferDesc& desc);
		~VulkanFrameBuffer();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanFrameBuffer(const VulkanFrameBuffer&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanFrameBuffer& operator=(const VulkanFrameBuffer&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanFrameBuffer(VulkanFrameBuffer&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanFrameBuffer& operator=(VulkanFrameBuffer&&) = default;

		VkFramebuffer GetVkFrameBuffer() const;

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
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