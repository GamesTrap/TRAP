#ifndef _TRAP_VULKANROOTSIGNATURE_H_
#define _TRAP_VULKANROOTSIGNATURE_H_

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/RootSignature.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

namespace TRAP::Graphics::API
{
	class VulkanRootSignature final : public RootSignature
	{
	public:
		explicit VulkanRootSignature(const RendererAPI::RootSignatureDesc& desc);
		~VulkanRootSignature();

		RendererAPI::PipelineType GetPipelineType() const override;
		VkPipelineLayout GetVkPipelineLayout() const;
		const std::array<VkDescriptorSetLayout, static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT)>& GetVkDescriptorSetLayouts() const;
		const std::array<uint32_t, static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT)>& GetVkCumulativeDescriptorCounts() const;
		const std::array<uint16_t, static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT)>& GetVkDescriptorCounts() const;
		const std::array<uint8_t, static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT)>& GetVkDynamicDescriptorCounts() const;
		const std::array<uint8_t, static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT)>& GetVkRayTracingDescriptorCounts() const;

		const std::array<VkDescriptorUpdateTemplate, static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT)>& GetUpdateTemplates() const;
		const std::array<VkDescriptorSet, static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT)>& GetVkEmptyDescriptorSets() const;
		const std::array<std::vector<VulkanRenderer::DescriptorUpdateData>, static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT)>& GetUpdateTemplateData() const;

		RendererAPI::DescriptorInfo* GetDescriptor(const char* resName);
		uint32_t GetDescriptorCount() const override;
		const std::vector<RendererAPI::DescriptorInfo>& GetDescriptors() const override;
		const TRAP::Scope<RendererAPI::DescriptorIndexMap>& GetDescriptorNameToIndexMap() const override;
	private:
		TRAP::Ref<VulkanDevice> m_device;

		std::array<VkDescriptorSetLayout, static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT)> m_vkDescriptorSetLayouts;
		std::array<uint32_t, static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT)> m_vkCumulativeDescriptorsCounts;
		std::array<uint16_t, static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT)> m_vkDescriptorCounts;
		std::array<uint8_t, static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT)> m_vkDynamicDescriptorCounts;
		std::array<uint8_t, static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT)> m_vkRayTracingDescriptorCounts;
		VkPipelineLayout m_pipelineLayout;
		std::array<VkDescriptorUpdateTemplate, static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT)> m_updateTemplates;
		std::array<VkDescriptorSet, static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT)> m_vkEmptyDescriptorSets;
		std::array<std::vector<VulkanRenderer::DescriptorUpdateData>, static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT)> m_updateTemplateData;
		uint32_t m_vkPushConstantCount;
	};
}

#endif /*_TRAP_VULKANROOTSIGNATURE_H_*/