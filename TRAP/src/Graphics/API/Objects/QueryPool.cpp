#include "TRAPPCH.h"
#include "QueryPool.h"

#include "Graphics/API/Vulkan/Objects/VulkanQueryPool.h"

TRAP::Graphics::QueryPool::QueryPool()
{
	ZoneScopedC(tracy::Color::Red);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererQueryPoolPrefix, "Creating QueryPool");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::QueryPool::~QueryPool()
{
	ZoneScopedC(tracy::Color::Red);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererQueryPoolPrefix, "Destroying QueryPool");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::QueryPool> TRAP::Graphics::QueryPool::Create(const RendererAPI::QueryPoolDesc& desc)
{
	ZoneScopedC(tracy::Color::Red);

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanQueryPool>(desc);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}