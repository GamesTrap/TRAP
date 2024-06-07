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
	public:
		/// @brief Constructor.
		/// @param numDescriptorSets Max number of descriptor sets that can be allocated from the pool.
		/// @param name Optional: Name for the descriptor pool.
		explicit VulkanDescriptorPool(u32 numDescriptorSets, [[maybe_unused]] std::string_view name = "");
		/// @brief Destructor.
		~VulkanDescriptorPool() override;

		/// @brief Copy constructor.
		consteval VulkanDescriptorPool(const VulkanDescriptorPool&) = delete;
		/// @brief Copy assignment operator.
		consteval VulkanDescriptorPool& operator=(const VulkanDescriptorPool&) = delete;
		/// @brief Move constructor.
		consteval VulkanDescriptorPool(VulkanDescriptorPool&&) = delete;
		/// @brief Move assignment operator.
		consteval VulkanDescriptorPool& operator=(VulkanDescriptorPool&&) = delete;

		/// @brief Reset the descriptor pool.
		/// @note This implicitly frees all descriptor sets allocated from the pool.
		void Reset() override;

		/// @brief Retrieve the current VkDescriptorPool handle.
		/// @return VkDescriptorPool handle.
		[[nodiscard]] constexpr VkDescriptorPool GetCurrentVkDescriptorPool() const noexcept;
		/// @brief Retrieve the descriptor pool sizes for each descriptor type.
		/// @return Descriptor pool sizes.
		[[nodiscard]] constexpr const std::vector<VkDescriptorPoolSize>& GetDescriptorPoolSizes() const noexcept;
		/// @brief Retrieve the count of used descriptor sets.
		/// @return Count of used descriptor sets.
		[[nodiscard]] constexpr u32 GetUsedDescriptorSetsCount() const noexcept;

		/// @brief Retrieve a new descriptor set from description.
		/// @param desc Descriptor set description.
		/// @return New descriptor set.
		[[nodiscard]] TRAP::Scope<DescriptorSet> RetrieveDescriptorSet(const RendererAPI::DescriptorSetDesc& desc) override;

		/// @brief Retrieve a new VkDescriptorSet with the given layout.
		/// @param layout Descriptor set layout.
		/// @return VkDescriptorSet handle.
		[[nodiscard]] VkDescriptorSet RetrieveVkDescriptorSet(VkDescriptorSetLayout layout);

	private:
		VkDescriptorPool m_currentPool = VK_NULL_HANDLE;
		std::vector<VkDescriptorPool> m_descriptorPools{};
		std::vector<VkDescriptorPoolSize> m_descriptorPoolSizes{};
		u32 m_usedDescriptorSetCount = 0;
		TracyLockable(std::mutex, m_mutex);

		TRAP::Ref<VulkanDevice> m_device = dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice();
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
