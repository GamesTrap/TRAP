#include "TRAPPCH.h"
#include "CommandPool.h"

#include "Graphics/API/Vulkan/Objects/VulkanCommandPool.h"
#include "Graphics/API/Objects/CommandBuffer.h"

TRAP::Graphics::CommandPool::CommandPool(TRAP::Ref<TRAP::Graphics::Queue> queue)
	: m_queue(std::move(queue))
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	TRAP_ASSERT(m_queue != nullptr, "CommandPool::CommandPool(): m_queue is nullptr!");

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererCommandPoolPrefix, "Creating CommandPool");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::CommandPool::~CommandPool()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererCommandPoolPrefix, "Destroying CommandPool");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::CommandPool> TRAP::Graphics::CommandPool::Create(const RendererAPI::CommandPoolDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanCommandPool>(desc);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "CommandPool::Create(): Unknown RenderAPI");
		return nullptr;
	}
}
