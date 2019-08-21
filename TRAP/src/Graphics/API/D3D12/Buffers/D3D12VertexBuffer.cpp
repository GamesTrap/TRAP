#include "TRAPPCH.h"
#include "D3D12VertexBuffer.h"

#ifdef TRAP_PLATFORM_WINDOWS

TRAP::Graphics::API::D3D12VertexBuffer::D3D12VertexBuffer(float* vertices, uint32_t size)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12VertexBuffer::~D3D12VertexBuffer() = default;

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Graphics::BufferLayout& TRAP::Graphics::API::D3D12VertexBuffer::GetLayout() const
{
	return m_layout;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12VertexBuffer::SetLayout(const BufferLayout& layout)
{
	m_layout = layout;
}

#endif