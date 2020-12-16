#include "TRAPPCH.h"
#include "VulkanDescriptorSet.h"

TRAP::Graphics::API::VulkanDescriptorSet::~VulkanDescriptorSet()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Destroying DescriptorSet");
#endif	
}

TRAP::Graphics::API::VulkanDescriptorSet::VulkanDescriptorSet(std::vector<VkDescriptorSet> vkDescriptorSetHandles,
	TRAP::Ref<VulkanRootSignature> rootSignature,
	std::vector<std::vector<VulkanRenderer::DescriptorUpdateData>> updateData,
	std::vector<VulkanRenderer::SizeOffset> dynamicSizeOffsets,
	const uint32_t maxSets,
	const uint8_t dynamicOffsetCount,
	const RendererAPI::DescriptorUpdateFrequency updateFrequency)
		: m_vkDescriptorSetHandles(std::move(vkDescriptorSetHandles)),
		  m_rootSignature(std::move(rootSignature)),
		  m_updateData(std::move(updateData)),
		  m_dynamicSizeOffsets(std::move(dynamicSizeOffsets)),
		  m_maxSets(maxSets),
		  m_dynamicOffsetCount(dynamicOffsetCount),
		  m_updateFrequency(updateFrequency)
{
	TRAP_ASSERT(rootSignature, "rootSignature is nullptr");
	
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Creating DescriptorSet");
#endif
}