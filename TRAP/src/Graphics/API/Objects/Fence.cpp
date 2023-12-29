#include "TRAPPCH.h"
#include "Fence.h"

#include "Graphics/API/Vulkan/Objects/VulkanFence.h"

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Fence> TRAP::Graphics::Fence::Create(const bool signalled)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanFence>(signalled);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Fence::Create(): Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Fence::~Fence()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererFencePrefix, "Destroying Fence");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Fence::Fence()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererFencePrefix, "Creating Fence");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Fence::WaitForFences(std::vector<Fence>& fences)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	TRAP_ASSERT(!fences.empty(), "Fence::WaitForFences(): Fences count can not be 0!");

	for(Fence& f : fences)
		f.Wait();
}