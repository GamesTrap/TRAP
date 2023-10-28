#include "TRAPPCH.h"
#include "IndexBuffer.h"

[[nodiscard]] TRAP::Scope<TRAP::Graphics::IndexBuffer> TRAP::Graphics::IndexBuffer::Create(const std::span<const u32> indices,
                                                                                           const UpdateFrequency updateFrequency)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	return Init<u32>(indices.data(), indices.size_bytes(), updateFrequency);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Graphics::IndexBuffer> TRAP::Graphics::IndexBuffer::Create(const std::span<const u16> indices,
                                                                                           const UpdateFrequency updateFrequency)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	return Init<u16>(indices.data(), indices.size_bytes(), updateFrequency);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Graphics::IndexBuffer> TRAP::Graphics::IndexBuffer::Create(const u64 size,
                                                                                           const UpdateFrequency updateFrequency)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	return Init<u16>(nullptr, size, updateFrequency); //u16 gets ignored
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] u32 TRAP::Graphics::IndexBuffer::GetCount() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return NumericCast<u32>(m_indexBuffer->GetSize() /
	                             ((m_indexType == RendererAPI::IndexType::UInt16) ? sizeof(u16) :
	                                                                                sizeof(u32)));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] u64 TRAP::Graphics::IndexBuffer::GetSize() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_indexBuffer->GetSize();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::UpdateFrequency TRAP::Graphics::IndexBuffer::GetUpdateFrequency() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return (m_indexBuffer->GetMemoryUsage() == RendererAPI::ResourceMemoryUsage::GPUOnly) ? UpdateFrequency::Static :
	                                                                                        UpdateFrequency::Dynamic;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::IndexBuffer::SetData(const std::span<const u16> indices, const u64 offset)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	SetDataInternal(indices, offset);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::IndexBuffer::SetData(const std::span<const u32> indices, const u64 offset)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	SetDataInternal(indices, offset);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::IndexBuffer::Use(const Window* const window) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	RendererAPI::GetRenderer()->BindIndexBuffer(m_indexBuffer, m_indexType, window);
}
#else
void TRAP::Graphics::IndexBuffer::Use() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	RendererAPI::GetRenderer()->BindIndexBuffer(m_indexBuffer, m_indexType);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::IndexBuffer::IsLoaded() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	return RendererAPI::GetResourceLoader()->IsTokenCompleted(&m_token);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::IndexBuffer::AwaitLoading() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	RendererAPI::GetResourceLoader()->WaitForToken(&m_token);
}