#ifndef _TRAP_VULKANFRAMEBUFFER_H_
#define _TRAP_VULKANFRAMEBUFFER_H_

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

		VkFramebuffer& GetVkFrameBuffer();

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

#endif /*_TRAP_VULKANFRAMEBUFFER_H_*/