#include "TRAPPCH.h"
#include "DescriptorPool.h"

#include "Graphics/API/Vulkan/Objects/VulkanDescriptorPool.h"

[[nodiscard]] TRAP::Ref<TRAP::Graphics::DescriptorPool> TRAP::Graphics::DescriptorPool::Create(const u32 numDescriptorSets)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

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

TRAP::Graphics::DescriptorPool::DescriptorPool(const u32 numDescriptorSets)
	: m_numDescriptorSets(numDescriptorSets)
{
	TRAP_ASSERT(numDescriptorSets > 0, "DescriptorPool::DescriptorPool(): m_numDescriptorSets is 0");

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererDescriptorPoolPrefix, "Creating DescriptorPool");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}
