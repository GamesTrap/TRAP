#include "TRAPPCH.h"
#include "VertexBuffer.h"

#include "Application.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/Buffer.h"

TRAP::Scope<TRAP::Graphics::VertexBuffer> TRAP::Graphics::VertexBuffer::Create(float* vertices, const uint64_t size,
                                                                               const BufferUsage usage)
{
	TP_PROFILE_FUNCTION();

	return Init(vertices, size, usage);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::VertexBuffer> TRAP::Graphics::VertexBuffer::Create(const uint64_t size,
                                                                               const BufferUsage usage)
{
	TP_PROFILE_FUNCTION();

	return Init(nullptr, size, usage);
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

const TRAP::Graphics::VertexBufferLayout& TRAP::Graphics::VertexBuffer::GetLayout() const
{
	return m_bufferLayout;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::VertexBuffer::SetLayout(const VertexBufferLayout& layout)
{
	m_bufferLayout = layout;
}

//-------------------------------------------------------------------------------------------------------------------//

uint64_t TRAP::Graphics::VertexBuffer::GetSize() const
{
	return m_vertexBuffer->GetSize();
}

//-------------------------------------------------------------------------------------------------------------------//

uint64_t TRAP::Graphics::VertexBuffer::GetCount() const
{
	return m_vertexBuffer->GetSize() / sizeof(float);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::BufferUsage TRAP::Graphics::VertexBuffer::GetBufferUsage() const
{
	return (m_vertexBuffer->GetMemoryUsage() == RendererAPI::ResourceMemoryUsage::GPUOnly) ? BufferUsage::Static :
	                                                                                         BufferUsage::Dynamic;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::VertexBuffer::Use(Window* window) const
{
	RendererAPI::GetRenderer()->BindVertexBuffer(m_vertexBuffer, m_bufferLayout, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::VertexBuffer::SetData(float* data, const uint64_t size, const uint64_t offset)
{
	TRAP_ASSERT(data);
	TRAP_ASSERT(size + offset <= m_vertexBuffer->GetSize());

	RendererAPI::BufferUpdateDesc desc{};
	desc.Buffer = m_vertexBuffer;
	desc.DstOffset = offset;
	RendererAPI::GetResourceLoader()->BeginUpdateResource(desc);
	std::memcpy(desc.MappedData, static_cast<void*>(data), size);
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

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::VertexBuffer> TRAP::Graphics::VertexBuffer::Init(float* vertices, const uint64_t size,
 																		     const BufferUsage usage)
{
	TRAP::Scope<VertexBuffer> buffer = TRAP::Scope<VertexBuffer>(new VertexBuffer());

	RendererAPI::BufferLoadDesc desc{};
	desc.Desc.MemoryUsage = (usage == BufferUsage::Static) ? RendererAPI::ResourceMemoryUsage::GPUOnly :
	                                                         RendererAPI::ResourceMemoryUsage::CPUToGPU;
	desc.Desc.Descriptors = RendererAPI::DescriptorType::VertexBuffer;
	desc.Desc.Size = size;
	desc.Desc.Flags = (usage == BufferUsage::Dynamic) ? RendererAPI::BufferCreationFlags::PersistentMap :
	                                                    RendererAPI::BufferCreationFlags::None;
	desc.Data = vertices;

	RendererAPI::GetResourceLoader()->AddResource(desc, &buffer->m_token);

	buffer->m_vertexBuffer = desc.Buffer;

	return buffer;
}