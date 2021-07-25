#include "TRAPPCH.h"
#include "CommandPool.h"

#include "Graphics/API/Vulkan/Objects/VulkanCommandPool.h"

TRAP::Ref<TRAP::Graphics::CommandPool> TRAP::Graphics::CommandPool::Create(const RendererAPI::CommandPoolDesc& desc)
{
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