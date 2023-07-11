#ifndef TRAP_VULKANROOTSIGNATURE_H
#define TRAP_VULKANROOTSIGNATURE_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/RootSignature.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

namespace TRAP::Graphics::API
{
	class VulkanRootSignature final : public RootSignature
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="desc">Root signature description.</param>
		explicit VulkanRootSignature(const RendererAPI::RootSignatureDesc& desc);
		/// <summary>
		/// Destructor.
		/// </summary>
		~VulkanRootSignature() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr VulkanRootSignature(const VulkanRootSignature&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr VulkanRootSignature& operator=(const VulkanRootSignature&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanRootSignature(VulkanRootSignature&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanRootSignature& operator=(VulkanRootSignature&&) noexcept = default;

		/// <summary>
		/// Retrieve the Vulkan pipeline layout.
		/// </summary>
		/// <returns>Vulkan pipeline layout.</returns>
		[[nodiscard]] constexpr VkPipelineLayout GetVkPipelineLayout() const noexcept;
		/// <summary>
		/// Retrieve the root signature's Vulkan descriptor set layouts.
		/// </summary>
		/// <returns>Vulkan descriptor set layouts.</returns>
		[[nodiscard]] constexpr const std::array<VkDescriptorSetLayout,
		                               RendererAPI::MaxDescriptorSets>& GetVkDescriptorSetLayouts() const noexcept;
		/// <summary>
		/// Retrieve the root signature's Vulkan cumulative descriptor counts.
		/// </summary>
		/// <returns>Vulkan cumulative descriptor counts.</returns>
		[[nodiscard]] constexpr const std::array<uint32_t,
		                               RendererAPI::MaxDescriptorSets>& GetVkCumulativeDescriptorCounts() const noexcept;
		/// <summary>
		/// Retrieve the root signature's Vulkan descriptor counts.
		/// </summary>
		/// <returns>Vulkan descriptor counts.</returns>
		[[nodiscard]] constexpr const std::array<uint16_t,
		                               RendererAPI::MaxDescriptorSets>& GetVkDescriptorCounts() const noexcept;
		/// <summary>
		/// Retrieve the root signature's Vulkan dynamic descriptor counts.
		/// </summary>
		/// <returns>Vulkan dynamic descriptor counts.</returns>
		[[nodiscard]] constexpr const std::array<uint8_t,
		                               RendererAPI::MaxDescriptorSets>& GetVkDynamicDescriptorCounts() const noexcept;
		/// <summary>
		/// Retrieve the root signature's Vulkan RayTracing descriptor counts.
		/// </summary>
		/// <returns>Vulkan RayTracing descriptor counts.</returns>
		[[nodiscard]] constexpr const std::array<uint8_t,
		                               RendererAPI::MaxDescriptorSets>& GetVkRayTracingDescriptorCounts() const noexcept;

		/// <summary>
		/// Retrieve the root signature's Vulkan descriptor update templates.
		/// </summary>
		/// <returns>Vulkan descriptor update templates.</returns>
		[[nodiscard]] constexpr const std::array<VkDescriptorUpdateTemplate,
		                               RendererAPI::MaxDescriptorSets>& GetUpdateTemplates() const noexcept;
		/// <summary>
		/// Retrieve the root signature's empty Vulkan descriptor sets.
		/// </summary>
		/// <returns>Vulkan empty descriptor sets.</returns>
		[[nodiscard]] constexpr const std::array<VkDescriptorSet,
		                               RendererAPI::MaxDescriptorSets>& GetVkEmptyDescriptorSets() const noexcept;
		/// <summary>
		/// Retrieve the root signature's descriptor update data.
		/// </summary>
		/// <returns>Descriptor update data.</returns>
		[[nodiscard]] constexpr const std::array<std::vector<VulkanRenderer::DescriptorUpdateData>,
		                               RendererAPI::MaxDescriptorSets>& GetUpdateTemplateData() const noexcept;

		/// <summary>
		/// Retrieve a descriptor via its name.
		/// </summary>
		/// <returns>Descriptor if found, nullptr otherwise.</returns>
		[[nodiscard]] RendererAPI::DescriptorInfo* GetDescriptor(std::string_view resName);
		/// <summary>
		/// Retrieve a descriptor via its name.
		/// </summary>
		/// <returns>Descriptor if found, nullptr otherwise.</returns>
		[[nodiscard]] const RendererAPI::DescriptorInfo* GetDescriptor(std::string_view resName) const;
	private:
		TRAP::Ref<VulkanDevice> m_device;

		std::array<VkDescriptorSetLayout,
		           RendererAPI::MaxDescriptorSets> m_vkDescriptorSetLayouts;
		std::array<uint32_t,
		           RendererAPI::MaxDescriptorSets> m_vkCumulativeDescriptorsCounts;
		std::array<uint16_t,
		           RendererAPI::MaxDescriptorSets> m_vkDescriptorCounts;
		std::array<uint8_t,
		           RendererAPI::MaxDescriptorSets> m_vkDynamicDescriptorCounts;
		std::array<uint8_t,
		           RendererAPI::MaxDescriptorSets> m_vkRayTracingDescriptorCounts;
		VkPipelineLayout m_pipelineLayout;
		std::array<VkDescriptorUpdateTemplate,
		           RendererAPI::MaxDescriptorSets> m_updateTemplates;
		std::array<VkDescriptorSet,
		           RendererAPI::MaxDescriptorSets> m_vkEmptyDescriptorSets;
		std::array<std::vector<VulkanRenderer::DescriptorUpdateData>,
		           RendererAPI::MaxDescriptorSets> m_updateTemplateData;
		uint32_t m_vkPushConstantCount;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr auto TRAP::Graphics::API::VulkanRootSignature::GetUpdateTemplateData() const noexcept ->
	const std::array<std::vector<TRAP::Graphics::API::VulkanRenderer::DescriptorUpdateData>,
	                 TRAP::Graphics::RendererAPI::MaxDescriptorSets>&
{
	return m_updateTemplateData;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkPipelineLayout TRAP::Graphics::API::VulkanRootSignature::GetVkPipelineLayout() const noexcept
{
	return m_pipelineLayout;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr auto TRAP::Graphics::API::VulkanRootSignature::GetVkDescriptorSetLayouts() const noexcept ->
              const std::array<VkDescriptorSetLayout, TRAP::Graphics::RendererAPI::MaxDescriptorSets>&
{
	return m_vkDescriptorSetLayouts;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr auto TRAP::Graphics::API::VulkanRootSignature::GetVkCumulativeDescriptorCounts() const noexcept ->
	          const std::array<uint32_t, TRAP::Graphics::RendererAPI::MaxDescriptorSets>&
{
	return m_vkCumulativeDescriptorsCounts;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr auto TRAP::Graphics::API::VulkanRootSignature::GetVkDescriptorCounts() const noexcept ->
	          const std::array<uint16_t, TRAP::Graphics::RendererAPI::MaxDescriptorSets>&
{
	return m_vkDescriptorCounts;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr auto TRAP::Graphics::API::VulkanRootSignature::GetVkDynamicDescriptorCounts() const noexcept ->
	          const std::array<uint8_t, TRAP::Graphics::RendererAPI::MaxDescriptorSets>&
{
	return m_vkDynamicDescriptorCounts;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr auto TRAP::Graphics::API::VulkanRootSignature::GetVkRayTracingDescriptorCounts() const noexcept ->
	          const std::array<uint8_t, TRAP::Graphics::RendererAPI::MaxDescriptorSets>&
{
	return m_vkRayTracingDescriptorCounts;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr auto TRAP::Graphics::API::VulkanRootSignature::GetUpdateTemplates() const noexcept ->
	          const std::array<VkDescriptorUpdateTemplate, TRAP::Graphics::RendererAPI::MaxDescriptorSets>&
{
	return m_updateTemplates;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr auto TRAP::Graphics::API::VulkanRootSignature::GetVkEmptyDescriptorSets() const noexcept ->
	          const std::array<VkDescriptorSet, TRAP::Graphics::RendererAPI::MaxDescriptorSets>&
{
	return m_vkEmptyDescriptorSets;
}

#endif /*TRAP_VULKANROOTSIGNATURE_H*/