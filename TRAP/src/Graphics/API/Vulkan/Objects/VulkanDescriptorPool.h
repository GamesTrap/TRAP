#ifndef TRAP_VULKANDESCRIPTORPOOL_H
#define TRAP_VULKANDESCRIPTORPOOL_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/DescriptorPool.h"

namespace TRAP::Graphics::API
{
	class VulkanRootSignature;
	class VulkanDescriptorSet;
	class VulkanDevice;

	class VulkanDescriptorPool final : public DescriptorPool
	{
	private:
		inline static constexpr uint32_t DESCRIPTOR_TYPE_RANGE_SIZE = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT + 2;

	public:
		explicit VulkanDescriptorPool(uint32_t numDescriptorSets);
		~VulkanDescriptorPool() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanDescriptorPool(const VulkanDescriptorPool&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanDescriptorPool& operator=(const VulkanDescriptorPool&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanDescriptorPool(VulkanDescriptorPool&&) = delete;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanDescriptorPool& operator=(VulkanDescriptorPool&&) = delete;

		void Reset() override;

		VkDescriptorPool GetCurrentVkDescriptorPool() const;
		const std::vector<VkDescriptorPoolSize>& GetDescriptorPoolSizes() const;
		uint32_t GetUsedDescriptorSetsCount() const;

		DescriptorSet* RetrieveDescriptorSet(const RendererAPI::DescriptorSetDesc& desc) override;

		inline static constexpr uint32_t DescriptorTypeRangeSize = DESCRIPTOR_TYPE_RANGE_SIZE - 1;
	private:
		VkDescriptorSet RetrieveVkDescriptorSet(VkDescriptorSetLayout layout);

		VkDescriptorPool m_currentPool;
		std::vector<VkDescriptorPool> m_descriptorPools;
		std::vector<VkDescriptorPoolSize> m_descriptorPoolSizes;
		uint32_t m_usedDescriptorSetCount;
		std::mutex m_mutex;

		TRAP::Ref<VulkanDevice> m_device;

		std::vector<TRAP::Scope<VulkanDescriptorSet>> m_descriptorSets;

		static std::array<VkDescriptorPoolSize, DESCRIPTOR_TYPE_RANGE_SIZE> s_descriptorPoolSizes;

		friend VulkanDescriptorSet;
		friend VulkanRootSignature;
	};
}

#endif /*TRAP_VULKANDESCRIPTORPOOL_H*/