#include "TRAPPCH.h"
#include "Queue.h"

#include "Graphics/API/Vulkan/Objects/VulkanQueue.h"

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Queue> TRAP::Graphics::Queue::Create(const RendererAPI::QueueDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

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

TRAP::Graphics::Queue::~Queue()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererQueuePrefix, "Destroying Queue");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Queue::Queue(const RendererAPI::QueueType queueType)
	: m_type(queueType)
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererQueuePrefix, "Creating Queue");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}
