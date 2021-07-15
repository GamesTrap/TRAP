#include "TRAPPCH.h"
#include "IndexBuffer.h"

#include "Graphics/API/Objects/Buffer.h"
#include "VertexBufferLayout.h"

TRAP::Scope<TRAP::Graphics::IndexBuffer> TRAP::Graphics::IndexBuffer::Create(uint32_t* indices, const uint64_t size,
                                                                             const BufferUsage usage)
{
	TP_PROFILE_FUNCTION();

	return Init<uint32_t>(indices, size, usage);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::IndexBuffer> TRAP::Graphics::IndexBuffer::Create(uint16_t* indices, const uint64_t size,
                                                                             const BufferUsage usage)
{
	TP_PROFILE_FUNCTION();

	return Init<uint16_t>(indices, size, usage);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::IndexBuffer> TRAP::Graphics::IndexBuffer::Create(const uint64_t size,
                                                                             const BufferUsage usage)
{
	TP_PROFILE_FUNCTION();

	return Init<uint16_t>(nullptr, size, usage); //uint16_t get ignored
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::IndexBuffer::IndexBuffer()
	: m_indexBuffer(nullptr), m_token(), m_indexType(RendererAPI::IndexType::UInt32)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::IndexBuffer::~IndexBuffer()
{
	m_indexBuffer.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

uint64_t TRAP::Graphics::IndexBuffer::GetCount() const
{
	return m_indexBuffer->GetSize() / ((m_indexType == RendererAPI::IndexType::UInt16) ? sizeof(uint16_t) :
	                                                                                     sizeof(uint32_t));
}

//-------------------------------------------------------------------------------------------------------------------//

uint64_t TRAP::Graphics::IndexBuffer::GetSize() const
{
	return m_indexBuffer->GetSize();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::BufferUsage TRAP::Graphics::IndexBuffer::GetBufferUsage() const
{
	return (m_indexBuffer->GetMemoryUsage() == RendererAPI::ResourceMemoryUsage::GPUOnly) ? BufferUsage::Static :
	                                                                                        BufferUsage::Dynamic;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::IndexBuffer::SetData(uint16_t* indices, const uint64_t size, const uint64_t offset)
{
	SetDataInternal(indices, size, offset);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::IndexBuffer::SetData(uint32_t* indices, const uint64_t size, const uint64_t offset)
{
	SetDataInternal(indices, size, offset);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::IndexBuffer::Use(Window* window) const
{
	RendererAPI::GetRenderer()->BindIndexBuffer(m_indexBuffer, m_indexType, window);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::IndexBuffer::IsLoaded() const
{
	return RendererAPI::GetResourceLoader()->IsTokenCompleted(&m_token);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::IndexBuffer::AwaitLoading() const
{
	RendererAPI::GetResourceLoader()->WaitForToken(&m_token);
}