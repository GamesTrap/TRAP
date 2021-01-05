#include "TRAPPCH.h"
#include "Semaphore.h"

#include "Graphics/API/Vulkan/Objects/VulkanSemaphore.h"

TRAP::Ref<TRAP::Graphics::Semaphore> TRAP::Graphics::Semaphore::Create()
{
	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanSemaphore>();
		
	case RenderAPI::NONE:
		return nullptr;
		
	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Semaphore::Semaphore()
	: m_signaled(false)
{}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::Semaphore::IsSignaled() const
{
	return m_signaled;
}