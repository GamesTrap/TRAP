#ifndef _TRAP_VULKANDESCRIPTORSET_H_
#define _TRAP_VULKANDESCRIPTORSET_H_

#include "VulkanDescriptorPool.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

namespace TRAP::Graphics::API
{	
	class VulkanDescriptorSet
	{
	public:
		~VulkanDescriptorSet();
		
	private:
		friend VulkanDescriptorPool;
		
		VulkanDescriptorSet(std::vector<VkDescriptorSet> vkDescriptorSetHandles,
		                    TRAP::Ref<VulkanRootSignature> rootSignature,
		                    std::vector<std::vector<union VulkanRenderer::DescriptorUpdateData>> updateData,
		                    std::vector<VulkanRenderer::SizeOffset> dynamicSizeOffsets,
							uint32_t maxSets,
							uint8_t dynamicOffsetCount,
							RendererAPI::DescriptorUpdateFrequency updateFrequency);

		std::vector<VkDescriptorSet> m_vkDescriptorSetHandles;
		TRAP::Ref<VulkanRootSignature> m_rootSignature;
		//Values passed to vkUpdateDescriptorSetWithTemplate.
		//Initialized to default descriptor values.
		std::vector<std::vector<union VulkanRenderer::DescriptorUpdateData>> m_updateData;
		std::vector<VulkanRenderer::SizeOffset> m_dynamicSizeOffsets;
		uint32_t m_maxSets;
		uint8_t m_dynamicOffsetCount;
		RendererAPI::DescriptorUpdateFrequency m_updateFrequency;
		
	};
}

#endif /*_TRAP_VULKANDESCRIPTORSET_H_*/