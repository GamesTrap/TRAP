#include "TRAPPCH.h"
#include "DescriptorPool.h"

#include "Graphics/API/Vulkan/Objects/VulkanDescriptorPool.h"

[[nodiscard]] TRAP::Ref<TRAP::Graphics::DescriptorPool> TRAP::Graphics::DescriptorPool::Create(const uint32_t numDescriptorSets)
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

TRAP::Graphics::DescriptorPool::~DescriptorPool()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererDescriptorPoolPrefix, "Destroying DescriptorPool");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::DescriptorPool::DescriptorPool()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererDescriptorPoolPrefix, "Creating DescriptorPool");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}
