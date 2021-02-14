#include "TRAPPCH.h"
#include "IndexBuffer.h"

#include "Graphics/API/Objects/Buffer.h"
#include "BufferLayout.h"

TRAP::Scope<TRAP::Graphics::IndexBuffer> TRAP::Graphics::IndexBuffer::Create(uint32_t* indices, const uint32_t size, const BufferUsage usage)
{
	TP_PROFILE_FUNCTION();

	TRAP::Scope<IndexBuffer> buffer = TRAP::Scope<IndexBuffer>(new IndexBuffer());

	RendererAPI::BufferLoadDesc desc{};
	desc.Desc.MemoryUsage = (usage == BufferUsage::Static) ? RendererAPI::ResourceMemoryUsage::GPUOnly : RendererAPI::ResourceMemoryUsage::CPUToGPU;
	desc.Desc.Descriptors = RendererAPI::DescriptorType::IndexBuffer;
	desc.Desc.Size = size;
	desc.Desc.Flags = (usage == BufferUsage::Dynamic) ? RendererAPI::BufferCreationFlags::PersistentMap : RendererAPI::BufferCreationFlags::None;
	desc.Data = indices;

	RendererAPI::GetResourceLoader()->AddResource(desc, &buffer->m_token);

	buffer->m_indexType = RendererAPI::IndexType::UInt32;
	buffer->m_indexBuffer = desc.Buffer;

	return buffer;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::IndexBuffer> TRAP::Graphics::IndexBuffer::Create(uint16_t* indices, const uint32_t size, const BufferUsage usage)
{
	TP_PROFILE_FUNCTION();

	TRAP::Scope<IndexBuffer> buffer = TRAP::Scope<IndexBuffer>(new IndexBuffer());

	RendererAPI::BufferLoadDesc desc{};
	desc.Desc.MemoryUsage = (usage == BufferUsage::Static) ? RendererAPI::ResourceMemoryUsage::GPUOnly : RendererAPI::ResourceMemoryUsage::CPUToGPU;
	desc.Desc.Descriptors = RendererAPI::DescriptorType::IndexBuffer;
	desc.Desc.Size = size;
	desc.Desc.Flags = (usage == BufferUsage::Dynamic) ? RendererAPI::BufferCreationFlags::PersistentMap : RendererAPI::BufferCreationFlags::None;
	desc.Data = indices;

	RendererAPI::GetResourceLoader()->AddResource(desc, &buffer->m_token);

	buffer->m_indexType = RendererAPI::IndexType::UInt16;
	buffer->m_indexBuffer = desc.Buffer;

	return buffer;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::IndexBuffer> TRAP::Graphics::IndexBuffer::Create(const uint32_t size, const BufferUsage usage)
{
	TP_PROFILE_FUNCTION();

	TRAP::Scope<IndexBuffer> buffer = TRAP::Scope<IndexBuffer>(new IndexBuffer());

	RendererAPI::BufferLoadDesc desc{};
	desc.Desc.MemoryUsage = (usage == BufferUsage::Static) ? RendererAPI::ResourceMemoryUsage::GPUOnly : RendererAPI::ResourceMemoryUsage::CPUToGPU;
	desc.Desc.Descriptors = RendererAPI::DescriptorType::IndexBuffer;
	desc.Desc.Size = size;
	desc.Desc.Flags = (usage == BufferUsage::Dynamic) ? RendererAPI::BufferCreationFlags::PersistentMap : RendererAPI::BufferCreationFlags::None;
	desc.Data = nullptr;

	RendererAPI::GetResourceLoader()->AddResource(desc, &buffer->m_token);

	buffer->m_indexType = RendererAPI::IndexType::UInt16;
	buffer->m_indexBuffer = desc.Buffer;

	return buffer;
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
	return m_indexBuffer->GetSize() / ((m_indexType == RendererAPI::IndexType::UInt16) ? sizeof(uint16_t) : sizeof(uint32_t));
}

//-------------------------------------------------------------------------------------------------------------------//

uint64_t TRAP::Graphics::IndexBuffer::GetSize() const
{
	return m_indexBuffer->GetSize();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::BufferUsage TRAP::Graphics::IndexBuffer::GetBufferUsage() const
{
	return (m_indexBuffer->GetMemoryUsage() == RendererAPI::ResourceMemoryUsage::GPUOnly) ? BufferUsage::Static : BufferUsage::Dynamic;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::IndexBuffer::SetData(uint16_t* indices, const uint32_t size, const uint32_t offset)
{
	TRAP_ASSERT(size + offset <= m_indexBuffer->GetSize());

	RendererAPI::BufferUpdateDesc desc{ m_indexBuffer };
	desc.DstOffset = offset;
	RendererAPI::GetResourceLoader()->BeginUpdateResource(desc);
	std::memcpy(desc.MappedData, indices, size);
	RendererAPI::GetResourceLoader()->EndUpdateResource(desc, &m_token);
	m_indexType = RendererAPI::IndexType::UInt16;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::IndexBuffer::SetData(uint32_t* indices, const uint32_t size, const uint32_t offset)
{
	TRAP_ASSERT(size + offset <= m_indexBuffer->GetSize());

	RendererAPI::BufferUpdateDesc desc{ m_indexBuffer };
	desc.DstOffset = offset;
	RendererAPI::GetResourceLoader()->BeginUpdateResource(desc);
	std::memcpy(desc.MappedData, indices, size);
	RendererAPI::GetResourceLoader()->EndUpdateResource(desc, &m_token);
	m_indexType = RendererAPI::IndexType::UInt32;
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