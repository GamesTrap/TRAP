#include "TRAPPCH.h"
#include "Fence.h"

#include "Graphics/API/Vulkan/Objects/VulkanFence.h"

TRAP::Ref<TRAP::Graphics::Fence> TRAP::Graphics::Fence::Create()
{
	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanFence>();

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Fence::Fence()
	: m_submitted(false)
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererFencePrefix, "Creating Fence");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Fence::~Fence()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererFencePrefix, "Destroying Fence");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::Fence::IsSubmitted() const
{
	return m_submitted;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Fence::WaitForFence(std::vector<Fence>& fences)
{
	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		API::VulkanFence::WaitForFence(fences);
		break;

	case RenderAPI::NONE:
	default:
		break;
	}
}