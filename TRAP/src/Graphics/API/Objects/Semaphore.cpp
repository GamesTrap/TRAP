#include "TRAPPCH.h"
#include "Semaphore.h"

#include "Graphics/API/Vulkan/Objects/VulkanSemaphore.h"

TRAP::Ref<TRAP::Graphics::Semaphore> TRAP::Graphics::Semaphore::Create()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanSemaphore>();

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Semaphore::Create(): Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Semaphore::Semaphore()
	: m_signaled(false)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererSemaphorePrefix, "Creating Semaphore");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Semaphore::~Semaphore()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererSemaphorePrefix, "Destroying Semaphore");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::Semaphore::IsSignaled() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_signaled;
}