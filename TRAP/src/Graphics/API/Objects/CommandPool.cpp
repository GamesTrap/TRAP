#include "TRAPPCH.h"
#include "CommandPool.h"

#include "Graphics/API/Objects/CommandBuffer.h"
#include "Graphics/API/Vulkan/Objects/VulkanCommandPool.h"

TRAP::Graphics::CommandPool::CommandPool()
{
	ZoneScopedC(tracy::Color::Red);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererCommandPoolPrefix, "Creating CommandPool");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::CommandPool::~CommandPool()
{
	ZoneScopedC(tracy::Color::Red);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererCommandPoolPrefix, "Destroying CommandPool");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::CommandPool> TRAP::Graphics::CommandPool::Create(const RendererAPI::CommandPoolDesc& desc)
{
	ZoneScopedC(tracy::Color::Red);

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanCommandPool>(desc);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}