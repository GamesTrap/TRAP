#include "TRAPPCH.h"
#include "DescriptorPool.h"

#include "Graphics/API/Vulkan/Objects/VulkanDescriptorPool.h"

TRAP::Ref<TRAP::Graphics::DescriptorPool> TRAP::Graphics::DescriptorPool::Create(const uint32_t numDescriptorSets)
{
	ZoneScoped;

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
	ZoneScoped;

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererDescriptorPoolPrefix, "Creating DescriptorPool");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::DescriptorPool::~DescriptorPool()
{
	ZoneScoped;

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererDescriptorPoolPrefix, "Destroying DescriptorPool");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::DescriptorPool::GetDescriptorSetsNum() const
{
	ZoneScoped;

	return m_numDescriptorSets;
}