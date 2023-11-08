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
		/// @brief Constructor.
		/// @param desc Root signature description.
		explicit VulkanRootSignature(const RendererAPI::RootSignatureDesc& desc);
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
		                               RendererAPI::MaxDescriptorSets>& GetVkDescriptorSetLayouts() const noexcept;
		/// @brief Retrieve the root signature's Vulkan cumulative descriptor counts.
		/// @return Vulkan cumulative descriptor counts.
		[[nodiscard]] constexpr const std::array<u32,
		                               RendererAPI::MaxDescriptorSets>& GetVkCumulativeDescriptorCounts() const noexcept;
		/// @brief Retrieve the root signature's Vulkan descriptor counts.
		/// @return Vulkan descriptor counts.
		[[nodiscard]] constexpr const std::array<u16,
		                               RendererAPI::MaxDescriptorSets>& GetVkDescriptorCounts() const noexcept;
		/// @brief Retrieve the root signature's Vulkan dynamic descriptor counts.
		/// @return Vulkan dynamic descriptor counts.
		[[nodiscard]] constexpr const std::array<u8,
		                               RendererAPI::MaxDescriptorSets>& GetVkDynamicDescriptorCounts() const noexcept;
		/// @brief Retrieve the root signature's Vulkan RayTracing descriptor counts.
		/// @return Vulkan RayTracing descriptor counts.
		[[nodiscard]] constexpr const std::array<u8,
		                               RendererAPI::MaxDescriptorSets>& GetVkRayTracingDescriptorCounts() const noexcept;

		/// @brief Retrieve the root signature's Vulkan descriptor update templates.
		/// @return Vulkan descriptor update templates.
		[[nodiscard]] constexpr const std::array<VkDescriptorUpdateTemplate,
		                               RendererAPI::MaxDescriptorSets>& GetUpdateTemplates() const noexcept;
		/// @brief Retrieve the root signature's empty Vulkan descriptor sets.
		/// @return Vulkan empty descriptor sets.
		[[nodiscard]] constexpr const std::array<VkDescriptorSet,
		                               RendererAPI::MaxDescriptorSets>& GetVkEmptyDescriptorSets() const noexcept;
		/// @brief Retrieve the root signature's descriptor update data.
		/// @return Descriptor update data.
		[[nodiscard]] constexpr const std::array<std::vector<VulkanRenderer::DescriptorUpdateData>,
		                               RendererAPI::MaxDescriptorSets>& GetUpdateTemplateData() const noexcept;

		/// @brief Retrieve a descriptor via its name.
		/// @return Descriptor if found, nullptr otherwise.
		[[nodiscard]] RendererAPI::DescriptorInfo* GetDescriptor(std::string_view resName);
		/// @brief Retrieve a descriptor via its name.
		/// @return Descriptor if found, nullptr otherwise.
		[[nodiscard]] const RendererAPI::DescriptorInfo* GetDescriptor(std::string_view resName) const;
	private:
		TRAP::Ref<VulkanDevice> m_device = dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice();

		std::array<VkDescriptorSetLayout,
		           RendererAPI::MaxDescriptorSets> m_vkDescriptorSetLayouts{};
		std::array<u32,
		           RendererAPI::MaxDescriptorSets> m_vkCumulativeDescriptorsCounts{};
		std::array<u16,
		           RendererAPI::MaxDescriptorSets> m_vkDescriptorCounts{};
		std::array<u8,
		           RendererAPI::MaxDescriptorSets> m_vkDynamicDescriptorCounts{};
		std::array<u8,
		           RendererAPI::MaxDescriptorSets> m_vkRayTracingDescriptorCounts{};
		VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
		std::array<VkDescriptorUpdateTemplate,
		           RendererAPI::MaxDescriptorSets> m_updateTemplates{};
		std::array<VkDescriptorSet,
		           RendererAPI::MaxDescriptorSets> m_vkEmptyDescriptorSets{};
		std::array<std::vector<VulkanRenderer::DescriptorUpdateData>,
		           RendererAPI::MaxDescriptorSets> m_updateTemplateData{};
		u32 m_vkPushConstantCount = 0;
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
	          const std::array<u32, TRAP::Graphics::RendererAPI::MaxDescriptorSets>&
{
	return m_vkCumulativeDescriptorsCounts;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr auto TRAP::Graphics::API::VulkanRootSignature::GetVkDescriptorCounts() const noexcept ->
	          const std::array<u16, TRAP::Graphics::RendererAPI::MaxDescriptorSets>&
{
	return m_vkDescriptorCounts;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr auto TRAP::Graphics::API::VulkanRootSignature::GetVkDynamicDescriptorCounts() const noexcept ->
	          const std::array<u8, TRAP::Graphics::RendererAPI::MaxDescriptorSets>&
{
	return m_vkDynamicDescriptorCounts;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr auto TRAP::Graphics::API::VulkanRootSignature::GetVkRayTracingDescriptorCounts() const noexcept ->
	          const std::array<u8, TRAP::Graphics::RendererAPI::MaxDescriptorSets>&
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