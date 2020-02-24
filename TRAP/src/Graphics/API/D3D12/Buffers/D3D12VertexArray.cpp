#include "TRAPPCH.h"
#include "D3D12VertexArray.h"

#ifdef TRAP_PLATFORM_WINDOWS

TRAP::Graphics::API::D3D12VertexArray::D3D12VertexArray()
{
	TP_PROFILE_FUNCTION();

	TP_WARN("[VertexArray][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12VertexArray::~D3D12VertexArray()
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12VertexArray::AddVertexBuffer(Scope<VertexBuffer>& buffer)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12VertexArray::SetIndexBuffer(Scope<IndexBuffer>& buffer)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12VertexArray::Bind() const
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12VertexArray::Unbind() const
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Scope<TRAP::Graphics::VertexBuffer>>& TRAP::Graphics::API::D3D12VertexArray::GetVertexBuffers()
{
	TP_PROFILE_FUNCTION();
	
	return m_vertexBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::IndexBuffer>& TRAP::Graphics::API::D3D12VertexArray::GetIndexBuffer()
{
	TP_PROFILE_FUNCTION();
	
	return m_indexBuffer;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::D3D12VertexArray::GetIndexCount() const
{
	TP_PROFILE_FUNCTION();
	
	return 0;
}

#endif