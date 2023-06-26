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
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="device">Vulkan device.</param>
		/// <param name="vkDescriptorSetHandles">Vulkan descriptor set handles.</param>
		/// <param name="rootSignature">Root signature.</param>
		/// <param name="updateData">Update template data.</param>
		/// <param name="maxSets">Max number of sets for the descriptor set.</param>
		/// <param name="dynamicOffsetCount">Number of dynamic offsets.</param>
		/// <param name="set">Update frequency.</param>
		VulkanDescriptorSet(TRAP::Ref<VulkanDevice> device, std::vector<VkDescriptorSet> vkDescriptorSetHandles,
		                    TRAP::Ref<VulkanRootSignature> rootSignature,
		                    std::vector<std::vector<VulkanRenderer::DescriptorUpdateData>> updateData,
		                    uint32_t maxSets, uint8_t dynamicOffsetCount,
							uint32_t set);

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanDescriptorSet(const VulkanDescriptorSet&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanDescriptorSet& operator=(const VulkanDescriptorSet&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanDescriptorSet(VulkanDescriptorSet&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanDescriptorSet& operator=(VulkanDescriptorSet&&) noexcept = default;

		/// <summary>
		/// Destructor.
		/// </summary>
		~VulkanDescriptorSet() override = default;

		/// <summary>
		/// Retrieve the VkDescriptorSet handles.
		/// </summary>
		/// <returns>VkDescriptorSet handles.</returns>
		[[nodiscard]] constexpr const std::vector<VkDescriptorSet>& GetVkDescriptorSets() const noexcept;
		/// <summary>
		/// Retrieve the root signature.
		/// </summary>
		/// <returns>Root signature.</returns>
		[[nodiscard]] TRAP::Ref<VulkanRootSignature> GetRootSignature() const noexcept;
		/// <summary>
		/// Retrieve the update frequency used by the descriptor set.
		/// </summary>
		/// <returns>Update frequency.</returns>
		[[nodiscard]] RendererAPI::DescriptorUpdateFrequency GetUpdateFrequency() const noexcept;
		/// <summary>
		/// Retrieve the amount of dynamic offsets used by the descriptor set.
		/// </summary>
		/// <returns>Amount of dynamic offsets.</returns>
		[[nodiscard]] uint8_t GetDynamicOffsetCount() const noexcept;
		/// <summary>
		/// Retrieve the dynamic size offsets used by the descriptor set.
		/// </summary>
		/// <returns>Dynamic size offsets.</returns>
		[[nodiscard]] constexpr const std::vector<VulkanRenderer::SizeOffset>& GetDynamicSizeOffsets() const noexcept;
		/// <summary>
		/// Retrieve the max number of sets for the descriptor set.
		/// </summary>
		/// <returns>Max number of sets.</returns>
		[[nodiscard]] uint32_t GetMaxSets() const noexcept;
		/// <summary>
		/// Retrieve the index of the set for the descriptor set.
		/// </summary>
		/// <returns>Index of the set.</returns>
		[[nodiscard]] uint32_t GetSet() const noexcept;

		/// <summary>
		/// Update the descriptor set.
		/// </summary>
		/// <param name="index">Index of the set to update.</param>
		/// <param name="params">Data to update.</param>
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

#endif /*TRAP_VULKANDESCRIPTORSET_H*/