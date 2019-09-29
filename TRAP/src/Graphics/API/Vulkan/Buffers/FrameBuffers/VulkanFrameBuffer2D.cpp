#include "TRAPPCH.h"

#include "VulkanFrameBuffer2D.h"

TRAP::Graphics::API::VulkanFrameBuffer2D::VulkanFrameBuffer2D(const uint32_t width, const uint32_t height)
	: m_frameBufferHandle(0), m_depthBufferHandle(0), m_width(width), m_height(height), m_clearColor(0.1f, 0.1f, 0.1f, 1.0f), m_texture(nullptr)
{
	TP_WARN("[FrameBuffer2D][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanFrameBuffer2D::~VulkanFrameBuffer2D() = default;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanFrameBuffer2D::Init()
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanFrameBuffer2D::Bind() const
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanFrameBuffer2D::Unbind() const
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanFrameBuffer2D::Clear()
{	
}