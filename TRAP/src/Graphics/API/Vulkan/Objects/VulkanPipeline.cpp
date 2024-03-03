#include "TRAPPCH.h"
#include "VulkanPipeline.h"

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
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_device, "VulkanPipeline(): Vulkan Device is nullptr!");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanPipelinePrefix, "Creating Pipeline");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

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
		TRAP::Graphics::API::VkSetObjectName(m_device->GetVkDevice(), std::bit_cast<u64>(m_vkPipeline), VK_OBJECT_TYPE_PIPELINE, desc.Name);
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanPipeline::~VulkanPipeline()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanPipelinePrefix, "Destroying Pipeline");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	vkDestroyPipeline(m_device->GetVkDevice(), m_vkPipeline, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanPipeline::InitComputePipeline(const RendererAPI::PipelineDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const auto& computeDesc = std::get<RendererAPI::ComputePipelineDesc>(desc.Pipeline);
	VkPipelineCache psoCache = desc.Cache ?
	                           std::dynamic_pointer_cast<VulkanPipelineCache>(desc.Cache)->GetVkPipelineCache() :
							   VK_NULL_HANDLE;

	TRAP_ASSERT(computeDesc.ShaderProgram, "VulkanPipeline::InitComputePipeline(): ShaderProgram is nullptr!");
	TRAP_ASSERT(computeDesc.RootSignature, "VulkanPipeline::InitComputePipeline(): RootSignature is nullptr!");
	const VulkanShader* const vShader = dynamic_cast<VulkanShader*>(computeDesc.ShaderProgram);
	TRAP_ASSERT(!vShader->GetVkShaderModules().empty(), "VulkanPipeline::InitComputePipeline(): ShaderModules is empty!");
	TRAP_ASSERT(vShader->GetVkShaderModules()[0] != VK_NULL_HANDLE, "VulkanPipeline::InitComputePipeline(): ShaderModule is nullptr!");
	TRAP_ASSERT(vShader->GetReflection() != nullptr, "VulkanPipeline::InitComputePipeline(): ShaderReflection is nullptr!");
	TRAP_ASSERT(!vShader->GetReflection()->StageReflections.empty(), "VulkanPipeline::InitComputePipeline(): StageReflections is empty!");

	m_type = RendererAPI::PipelineType::Compute;

	//Pipeline
	{
		const VkPipelineShaderStageCreateInfo stage = VulkanInits::PipelineShaderStageCreateInfo
		(
			 VK_SHADER_STAGE_COMPUTE_BIT,
			 vShader->GetVkShaderModules()[0],
			 vShader->GetReflection()->StageReflections[0].EntryPoint
		);

		const VkComputePipelineCreateInfo info = VulkanInits::ComputePipelineCreateInfo
		(
			stage,
			std::dynamic_pointer_cast<VulkanRootSignature>(computeDesc.RootSignature)->GetVkPipelineLayout()
		);

		VkCall(vkCreateComputePipelines(m_device->GetVkDevice(), psoCache, 1, &info, nullptr, &m_vkPipeline));
		TRAP_ASSERT(m_vkPipeline, "VulkanPipeline::InitComputePipeline(): Vulkan Pipeline is nullptr!");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

namespace
{
	[[nodiscard]] TRAP::Scope<TRAP::Graphics::API::VulkanRenderPass> CreateTemporaryRenderPass(const TRAP::Ref<TRAP::Graphics::API::VulkanDevice>& device,
	                                                                                           const TRAP::Graphics::RendererAPI::GraphicsPipelineDesc& gpd)
	{
		TRAP::Graphics::API::VulkanRenderer::RenderPassDesc renderPassDesc{};
		renderPassDesc.RenderTargetCount = gpd.RenderTargetCount;
		renderPassDesc.ColorFormats = gpd.ColorFormats;
		renderPassDesc.SampleCount = gpd.SampleCount;
		renderPassDesc.DepthStencilFormat = gpd.DepthStencilFormat;
		renderPassDesc.ShadingRateFormat = gpd.ShadingRateTexture ? gpd.ShadingRateTexture->GetImageFormat() : TRAP::Graphics::API::ImageFormat::Undefined;

		return TRAP::MakeScope<TRAP::Graphics::API::VulkanRenderPass>(device, renderPassDesc);
	}

	[[nodiscard]] std::vector<VkPipelineShaderStageCreateInfo> GetGraphicsShaderStageCreateInfos(const TRAP::Graphics::API::VulkanShader& vkShader)
	{
		static constexpr std::array<TRAP::Graphics::RendererAPI::ShaderStage, 5> GraphicsShaderStages
		{
			TRAP::Graphics::RendererAPI::ShaderStage::Vertex,
			TRAP::Graphics::RendererAPI::ShaderStage::TessellationControl,
			TRAP::Graphics::RendererAPI::ShaderStage::TessellationEvaluation,
			TRAP::Graphics::RendererAPI::ShaderStage::Geometry,
			TRAP::Graphics::RendererAPI::ShaderStage::Fragment
		};

		std::vector<VkPipelineShaderStageCreateInfo> stageInfos{};
		stageInfos.reserve(GraphicsShaderStages.size());

		VkPipelineShaderStageCreateInfo stageInfo{};
		stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

		for(const TRAP::Graphics::RendererAPI::ShaderStage stage : GraphicsShaderStages)
		{
			if((vkShader.GetShaderStages() & stage) != stage)
				continue;

			stageInfo.stage = TRAP::Graphics::API::ShaderStageToVkShaderStageFlagBits(stage);

			switch(stage)
			{
			case TRAP::Graphics::RendererAPI::ShaderStage::Vertex:
			{
				const u32 stageIndex = vkShader.GetReflection()->VertexStageIndex.value();
				stageInfo.pName = vkShader.GetReflection()->StageReflections[stageIndex].EntryPoint.data();
				stageInfo.module = vkShader.GetVkShaderModules()[stageIndex];
				break;
			}

			case TRAP::Graphics::RendererAPI::ShaderStage::TessellationControl:
			{
				const u32 stageIndex = vkShader.GetReflection()->TessellationControlStageIndex.value();
				stageInfo.pName = vkShader.GetReflection()->StageReflections[stageIndex].EntryPoint.data();
				stageInfo.module = vkShader.GetVkShaderModules()[stageIndex];
				break;
			}

			case TRAP::Graphics::RendererAPI::ShaderStage::TessellationEvaluation:
			{
				const u32 stageIndex = vkShader.GetReflection()->TessellationEvaluationStageIndex.value();
				stageInfo.pName = vkShader.GetReflection()->StageReflections[stageIndex].EntryPoint.data();
				stageInfo.module = vkShader.GetVkShaderModules()[stageIndex];
				break;
			}

			case TRAP::Graphics::RendererAPI::ShaderStage::Geometry:
			{
				const u32 stageIndex = vkShader.GetReflection()->GeometryStageIndex.value();
				stageInfo.pName = vkShader.GetReflection()->StageReflections[stageIndex].EntryPoint.data();
				stageInfo.module = vkShader.GetVkShaderModules()[stageIndex];
				break;
			}

			case TRAP::Graphics::RendererAPI::ShaderStage::Fragment:
			{
				const u32 stageIndex = vkShader.GetReflection()->FragmentStageIndex.value();
				stageInfo.pName = vkShader.GetReflection()->StageReflections[stageIndex].EntryPoint.data();
				stageInfo.module = vkShader.GetVkShaderModules()[stageIndex];
				break;
			}

			default:
				std::unreachable();
			}

			stageInfos.push_back(stageInfo);
		}

		//Make sure there's at least one shader stage
		TRAP_ASSERT(!stageInfos.empty(), "VulkanPipeline::GetGraphicsShaderStageCreateInfos(): No Shader stage found!");

		return stageInfos;
	}

	[[nodiscard]] std::vector<VkVertexInputBindingDescription> GetVertexInputBindingDescriptions(const TRAP::Graphics::RendererAPI::VertexLayout* const vertexLayout)
	{
		//Make sure there are attributes
		if(vertexLayout == nullptr)
			return {};

		std::vector<VkVertexInputBindingDescription> bindingDescs{};
		bindingDescs.reserve(TRAP::Graphics::RendererAPI::GPUSettings.MaxVertexInputBindings);

		//Ignore everything that's beyond MaxVertexInputAttributes limit
		const u32 attribCount = TRAP::Math::Min(vertexLayout->AttributeCount, TRAP::Graphics::RendererAPI::GPUSettings.MaxVertexInputAttributes);
		u32 bindingValue = std::numeric_limits<u32>::max();

		//Initial values
		VkVertexInputBindingDescription* currInputBindingDesc = nullptr;
		for(u32 i = 0; i < attribCount; ++i)
		{
			const TRAP::Graphics::RendererAPI::VertexAttribute& attribute = vertexLayout->Attributes[i];

			//Use a new bindingDescs entry if current binding does match, else use last binding
			if(bindingValue != attribute.Binding)
			{
				bindingValue = attribute.Binding;
				currInputBindingDesc = &bindingDescs.emplace_back();
			}

			currInputBindingDesc->binding = bindingValue;
			currInputBindingDesc->inputRate = TRAP::Graphics::API::VkVertexInputRateTranslator[std::to_underlying(attribute.Rate)];
			currInputBindingDesc->stride += TRAP::Graphics::API::ImageFormatBitSizeOfBlock(attribute.Format) / 8;
		}

		return bindingDescs;
	}

	[[nodiscard]] std::vector<VkVertexInputAttributeDescription> GetVertexInputAttributeDescriptions(const TRAP::Graphics::RendererAPI::VertexLayout* const vertexLayout)
	{
		//Make sure there are attributes
		if(vertexLayout == nullptr)
			return {};

		//Ignore everything that's beyond MaxVertexInputAttributes limit
		const u32 attribCount = TRAP::Math::Min(vertexLayout->AttributeCount, TRAP::Graphics::RendererAPI::GPUSettings.MaxVertexInputAttributes);

		std::vector<VkVertexInputAttributeDescription> attributeDescs(attribCount);

		//Initial values
		for(u32 i = 0; i < attribCount; ++i)
		{
			const TRAP::Graphics::RendererAPI::VertexAttribute& attribute = vertexLayout->Attributes[i];

			attributeDescs[i].location = attribute.Location;
			attributeDescs[i].binding = attribute.Binding;
			attributeDescs[i].format = ImageFormatToVkFormat(attribute.Format);
			attributeDescs[i].offset = attribute.Offset;
		}

		return attributeDescs;
	}

	[[nodiscard]] TRAP::Optional<VkPipelineTessellationStateCreateInfo> GetVkPipelineTessellationStateCreateInfo(const TRAP::Graphics::API::VulkanShader& vkShader)
	{
		using RendererAPI = TRAP::Graphics::RendererAPI;

		if (((vkShader.GetShaderStages() & RendererAPI::ShaderStage::TessellationControl) != RendererAPI::ShaderStage::None) &&
			((vkShader.GetShaderStages() & RendererAPI::ShaderStage::TessellationEvaluation) != RendererAPI::ShaderStage::None))
		{
			return TRAP::Graphics::API::VulkanInits::PipelineTessellationStateCreateInfo
			(
				vkShader.GetReflection()->StageReflections[vkShader.GetReflection()->TessellationControlStageIndex.value()].NumControlPoint
			);
		}

		return TRAP::NullOpt;
	}

	void SetupBlendStateRenderTargetMasking(const TRAP::Graphics::RendererAPI::GraphicsPipelineDesc& gpd,
	                                        const TRAP::Graphics::API::VulkanPhysicalDevice& device)
	{
		//Set affected render target mask for blending (only enable blending for supported formats)
		if(gpd.BlendState == nullptr)
			return;

		gpd.BlendState->RenderTargetMask = {};
		for(usize i = 0; i < gpd.ColorFormats.size(); ++i)
		{
			const TRAP::Graphics::API::ImageFormat fmt = gpd.ColorFormats[i];
			const auto formatProps = device.GetVkPhysicalDeviceFormatProperties(fmt);
			if((formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT) != 0u)
				gpd.BlendState->RenderTargetMask |= static_cast<TRAP::Graphics::RendererAPI::BlendStateTargets>(BIT(NumericCast<u32>(i)));
		}
	}

	[[nodiscard]] VkPipelineColorBlendStateCreateInfo GetVkPipelineColorBlendStateCreateInfo(const TRAP::Graphics::RendererAPI::GraphicsPipelineDesc& gpd,
	                                                                                         std::vector<VkPipelineColorBlendAttachmentState>& cbAttachments)
	{
		if(gpd.BlendState == nullptr)
			return TRAP::Graphics::API::VulkanRenderer::DefaultBlendDesc;

		VkPipelineColorBlendStateCreateInfo cb = TRAP::Graphics::API::UtilToBlendDesc(*gpd.BlendState, cbAttachments);
		cb.attachmentCount = gpd.RenderTargetCount;

		return cb;
	}

	[[nodiscard]] TRAP::Optional<VkPipelineFragmentShadingRateStateCreateInfoKHR> GetVkPipelineFragmentShadingRateStateCreateInfoKHR(const TRAP::Graphics::RendererAPI::GraphicsPipelineDesc& gpd)
	{
		if(!TRAP::Graphics::API::VulkanRenderer::s_shadingRate)
			return TRAP::NullOpt;

		const std::array<VkFragmentShadingRateCombinerOpKHR, 2> rateCombiners
		{
			TRAP::Graphics::API::ShadingRateCombinerToVkFragmentShadingRateCombinerOpKHR(std::get<0>(gpd.ShadingRateCombiners)),
			TRAP::Graphics::API::ShadingRateCombinerToVkFragmentShadingRateCombinerOpKHR(std::get<1>(gpd.ShadingRateCombiners))
		};
		const VkExtent2D shadingRate = TRAP::Graphics::API::ShadingRateToVkExtent2D(gpd.ShadingRate);

		return TRAP::Graphics::API::VulkanInits::PipelineFragmentShadingRateStateCreateInfo(shadingRate, rateCombiners);
	}

	[[nodiscard]] std::pair<VkPipelineDynamicStateCreateInfo, std::vector<VkDynamicState>> GetVkPipelineDynamicStateCreateInfo()
	{
		std::vector<VkDynamicState> dynamicStates =
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR,
			VK_DYNAMIC_STATE_DEPTH_BIAS,
			VK_DYNAMIC_STATE_BLEND_CONSTANTS,
			VK_DYNAMIC_STATE_DEPTH_BOUNDS,
			VK_DYNAMIC_STATE_STENCIL_REFERENCE
		};
		if(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateCaps != TRAP::Graphics::RendererAPI::ShadingRateCaps::NotSupported)
			dynamicStates.emplace_back(VK_DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR);

		return {TRAP::Graphics::API::VulkanInits::PipelineDynamicStateCreateInfo(dynamicStates), std::move(dynamicStates)};
	}
}

void TRAP::Graphics::API::VulkanPipeline::InitGraphicsPipeline(const RendererAPI::PipelineDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const auto& graphicsDesc = std::get<RendererAPI::GraphicsPipelineDesc>(desc.Pipeline);
	VkPipelineCache psoCache = desc.Cache ?
	                           std::dynamic_pointer_cast<VulkanPipelineCache>(desc.Cache)->GetVkPipelineCache() :
							   VK_NULL_HANDLE;
	const VulkanShader* const vShader = dynamic_cast<const VulkanShader* const>(graphicsDesc.ShaderProgram);

	TRAP_ASSERT(graphicsDesc.ShaderProgram, "VulkanPipeline::InitGraphicsPipeline(): ShaderProgram is nullptr!");
	TRAP_ASSERT(graphicsDesc.RootSignature, "VulkanPipeline::InitGraphicsPipeline(): RootSignature is nullptr!");
	TRAP_ASSERT(!vShader->GetVkShaderModules().empty(), "VulkanPipeline::InitGraphicsPipeline(): ShaderModules is empty!");
	TRAP_ASSERT(vShader->GetReflection() != nullptr, "VulkanPipeline::InitGraphicsPipeline(): ShaderReflection is nullptr!");
	TRAP_ASSERT(!vShader->GetReflection()->StageReflections.empty(), "VulkanPipeline::InitGraphicsPipeline(): StageReflections is empty!");
	TRAP_ASSERT(vShader->GetReflection()->StageReflections.size() == vShader->GetVkShaderModules().size(), "VulkanPipeline::InitGraphicsPipeline(): ShaderModule count doesn't match with reflection!");
	for(u32 i = 0; i < vShader->GetReflection()->StageReflections.size(); ++i)
		TRAP_ASSERT(vShader->GetVkShaderModules()[i] != VK_NULL_HANDLE, "VulkanPipeline::InitGraphicsPipeline(): ShaderModule is nullptr!");

	m_type = RendererAPI::PipelineType::Graphics;

	//Pipeline
	{
		const std::vector<VkPipelineShaderStageCreateInfo> stages = GetGraphicsShaderStageCreateInfos(*vShader);
		const std::vector<VkVertexInputBindingDescription> inputBindings = GetVertexInputBindingDescriptions(graphicsDesc.VertexLayout.get());
		const std::vector<VkVertexInputAttributeDescription> inputAttributes = GetVertexInputAttributeDescriptions(graphicsDesc.VertexLayout.get());

		const VkPipelineVertexInputStateCreateInfo vi = VulkanInits::PipelineVertexInputStateCreateInfo(inputBindings, inputAttributes);

		const VkPrimitiveTopology topology = TRAP::Graphics::API::PrimitiveTopologyToVkPrimitiveTopology(graphicsDesc.PrimitiveTopology);
		const VkPipelineInputAssemblyStateCreateInfo ia = VulkanInits::PipelineInputAssemblyStateCreateInfo(topology, VK_FALSE);

		const TRAP::Optional<VkPipelineTessellationStateCreateInfo> ts = GetVkPipelineTessellationStateCreateInfo(*vShader);

		const VkPipelineViewportStateCreateInfo vs = VulkanInits::PipelineViewportStateCreateInfo();

		const VkPipelineMultisampleStateCreateInfo ms = VulkanInits::PipelineMultisampleStateCreateInfo
		(
			SampleCountToVkSampleCount(graphicsDesc.SampleCount),
			graphicsDesc.SampleCount != RendererAPI::SampleCount::One ? RendererAPI::GPUSettings.SampleRateShadingSupported : false,
			graphicsDesc.SampleCount != RendererAPI::SampleCount::One ? 0.25f : 0.0f
		);

		const VkPipelineRasterizationStateCreateInfo rs = graphicsDesc.RasterizerState ?
		                                                  UtilToRasterizerDesc(*graphicsDesc.RasterizerState) :
													      VulkanRenderer::DefaultRasterizerDesc;

		const VkPipelineDepthStencilStateCreateInfo ds = graphicsDesc.DepthState ?
		                                                 UtilToDepthDesc(*graphicsDesc.DepthState) :
												         VulkanRenderer::DefaultDepthDesc;

		SetupBlendStateRenderTargetMasking(graphicsDesc, m_device->GetPhysicalDevice());

		std::vector<VkPipelineColorBlendAttachmentState> cbAttachments{};
		const VkPipelineColorBlendStateCreateInfo cb = GetVkPipelineColorBlendStateCreateInfo(graphicsDesc, cbAttachments);

		const TRAP::Optional<VkPipelineFragmentShadingRateStateCreateInfoKHR> fsr = GetVkPipelineFragmentShadingRateStateCreateInfoKHR(graphicsDesc);

		const auto [dy, dynamicState] = GetVkPipelineDynamicStateCreateInfo();

		const TRAP::Scope<VulkanRenderPass> renderPass = CreateTemporaryRenderPass(m_device, graphicsDesc);
		VkGraphicsPipelineCreateInfo info = VulkanInits::GraphicsPipelineCreateInfo(stages,
																					vi, ia, vs, rs, ms, ds, cb, dy,
																					std::dynamic_pointer_cast<VulkanRootSignature>(graphicsDesc.RootSignature)->GetVkPipelineLayout(),
																					renderPass->GetVkRenderPass()
		);
		if(ts)
			info.pTessellationState = &ts.Value();
		if(fsr) //Only use shading rate extension if supported
			info.pNext = &fsr.Value();

		VkCall(vkCreateGraphicsPipelines(m_device->GetVkDevice(), psoCache, 1, &info, nullptr, &m_vkPipeline));
		TRAP_ASSERT(m_vkPipeline, "VulkanPipeline::InitGraphicsPipeline(): Vulkan Pipeline is nullptr!");
	}
}
