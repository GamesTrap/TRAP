#include "TRAPPCH.h"
#include "D3D12IndexBuffer.h"

#ifdef TRAP_PLATFORM_WINDOWS

TRAP::Graphics::API::D3D12IndexBuffer::D3D12IndexBuffer(uint32_t* indices, const uint32_t size)
	: m_count(size)
{
	TP_WARN("[IndexBuffer][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12IndexBuffer::~D3D12IndexBuffer() = default;

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::D3D12IndexBuffer::GetCount() const
{
	return m_count;
}

#endif