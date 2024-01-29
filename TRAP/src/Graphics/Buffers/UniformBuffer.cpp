#include "TRAPPCH.h"
#include "UniformBuffer.h"

#include "Graphics/API/Objects/Buffer.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/Shaders/Shader.h"

[[nodiscard]] TRAP::Scope<TRAP::Graphics::UniformBuffer> TRAP::Graphics::UniformBuffer::Create(const u64 size,
																				               const UpdateFrequency updateFrequency)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	return Init((u8*)nullptr, size, updateFrequency);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] u64 TRAP::Graphics::UniformBuffer::GetSize() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return m_uniformBuffers[0]->GetSize();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::UniformBuffer::SetData(const void* const data, const u64 size, const u64 offset)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	TRAP_ASSERT(data, "UniformBuffer::SetData(): Data is nullptr!");
	TRAP_ASSERT(size + offset <= m_uniformBuffers[0]->GetSize(), "UniformBuffer::SetData(): Out of bounds!");

	for(usize i = 0; i < m_uniformBuffers.size(); ++i)
	{
		RendererAPI::BufferUpdateDesc desc{};
		desc.Buffer = m_uniformBuffers[i];
		desc.DstOffset = offset;
		API::ResourceLoader::BeginUpdateResource(desc);
		std::copy_n(static_cast<const u8*>(data), size, static_cast<u8*>(desc.MappedData));
		RendererAPI::GetResourceLoader()->EndUpdateResource(desc, &m_tokens[i]);
	}
	AwaitLoading();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::UniformBuffer::IsLoaded() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	for(usize i = 0; i < m_uniformBuffers.size(); ++i)
	{
	   if(!RendererAPI::GetResourceLoader()->IsTokenCompleted(&m_tokens[i]))
			return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::UniformBuffer::AwaitLoading() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	for(usize i = 0; i < m_uniformBuffers.size(); ++i)
		RendererAPI::GetResourceLoader()->WaitForToken(&m_tokens[i]);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] u64 TRAP::Graphics::UniformBuffer::CalculateAlignedSize(const u64 byteSize) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	const u64 minUBOAlignment = RendererAPI::GPUSettings.UniformBufferAlignment;
	u64 alignedSize = byteSize;

	if(minUBOAlignment > 0)
		alignedSize = (alignedSize + minUBOAlignment - 1) & ~(minUBOAlignment - 1);

	return alignedSize;
}
