#include "TRAPPCH.h"
#include "Queue.h"

#include "Graphics/API/Vulkan/Objects/VulkanQueue.h"
#include "Graphics/API/RendererAPI.h"

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Queue> TRAP::Graphics::Queue::Create(const QueueDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanQueue>(desc);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Queue::Create(): Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Queue::DebugLog(const std::string_view msg)
{
	TP_DEBUG(TRAP::Log::RendererQueuePrefix, msg);
}
