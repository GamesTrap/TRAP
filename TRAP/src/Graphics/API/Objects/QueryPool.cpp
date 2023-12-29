#include "TRAPPCH.h"
#include "QueryPool.h"

#include "Graphics/API/Vulkan/Objects/VulkanQueryPool.h"

[[nodiscard]] TRAP::Ref<TRAP::Graphics::QueryPool> TRAP::Graphics::QueryPool::Create(const RendererAPI::QueryPoolDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanQueryPool>(desc);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "QueryPool::Create(): Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::QueryPool::~QueryPool()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererQueryPoolPrefix, "Destroying QueryPool");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::QueryPool::QueryPool()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererQueryPoolPrefix, "Creating QueryPool");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}
