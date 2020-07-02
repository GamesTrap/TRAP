#include "TRAPPCH.h"

#include "VulkanFrameBuffer.h"

TRAP::Graphics::API::VulkanFrameBuffer::VulkanFrameBuffer(const FrameBufferProps& props)
	: m_props(props)
{
	TP_PROFILE_FUNCTION();

	TP_WARN("[FrameBuffer2D][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanFrameBuffer::~VulkanFrameBuffer()
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanFrameBuffer::Bind()
{
	TP_PROFILE_FUNCTION();	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanFrameBuffer::Unbind()
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanFrameBuffer::Resize(uint32_t width, uint32_t height)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanFrameBuffer::GetColorAttachmentRendererID() const
{
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Graphics::FrameBufferProps& TRAP::Graphics::API::VulkanFrameBuffer::GetProps() const
{
	return m_props;
}