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
		VulkanRootSignature(const VulkanRootSignature&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanRootSignature& operator=(const VulkanRootSignature&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanRootSignature(VulkanRootSignature&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanRootSignature& operator=(VulkanRootSignature&&) noexcept = default;

		/// <summary>
		/// Retrieve the pipeline type used by the shaders of the root signature.
		/// </summary>
		/// <returns>Pipeline type.</returns>
		[[nodiscard]] RendererAPI::PipelineType GetPipelineType() const noexcept override;
		/// <summary>
		/// Retrieve the Vulkan pipeline layout.
		/// </summary>
		/// <returns>Vulkan pipeline layout.</returns>
		[[nodiscard]] VkPipelineLayout GetVkPipelineLayout() const noexcept;
		/// <summary>
		/// Retrieve the root signature's Vulkan descriptor set layouts.
		/// </summary>
		/// <returns>Vulkan descriptor set layouts.</returns>
		[[nodiscard]] const std::array<VkDescriptorSetLayout,
		                               RendererAPI::MaxDescriptorSets>& GetVkDescriptorSetLayouts() const noexcept;
		/// <summary>
		/// Retrieve the root signature's Vulkan cumulative descriptor counts.
		/// </summary>
		/// <returns>Vulkan cumulative descriptor counts.</returns>
		[[nodiscard]] const std::array<uint32_t,
		                               RendererAPI::MaxDescriptorSets>& GetVkCumulativeDescriptorCounts() const noexcept;
		/// <summary>
		/// Retrieve the root signature's Vulkan descriptor counts.
		/// </summary>
		/// <returns>Vulkan descriptor counts.</returns>
		[[nodiscard]] const std::array<uint16_t,
		                               RendererAPI::MaxDescriptorSets>& GetVkDescriptorCounts() const noexcept;
		/// <summary>
		/// Retrieve the root signature's Vulkan dynamic descriptor counts.
		/// </summary>
		/// <returns>Vulkan dynamic descriptor counts.</returns>
		[[nodiscard]] const std::array<uint8_t,
		                               RendererAPI::MaxDescriptorSets>& GetVkDynamicDescriptorCounts() const noexcept;
		/// <summary>
		/// Retrieve the root signature's Vulkan RayTracing descriptor counts.
		/// </summary>
		/// <returns>Vulkan RayTracing descriptor counts.</returns>
		[[nodiscard]] const std::array<uint8_t,
		                               RendererAPI::MaxDescriptorSets>& GetVkRayTracingDescriptorCounts() const noexcept;

		/// <summary>
		/// Retrieve the root signature's Vulkan descriptor update templates.
		/// </summary>
		/// <returns>Vulkan descriptor update templates.</returns>
		[[nodiscard]] const std::array<VkDescriptorUpdateTemplate,
		                               RendererAPI::MaxDescriptorSets>& GetUpdateTemplates() const noexcept;
		/// <summary>
		/// Retrieve the root signature's empty Vulkan descriptor sets.
		/// </summary>
		/// <returns>Vulkan empty descriptor sets.</returns>
		[[nodiscard]] const std::array<VkDescriptorSet,
		                               RendererAPI::MaxDescriptorSets>& GetVkEmptyDescriptorSets() const noexcept;
		/// <summary>
		/// Retrieve the root signature's descriptor update data.
		/// </summary>
		/// <returns>Descriptor update data.</returns>
		[[nodiscard]] const std::array<std::vector<VulkanRenderer::DescriptorUpdateData>,
		                               RendererAPI::MaxDescriptorSets>& GetUpdateTemplateData() const noexcept;

		/// <summary>
		/// Retrieve a descriptor via its name.
		/// </summary>
		/// <returns>Descriptor if found, nullptr otherwise.</returns>
		[[nodiscard]] RendererAPI::DescriptorInfo* GetDescriptor(const char* resName);
		/// <summary>
		/// Retrieve a descriptor via its name.
		/// </summary>
		/// <returns>Descriptor if found, nullptr otherwise.</returns>
		[[nodiscard]] const RendererAPI::DescriptorInfo* GetDescriptor(const char* resName) const;
		/// <summary>
		/// Retrieve the amount of descriptors contained in the root signature.
		/// </summary>
		/// <returns>Amount of descriptors.</returns>
		[[nodiscard]] uint32_t GetDescriptorCount() const noexcept override;
		/// <summary>
		/// Retrieve the list of descriptors contained in the root signature.
		/// </summary>
		/// <returns>List of descriptors.</returns>
		[[nodiscard]] const std::vector<RendererAPI::DescriptorInfo>& GetDescriptors() const noexcept override;
		/// <summary>
		/// Retrieve the map which converts a descriptor name to its index.
		/// </summary>
		/// <returns>Map which converts a descriptor name to its index.</returns>
		[[nodiscard]] const RendererAPI::DescriptorIndexMap& GetDescriptorNameToIndexMap() const noexcept override;
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

#endif /*TRAP_VULKANROOTSIGNATURE_H*/