#include "TRAPPCH.h"
#include "CommandBuffer.h"

TRAP::Graphics::CommandBuffer::CommandBuffer()
	: m_queue(nullptr)
{
	ZoneScopedC(tracy::Color::Red);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererCommandBufferPrefix, "Creating CommandBuffer");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::CommandBuffer::~CommandBuffer()
{
	ZoneScopedC(tracy::Color::Red);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererCommandBufferPrefix, "Destroying CommandBuffer");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Queue> TRAP::Graphics::CommandBuffer::GetQueue() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_queue;
}