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
		const std::array<VkDescriptorSetLayout,
		                 RendererAPI::MaxDescriptorSets>& GetVkDescriptorSetLayouts() const;
		const std::array<uint32_t,
		                 RendererAPI::MaxDescriptorSets>& GetVkCumulativeDescriptorCounts() const;
		const std::array<uint16_t,
		                 RendererAPI::MaxDescriptorSets>& GetVkDescriptorCounts() const;
		const std::array<uint8_t,
		                 RendererAPI::MaxDescriptorSets>& GetVkDynamicDescriptorCounts() const;
		const std::array<uint8_t,
		                 RendererAPI::MaxDescriptorSets>& GetVkRayTracingDescriptorCounts() const;

		const std::array<VkDescriptorUpdateTemplate,
		                 RendererAPI::MaxDescriptorSets>& GetUpdateTemplates() const;
		const std::array<VkDescriptorSet,
		                 RendererAPI::MaxDescriptorSets>& GetVkEmptyDescriptorSets() const;
		const std::array<std::vector<VulkanRenderer::DescriptorUpdateData>,
		                 RendererAPI::MaxDescriptorSets>& GetUpdateTemplateData() const;

		RendererAPI::DescriptorInfo* GetDescriptor(const char* resName);
		uint32_t GetDescriptorCount() const override;
		const std::vector<RendererAPI::DescriptorInfo>& GetDescriptors() const override;
		const TRAP::Scope<RendererAPI::DescriptorIndexMap>& GetDescriptorNameToIndexMap() const override;
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

#endif /*_TRAP_VULKANROOTSIGNATURE_H_*/