#include "TRAPPCH.h"
#include "DescriptorPool.h"

#include "Graphics/API/Vulkan/Objects/VulkanDescriptorPool.h"

TRAP::Ref<TRAP::Graphics::DescriptorPool> TRAP::Graphics::DescriptorPool::Create(const uint32_t numDescriptorSets)
{
	ZoneScopedC(tracy::Color::Red);

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanDescriptorPool>(numDescriptorSets);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::DescriptorPool::DescriptorPool()
	: m_numDescriptorSets()
{
	ZoneScopedC(tracy::Color::Red);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererDescriptorPoolPrefix, "Creating DescriptorPool");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::DescriptorPool::~DescriptorPool()
{
	ZoneScopedC(tracy::Color::Red);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererDescriptorPoolPrefix, "Destroying DescriptorPool");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::DescriptorPool::GetDescriptorSetsNum() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_numDescriptorSets;
}