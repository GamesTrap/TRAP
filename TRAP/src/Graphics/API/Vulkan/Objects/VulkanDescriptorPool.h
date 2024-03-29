#ifndef TRAP_VULKANDESCRIPTORPOOL_H
#define TRAP_VULKANDESCRIPTORPOOL_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/DescriptorPool.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

namespace TRAP::Graphics::API
{
	class VulkanRootSignature;
	class VulkanDescriptorSet;
	class VulkanDevice;

	class VulkanDescriptorPool final : public DescriptorPool
	{
	private:
		inline static constexpr u32 DESCRIPTOR_TYPE_RANGE_SIZE = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT + 2;

	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="numDescriptorSets">Max number of descriptor sets that can be allocated from the pool.</param>
		explicit VulkanDescriptorPool(u32 numDescriptorSets);
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
		[[nodiscard]] constexpr VkDescriptorPool GetCurrentVkDescriptorPool() const noexcept;
		/// <summary>
		/// Retrieve the descriptor pool sizes for each descriptor type.
		/// </summary>
		/// <returns>Descriptor pool sizes.</returns>
		[[nodiscard]] constexpr const std::vector<VkDescriptorPoolSize>& GetDescriptorPoolSizes() const noexcept;
		/// <summary>
		/// Retrieve the count of used descriptor sets.
		/// </summary>
		/// <returns>Count of used descriptor sets.</returns>
		[[nodiscard]] constexpr u32 GetUsedDescriptorSetsCount() const noexcept;

		/// <summary>
		/// Retrieve a new descriptor set from description.
		/// </summary>
		/// <param name="desc">Descriptor set description.</param>
		/// <returns>New descriptor set.</returns>
		[[nodiscard]] TRAP::Scope<DescriptorSet> RetrieveDescriptorSet(const RendererAPI::DescriptorSetDesc& desc) override;

		inline static constexpr u32 DescriptorTypeRangeSize = DESCRIPTOR_TYPE_RANGE_SIZE - 1;
	private:
		/// <summary>
		/// Retrieve a new VkDescriptorSet with the given layout.
		/// </summary>
		/// <param name="layout">Descriptor set layout.</param>
		/// <returns>VkDescriptorSet handle.</returns>
		[[nodiscard]] VkDescriptorSet RetrieveVkDescriptorSet(VkDescriptorSetLayout layout);

		VkDescriptorPool m_currentPool = VK_NULL_HANDLE;
		std::vector<VkDescriptorPool> m_descriptorPools{};
		std::vector<VkDescriptorPoolSize> m_descriptorPoolSizes;
		u32 m_usedDescriptorSetCount = 0;
		TracyLockable(std::mutex, m_mutex);

		TRAP::Ref<VulkanDevice> m_device = dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice();

		inline constinit static std::array<VkDescriptorPoolSize, DESCRIPTOR_TYPE_RANGE_SIZE> s_descriptorPoolSizes
		{
			{
				{VK_DESCRIPTOR_TYPE_SAMPLER, 1024},
				{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1},
				{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 8192},
				{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1024},
				{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1024},
				{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1024},
				{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 8192},
				{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1024},
				{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1024},
				{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1},
				{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1}
			}
		};

		friend VulkanDescriptorSet;
		friend VulkanRootSignature;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkDescriptorPool TRAP::Graphics::API::VulkanDescriptorPool::GetCurrentVkDescriptorPool() const noexcept
{
	return m_currentPool;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<VkDescriptorPoolSize>& TRAP::Graphics::API::VulkanDescriptorPool::GetDescriptorPoolSizes() const noexcept
{
	return m_descriptorPoolSizes;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::API::VulkanDescriptorPool::GetUsedDescriptorSetsCount() const noexcept
{
	return m_usedDescriptorSetCount;
}

#endif /*TRAP_VULKANDESCRIPTORPOOL_H*/