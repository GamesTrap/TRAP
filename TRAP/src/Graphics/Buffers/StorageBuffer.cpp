#include "TRAPPCH.h"
#include "StorageBuffer.h"

#include "Graphics/API/RendererAPI.h"
#include "Graphics/Shaders/Shader.h"

[[nodiscard]] TRAP::Scope<TRAP::Graphics::StorageBuffer> TRAP::Graphics::StorageBuffer::Create(const u64 size,
																				               const UpdateFrequency updateFrequency)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	return Init((u8*)nullptr, size, updateFrequency);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] u64 TRAP::Graphics::StorageBuffer::GetSize() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return m_storageBuffers[0]->GetSize();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::StorageBuffer::SetData(const void* const data, const u64 size, const u64 offset)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	TRAP_ASSERT(data, "StorageBuffer::SetData(): Data is nullptr!");
	TRAP_ASSERT(size + offset <= m_storageBuffers[0]->GetSize(), "StorageBuffer::SetData(): Out of bounds!");

	for(usize i = 0; i < m_storageBuffers.size(); ++i)
	{
		RendererAPI::BufferUpdateDesc desc{};
		desc.Buffer = m_storageBuffers[i];
		desc.DstOffset = offset;
		API::ResourceLoader::BeginUpdateResource(desc);
		std::copy_n(static_cast<const u8*>(data), size, desc.MappedData.begin());
		RendererAPI::GetResourceLoader()->EndUpdateResource(desc, &m_tokens[i]);
	}
	AwaitLoading();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::StorageBuffer::IsLoaded() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	for(usize i = 0; i < m_storageBuffers.size(); ++i)
	{
	   if(!RendererAPI::GetResourceLoader()->IsTokenCompleted(&m_tokens[i]))
			return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::StorageBuffer::AwaitLoading() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	for(usize i = 0; i < m_storageBuffers.size(); ++i)
		RendererAPI::GetResourceLoader()->WaitForToken(&m_tokens[i]);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] u64 TRAP::Graphics::StorageBuffer::CalculateAlignedSize(const u64 byteSize) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	const u64 minSSBOAlignment = RendererAPI::GPUSettings.StorageBufferAlignment;
	u64 alignedSize = byteSize;

	if(minSSBOAlignment > 0)
		alignedSize = (alignedSize + minSSBOAlignment - 1) & ~(minSSBOAlignment - 1);

	return alignedSize;
}
