#include "TRAPPCH.h"
#include "VulkanPipeline.h"

#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/Internals/VulkanInitializers.h"
#include "VulkanDevice.h"
#include "VulkanRenderPass.h"

TRAP::Graphics::API::Vulkan::Pipeline::Pipeline(const Scope<Device>& device, RenderPass* renderPass)
	: m_pipelineLayout(),
	  m_device(device.get()),
	  m_pipeline(),
	  m_renderPass(renderPass),
	  m_primitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST),
	  m_primitiveRestart(false),
	  m_polygonMode(VK_POLYGON_MODE_FILL),
	  m_cullMode(VK_CULL_MODE_BACK_BIT),
	  m_frontFace(VK_FRONT_FACE_CLOCKWISE),
	  m_blend(false),
	  m_srcColorBlendFactor(VK_BLEND_FACTOR_ONE),
	  m_srcAlphaBlendFactor(VK_BLEND_FACTOR_ONE),
	  m_dstColorBlendFactor(VK_BLEND_FACTOR_ZERO),
	  m_dstAlphaBlendFactor(VK_BLEND_FACTOR_ZERO),
	  m_colorBlendOp(VK_BLEND_OP_ADD),
	  m_alphaBlendOp(VK_BLEND_OP_ADD),
	  m_depthTest(true),
	  m_depthWrite(true),
	  m_depthOp(VK_COMPARE_OP_LESS_OR_EQUAL),
	  m_viewportCount(1),
	  m_scissorCount(1),
	  m_multiSampleCount(VK_SAMPLE_COUNT_1_BIT),
	  m_vertexInputBindingDescriptions(1, {0, 0, VK_VERTEX_INPUT_RATE_VERTEX})
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::Vulkan::Pipeline::Pipeline(const Scope<Device>& device,
                                                std::vector<VkDescriptorSetLayout>& descriptorLayouts,
                                                std::vector<VkPushConstantRange>& pushConstants,
                                                RenderPass* renderPass)
	: m_pipelineLayout(),
	  m_device(device.get()),
	  m_pipeline(),
	  m_renderPass(renderPass),
	  m_descriptorLayouts(descriptorLayouts),
	  m_pushConstants(pushConstants),
	  m_primitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST),
	  m_primitiveRestart(false),
	  m_polygonMode(VK_POLYGON_MODE_FILL),
	  m_cullMode(VK_CULL_MODE_BACK_BIT),
	  m_frontFace(VK_FRONT_FACE_CLOCKWISE),
	  m_blend(false),
	  m_srcColorBlendFactor(VK_BLEND_FACTOR_ONE),
	  m_srcAlphaBlendFactor(VK_BLEND_FACTOR_ONE),
	  m_dstColorBlendFactor(VK_BLEND_FACTOR_ZERO),
	  m_dstAlphaBlendFactor(VK_BLEND_FACTOR_ZERO),
	  m_colorBlendOp(VK_BLEND_OP_ADD),
	  m_alphaBlendOp(VK_BLEND_OP_ADD),
	  m_depthTest(true),
	  m_depthWrite(true),
	  m_depthOp(VK_COMPARE_OP_LESS_OR_EQUAL),
	  m_viewportCount(1),
	  m_scissorCount(1),
	  m_multiSampleCount(VK_SAMPLE_COUNT_1_BIT),
	  m_vertexInputBindingDescriptions(1, { 0, 0, VK_VERTEX_INPUT_RATE_VERTEX })
{
	//TODO Unused
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::Vulkan::Pipeline::~Pipeline()
{
	if (m_pipeline)
	{
		vkDestroyPipeline(m_device->GetDevice(), m_pipeline, nullptr);
		m_pipeline = nullptr;
	}
	
	if (m_pipelineLayout)
	{
		vkDestroyPipelineLayout(m_device->GetDevice(), m_pipelineLayout, nullptr);
		m_pipelineLayout = nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Vulkan::Pipeline::SetShaders(const std::vector<VkPipelineShaderStageCreateInfo>& shaderStageCreateInfos,
	const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions,
	const uint32_t stride)
{
	m_shaderStageCreateInfos = shaderStageCreateInfos;

	for(const auto& i : m_shaderStageCreateInfos)
	{
		if(i.stage == VK_SHADER_STAGE_VERTEX_BIT)
		{
			m_vertexInputBindingDescriptions[0].stride = stride;
			m_vertexInputAttributeDescriptions = attributeDescriptions;
			break;
		}
	}
	RecreatePipeline(); //TODO Move this to draw call because this needs all bindings
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<VkPipelineShaderStageCreateInfo>& TRAP::Graphics::API::Vulkan::Pipeline::GetShaders()
{
	return m_shaderStageCreateInfos;
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipeline& TRAP::Graphics::API::Vulkan::Pipeline::GetPipeline()
{
	return m_pipeline;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Vulkan::Pipeline::RecreatePipeline()
{
	//TODO Optimization instead of deleting pipeline save it in a std::map after creation and use a struct containing all its properties as key to be able to reuse it
	
	//Delete old Pipeline if exists
	if(m_pipeline)
	{
		vkDestroyPipeline(m_device->GetDevice(), m_pipeline, nullptr);
		m_pipeline = nullptr;
	}
	//Delete old Pipeline Layout if exists
	if (m_pipelineLayout)
	{
		vkDestroyPipelineLayout(m_device->GetDevice(), m_pipelineLayout, nullptr);
		m_pipelineLayout = nullptr;
	}

	//Create new Pipeline Layout with new settings
	VkPipelineLayoutCreateInfo info = Initializers::PipelineLayoutCreateInfo(m_descriptorLayouts, m_pushConstants);
	VkCall(vkCreatePipelineLayout(m_device->GetDevice(), &info, nullptr, &m_pipelineLayout));
	
	//Setup new Pipeline with new settings
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = Initializers::PipelineInputAssemblyStateCreateInfo(m_primitiveTopology, m_primitiveRestart);
	VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = Initializers::PipelineRasterizationStateCreateInfo(m_polygonMode, m_cullMode, m_frontFace);
	VkPipelineColorBlendAttachmentState colorBlendAttachmentState;
	if (m_blend)
		colorBlendAttachmentState = Initializers::PipelineColorBlendAttachmentState(m_srcColorBlendFactor,
		                                                                            m_srcAlphaBlendFactor,
		                                                                            m_dstColorBlendFactor,
		                                                                            m_dstAlphaBlendFactor,
		                                                                            m_colorBlendOp,
		                                                                            m_alphaBlendOp);
	else
		colorBlendAttachmentState = Initializers::PipelineColorBlendAttachmentState();
	VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = Initializers::PipelineColorBlendStateCreateInfo(colorBlendAttachmentState);
	VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = Initializers::PipelineDepthStencilStateCreateInfo(m_depthTest, m_depthWrite, m_depthOp);
	VkPipelineViewportStateCreateInfo viewportStateCreateInfo = Initializers::PipelineViewportStateCreateInfo(m_viewportCount, m_scissorCount);
	VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = Initializers::PipelineMultisampleStateCreateInfo(m_multiSampleCount);
	std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = Initializers::PipelineDynamicStateCreateInfo(dynamicStates);
	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = Initializers::PipelineVertexInputStateCreateInfo(m_vertexInputBindingDescriptions, m_vertexInputAttributeDescriptions);

	//TODO VULKAN Tessellation Shaders are ignored for now
	//VkPipelineTessellationStateCreateInfo x = {};
	
	//Create new Pipeline with new settings
	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = Initializers::GraphicsPipelineCreateInfo(m_shaderStageCreateInfos,
		vertexInputStateCreateInfo,
		inputAssemblyStateCreateInfo, 
		viewportStateCreateInfo,
		rasterizationStateCreateInfo,
		multisampleStateCreateInfo,
		depthStencilStateCreateInfo,
		colorBlendStateCreateInfo,
		dynamicStateCreateInfo,
		m_pipelineLayout,
		m_renderPass->GetRenderPass(),
		0);
	VkCall(vkCreateGraphicsPipelines(m_device->GetDevice(), nullptr, 1, &graphicsPipelineCreateInfo, nullptr, &m_pipeline));
	//BUG Vulkan Crash on AMD Ryzen 7 3750H iGPU

	//TODO VULKAN Pipeline Cache
}