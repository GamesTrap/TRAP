#include "TRAPPCH.h"
#include "DescriptorPool.h"

#include "Graphics/API/Vulkan/Objects/VulkanDescriptorPool.h"

TRAP::Ref<TRAP::Graphics::DescriptorPool> TRAP::Graphics::DescriptorPool::Create(const uint32_t numDescriptorSets)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(numDescriptorSets, "DescriptorPool::Create(): DescriptorSets count can not be 0!");

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanDescriptorPool>(numDescriptorSets);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "DescriptorPool::Create(): Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::DescriptorPool::DescriptorPool()
	: m_numDescriptorSets()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererDescriptorPoolPrefix, "Creating DescriptorPool");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::DescriptorPool::~DescriptorPool()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererDescriptorPoolPrefix, "Destroying DescriptorPool");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::DescriptorPool::GetDescriptorSetsNum() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_numDescriptorSets;
}