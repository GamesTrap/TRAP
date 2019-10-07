#include "TRAPPCH.h"

#include "VulkanFrameBuffer2D.h"

TRAP::Graphics::API::VulkanFrameBuffer2D::VulkanFrameBuffer2D(const uint32_t width, const uint32_t height)
	: m_width(width), m_height(height), m_clearColor(0.1f, 0.1f, 0.1f, 1.0f), m_texture(nullptr)
{
	TP_WARN("[FrameBuffer2D][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanFrameBuffer2D::~VulkanFrameBuffer2D() = default;

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

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanFrameBuffer2D::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanFrameBuffer2D::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::Texture>& TRAP::Graphics::API::VulkanFrameBuffer2D::GetTexture() const
{
	return dynamic_cast<const Scope<Texture>&>(m_texture);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanFrameBuffer2D::SetClearColor(const Math::Vec4& color)
{
	m_clearColor = color;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanFrameBuffer2D::Init()
{
}