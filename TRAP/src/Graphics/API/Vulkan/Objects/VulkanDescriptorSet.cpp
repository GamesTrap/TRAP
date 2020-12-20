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
	TRAP_ASSERT(m_rootSignature, "rootSignature is nullptr");
	
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Creating DescriptorSet");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<VkDescriptorSet>& TRAP::Graphics::API::VulkanDescriptorSet::GetVkDescriptorSets() const
{
	return m_vkDescriptorSetHandles;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::API::VulkanRootSignature> TRAP::Graphics::API::VulkanDescriptorSet::GetRootSignature() const
{
	return m_rootSignature;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::DescriptorUpdateFrequency TRAP::Graphics::API::VulkanDescriptorSet::GetUpdateFrequency() const
{
	return m_updateFrequency;
}

//-------------------------------------------------------------------------------------------------------------------//

uint8_t TRAP::Graphics::API::VulkanDescriptorSet::GetDynamicOffsetCount() const
{
	return m_dynamicOffsetCount;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Graphics::API::VulkanRenderer::SizeOffset> TRAP::Graphics::API::VulkanDescriptorSet::GetDynamicSizeOffsets() const
{
	return m_dynamicSizeOffsets;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanDescriptorSet::GetMaxSets() const
{
	return m_maxSets;
}
