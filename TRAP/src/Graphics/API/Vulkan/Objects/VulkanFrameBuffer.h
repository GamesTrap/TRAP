#ifndef _TRAP_VULKANFRAMEBUFFER_H_
#define _TRAP_VULKANFRAMEBUFFER_H_

namespace TRAP::Graphics::API
{
	class VulkanFrameBuffer
	{
	public:
		VulkanFrameBuffer();
		~VulkanFrameBuffer();

		VkFramebuffer& GetVkFrameBuffer();
		
	private:
		VkFramebuffer m_framebuffer;
	};
}

#endif /*_TRAP_VULKANFRAMEBUFFER_H_*/