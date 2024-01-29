#include "TRAPPCH.h"
#include "VertexBuffer.h"

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/Buffer.h"

[[nodiscard]] TRAP::Scope<TRAP::Graphics::VertexBuffer> TRAP::Graphics::VertexBuffer::Create(const std::span<const f32> vertices,
                                                                                             const UpdateFrequency updateFrequency)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	return Init(vertices.data(), vertices.size_bytes(), updateFrequency);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Graphics::VertexBuffer> TRAP::Graphics::VertexBuffer::Create(const u64 size,
                                                                                             const UpdateFrequency updateFrequency)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	return Init(nullptr, size, updateFrequency);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] u64 TRAP::Graphics::VertexBuffer::GetSize() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return m_vertexBuffer->GetSize();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] u32 TRAP::Graphics::VertexBuffer::GetCount() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return NumericCast<u32>(m_vertexBuffer->GetSize() / NumericCast<u64>(m_bufferLayout.GetStride()));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::UpdateFrequency TRAP::Graphics::VertexBuffer::GetUpdateFrequency() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return (m_vertexBuffer->GetMemoryUsage() == RendererAPI::ResourceMemoryUsage::GPUOnly) ? UpdateFrequency::Static :
	                                                                                         UpdateFrequency::Dynamic;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::VertexBuffer::Use(const Window* const window) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	RendererAPI::GetRenderer()->BindVertexBuffer(m_vertexBuffer, m_bufferLayout, window);
}
#else
void TRAP::Graphics::VertexBuffer::Use() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	RendererAPI::GetRenderer()->BindVertexBuffer(m_vertexBuffer, m_bufferLayout);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::VertexBuffer::SetData(const std::span<const f32> data, const u64 offset)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	TRAP_ASSERT(!data.empty(), "VertexBuffer::SetData(): Data is nullptr!");
	TRAP_ASSERT(data.size_bytes() + offset <= m_vertexBuffer->GetSize(), "VertexBuffer::SetData(): Out of bounds!");

	RendererAPI::BufferUpdateDesc desc{};
	desc.Buffer = m_vertexBuffer;
	desc.DstOffset = offset;
	API::ResourceLoader::BeginUpdateResource(desc);
	std::copy_n(data.data(), data.size(), static_cast<f32*>(desc.MappedData));
	RendererAPI::GetResourceLoader()->EndUpdateResource(desc, &m_token);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::VertexBuffer::IsLoaded() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	return RendererAPI::GetResourceLoader()->IsTokenCompleted(&m_token);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::VertexBuffer::AwaitLoading() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	RendererAPI::GetResourceLoader()->WaitForToken(&m_token);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Graphics::VertexBuffer> TRAP::Graphics::VertexBuffer::Init(const f32* const vertices, const u64 size,
 																		                   const UpdateFrequency updateFrequency)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	TRAP::Scope<VertexBuffer> buffer = TRAP::Scope<VertexBuffer>(new VertexBuffer());

	RendererAPI::BufferLoadDesc desc{};
	desc.Desc.MemoryUsage = (updateFrequency == UpdateFrequency::Static) ? RendererAPI::ResourceMemoryUsage::GPUOnly :
	                                                                       RendererAPI::ResourceMemoryUsage::CPUToGPU;
	desc.Desc.Descriptors = RendererAPI::DescriptorType::VertexBuffer;
	desc.Desc.Size = size;
	desc.Desc.StructStride = sizeof(f32);
	desc.Desc.ElementCount = size / desc.Desc.StructStride;
	desc.Desc.Flags = (updateFrequency != UpdateFrequency::Static) ? RendererAPI::BufferCreationFlags::PersistentMap :
	                                                                 RendererAPI::BufferCreationFlags::None;
	desc.Data = vertices;

	RendererAPI::GetResourceLoader()->AddResource(desc, &buffer->m_token);

	buffer->m_vertexBuffer = desc.Buffer;

	return buffer;
}