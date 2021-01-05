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
{}

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
		break;
		
	default:
		break;
	}
}