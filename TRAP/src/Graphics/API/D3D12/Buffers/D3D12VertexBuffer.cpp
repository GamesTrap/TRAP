#include "TRAPPCH.h"
#include "D3D12VertexBuffer.h"

#ifdef TRAP_PLATFORM_WINDOWS

TRAP::Graphics::API::D3D12VertexBuffer::D3D12VertexBuffer(float* vertices, uint32_t size)
{
	TP_PROFILE_FUNCTION();

	TP_WARN("[VertexBuffer][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12VertexBuffer::D3D12VertexBuffer(uint32_t size)
{
	TP_PROFILE_FUNCTION();

	TP_WARN("[VertexBuffer][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12VertexBuffer::~D3D12VertexBuffer()
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Graphics::BufferLayout& TRAP::Graphics::API::D3D12VertexBuffer::GetLayout() const
{
	TP_PROFILE_FUNCTION();
	
	return m_layout;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12VertexBuffer::SetLayout(const BufferLayout& layout)
{
	TP_PROFILE_FUNCTION();
	
	m_layout = layout;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::D3D12VertexBuffer::GetVertexCount() const
{
	TP_PROFILE_FUNCTION();
	
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12VertexBuffer::SetData(const void* data, uint32_t size)
{
	TP_PROFILE_FUNCTION();
}

#endif