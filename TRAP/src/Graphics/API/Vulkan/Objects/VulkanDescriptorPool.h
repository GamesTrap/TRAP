#ifndef _TRAP_VULKANDESCRIPTORPOOL_H_
#define _TRAP_VULKANDESCRIPTORPOOL_H_

namespace TRAP::Graphics::API
{
	class VulkanDescriptorSet;
	class VulkanDevice;

	class VulkanDescriptorPool
	{
	private:
		static constexpr uint32_t DESCRIPTOR_TYPE_RANGE_SIZE = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT + 2;
		
	public:
		VulkanDescriptorPool(TRAP::Ref<VulkanDevice> device, uint32_t numDescriptorSets);
		~VulkanDescriptorPool();

		void Reset();
		
		VkDescriptorPool& GetCurrentVkDescriptorPool();
		const std::vector<VkDescriptorPoolSize>& GetDescriptorPoolSizes() const;
		uint32_t GetDescriptorSetsNum() const;
		uint32_t GetUsedDescriptorSetsCount() const;

		VulkanDescriptorSet RetrieveDescriptorSet();

		static constexpr uint32_t DescriptorTypeRangeSize = DESCRIPTOR_TYPE_RANGE_SIZE - 1;
	private:
		VkDescriptorSet RetrieveVkDescriptorSet(VkDescriptorSetLayout layout);
		
		VkDescriptorPool m_currentPool;
		std::vector<VkDescriptorPool> m_descriptorPools;
		std::vector<VkDescriptorPoolSize> m_descriptorPoolSizes;
		uint32_t m_numDescriptorSets;
		uint32_t m_usedDescriptorSetCount;
		std::mutex m_mutex;
		
		TRAP::Ref<VulkanDevice> m_device;

		std::vector<TRAP::Scope<VulkanDescriptorSet>> m_descriptorSets;

		static std::array<VkDescriptorPoolSize, DESCRIPTOR_TYPE_RANGE_SIZE> s_descriptorPoolSizes;

		friend VulkanDescriptorSet;
	};
}

#endif /*_TRAP_VULKANDESCRIPTORPOOL_H_*/