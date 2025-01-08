#ifndef TRAP_VULKANROOTSIGNATURE_H
#define TRAP_VULKANROOTSIGNATURE_H

#include "Graphics/API/Objects/RootSignature.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

namespace TRAP::Graphics::API
{
	class VulkanRootSignature final : public RootSignature
	{
	public:
		/// @brief Constructor.
		/// @param desc Root signature description.
		explicit VulkanRootSignature(const RootSignatureDesc& desc);
		/// @brief Destructor.
		~VulkanRootSignature() override;

		/// @brief Copy constructor.
		consteval VulkanRootSignature(const VulkanRootSignature&) = delete;
		/// @brief Copy assignment operator.
		consteval VulkanRootSignature& operator=(const VulkanRootSignature&) = delete;
		/// @brief Move constructor.
		VulkanRootSignature(VulkanRootSignature&&) noexcept = default;
		/// @brief Move assignment operator.
		VulkanRootSignature& operator=(VulkanRootSignature&&) noexcept = default;

		/// @brief Retrieve the Vulkan pipeline layout.
		/// @return Vulkan pipeline layout.
		[[nodiscard]] constexpr VkPipelineLayout GetVkPipelineLayout() const noexcept;
		/// @brief Retrieve the root signature's Vulkan descriptor set layouts.
		/// @return Vulkan descriptor set layouts.
		[[nodiscard]] constexpr const std::array<VkDescriptorSetLayout,
		                               MaxDescriptorSets>& GetVkDescriptorSetLayouts() const noexcept;
		/// @brief Retrieve the root signature's Vulkan cumulative descriptor counts.
		/// @return Vulkan cumulative descriptor counts.
		[[nodiscard]] constexpr const std::array<u32,
		                               MaxDescriptorSets>& GetVkCumulativeDescriptorCounts() const noexcept;
		/// @brief Retrieve the root signature's Vulkan descriptor counts.
		/// @return Vulkan descriptor counts.
		[[nodiscard]] constexpr const std::array<u16,
		                               MaxDescriptorSets>& GetVkDescriptorCounts() const noexcept;
		/// @brief Retrieve the root signature's Vulkan dynamic descriptor counts.
		/// @return Vulkan dynamic descriptor counts.
		[[nodiscard]] constexpr const std::array<u8,
		                               MaxDescriptorSets>& GetVkDynamicDescriptorCounts() const noexcept;
		/// @brief Retrieve the root signature's Vulkan RayTracing descriptor counts.
		/// @return Vulkan RayTracing descriptor counts.
		[[nodiscard]] constexpr const std::array<u8,
		                               MaxDescriptorSets>& GetVkRayTracingDescriptorCounts() const noexcept;

		/// @brief Retrieve the root signature's Vulkan descriptor update templates.
		/// @return Vulkan descriptor update templates.
		[[nodiscard]] constexpr const std::array<VkDescriptorUpdateTemplate,
		                               MaxDescriptorSets>& GetUpdateTemplates() const noexcept;
		/// @brief Retrieve the root signature's empty Vulkan descriptor sets.
		/// @return Vulkan empty descriptor sets.
		[[nodiscard]] constexpr const std::array<VkDescriptorSet,
		                               MaxDescriptorSets>& GetVkEmptyDescriptorSets() const noexcept;
		/// @brief Retrieve the root signature's descriptor update data.
		/// @return Descriptor update data.
		[[nodiscard]] constexpr const std::array<std::vector<VulkanRenderer::DescriptorUpdateData>,
		                               MaxDescriptorSets>& GetUpdateTemplateData() const noexcept;

		/// @brief Retrieve a descriptor via its name.
		/// @return Descriptor if found, nullptr otherwise.
		[[nodiscard]] const DescriptorInfo* GetDescriptor(std::string_view resName) const;

	private:
		TRAP::Ref<VulkanDevice> m_device = dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice();

		std::array<VkDescriptorSetLayout,
		           MaxDescriptorSets> m_vkDescriptorSetLayouts{};
		std::array<u32,
		           MaxDescriptorSets> m_vkCumulativeDescriptorsCounts{};
		std::array<u16,
		           MaxDescriptorSets> m_vkDescriptorCounts{};
		std::array<u8,
		           MaxDescriptorSets> m_vkDynamicDescriptorCounts{};
		std::array<u8,
		           MaxDescriptorSets> m_vkRayTracingDescriptorCounts{};
		VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
		std::array<VkDescriptorUpdateTemplate,
		           MaxDescriptorSets> m_updateTemplates{};
		std::array<VkDescriptorSet,
		           MaxDescriptorSets> m_vkEmptyDescriptorSets{};
		std::array<std::vector<VulkanRenderer::DescriptorUpdateData>,
		           MaxDescriptorSets> m_updateTemplateData{};
		u32 m_vkPushConstantCount = 0;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::array<std::vector<TRAP::Graphics::API::VulkanRenderer::DescriptorUpdateData>, TRAP::Graphics::MaxDescriptorSets>&
	TRAP::Graphics::API::VulkanRootSignature::GetUpdateTemplateData() const noexcept
{
	return m_updateTemplateData;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkPipelineLayout TRAP::Graphics::API::VulkanRootSignature::GetVkPipelineLayout() const noexcept
{
	return m_pipelineLayout;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::array<VkDescriptorSetLayout, TRAP::Graphics::MaxDescriptorSets>&
	TRAP::Graphics::API::VulkanRootSignature::GetVkDescriptorSetLayouts() const noexcept
{
	return m_vkDescriptorSetLayouts;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::array<u32, TRAP::Graphics::MaxDescriptorSets>&
	TRAP::Graphics::API::VulkanRootSignature::GetVkCumulativeDescriptorCounts() const noexcept
{
	return m_vkCumulativeDescriptorsCounts;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::array<u16, TRAP::Graphics::MaxDescriptorSets>&
	TRAP::Graphics::API::VulkanRootSignature::GetVkDescriptorCounts() const noexcept
{
	return m_vkDescriptorCounts;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::array<u8, TRAP::Graphics::MaxDescriptorSets>&
	TRAP::Graphics::API::VulkanRootSignature::GetVkDynamicDescriptorCounts() const noexcept
{
	return m_vkDynamicDescriptorCounts;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::array<u8, TRAP::Graphics::MaxDescriptorSets>&
	TRAP::Graphics::API::VulkanRootSignature::GetVkRayTracingDescriptorCounts() const noexcept
{
	return m_vkRayTracingDescriptorCounts;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::array<VkDescriptorUpdateTemplate, TRAP::Graphics::MaxDescriptorSets>&
	TRAP::Graphics::API::VulkanRootSignature::GetUpdateTemplates() const noexcept
{
	return m_updateTemplates;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::array<VkDescriptorSet, TRAP::Graphics::MaxDescriptorSets>&
	TRAP::Graphics::API::VulkanRootSignature::GetVkEmptyDescriptorSets() const noexcept
{
	return m_vkEmptyDescriptorSets;
}

#endif /*TRAP_VULKANROOTSIGNATURE_H*/
