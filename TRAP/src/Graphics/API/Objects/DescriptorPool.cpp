#include "TRAPPCH.h"
#include "DescriptorPool.h"

#include "Graphics/API/Vulkan/Objects/VulkanDescriptorPool.h"
#include "Graphics/API/RendererAPI.h"

[[nodiscard]] TRAP::Ref<TRAP::Graphics::DescriptorPool> TRAP::Graphics::DescriptorPool::Create(const u32 numDescriptorSets,
                                                                                               [[maybe_unused]] const std::string& name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	TRAP_ASSERT(numDescriptorSets, "DescriptorPool::Create(): DescriptorSets count can not be 0!");

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanDescriptorPool>(numDescriptorSets, name);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "DescriptorPool::Create(): Unknown RenderAPI");
		return nullptr;
	}
}
