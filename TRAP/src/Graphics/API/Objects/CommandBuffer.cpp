#include "TRAPPCH.h"
#include "CommandBuffer.h"

TRAP::Graphics::CommandBuffer::CommandBuffer()
	: m_queue(nullptr)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererCommandBufferPrefix, "Creating CommandBuffer");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::CommandBuffer::~CommandBuffer()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererCommandBufferPrefix, "Destroying CommandBuffer");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Queue> TRAP::Graphics::CommandBuffer::GetQueue() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_queue;
}