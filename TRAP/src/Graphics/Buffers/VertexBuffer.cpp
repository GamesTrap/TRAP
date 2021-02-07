#include "TRAPPCH.h"
#include "VertexBuffer.h"

#include "Application.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/Buffer.h"

TRAP::Scope<TRAP::Graphics::VertexBuffer> TRAP::Graphics::VertexBuffer::Create(float* vertices, const uint32_t size, const BufferUsage usage)
{
	TP_PROFILE_FUNCTION();

	TRAP::Scope<VertexBuffer> buffer = TRAP::Scope<VertexBuffer>(new VertexBuffer());

	RendererAPI::BufferLoadDesc desc{};
	desc.Desc.MemoryUsage = (usage == BufferUsage::Static) ? RendererAPI::ResourceMemoryUsage::GPUOnly : RendererAPI::ResourceMemoryUsage::CPUToGPU;
	desc.Desc.Descriptors = RendererAPI::DescriptorType::VertexBuffer;
	desc.Desc.Size = size;
	desc.Data = vertices;

	RendererAPI::GetResourceLoader()->AddResource(desc, &buffer->m_token);

	buffer->m_vertexBuffer = desc.Buffer;

	return buffer;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::VertexBuffer> TRAP::Graphics::VertexBuffer::Create(const uint32_t size, const BufferUsage usage)
{
	TP_PROFILE_FUNCTION();
	
	TRAP::Scope<VertexBuffer> buffer = TRAP::Scope<VertexBuffer>(new VertexBuffer());
	
	RendererAPI::BufferLoadDesc desc{};
	desc.Desc.MemoryUsage = (usage == BufferUsage::Static) ? RendererAPI::ResourceMemoryUsage::GPUOnly : RendererAPI::ResourceMemoryUsage::CPUToGPU;
	desc.Desc.Descriptors = RendererAPI::DescriptorType::VertexBuffer;
	desc.Desc.Size = size;
	
	RendererAPI::GetResourceLoader()->AddResource(desc, &buffer->m_token);

	buffer->m_vertexBuffer = desc.Buffer;
	
	return buffer;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::VertexBuffer::VertexBuffer()
	: m_vertexBuffer(nullptr), m_token()
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::VertexBuffer::~VertexBuffer()
{
	m_vertexBuffer.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Graphics::BufferLayout& TRAP::Graphics::VertexBuffer::GetLayout() const
{
	return m_bufferLayout;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::VertexBuffer::SetLayout(const BufferLayout& layout)
{
	m_bufferLayout = layout;
}

//-------------------------------------------------------------------------------------------------------------------//

uint64_t TRAP::Graphics::VertexBuffer::GetSize() const
{
	return m_vertexBuffer->GetSize();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::BufferUsage TRAP::Graphics::VertexBuffer::GetBufferUsage() const
{
	return (m_vertexBuffer->GetMemoryUsage() == RendererAPI::ResourceMemoryUsage::GPUOnly) ? BufferUsage::Static : BufferUsage::Dynamic;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::VertexBuffer::Use(Window* window) const
{	
	RendererAPI::GetRenderer()->BindVertexBuffer(m_vertexBuffer, m_bufferLayout, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::VertexBuffer::SetData(void* data, const uint32_t size, const uint32_t offset)
{
	TRAP_ASSERT(size <= m_vertexBuffer->GetSize());
	
	RendererAPI::BufferUpdateDesc desc{ m_vertexBuffer };
	desc.DstOffset = offset;
	RendererAPI::GetResourceLoader()->BeginUpdateResource(desc);
	std::memcpy(desc.MappedData, data, size);
	RendererAPI::GetResourceLoader()->EndUpdateResource(desc, &m_token);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::VertexBuffer::IsLoaded() const
{
	return RendererAPI::GetResourceLoader()->IsTokenCompleted(&m_token);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::VertexBuffer::AwaitLoading() const
{
	RendererAPI::GetResourceLoader()->WaitForToken(&m_token);
}