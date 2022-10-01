#include "TRAPPCH.h"
#include "VertexBuffer.h"

#include "Application.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/Buffer.h"

TRAP::Scope<TRAP::Graphics::VertexBuffer> TRAP::Graphics::VertexBuffer::Create(const float* const vertices, const uint64_t size,
                                                                               const UpdateFrequency updateFrequency)
{
	ZoneScopedC(tracy::Color::Red);

	return Init(vertices, size, updateFrequency);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::VertexBuffer> TRAP::Graphics::VertexBuffer::Create(const uint64_t size,
                                                                               const UpdateFrequency updateFrequency)
{
	ZoneScopedC(tracy::Color::Red);

	return Init(nullptr, size, updateFrequency);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::VertexBuffer::VertexBuffer()
	: m_vertexBuffer(nullptr), m_token()
{
	ZoneScopedC(tracy::Color::Red);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::VertexBuffer::~VertexBuffer()
{
	ZoneScopedC(tracy::Color::Red);

	m_vertexBuffer.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Graphics::VertexBufferLayout& TRAP::Graphics::VertexBuffer::GetLayout() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_bufferLayout;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::VertexBuffer::SetLayout(const VertexBufferLayout& layout)
{
	ZoneScopedC(tracy::Color::Red);

	m_bufferLayout = layout;
}

//-------------------------------------------------------------------------------------------------------------------//

uint64_t TRAP::Graphics::VertexBuffer::GetSize() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_vertexBuffer->GetSize();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::VertexBuffer::GetCount() const
{
	ZoneScopedC(tracy::Color::Red);

	return static_cast<uint32_t>(m_vertexBuffer->GetSize() / static_cast<uint64_t>(m_bufferLayout.GetStride()));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::UpdateFrequency TRAP::Graphics::VertexBuffer::GetUpdateFrequency() const
{
	ZoneScopedC(tracy::Color::Red);

	return (m_vertexBuffer->GetMemoryUsage() == RendererAPI::ResourceMemoryUsage::GPUOnly) ? UpdateFrequency::Static :
	                                                                                         UpdateFrequency::Dynamic;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::VertexBuffer::Use(const Window* const window) const
{
	ZoneScopedC(tracy::Color::Red);

	RendererAPI::GetRenderer()->BindVertexBuffer(m_vertexBuffer, m_bufferLayout, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::VertexBuffer::SetData(const float* const data, const uint64_t size, const uint64_t offset)
{
	ZoneScopedC(tracy::Color::Red);

	TRAP_ASSERT(data);
	TRAP_ASSERT(size + offset <= m_vertexBuffer->GetSize());

	RendererAPI::BufferUpdateDesc desc{};
	desc.Buffer = m_vertexBuffer;
	desc.DstOffset = offset;
	RendererAPI::GetResourceLoader()->BeginUpdateResource(desc);
	std::copy_n(data, size / sizeof(float), static_cast<float*>(desc.MappedData));
	RendererAPI::GetResourceLoader()->EndUpdateResource(desc, &m_token);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::VertexBuffer::IsLoaded() const
{
	ZoneScopedC(tracy::Color::Red);

	return RendererAPI::GetResourceLoader()->IsTokenCompleted(&m_token);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::VertexBuffer::AwaitLoading() const
{
	ZoneScopedC(tracy::Color::Red);

	RendererAPI::GetResourceLoader()->WaitForToken(&m_token);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::VertexBuffer> TRAP::Graphics::VertexBuffer::Init(const float* const vertices, const uint64_t size,
 																		     const UpdateFrequency updateFrequency)
{
	ZoneScopedC(tracy::Color::Red);

	TRAP::Scope<VertexBuffer> buffer = TRAP::Scope<VertexBuffer>(new VertexBuffer());

	RendererAPI::BufferLoadDesc desc{};
	desc.Desc.MemoryUsage = (updateFrequency == UpdateFrequency::Static) ? RendererAPI::ResourceMemoryUsage::GPUOnly :
	                                                                       RendererAPI::ResourceMemoryUsage::CPUToGPU;
	desc.Desc.Descriptors = RendererAPI::DescriptorType::VertexBuffer;
	desc.Desc.Size = size;
	desc.Desc.Flags = (updateFrequency != UpdateFrequency::Static) ? RendererAPI::BufferCreationFlags::PersistentMap :
	                                                                 RendererAPI::BufferCreationFlags::None;
	desc.Data = vertices;

	RendererAPI::GetResourceLoader()->AddResource(desc, &buffer->m_token);

	buffer->m_vertexBuffer = desc.Buffer;

	return buffer;
}