#include "TRAPPCH.h"
#include "D3D12VertexArray.h"

#ifdef TRAP_PLATFORM_WINDOWS

TRAP::Graphics::API::D3D12VertexArray::D3D12VertexArray() = default;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12VertexArray::~D3D12VertexArray() = default;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12VertexArray::AddBuffer(std::unique_ptr<VertexBuffer>& buffer)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12VertexArray::Bind() const
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12VertexArray::Unbind() const
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12VertexArray::Draw(const uint32_t count) const
{
}

#endif