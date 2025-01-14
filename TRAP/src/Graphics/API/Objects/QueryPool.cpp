#include "TRAPPCH.h"
#include "QueryPool.h"

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/Objects/VulkanQueryPool.h"

[[nodiscard]] TRAP::Ref<TRAP::Graphics::QueryPool> TRAP::Graphics::QueryPool::Create(const QueryPoolDesc& desc)
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

void TRAP::Graphics::QueryPool::DebugLog(const std::string_view msg)
{
	TP_DEBUG(TRAP::Log::RendererQueryPoolPrefix, msg);
}
