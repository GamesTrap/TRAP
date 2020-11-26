#ifndef _TRAP_VULKANDESCRIPTORPOOL_H_
#define _TRAP_VULKANDESCRIPTORPOOL_H_

namespace TRAP::Graphics::API
{
	class VulkanDevice;

	class VulkanDescriptorPool
	{
	private:
		static constexpr uint32_t DESCRIPTOR_TYPE_RANGE_SIZE = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT + 2;
		
	public:
		VulkanDescriptorPool(TRAP::Ref<VulkanDevice> device, uint32_t numDescriptorSets);
		~VulkanDescriptorPool();

		void Reset();
		
		VkDescriptorPool& GetVkDescriptorPool();
		const std::vector<VkDescriptorPoolSize>& GetDescriptorPoolSizes() const;
		uint32_t GetDescriptorSetsNum() const;
		uint32_t GetUsedDescriptorSetsCount() const;
		
		static constexpr uint32_t DescriptorTypeRangeSize = DESCRIPTOR_TYPE_RANGE_SIZE - 1;
	private:
		VkDescriptorPool m_descriptorPool;
		std::vector<VkDescriptorPoolSize> m_descriptorPoolSizes;
		uint32_t m_numDescriptorSets;
		uint32_t m_usedDescriptorSetCount;
		std::mutex m_Mutex;
		
		TRAP::Ref<VulkanDevice> m_device;

		static std::array<VkDescriptorPoolSize, DESCRIPTOR_TYPE_RANGE_SIZE> s_descriptorPoolSizes;
	};
}

#endif /*_TRAP_VULKANDESCRIPTORPOOL_H_*/