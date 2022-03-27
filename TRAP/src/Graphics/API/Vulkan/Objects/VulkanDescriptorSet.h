#ifndef TRAP_VULKANDESCRIPTORSET_H
#define TRAP_VULKANDESCRIPTORSET_H

#include "VulkanDescriptorPool.h"
#include "Graphics/API/Objects/DescriptorSet.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

namespace TRAP::Graphics::API
{
	class VulkanDescriptorSet final : public DescriptorSet
	{
	public:
		VulkanDescriptorSet(TRAP::Ref<VulkanDevice> device, std::vector<VkDescriptorSet> vkDescriptorSetHandles,
		                    TRAP::Ref<VulkanRootSignature> rootSignature,
		                    std::vector<std::vector<union VulkanRenderer::DescriptorUpdateData>> updateData,
		                    uint32_t maxSets, uint8_t dynamicOffsetCount,
							uint32_t set);

		~VulkanDescriptorSet() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanDescriptorSet(const VulkanDescriptorSet&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanDescriptorSet& operator=(const VulkanDescriptorSet&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanDescriptorSet(VulkanDescriptorSet&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanDescriptorSet& operator=(VulkanDescriptorSet&&) = default;

		const std::vector<VkDescriptorSet>& GetVkDescriptorSets() const;
		TRAP::Ref<VulkanRootSignature> GetRootSignature() const;
		RendererAPI::DescriptorUpdateFrequency GetUpdateFrequency() const;
		uint8_t GetDynamicOffsetCount() const;
		std::vector<VulkanRenderer::SizeOffset> GetDynamicSizeOffsets() const;
		uint32_t GetMaxSets() const;
		uint32_t GetSet() const;

		void Update(uint32_t index, const std::vector<RendererAPI::DescriptorData>& params) override;

	private:
		std::vector<VkDescriptorSet> m_vkDescriptorSetHandles;
		TRAP::Ref<VulkanRootSignature> m_rootSignature;
		//Values passed to vkUpdateDescriptorSetWithTemplate.
		//Initialized to default descriptor values.
		std::vector<std::vector<union VulkanRenderer::DescriptorUpdateData>> m_updateData;
		std::vector<VulkanRenderer::SizeOffset> m_dynamicSizeOffsets;
		uint32_t m_maxSets;
		uint8_t m_dynamicOffsetCount;
		uint32_t m_set;

		TRAP::Ref<VulkanDevice> m_device;
	};
}

#endif /*TRAP_VULKANDESCRIPTORSET_H*/