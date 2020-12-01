#include "TRAPPCH.h"
#include "VulkanFrameBuffer.h"

TRAP::Graphics::API::VulkanFrameBuffer::VulkanFrameBuffer()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanFrameBufferPrefix, "Creating FrameBuffer");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanFrameBuffer::~VulkanFrameBuffer()
{
	if (m_framebuffer)
	{
#ifdef ENABLE_GRAPHICS_DEBUG
		TP_DEBUG(Log::RendererVulkanFrameBufferPrefix, "Destroying FrameBuffer");
#endif
		//TODO
		m_framebuffer = nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VkFramebuffer& TRAP::Graphics::API::VulkanFrameBuffer::GetVkFrameBuffer()
{
	return m_framebuffer;
}