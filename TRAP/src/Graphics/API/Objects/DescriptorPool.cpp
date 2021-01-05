#include "TRAPPCH.h"
#include "DescriptorPool.h"

#include "Graphics/API/Vulkan/Objects/VulkanDescriptorPool.h"

TRAP::Ref<TRAP::Graphics::DescriptorPool> TRAP::Graphics::DescriptorPool::Create(uint32_t numDescriptorSets)
{
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
{}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::DescriptorPool::GetDescriptorSetsNum() const
{
	return m_numDescriptorSets;
}