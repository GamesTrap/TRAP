#include "TRAPPCH.h"
#include "Fence.h"

#include "Graphics/API/Vulkan/Objects/VulkanFence.h"

TRAP::Ref<TRAP::Graphics::Fence> TRAP::Graphics::Fence::Create()
{
	ZoneScopedC(tracy::Color::Red);

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
	ZoneScopedC(tracy::Color::Red);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererFencePrefix, "Creating Fence");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Fence::~Fence()
{
	ZoneScopedC(tracy::Color::Red);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererFencePrefix, "Destroying Fence");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::Fence::IsSubmitted() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_submitted;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Fence::WaitForFences(std::vector<Fence>& fences)
{
	ZoneScopedC(tracy::Color::Red);

	if(fences.empty())
		return;

	for(Fence& f : fences)
		f.Wait();
}