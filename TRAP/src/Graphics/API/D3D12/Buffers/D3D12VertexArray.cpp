#include "TRAPPCH.h"
#include "D3D12VertexArray.h"

#ifdef TRAP_PLATFORM_WINDOWS

TRAP::Graphics::API::D3D12VertexArray::D3D12VertexArray() = default;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12VertexArray::~D3D12VertexArray() = default;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12VertexArray::AddVertexBuffer(std::unique_ptr<VertexBuffer>& buffer)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12VertexArray::SetIndexBuffer(std::unique_ptr<IndexBuffer>& buffer)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::unique_ptr<TRAP::Graphics::API::VertexBuffer>>& TRAP::Graphics::API::D3D12VertexArray::GetVertexBuffers()
{
	return m_vertexBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::IndexBuffer* TRAP::Graphics::API::D3D12VertexArray::GetIndexBuffer()
{
	return m_indexBuffer.get();
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

void TRAP::Graphics::API::D3D12VertexArray::Draw() const
{
}

#endif