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
		                    u32 maxSets, u8 dynamicOffsetCount,
							u32 set);

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
		[[nodiscard]] constexpr RendererAPI::DescriptorUpdateFrequency GetUpdateFrequency() const noexcept;
		/// <summary>
		/// Retrieve the amount of dynamic offsets used by the descriptor set.
		/// </summary>
		/// <returns>Amount of dynamic offsets.</returns>
		[[nodiscard]] constexpr u8 GetDynamicOffsetCount() const noexcept;
		/// <summary>
		/// Retrieve the dynamic size offsets used by the descriptor set.
		/// </summary>
		/// <returns>Dynamic size offsets.</returns>
		[[nodiscard]] constexpr const std::vector<VulkanRenderer::SizeOffset>& GetDynamicSizeOffsets() const noexcept;
		/// <summary>
		/// Retrieve the max number of sets for the descriptor set.
		/// </summary>
		/// <returns>Max number of sets.</returns>
		[[nodiscard]] constexpr u32 GetMaxSets() const noexcept;
		/// <summary>
		/// Retrieve the index of the set for the descriptor set.
		/// </summary>
		/// <returns>Index of the set.</returns>
		[[nodiscard]] constexpr u32 GetSet() const noexcept;

		/// <summary>
		/// Update the descriptor set.
		/// </summary>
		/// <param name="index">Index of the set to update.</param>
		/// <param name="params">Data to update.</param>
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