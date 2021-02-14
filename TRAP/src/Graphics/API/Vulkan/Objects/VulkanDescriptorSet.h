#ifndef _TRAP_VULKANDESCRIPTORSET_H_
#define _TRAP_VULKANDESCRIPTORSET_H_

#include "VulkanDescriptorPool.h"
#include "Graphics/API/Objects/DescriptorSet.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

namespace TRAP::Graphics::API
{	
	class VulkanDescriptorSet final : public DescriptorSet
	{
	public:
		~VulkanDescriptorSet();

		const std::vector<VkDescriptorSet>& GetVkDescriptorSets() const;
		TRAP::Ref<VulkanRootSignature> GetRootSignature() const;
		RendererAPI::DescriptorUpdateFrequency GetUpdateFrequency() const;
		uint8_t GetDynamicOffsetCount() const;
		std::vector<VulkanRenderer::SizeOffset> GetDynamicSizeOffsets() const;
		uint32_t GetMaxSets() const;

		void Update(uint32_t index, const std::vector<RendererAPI::DescriptorData>& params) override;
		
	private:
		friend VulkanDescriptorPool;
		
		VulkanDescriptorSet(TRAP::Ref<VulkanDevice> device,
							std::vector<VkDescriptorSet> vkDescriptorSetHandles,
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

		TRAP::Ref<VulkanDevice> m_device;
	};
}

#endif /*_TRAP_VULKANDESCRIPTORSET_H_*/