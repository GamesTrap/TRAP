#include "TRAPPCH.h"
#include "IndexBuffer.h"

namespace
{
	/// @brief Set new index buffer data.
	/// @param indexBuffer Index buffer to update.
	/// @param outSyncToken Sync token to update.
	/// @param outIndexType Index type to update.
	/// @param indices Pointer to the data to upload.
	/// @param offset Byte offset into the currently used index data.
	template<typename T, usize Size = std::dynamic_extent>
	requires std::same_as<T, u16> || std::same_as<T, u32>
	void SetDataInternal(const TRAP::Ref<TRAP::Graphics::Buffer>& indexBuffer,
	                     TRAP::Graphics::API::SyncToken& outSyncToken,
						 TRAP::Graphics::RendererAPI::IndexType& outIndexType,
						 const std::span<const T, Size> indices, const u64 offset = 0)
	{
		ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

		TRAP_ASSERT(indexBuffer, "IndexBuffer::SetDataInternal(): indexBuffer is nullptr!");
		TRAP_ASSERT(!indices.empty(), "IndexBuffer::SetDataInternal(): Indices is empty!");
		TRAP_ASSERT(indices.size_bytes() + offset <= indexBuffer->GetSize(), "IndexBuffer::SetDataInternal(): Out of bounds!");

		TRAP::Graphics::RendererAPI::BufferUpdateDesc desc{};
		desc.Buffer = indexBuffer;
		desc.DstOffset = offset;
		TRAP::Graphics::API::ResourceLoader::BeginUpdateResource(desc);
		std::ranges::copy(indices, reinterpret_cast<T*>(desc.MappedData.data()));
		TRAP::Graphics::RendererAPI::GetResourceLoader()->EndUpdateResource(desc, &outSyncToken);

		if constexpr(std::same_as<T, u16>)
			outIndexType = TRAP::Graphics::RendererAPI::IndexType::UInt16;
		else if constexpr(std::same_as<T, u32>)
			outIndexType = TRAP::Graphics::RendererAPI::IndexType::UInt32;
	}

}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::IndexBuffer::IndexBuffer(const RendererAPI::IndexType indexType, const API::SyncToken syncToken,
                                         const TRAP::Ref<TRAP::Graphics::Buffer>& indexBuffer) noexcept
	: m_indexBuffer(indexBuffer), m_token(syncToken), m_indexType(indexType)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Graphics::IndexBuffer> TRAP::Graphics::IndexBuffer::Create(const std::span<const u32> indices,
                                                                                           const UpdateFrequency updateFrequency)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	return Init<u32>(indices.data(), indices.size_bytes(), updateFrequency);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Graphics::IndexBuffer> TRAP::Graphics::IndexBuffer::Create(const std::span<const u16> indices,
                                                                                           const UpdateFrequency updateFrequency)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	return Init<u16>(indices.data(), indices.size_bytes(), updateFrequency);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Graphics::IndexBuffer> TRAP::Graphics::IndexBuffer::Create(const u64 size,
                                                                                           const UpdateFrequency updateFrequency)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	return Init<u16>(nullptr, size, updateFrequency); //u16 gets ignored
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] u32 TRAP::Graphics::IndexBuffer::GetCount() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return NumericCast<u32>(m_indexBuffer->GetSize() /
	                             ((m_indexType == RendererAPI::IndexType::UInt16) ? sizeof(u16) :
	                                                                                sizeof(u32)));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] u64 TRAP::Graphics::IndexBuffer::GetSize() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return m_indexBuffer->GetSize();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::UpdateFrequency TRAP::Graphics::IndexBuffer::GetUpdateFrequency() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return (m_indexBuffer->GetMemoryUsage() == RendererAPI::ResourceMemoryUsage::GPUOnly) ? UpdateFrequency::Static :
	                                                                                        UpdateFrequency::Dynamic;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::IndexBuffer::SetData(const std::span<const u16> indices, const u64 offset)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	SetDataInternal(m_indexBuffer, m_token, m_indexType, indices, offset);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::IndexBuffer::SetData(const std::span<const u32> indices, const u64 offset)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	SetDataInternal(m_indexBuffer, m_token, m_indexType, indices, offset);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::IndexBuffer::Use(const Window& window) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	RendererAPI::GetRenderer()->BindIndexBuffer(*m_indexBuffer, m_indexType, window);
}
#else
void TRAP::Graphics::IndexBuffer::Use() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	RendererAPI::GetRenderer()->BindIndexBuffer(*m_indexBuffer, m_indexType);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::IndexBuffer::IsLoaded() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	return RendererAPI::GetResourceLoader()->IsTokenCompleted(m_token);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::IndexBuffer::AwaitLoading() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	RendererAPI::GetResourceLoader()->WaitForToken(m_token);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::same_as<T, u16> || std::same_as<T, u32>
[[nodiscard]] TRAP::Scope<TRAP::Graphics::IndexBuffer> TRAP::Graphics::IndexBuffer::Init(const T* const indices, const u64 size,
															                             const TRAP::Graphics::UpdateFrequency updateFrequency)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	TRAP::Graphics::RendererAPI::IndexType indexType = TRAP::Graphics::RendererAPI::IndexType::UInt32;
	if(indices)
	{
		if constexpr(std::same_as<T, u16>)
			indexType = TRAP::Graphics::RendererAPI::IndexType::UInt16;
		else if constexpr(std::same_as<T, u32>)
			indexType = TRAP::Graphics::RendererAPI::IndexType::UInt32;
	}
	else
	{
		if(size / sizeof(u16) < std::numeric_limits<u16>::max())
			indexType = TRAP::Graphics::RendererAPI::IndexType::UInt16;
		else
			indexType = TRAP::Graphics::RendererAPI::IndexType::UInt32;
	}

	const RendererAPI::BufferDesc bufferDesc
	{
		.Size = size,
		.MemoryUsage = (updateFrequency == TRAP::Graphics::UpdateFrequency::Static) ? TRAP::Graphics::RendererAPI::ResourceMemoryUsage::GPUOnly :
																					  TRAP::Graphics::RendererAPI::ResourceMemoryUsage::CPUToGPU,
		.Flags = (updateFrequency != TRAP::Graphics::UpdateFrequency::Static) ? TRAP::Graphics::RendererAPI::BufferCreationFlags::PersistentMap :
																				TRAP::Graphics::RendererAPI::BufferCreationFlags::None,
		.ElementCount = size / sizeof(T),
		.StructStride = sizeof(T),
		.Descriptors = TRAP::Graphics::RendererAPI::DescriptorType::IndexBuffer
	};

	TRAP::Graphics::RendererAPI::BufferLoadDesc desc
	{
		.Data = indices,
		.Desc = bufferDesc
	};

	u64 syncToken = 0;
	TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &syncToken);

	return TRAP::Scope<TRAP::Graphics::IndexBuffer>(new TRAP::Graphics::IndexBuffer(indexType, syncToken, desc.Buffer));
}
