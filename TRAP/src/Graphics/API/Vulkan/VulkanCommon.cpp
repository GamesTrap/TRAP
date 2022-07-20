#include "TRAPPCH.h"
#include "VulkanCommon.h"

#include "Core/Base.h"

#include "Objects/VulkanQueue.h"
#include "VulkanRenderer.h"
#include "Graphics/RenderCommand.h"
#include "Objects/VulkanInits.h"

#ifdef ENABLE_DEBUG_UTILS_EXTENSION
void TRAP::Graphics::API::VkSetObjectName([[maybe_unused]] VkDevice device, [[maybe_unused]] const uint64_t handle,
										  [[maybe_unused]] const VkObjectType type, [[maybe_unused]] const std::string_view name)
{
#if defined(ENABLE_GRAPHICS_DEBUG)
	if (VulkanRenderer::s_debugUtilsExtension)
	{
		const VkDebugUtilsObjectNameInfoEXT nameInfo = VulkanInits::DebugUtilsObjectNameInfo(type, handle, name);
		VkCall(vkSetDebugUtilsObjectNameEXT(device, &nameInfo));
	}
#endif
}
#else
void TRAP::Graphics::API::VkSetObjectName([[maybe_unused]] VkDevice device, [[maybe_unused]] const uint64_t handle,
										  [[maybe_unused]] const VkDebugReportObjectTypeEXT type,
                                          [[maybe_unused]] const std::string_view name)
{
#if defined(ENABLE_GRAPHICS_DEBUG)
	if (VulkanRenderer::s_debugReportExtension)
	{
		const VkDebugMarkerObjectNameInfoEXT nameInfo = VulkanInits::DebugMarkerObjectNameInfo(type, handle, name);
		VkCall(vkDebugMarkerSetObjectNameEXT(device, &nameInfo));
	}
#endif
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

//Determines pipeline stages involved for given accesses
VkPipelineStageFlags TRAP::Graphics::API::DetermineVkPipelineStageFlags(const VkAccessFlags accessFlags,
                                                                        const RendererAPI::QueueType queueType)
{
	VkPipelineStageFlags flags = 0;

	switch(queueType)
	{
	case RendererAPI::QueueType::Graphics:
	{
		if ((accessFlags & (VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT)) != 0)
			flags |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;

		if((accessFlags & (VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT)) !=
		   0)
		{
			flags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
			flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			if (RendererAPI::GPUSettings.GeometryShaderSupported)
				flags |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
			if(RendererAPI::GPUSettings.TessellationSupported)
			{
				flags |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
				flags |= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
			}
			flags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			//RayTracing
			if (VulkanRenderer::s_rayTracingExtension)
				flags |= VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
		}

		if ((accessFlags & VK_ACCESS_INPUT_ATTACHMENT_READ_BIT) != 0)
			flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

		if ((accessFlags & (VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)) != 0)
			flags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		if ((accessFlags &
		     (VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)) != 0)
			flags |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

		break;
	}

	case RendererAPI::QueueType::Compute:
	{
		if ((accessFlags & (VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT)) != 0 ||
			(accessFlags & VK_ACCESS_INPUT_ATTACHMENT_READ_BIT) != 0 ||
			(accessFlags & (VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)) != 0 ||
			(accessFlags & (VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)) != 0)
			return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

		if ((accessFlags & (VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT)) !=
		    0)
			flags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

		break;
	}

	case RendererAPI::QueueType::Transfer:
		return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

	default:
		break;
	}

	//Compatible with both compute and graphics queues
	if ((accessFlags & VK_ACCESS_INDIRECT_COMMAND_READ_BIT) != 0)
		flags |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;

	if ((accessFlags & (VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT)) != 0)
		flags |= VK_PIPELINE_STAGE_TRANSFER_BIT;

	if ((accessFlags & (VK_ACCESS_HOST_READ_BIT | VK_ACCESS_HOST_WRITE_BIT)) != 0)
		flags |= VK_PIPELINE_STAGE_HOST_BIT;

	if (flags == 0)
		flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

	return flags;
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineColorBlendStateCreateInfo TRAP::Graphics::API::UtilToBlendDesc(const RendererAPI::BlendStateDesc& desc,
	                                                                     std::vector<VkPipelineColorBlendAttachmentState>& attachments)
{
	int32_t blendDescIndex = 0;

#ifdef ENABLE_GRAPHICS_DEBUG
	for(int32_t i = 0; i < 8; ++i)
	{
		if(static_cast<uint32_t>(desc.RenderTargetMask) & (1 << i))
		{
			TRAP_ASSERT(desc.SrcFactors[blendDescIndex] < RendererAPI::BlendConstant::MAX_BLEND_CONSTANTS);
			TRAP_ASSERT(desc.DstFactors[blendDescIndex] < RendererAPI::BlendConstant::MAX_BLEND_CONSTANTS);
			TRAP_ASSERT(desc.SrcAlphaFactors[blendDescIndex] < RendererAPI::BlendConstant::MAX_BLEND_CONSTANTS);
			TRAP_ASSERT(desc.DstAlphaFactors[blendDescIndex] < RendererAPI::BlendConstant::MAX_BLEND_CONSTANTS);
			TRAP_ASSERT(desc.BlendModes[blendDescIndex] < RendererAPI::BlendMode::MAX_BLEND_MODES);
			TRAP_ASSERT(desc.BlendAlphaModes[blendDescIndex] < RendererAPI::BlendMode::MAX_BLEND_MODES);
		}

		if (desc.IndependentBlend)
			++blendDescIndex;
	}

	blendDescIndex = 0;
#endif

	for(int32_t i = 0; i < 8; ++i)
	{
		if (static_cast<uint32_t>(desc.RenderTargetMask) & (1 << i))
		{
			const VkBool32 blendEnable =
			(
				VkBlendConstantTranslator[static_cast<uint32_t>(desc.SrcFactors[blendDescIndex])] != VK_BLEND_FACTOR_ONE ||
			    VkBlendConstantTranslator[static_cast<uint32_t>(desc.DstFactors[blendDescIndex])] != VK_BLEND_FACTOR_ZERO ||
			    VkBlendConstantTranslator[static_cast<uint32_t>(desc.SrcAlphaFactors[blendDescIndex])] != VK_BLEND_FACTOR_ONE ||
			    VkBlendConstantTranslator[static_cast<uint32_t>(desc.DstAlphaFactors[blendDescIndex])] != VK_BLEND_FACTOR_ZERO
			);

			attachments[i].blendEnable = blendEnable;
			attachments[i].colorWriteMask = desc.Masks[blendDescIndex];
			attachments[i].srcColorBlendFactor = VkBlendConstantTranslator[static_cast<uint32_t>(desc.SrcFactors[blendDescIndex])];
			attachments[i].dstColorBlendFactor = VkBlendConstantTranslator[static_cast<uint32_t>(desc.DstFactors[blendDescIndex])];
			attachments[i].colorBlendOp = VkBlendOpTranslator[static_cast<uint32_t>(desc.BlendModes[blendDescIndex])];
			attachments[i].srcAlphaBlendFactor = VkBlendConstantTranslator[static_cast<uint32_t>(desc.SrcAlphaFactors[blendDescIndex])];
			attachments[i].dstAlphaBlendFactor = VkBlendConstantTranslator[static_cast<uint32_t>(desc.DstAlphaFactors[blendDescIndex])];
			attachments[i].alphaBlendOp = VkBlendOpTranslator[static_cast<uint32_t>(desc.BlendAlphaModes[blendDescIndex])];
		}

		if (desc.IndependentBlend)
			++blendDescIndex;
	}

	return VulkanInits::PipelineColorBlendStateCreateInfo(attachments);
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineDepthStencilStateCreateInfo TRAP::Graphics::API::UtilToDepthDesc(const RendererAPI::DepthStateDesc& desc)
{
	TRAP_ASSERT(desc.DepthFunc < RendererAPI::CompareMode::MAX_COMPARE_MODES);
	TRAP_ASSERT(desc.StencilFrontFunc < RendererAPI::CompareMode::MAX_COMPARE_MODES);
	TRAP_ASSERT(desc.StencilFrontFail < RendererAPI::StencilOp::MAX_STENCIL_OPS);
	TRAP_ASSERT(desc.DepthFrontFail < RendererAPI::StencilOp::MAX_STENCIL_OPS);
	TRAP_ASSERT(desc.StencilFrontPass < RendererAPI::StencilOp::MAX_STENCIL_OPS);
	TRAP_ASSERT(desc.StencilBackFunc < RendererAPI::CompareMode::MAX_COMPARE_MODES);
	TRAP_ASSERT(desc.StencilBackFail < RendererAPI::StencilOp::MAX_STENCIL_OPS);
	TRAP_ASSERT(desc.DepthBackFail < RendererAPI::StencilOp::MAX_STENCIL_OPS);
	TRAP_ASSERT(desc.StencilBackPass < RendererAPI::StencilOp::MAX_STENCIL_OPS);

	VkPipelineDepthStencilStateCreateInfo ds{};
	ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	ds.pNext = nullptr;
	ds.flags = 0;
	ds.depthTestEnable = desc.DepthTest ? VK_TRUE : VK_FALSE;
	ds.depthWriteEnable = desc.DepthWrite ? VK_TRUE : VK_FALSE;
	ds.depthCompareOp = VkComparisonFuncTranslator[static_cast<uint32_t>(desc.DepthFunc)];
	ds.depthBoundsTestEnable = VK_FALSE;
	ds.stencilTestEnable = desc.StencilTest ? VK_TRUE : VK_FALSE;

	ds.front.failOp = VkStencilOpTranslator[static_cast<uint32_t>(desc.StencilFrontFail)];
	ds.front.passOp = VkStencilOpTranslator[static_cast<uint32_t>(desc.StencilFrontPass)];
	ds.front.depthFailOp = VkStencilOpTranslator[static_cast<uint32_t>(desc.DepthFrontFail)];
	ds.front.compareOp = static_cast<VkCompareOp>(desc.StencilFrontFunc);
	ds.front.compareMask = desc.StencilReadMask;
	ds.front.writeMask = desc.StencilWriteMask;
	ds.front.reference = 0;

	ds.back.failOp = VkStencilOpTranslator[static_cast<uint32_t>(desc.StencilBackFail)];
	ds.back.passOp = VkStencilOpTranslator[static_cast<uint32_t>(desc.StencilBackPass)];
	ds.back.depthFailOp = VkStencilOpTranslator[static_cast<uint32_t>(desc.DepthBackFail)];
	ds.back.compareOp = VkComparisonFuncTranslator[static_cast<uint32_t>(desc.StencilBackFail)];
	ds.back.compareMask = desc.StencilReadMask;
	ds.back.writeMask = desc.StencilWriteMask;
	ds.back.reference = 0;

	ds.minDepthBounds = 0;
	ds.maxDepthBounds = 1;

	return ds;
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineRasterizationStateCreateInfo TRAP::Graphics::API::UtilToRasterizerDesc(const RendererAPI::RasterizerStateDesc& desc)
{
	TRAP_ASSERT(desc.FillMode < RendererAPI::FillMode::MAX_FILL_MODES);
	TRAP_ASSERT(desc.CullMode < RendererAPI::CullMode::MAX_CULL_MODES);
	TRAP_ASSERT(desc.FrontFace == RendererAPI::FrontFace::CounterClockwise ||
	            desc.FrontFace == RendererAPI::FrontFace::Clockwise);

	VkPipelineRasterizationStateCreateInfo rs{};
	rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rs.pNext = nullptr;
	rs.flags = 0;
	rs.depthClampEnable = desc.DepthClampEnable ? VK_TRUE : VK_FALSE;
	rs.rasterizerDiscardEnable = VK_FALSE;
	rs.polygonMode = VkFillModeTranslator[static_cast<uint32_t>(desc.FillMode)];
	rs.cullMode = VkCullModeTranslator[static_cast<uint32_t>(desc.CullMode)];
	rs.frontFace = VkFrontFaceTranslator[static_cast<uint32_t>(desc.FrontFace)];
	rs.depthBiasEnable = (desc.DepthBias != 0) ? VK_TRUE : VK_FALSE;
	rs.depthBiasConstantFactor = static_cast<float>(desc.DepthBias);
	rs.depthBiasClamp = 0.0f;
	rs.depthBiasSlopeFactor = desc.SlopeScaledDepthBias;
	rs.lineWidth = 1.0f;

	return rs;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::UtilGetPlanarVkImageMemoryRequirement(VkDevice device, VkImage image,
																const uint32_t planesCount,
                                                                VkMemoryRequirements& memReq,
                                                                std::vector<uint64_t>& planesOffsets)
{
	memReq = {};

	VkImagePlaneMemoryRequirementsInfo imagePlaneMemReqInfo;
	imagePlaneMemReqInfo.sType = VK_STRUCTURE_TYPE_IMAGE_PLANE_MEMORY_REQUIREMENTS_INFO;
	imagePlaneMemReqInfo.pNext = nullptr;

	VkImageMemoryRequirementsInfo2 imagePlaneMemReqInfo2;
	imagePlaneMemReqInfo2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2;
	imagePlaneMemReqInfo2.pNext = &imagePlaneMemReqInfo;
	imagePlaneMemReqInfo2.image = image;

	VkMemoryDedicatedRequirements memDedicatedReq;
	memDedicatedReq.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS;
	memDedicatedReq.pNext = nullptr;
	VkMemoryRequirements2 memReq2;
	memReq2.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
	memReq2.pNext = &memDedicatedReq;

	for(uint32_t i = 0; i < planesCount; ++i)
	{
		imagePlaneMemReqInfo.planeAspect = static_cast<VkImageAspectFlagBits>(VK_IMAGE_ASPECT_PLANE_0_BIT << i);
		vkGetImageMemoryRequirements2(device, &imagePlaneMemReqInfo2, &memReq2);

		planesOffsets[i] += memReq.size;
		memReq.alignment = TRAP::Math::Max(memReq2.memoryRequirements.alignment, memReq.alignment);
		memReq.size += ((memReq2.memoryRequirements.size + memReq2.memoryRequirements.alignment - 1) /
		                memReq2.memoryRequirements.alignment) * memReq2.memoryRequirements.alignment;
		memReq.memoryTypeBits |= memReq2.memoryRequirements.memoryTypeBits;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ImageFormat TRAP::Graphics::API::VulkanGetRecommendedSwapchainFormat(const bool /*HDR*/,
																						  const bool SRGB)
{
#if !defined(TRAP_PLATFORM_ANDROID)
	if(SRGB)
		return TRAP::Graphics::API::ImageFormat::B8G8R8A8_SRGB;

	return TRAP::Graphics::API::ImageFormat::B8G8R8A8_UNORM;
#else
	if(SRGB)
		return TRAP::Graphics::API::ImageFormat::R8G8B8A8_SRGB;

	return TRAP::Graphics::API::ImageFormat::R8G8B8A8_UNORM;
#endif
}