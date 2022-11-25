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
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="numDescriptorSets">Max number of descriptor sets that can be allocated from the pool.</param>
		explicit VulkanDescriptorPool(uint32_t numDescriptorSets);
		/// <summary>
		/// Destructor.
		/// </summary>
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

		/// <summary>
		/// Reset the descriptor pool.
		/// Note: This implicitly frees all descriptor sets allocated from the pool.
		/// </summary>
		void Reset() override;

		/// <summary>
		/// Retrieve the current VkDescriptorPool handle.
		/// </summary>
		/// <returns>VkDescriptorPool handle.</returns>
		VkDescriptorPool GetCurrentVkDescriptorPool() const;
		/// <summary>
		/// Retrieve the descriptor pool sizes for each descriptor type.
		/// </summary>
		/// <returns>Descriptor pool sizes.</returns>
		const std::vector<VkDescriptorPoolSize>& GetDescriptorPoolSizes() const;
		/// <summary>
		/// Retrieve the count of used descriptor sets.
		/// </summary>
		/// <returns>Count of used descriptor sets.</returns>
		uint32_t GetUsedDescriptorSetsCount() const;

		/// <summary>
		/// Retrieve a new descriptor set from description.
		/// </summary>
		/// <param name="desc">Descriptor set description.</param>
		/// <returns>New descriptor set.</returns>
		TRAP::Scope<DescriptorSet> RetrieveDescriptorSet(const RendererAPI::DescriptorSetDesc& desc) override;

		inline static constexpr uint32_t DescriptorTypeRangeSize = DESCRIPTOR_TYPE_RANGE_SIZE - 1;
	private:
		/// <summary>
		/// Retrieve a new VkDescriptorSet with the given layout.
		/// </summary>
		/// <param name="layout">Descriptor set layout.</param>
		/// <returns>VkDescriptorSet handle.</returns>
		VkDescriptorSet RetrieveVkDescriptorSet(VkDescriptorSetLayout layout);

		VkDescriptorPool m_currentPool;
		std::vector<VkDescriptorPool> m_descriptorPools;
		std::vector<VkDescriptorPoolSize> m_descriptorPoolSizes;
		uint32_t m_usedDescriptorSetCount;
		TracyLockable(std::mutex, m_mutex);

		TRAP::Ref<VulkanDevice> m_device;

		static std::array<VkDescriptorPoolSize, DESCRIPTOR_TYPE_RANGE_SIZE> s_descriptorPoolSizes;

		friend VulkanDescriptorSet;
		friend VulkanRootSignature;
	};
}

#endif /*TRAP_VULKANDESCRIPTORPOOL_H*/