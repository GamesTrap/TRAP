#include "TRAPPCH.h"
#include "Semaphore.h"

#include "Graphics/API/Vulkan/Objects/VulkanSemaphore.h"
#include "Graphics/API/RendererAPI.h"

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Semaphore> TRAP::Graphics::Semaphore::Create(const SemaphoreType semaphoreType,
                                                                                     const std::string& name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanSemaphore>(semaphoreType, name);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Semaphore::Create(): Unknown RenderAPI");
		return nullptr;
	}
}
