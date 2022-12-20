#include "TRAPPCH.h"
#include "VulkanPipeline.h"

#include "Utils/Utils.h"
#include "VulkanRenderPass.h"
#include "VulkanRootSignature.h"
#include "VulkanShader.h"
#include "VulkanPipelineCache.h"
#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/Objects/VulkanPhysicalDevice.h"
#include "Graphics/API/Objects/RenderTarget.h"

TRAP::Graphics::API::VulkanPipeline::VulkanPipeline(const RendererAPI::PipelineDesc& desc)
	: m_vkPipeline(VK_NULL_HANDLE),
	  m_type(),
	  m_device(dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice())
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(m_device, "VulkanPipeline(): Vulkan Device is nullptr!");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanPipelinePrefix, "Creating Pipeline");
#endif

	switch(desc.Type)
	{
	case RendererAPI::PipelineType::Compute:
	{
		InitComputePipeline(desc);
		break;
	}

	case RendererAPI::PipelineType::Graphics:
	{
		InitGraphicsPipeline(desc);
		break;
	}

	case RendererAPI::PipelineType::RayTracing:
	{
		//AddRayTracingPipeline(desc); //TODO
		break;
	}

	default:
		TRAP_ASSERT(false, "VulkanPipeline(): Unknown Pipeline Type!");
		break;
	}

#ifdef ENABLE_GRAPHICS_DEBUG
	if(!desc.Name.empty() && m_vkPipeline != VK_NULL_HANDLE)
		SetPipelineName(desc.Name);
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanPipeline::~VulkanPipeline()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(m_device, "~VulkanPipeline(): Vulkan Device is nullptr!");
	TRAP_ASSERT(m_vkPipeline, "~VulkanPipeline(): Vulkan Pipeline is nullptr!");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanPipelinePrefix, "Destroying Pipeline");
