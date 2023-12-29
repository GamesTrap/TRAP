#include "TRAPPCH.h"
#include "CommandBuffer.h"

TRAP::Graphics::CommandBuffer::CommandBuffer(TRAP::Ref<Queue> queue)
	: m_queue(std::move(queue))
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	TRAP_ASSERT(m_queue != nullptr, "CommandBuffer::CommandBuffer(): queue is nullptr!");

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererCommandBufferPrefix, "Creating CommandBuffer");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::CommandBuffer::~CommandBuffer()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererCommandBufferPrefix, "Destroying CommandBuffer");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Queue> TRAP::Graphics::CommandBuffer::GetQueue() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return m_queue;
}