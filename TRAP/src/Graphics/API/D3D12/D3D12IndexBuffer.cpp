#include "TRAPPCH.h"
#include "D3D12IndexBuffer.h"

TRAP::Graphics::API::D3D12IndexBuffer::D3D12IndexBuffer(uint32_t* indices, const uint32_t size, BufferUsage usage)
	: m_count(size)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12IndexBuffer::~D3D12IndexBuffer() = default;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12IndexBuffer::Bind() const
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12IndexBuffer::Unbind() const
{
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::D3D12IndexBuffer::GetCount() const
{
	return m_count;
}