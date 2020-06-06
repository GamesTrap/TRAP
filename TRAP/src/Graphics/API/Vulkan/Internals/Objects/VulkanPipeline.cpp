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
	  m_multiSampleCount(VK_SAMPLE_COUNT_1_BIT)
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
	  m_multiSampleCount(VK_SAMPLE_COUNT_1_BIT)
{
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

void TRAP::Graphics::API::Vulkan::Pipeline::SetShaders(const std::vector<VkPipelineShaderStageCreateInfo>& shaderStageCreateInfos)
{
	m_shaderStageCreateInfos = shaderStageCreateInfos;
	RecreatePipeline();
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
	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = Initializers::PipelineVertexInputStateCreateInfo(); //TODO VULKAN Empty Vertex Data for now

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