#endif

	m_shaderStageNames.clear();

	vkDestroyPipeline(m_device->GetVkDevice(), m_vkPipeline, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanPipeline::InitComputePipeline(const RendererAPI::PipelineDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	const auto& computeDesc = std::get<RendererAPI::ComputePipelineDesc>(desc.Pipeline);
	const VkPipelineCache psoCache = desc.Cache ?
	                                 std::dynamic_pointer_cast<VulkanPipelineCache>(desc.Cache)->GetVkPipelineCache() :
							         VK_NULL_HANDLE;

	TRAP_ASSERT(computeDesc.ShaderProgram, "VulkanPipeline::InitComputePipeline(): ShaderProgram is nullptr!");
	TRAP_ASSERT(computeDesc.RootSignature, "VulkanPipeline::InitComputePipeline(): RootSignature is nullptr!");
	const VulkanShader* const vShader = dynamic_cast<VulkanShader*>(computeDesc.ShaderProgram);
	TRAP_ASSERT(vShader->GetVkShaderModules()[0] != VK_NULL_HANDLE, "VulkanPipeline::InitComputePipeline(): ShaderModule is nullptr!");

	m_type = RendererAPI::PipelineType::Compute;

	//Pipeline
	{
		const VkPipelineShaderStageCreateInfo stage = VulkanInits::PipelineShaderStageCreateInfo
		(
			 VK_SHADER_STAGE_COMPUTE_BIT,
			 vShader->GetVkShaderModules()[0],
			 vShader->GetReflection()->StageReflections[0].EntryPoint.data()
		);

		const VkComputePipelineCreateInfo info = VulkanInits::ComputePipelineCreateInfo
		(
			stage,
			std::dynamic_pointer_cast<VulkanRootSignature>(computeDesc.RootSignature)->GetVkPipelineLayout()
		);

		VkCall(vkCreateComputePipelines(m_device->GetVkDevice(), psoCache, 1, &info, nullptr, &m_vkPipeline));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanPipeline::InitGraphicsPipeline(const RendererAPI::PipelineDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	const auto& graphicsDesc = std::get<RendererAPI::GraphicsPipelineDesc>(desc.Pipeline);
	const VkPipelineCache psoCache = desc.Cache ?
	                                 std::dynamic_pointer_cast<VulkanPipelineCache>(desc.Cache)->GetVkPipelineCache() :
							         VK_NULL_HANDLE;

	TRAP_ASSERT(graphicsDesc.ShaderProgram, "VulkanPipeline::InitGraphicsPipeline(): ShaderProgram is nullptr!");
	TRAP_ASSERT(graphicsDesc.RootSignature, "VulkanPipeline::InitGraphicsPipeline(): RootSignature is nullptr!");

	const auto& shaderProgram = graphicsDesc.ShaderProgram;
	const auto& vertexLayout = graphicsDesc.VertexLayout;

	m_type = RendererAPI::PipelineType::Graphics;

	//Create temporary render pass for pipeline creation
	VulkanRenderer::RenderPassDesc renderPassDesc{};
	renderPassDesc.RenderTargetCount = graphicsDesc.RenderTargetCount;
	renderPassDesc.ColorFormats = graphicsDesc.ColorFormats;
	renderPassDesc.SampleCount = graphicsDesc.SampleCount;
	renderPassDesc.DepthStencilFormat = graphicsDesc.DepthStencilFormat;
	renderPassDesc.ShadingRateFormat = graphicsDesc.ShadingRateTexture ? graphicsDesc.ShadingRateTexture->GetImageFormat() : Graphics::API::ImageFormat::Undefined;
	TRAP::Scope<VulkanRenderPass> renderPass = TRAP::MakeScope<VulkanRenderPass>(m_device, renderPassDesc);

	const VulkanShader* const vShader = dynamic_cast<VulkanShader*>(shaderProgram);
	for(uint32_t i = 0; i < vShader->GetReflection()->StageReflectionCount; ++i)
	{
		TRAP_ASSERT(vShader->GetVkShaderModules()[i] != VK_NULL_HANDLE, "VulkanPipeline::InitGraphicsPipeline(): ShaderModule is nullptr!");
	}

	//Pipeline
	{
		std::size_t stageCount = 0;
		std::array<VkPipelineShaderStageCreateInfo, 5> stages{};
		for(std::size_t i = 0; i < stages.size(); ++i)
		{
			const RendererAPI::ShaderStage stageMask = static_cast<RendererAPI::ShaderStage>(BIT(static_cast<uint32_t>(i)));
			if (stageMask == (shaderProgram->GetShaderStages() & stageMask))
			{
				stages[stageCount].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				stages[stageCount].pNext = nullptr;
				stages[stageCount].flags = 0;
				stages[stageCount].pSpecializationInfo = nullptr;
				switch(stageMask)
				{
				case RendererAPI::ShaderStage::Vertex:
				{
					stages[stageCount].pName = vShader->GetReflection()->StageReflections[vShader->GetReflection()->VertexStageIndex].EntryPoint.data();
					stages[stageCount].stage = VK_SHADER_STAGE_VERTEX_BIT;
					stages[stageCount].module = vShader->GetVkShaderModules()[vShader->GetReflection()->VertexStageIndex];
					break;
				}

				case RendererAPI::ShaderStage::TessellationControl:
				{
					stages[stageCount].pName = vShader->GetReflection()->StageReflections[vShader->GetReflection()->TessellationControlStageIndex].EntryPoint.data();
					stages[stageCount].stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
					stages[stageCount].module = vShader->GetVkShaderModules()[vShader->GetReflection()->TessellationControlStageIndex];
					break;
				}

				case RendererAPI::ShaderStage::TessellationEvaluation:
				{
					stages[stageCount].pName = vShader->GetReflection()->StageReflections[vShader->GetReflection()->TessellationEvaluationStageIndex].EntryPoint.data();
					stages[stageCount].stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
					stages[stageCount].module = vShader->GetVkShaderModules()[vShader->GetReflection()->TessellationEvaluationStageIndex];
					break;
				}

				case RendererAPI::ShaderStage::Geometry:
				{
					stages[stageCount].pName = vShader->GetReflection()->StageReflections[vShader->GetReflection()->GeometryStageIndex].EntryPoint.data();
					stages[stageCount].stage = VK_SHADER_STAGE_GEOMETRY_BIT;
					stages[stageCount].module = vShader->GetVkShaderModules()[vShader->GetReflection()->GeometryStageIndex];
					break;
				}

				case RendererAPI::ShaderStage::Fragment:
				{
					stages[stageCount].pName = vShader->GetReflection()->StageReflections[vShader->GetReflection()->FragmentStageIndex].EntryPoint.data();
					stages[stageCount].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
					stages[stageCount].module = vShader->GetVkShaderModules()[vShader->GetReflection()->FragmentStageIndex];
					break;
				}

				default:
					TRAP_ASSERT(false, "VulkanPipeline::InitGraphicsPipeline(): Shader Stage is not supported!");
					break;
				}
				++stageCount;
			}
		}

		//Make sure there's a shader
		TRAP_ASSERT(stageCount != 0, "VulkanPipeline::InitGraphicsPipeline(): No Shader stage found!");

		uint32_t inputBindingCount = 0;
		std::vector<VkVertexInputBindingDescription> inputBindings(RendererAPI::GPUSettings.MaxVertexInputBindings);
		uint32_t inputAttributeCount = 0;
		std::vector<VkVertexInputAttributeDescription> inputAttributes(RendererAPI::GPUSettings.MaxVertexInputAttributes);

		//Make sure there's attributes
		if(vertexLayout != nullptr)
		{
			//Ignore everything that's beyond Max Vertex Attributes limit
			const uint32_t attribCount = TRAP::Math::Min(vertexLayout->AttributeCount, RendererAPI::GPUSettings.MaxVertexInputAttributes);
			uint32_t bindingValue = std::numeric_limits<uint32_t>::max();

			//Initial values
			for(uint32_t i = 0; i < attribCount; ++i)
			{
				const RendererAPI::VertexAttribute& attribute = vertexLayout->Attributes[i];

				if(bindingValue != attribute.Binding)
				{
					bindingValue = attribute.Binding;
					++inputBindingCount;
				}

				inputBindings[inputBindingCount - 1].binding = bindingValue;
				if (attribute.Rate == RendererAPI::VertexAttributeRate::Instance)
					inputBindings[inputBindingCount - 1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
				else
					inputBindings[inputBindingCount - 1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
				inputBindings[inputBindingCount - 1].stride += TRAP::Graphics::API::ImageFormatBitSizeOfBlock(attribute.Format) / 8;

				inputAttributes[inputAttributeCount].location = attribute.Location;
				inputAttributes[inputAttributeCount].binding = attribute.Binding;
				inputAttributes[inputAttributeCount].format = ImageFormatToVkFormat(attribute.Format);
				inputAttributes[inputAttributeCount].offset = attribute.Offset;
				++inputAttributeCount;
			}
		}

		const VkPipelineVertexInputStateCreateInfo vi = VulkanInits::PipelineVertexInputStateCreateInfo(inputBindingCount,
		                                                                                                inputBindings.data(),
																								        inputAttributeCount,
																								        inputAttributes.data());

		VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		switch(graphicsDesc.PrimitiveTopology)
		{
		case RendererAPI::PrimitiveTopology::PointList:
			topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
			break;

		case RendererAPI::PrimitiveTopology::LineList:
			topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			break;

		case RendererAPI::PrimitiveTopology::LineStrip:
			topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
			break;

		case RendererAPI::PrimitiveTopology::TriangleStrip:
			topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
			break;

		case RendererAPI::PrimitiveTopology::PatchList:
			topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
			break;

		case RendererAPI::PrimitiveTopology::TriangleList:
			topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			break;

		default:
			TRAP_ASSERT(false, "VulkanPipeline::InitGraphicsPipeline(): Primitive Topology not supported!");
			break;
		}

		const VkPipelineInputAssemblyStateCreateInfo ia = VulkanInits::PipelineInputAssemblyStateCreateInfo(topology,
		                                                                                                    VK_FALSE);

		VkPipelineTessellationStateCreateInfo ts{};
		if (static_cast<uint32_t>(shaderProgram->GetShaderStages() & RendererAPI::ShaderStage::TessellationControl) &&
			static_cast<uint32_t>(shaderProgram->GetShaderStages() & RendererAPI::ShaderStage::TessellationEvaluation))
		{
			ts = VulkanInits::PipelineTessellationStateCreateInfo
			(
				vShader->GetReflection()->StageReflections[vShader->GetReflection()->TessellationControlStageIndex].NumControlPoint
			);
		}

		const VkPipelineViewportStateCreateInfo vs = VulkanInits::PipelineViewportStateCreateInfo();

		const VkPipelineMultisampleStateCreateInfo ms = VulkanInits::PipelineMultisampleStateCreateInfo
		(
			SampleCountToVkSampleCount(graphicsDesc.SampleCount),
			graphicsDesc.SampleCount != RendererAPI::SampleCount::One ? RendererAPI::GPUSettings.SampleRateShadingSupported : VK_FALSE,
			graphicsDesc.SampleCount != RendererAPI::SampleCount::One ? 0.25f : 0.0f
		);

		const VkPipelineRasterizationStateCreateInfo rs = graphicsDesc.RasterizerState ?
		                                                  UtilToRasterizerDesc(*graphicsDesc.RasterizerState) :
													      VulkanRenderer::DefaultRasterizerDesc;

		const VkPipelineDepthStencilStateCreateInfo ds = graphicsDesc.DepthState ?
		                                                 UtilToDepthDesc(*graphicsDesc.DepthState) :
												         VulkanRenderer::DefaultDepthDesc;

		if(graphicsDesc.BlendState) //Set affected render target mask for blending (only enable blending for supported formats)
		{
			graphicsDesc.BlendState->RenderTargetMask = {};
			for(std::size_t i = 0; i < graphicsDesc.ColorFormats.size(); ++i)
			{
				const ImageFormat fmt = graphicsDesc.ColorFormats[i];
				const auto formatProps = m_device->GetPhysicalDevice()->GetVkPhysicalDeviceFormatProperties(ImageFormatToVkFormat(fmt));
				if(formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT)
					graphicsDesc.BlendState->RenderTargetMask |= static_cast<RendererAPI::BlendStateTargets>(BIT(i));
			}
		}

		std::vector<VkPipelineColorBlendAttachmentState> cbAttachments(8);
		VkPipelineColorBlendStateCreateInfo cb = graphicsDesc.BlendState ?
		                                         UtilToBlendDesc(*graphicsDesc.BlendState, cbAttachments) :
												 VulkanRenderer::DefaultBlendDesc;
		cb.attachmentCount = graphicsDesc.RenderTargetCount;

		VkPipelineFragmentShadingRateStateCreateInfoKHR fsr{};
		if(VulkanRenderer::s_shadingRate)
		{
			const std::array<VkFragmentShadingRateCombinerOpKHR, 2> rateCombiners
			{
				ShadingRateCombinerToVkFragmentShadingRateCombinerOpKHR(graphicsDesc.ShadingRateCombiners[0]),
				ShadingRateCombinerToVkFragmentShadingRateCombinerOpKHR(graphicsDesc.ShadingRateCombiners[1])
			};
			const VkExtent2D shadingRate = ShadingRateToVkExtent2D(graphicsDesc.ShadingRate);
			fsr = VulkanInits::PipelineFragmentShadingRateStateCreateInfo(shadingRate, rateCombiners);
		}

		std::vector<VkDynamicState> dynamicStates =
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR,
			VK_DYNAMIC_STATE_DEPTH_BIAS,
			VK_DYNAMIC_STATE_BLEND_CONSTANTS,
			VK_DYNAMIC_STATE_DEPTH_BOUNDS,
			VK_DYNAMIC_STATE_STENCIL_REFERENCE
		};
		if(static_cast<uint32_t>(RendererAPI::GPUSettings.ShadingRateCaps))
			dynamicStates.emplace_back(VK_DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR);
		const VkPipelineDynamicStateCreateInfo dy = VulkanInits::PipelineDynamicStateCreateInfo(dynamicStates);

		VkGraphicsPipelineCreateInfo info = VulkanInits::GraphicsPipelineCreateInfo(static_cast<uint32_t>(stageCount), stages.data(),
																					vi, ia, vs, rs, ms, ds, cb, dy,
																					std::dynamic_pointer_cast<VulkanRootSignature>(graphicsDesc.RootSignature)->GetVkPipelineLayout(),
																					renderPass->GetVkRenderPass()
		);
		if (static_cast<uint32_t>(shaderProgram->GetShaderStages() & RendererAPI::ShaderStage::TessellationControl) &&
			static_cast<uint32_t>(shaderProgram->GetShaderStages() & RendererAPI::ShaderStage::TessellationEvaluation))
			info.pTessellationState = &ts;

		if(VulkanRenderer::s_shadingRate) //Only use shading rate extension if supported
			info.pNext = &fsr; //Add shading rate extension

		VkCall(vkCreateGraphicsPipelines(m_device->GetVkDevice(), psoCache, 1, &info, nullptr, &m_vkPipeline));

		renderPass.reset();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipeline TRAP::Graphics::API::VulkanPipeline::GetVkPipeline() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_vkPipeline;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::PipelineType TRAP::Graphics::API::VulkanPipeline::GetPipelineType() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_type;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanPipeline::SetPipelineName(const std::string_view name) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	if(!VulkanRenderer::s_debugMarkerSupport)
		return;

#ifdef ENABLE_DEBUG_UTILS_EXTENSION
	VkSetObjectName(m_device->GetVkDevice(), Utils::BitCast<VkPipeline, uint64_t>(m_vkPipeline), VK_OBJECT_TYPE_PIPELINE, name);
#else
	VkSetObjectName(m_device->GetVkDevice(), Utils::BitCast<VkPipeline, uint64_t>(m_vkPipeline), VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT, name);
#endif
}