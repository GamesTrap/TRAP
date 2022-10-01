#include "TRAPPCH.h"
#include "Queue.h"

#include "Graphics/API/Vulkan/Objects/VulkanQueue.h"

TRAP::Graphics::Queue::Queue()
{
	ZoneScoped;

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererQueuePrefix, "Creating Queue");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Queue::~Queue()
{
	ZoneScoped;

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererQueuePrefix, "Destroying Queue");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Queue> TRAP::Graphics::Queue::Create(const RendererAPI::QueueDesc& desc)
{
	ZoneScoped;

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanQueue>(desc);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}