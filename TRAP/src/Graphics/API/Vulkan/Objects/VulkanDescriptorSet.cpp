#include "TRAPPCH.h"
#include "VulkanDescriptorSet.h"

TRAP::Graphics::API::VulkanDescriptorSet::VulkanDescriptorSet(const TRAP::Ref<VulkanDescriptorPool>& descriptorPool)
	: m_descriptorSet(VK_NULL_HANDLE)
{
	TRAP_ASSERT(descriptorPool, "descriptorPool is nullptr");

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Creating DescriptorSet");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanDescriptorSet::~VulkanDescriptorSet()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Destroying DescriptorSet");
#endif	
}

//-------------------------------------------------------------------------------------------------------------------//

VkDescriptorSet& TRAP::Graphics::API::VulkanDescriptorSet::GetVkDescriptorSet()
{
	return m_descriptorSet;
}