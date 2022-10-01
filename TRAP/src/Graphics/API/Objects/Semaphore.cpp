#include "TRAPPCH.h"
#include "Semaphore.h"

#include "Graphics/API/Vulkan/Objects/VulkanSemaphore.h"

TRAP::Ref<TRAP::Graphics::Semaphore> TRAP::Graphics::Semaphore::Create()
{
	ZoneScopedC(tracy::Color::Red);

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanSemaphore>();

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Semaphore::Semaphore()
	: m_signaled(false)
{
	ZoneScopedC(tracy::Color::Red);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererSemaphorePrefix, "Creating Semaphore");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Semaphore::~Semaphore()
{
	ZoneScopedC(tracy::Color::Red);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererSemaphorePrefix, "Destroying Semaphore");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::Semaphore::IsSignaled() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_signaled;
}