#include "TRAPPCH.h"

#include "D3D12FrameBuffer2D.h"

TRAP::Graphics::API::D3D12FrameBuffer2D::D3D12FrameBuffer2D(const uint32_t width, const uint32_t height)
	: m_frameBufferHandle(0), m_depthBufferHandle(0), m_width(width), m_height(height), m_clearColor(0.1f, 0.1f, 0.1f, 1.0f), m_texture(nullptr)
{
	TP_WARN("[FrameBuffer2D][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12FrameBuffer2D::~D3D12FrameBuffer2D() = default;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12FrameBuffer2D::Bind() const
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12FrameBuffer2D::Unbind() const
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12FrameBuffer2D::Clear()
{	
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::D3D12FrameBuffer2D::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::D3D12FrameBuffer2D::GetHeight() const
{
	return m_height;
}

const TRAP::Scope<TRAP::Graphics::Texture>& TRAP::Graphics::API::D3D12FrameBuffer2D::GetTexture() const
{
	return m_texture;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12FrameBuffer2D::SetClearColor(const Math::Vec4& color)
{
	m_clearColor = color;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12FrameBuffer2D::Init()
{
}