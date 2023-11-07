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
		/// @brief Constructor.
		/// @param device Vulkan device.
		/// @param vkDescriptorSetHandles Vulkan descriptor set handles.
		/// @param rootSignature Root signature.
		/// @param updateData Update template data.
		/// @param maxSets Max number of sets for the descriptor set.
		/// @param dynamicOffsetCount Number of dynamic offsets.
		/// @param set Update frequency.
		VulkanDescriptorSet(TRAP::Ref<VulkanDevice> device, std::vector<VkDescriptorSet> vkDescriptorSetHandles,
		                    TRAP::Ref<VulkanRootSignature> rootSignature,
		                    std::vector<std::vector<VulkanRenderer::DescriptorUpdateData>> updateData,
		                    u32 maxSets, u8 dynamicOffsetCount,
							u32 set);

		/// @brief Copy constructor.
		VulkanDescriptorSet(const VulkanDescriptorSet&) noexcept = default;
		/// @brief Copy assignment operator.
		VulkanDescriptorSet& operator=(const VulkanDescriptorSet&) noexcept = default;
		/// @brief Move constructor.
		VulkanDescriptorSet(VulkanDescriptorSet&&) noexcept = default;
		/// @brief Move assignment operator.
		VulkanDescriptorSet& operator=(VulkanDescriptorSet&&) noexcept = default;

		/// @brief Destructor.
		~VulkanDescriptorSet() override = default;

		/// @brief Retrieve the VkDescriptorSet handles.
		/// @return VkDescriptorSet handles.
		[[nodiscard]] constexpr const std::vector<VkDescriptorSet>& GetVkDescriptorSets() const noexcept;
		/// @brief Retrieve the root signature.
		/// @return Root signature.
		[[nodiscard]] TRAP::Ref<VulkanRootSignature> GetRootSignature() const noexcept;
		/// @brief Retrieve the update frequency used by the descriptor set.
		/// @return Update frequency.
		[[nodiscard]] constexpr RendererAPI::DescriptorUpdateFrequency GetUpdateFrequency() const noexcept;
		/// @brief Retrieve the amount of dynamic offsets used by the descriptor set.
		/// @return Amount of dynamic offsets.
		[[nodiscard]] constexpr u8 GetDynamicOffsetCount() const noexcept;
		/// @brief Retrieve the dynamic size offsets used by the descriptor set.
		/// @return Dynamic size offsets.
		[[nodiscard]] constexpr const std::vector<VulkanRenderer::SizeOffset>& GetDynamicSizeOffsets() const noexcept;
		/// @brief Retrieve the max number of sets for the descriptor set.
		/// @return Max number of sets.
		[[nodiscard]] constexpr u32 GetMaxSets() const noexcept;
		/// @brief Retrieve the index of the set for the descriptor set.
		/// @return Index of the set.
		[[nodiscard]] constexpr u32 GetSet() const noexcept;

		/// @brief Update the descriptor set.
		/// @param index Index of the set to update.
		/// @param params Data to update.
		void Update(u32 index, const std::vector<RendererAPI::DescriptorData>& params) override;

	private:
		std::vector<VkDescriptorSet> m_vkDescriptorSetHandles;
		TRAP::Ref<VulkanRootSignature> m_rootSignature;
		//Values passed to vkUpdateDescriptorSetWithTemplate.
		//Initialized to default descriptor values.
		std::vector<std::vector<union VulkanRenderer::DescriptorUpdateData>> m_updateData;
		std::vector<VulkanRenderer::SizeOffset> m_dynamicSizeOffsets;
		u32 m_maxSets;
		u8 m_dynamicOffsetCount;
		u32 m_set;

		TRAP::Ref<VulkanDevice> m_device;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<VkDescriptorSet>& TRAP::Graphics::API::VulkanDescriptorSet::GetVkDescriptorSets() const noexcept
{
	return m_vkDescriptorSetHandles;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<TRAP::Graphics::API::VulkanRenderer::SizeOffset>& TRAP::Graphics::API::VulkanDescriptorSet::GetDynamicSizeOffsets() const noexcept
{
	return m_dynamicSizeOffsets;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::DescriptorUpdateFrequency TRAP::Graphics::API::VulkanDescriptorSet::GetUpdateFrequency() const noexcept
{
	return m_set > 0 ? RendererAPI::DescriptorUpdateFrequency::Dynamic :
	                   RendererAPI::DescriptorUpdateFrequency::Static;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u8 TRAP::Graphics::API::VulkanDescriptorSet::GetDynamicOffsetCount() const noexcept
{
	return m_dynamicOffsetCount;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::API::VulkanDescriptorSet::GetMaxSets() const noexcept
{
	return m_maxSets;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::API::VulkanDescriptorSet::GetSet() const noexcept
{
	return m_set;
}

#endif /*TRAP_VULKANDESCRIPTORSET_H*/