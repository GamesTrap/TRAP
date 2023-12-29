#include "TRAPPCH.h"
#include "Semaphore.h"

#include "Graphics/API/Vulkan/Objects/VulkanSemaphore.h"

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Semaphore> TRAP::Graphics::Semaphore::Create()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

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

TRAP::Graphics::Semaphore::~Semaphore()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererSemaphorePrefix, "Destroying Semaphore");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Semaphore::Semaphore()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererSemaphorePrefix, "Creating Semaphore");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}
