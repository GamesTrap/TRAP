#ifndef _TRAP_VULKANPIPELINE_H_
#define _TRAP_VULKANPIPELINE_H_

namespace TRAP
{
	class BufferLayout;
}

namespace TRAP::Graphics::API::Vulkan
{
	class RenderPass;
	class Device;

	class Pipeline
	{
	public:
		explicit Pipeline(const Scope<Device>& device, RenderPass* renderPass);
		Pipeline(const Scope<Device>& device, std::vector<VkDescriptorSetLayout>& descriptorLayouts, std::vector<VkPushConstantRange>& pushConstants, RenderPass* renderPass);
		~Pipeline();
		Pipeline(const Pipeline&) = default;
		Pipeline& operator=(const Pipeline&) = default;
		Pipeline(Pipeline&&) = default;
		Pipeline& operator=(Pipeline&&) = default;

		void SetShaders(const std::vector<VkPipelineShaderStageCreateInfo>& shaderStageCreateInfos);
		std::vector<VkPipelineShaderStageCreateInfo>& GetShaders();
		VkPipeline& GetPipeline();
		
	private:
		void RecreatePipeline();
		
		VkPipelineLayout m_pipelineLayout;
		Device* m_device;
		VkPipeline m_pipeline;
		RenderPass* m_renderPass;

		//Pipeline Layout
		std::vector<VkDescriptorSetLayout> m_descriptorLayouts;
		std::vector<VkPushConstantRange> m_pushConstants;
		
		//Pipeline
		VkPrimitiveTopology m_primitiveTopology;
		bool m_primitiveRestart;
		VkPolygonMode m_polygonMode;
		VkCullModeFlagBits m_cullMode;
		VkFrontFace m_frontFace;
		bool m_blend;
		VkBlendFactor m_srcColorBlendFactor;
		VkBlendFactor m_srcAlphaBlendFactor;
		VkBlendFactor m_dstColorBlendFactor;
		VkBlendFactor m_dstAlphaBlendFactor;
		VkBlendOp m_colorBlendOp;
		VkBlendOp m_alphaBlendOp;
		bool m_depthTest;
		bool m_depthWrite;
		VkCompareOp m_depthOp;
		uint32_t m_viewportCount;
		uint32_t m_scissorCount;
		VkSampleCountFlagBits m_multiSampleCount;
		std::vector<VkPipelineShaderStageCreateInfo> m_shaderStageCreateInfos;
	};
}

#endif /*_TRAP_VULKANPIPELINE_H_*/