#include "TRAPPCH.h"
#include "VulkanDescriptorSet.h"

TRAP::Graphics::API::VulkanDescriptorSet::VulkanDescriptorSet()
{
	//TODO
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