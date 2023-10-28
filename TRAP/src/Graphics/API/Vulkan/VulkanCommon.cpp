#include "TRAPPCH.h"
#include "VulkanCommon.h"

#include "Core/Base.h"

#include "VulkanRenderer.h"
#include "Objects/VulkanInits.h"

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkQueueFlags TRAP::Graphics::API::QueueTypeToVkQueueFlags(const RendererAPI::QueueType queueType) noexcept
{
	switch(queueType)
	{
	case RendererAPI::QueueType::Graphics:
		return VK_QUEUE_GRAPHICS_BIT;

	case RendererAPI::QueueType::Transfer:
		return VK_QUEUE_TRANSFER_BIT;

	case RendererAPI::QueueType::Compute:
		return VK_QUEUE_COMPUTE_BIT;

	default:
		TRAP_ASSERT(false, "QueueTypeToVkQueueFlags(): Invalid Queue Type");
		return VK_QUEUE_FLAG_BITS_MAX_ENUM;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkSamplerMipmapMode TRAP::Graphics::API::MipMapModeToVkMipMapMode(const RendererAPI::MipMapMode mipMapMode) noexcept
{
	switch (mipMapMode)
	{
	case RendererAPI::MipMapMode::Nearest:
		return VK_SAMPLER_MIPMAP_MODE_NEAREST;

	case RendererAPI::MipMapMode::Linear:
		return VK_SAMPLER_MIPMAP_MODE_LINEAR;

	default:
		TRAP_ASSERT(false, "MipMapModeToVkMipMapMode(): Invalid Mip Map Mode");
		return VK_SAMPLER_MIPMAP_MODE_MAX_ENUM;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkDescriptorType TRAP::Graphics::API::DescriptorTypeToVkDescriptorType(const RendererAPI::DescriptorType type) noexcept
{
	switch(type)
	{
	case RendererAPI::DescriptorType::Undefined:
		TRAP_ASSERT(false, "DescriptorTypeToVkDescriptorType(): Invalid DescriptorInfo Type");
		return VK_DESCRIPTOR_TYPE_MAX_ENUM;

	case RendererAPI::DescriptorType::Sampler:
		return VK_DESCRIPTOR_TYPE_SAMPLER;

	case RendererAPI::DescriptorType::Texture:
		return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

	case RendererAPI::DescriptorType::UniformBuffer:
		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

	case RendererAPI::DescriptorType::RWTexture:
		return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

	case RendererAPI::DescriptorType::Buffer:
	case RendererAPI::DescriptorType::RWBuffer:
		return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

	case RendererAPI::DescriptorType::InputAttachment:
		return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;

	case RendererAPI::DescriptorType::TexelBuffer:
		return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;

	case RendererAPI::DescriptorType::RWTexelBuffer:
		return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;

	case RendererAPI::DescriptorType::CombinedImageSampler:
		return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

	//RayTracing
	case RendererAPI::DescriptorType::RayTracing:
		return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;

	default:
		TRAP_ASSERT(false, "DescriptorTypeToVkDescriptorType(): Invalid DescriptorInfo Type");
		return VK_DESCRIPTOR_TYPE_MAX_ENUM;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkShaderStageFlags TRAP::Graphics::API::ShaderStageToVkShaderStageFlags(const RendererAPI::ShaderStage stages) noexcept
{
	VkShaderStageFlags res = 0;

	if ((stages & RendererAPI::ShaderStage::AllGraphics) != RendererAPI::ShaderStage::None)
		return VK_SHADER_STAGE_ALL_GRAPHICS;

	if ((stages & RendererAPI::ShaderStage::Vertex) != RendererAPI::ShaderStage::None)
		res |= VK_SHADER_STAGE_VERTEX_BIT;
	if ((stages & RendererAPI::ShaderStage::Geometry) != RendererAPI::ShaderStage::None)
		res |= VK_SHADER_STAGE_GEOMETRY_BIT;
	if ((stages & RendererAPI::ShaderStage::TessellationEvaluation) != RendererAPI::ShaderStage::None)
		res |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	if ((stages & RendererAPI::ShaderStage::TessellationControl) != RendererAPI::ShaderStage::None)
		res |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	if ((stages & RendererAPI::ShaderStage::Fragment) != RendererAPI::ShaderStage::None)
		res |= VK_SHADER_STAGE_FRAGMENT_BIT;
	if ((stages & RendererAPI::ShaderStage::Compute) != RendererAPI::ShaderStage::None)
		res |= VK_SHADER_STAGE_COMPUTE_BIT;
	//RayTracing
	if ((stages & RendererAPI::ShaderStage::RayTracing) != RendererAPI::ShaderStage::None)
		res |= (VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR |
			    VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR |
			    VK_SHADER_STAGE_INTERSECTION_BIT_KHR | VK_SHADER_STAGE_CALLABLE_BIT_KHR);

	TRAP_ASSERT(res != 0, "ShaderStageToVkShaderStageFlags(): Invalid ShaderStage combination");
	return res;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef ENABLE_DEBUG_UTILS_EXTENSION
void TRAP::Graphics::API::VkSetObjectName([[maybe_unused]] VkDevice device, [[maybe_unused]] const u64 handle,
										  [[maybe_unused]] const VkObjectType type, [[maybe_unused]] const std::string_view name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

#ifdef ENABLE_GRAPHICS_DEBUG
	if (VulkanRenderer::s_debugUtilsExtension)
	{
		const VkDebugUtilsObjectNameInfoEXT nameInfo = VulkanInits::DebugUtilsObjectNameInfo(type, handle, name);
		VkCall(vkSetDebugUtilsObjectNameEXT(device, &nameInfo));
	}
#endif /*ENABLE_GRAPHICS_DEBUG*/
}
#else
void TRAP::Graphics::API::VkSetObjectName([[maybe_unused]] VkDevice device, [[maybe_unused]] const u64 handle,
										  [[maybe_unused]] const VkDebugReportObjectTypeEXT type,
                                          [[maybe_unused]] const std::string_view name)
{
#ifdef ENABLE_GRAPHICS_DEBUG
	if (VulkanRenderer::s_debugReportExtension)
	{
		const VkDebugMarkerObjectNameInfoEXT nameInfo = VulkanInits::DebugMarkerObjectNameInfo(type, handle, name);
		VkCall(vkDebugMarkerSetObjectNameEXT(device, &nameInfo));
	}
#endif /*ENABLE_GRAPHICS_DEBUG*/
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

//Determines pipeline stages involved for given accesses
[[nodiscard]] VkPipelineStageFlags TRAP::Graphics::API::DetermineVkPipelineStageFlags(const VkAccessFlags accessFlags,
                                                                                      const RendererAPI::QueueType queueType) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

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

[[nodiscard]] VkQueryType TRAP::Graphics::API::QueryTypeToVkQueryType(const RendererAPI::QueryType type) noexcept
{
	switch(type)
	{
	case RendererAPI::QueryType::Timestamp:
		return VK_QUERY_TYPE_TIMESTAMP;

	case RendererAPI::QueryType::PipelineStatistics:
		return VK_QUERY_TYPE_PIPELINE_STATISTICS;

	case RendererAPI::QueryType::Occlusion:
		return VK_QUERY_TYPE_OCCLUSION;

	default:
		TRAP_ASSERT(false, "QueryTypeToVkQueryType(): Invalid query heap type");
		return VK_QUERY_TYPE_MAX_ENUM;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkPipelineColorBlendStateCreateInfo TRAP::Graphics::API::UtilToBlendDesc(const RendererAPI::BlendStateDesc& desc,
	                                                                                   std::vector<VkPipelineColorBlendAttachmentState>& attachments)
{
	u32 blendDescIndex = 0;

#ifdef ENABLE_GRAPHICS_DEBUG
	for(u32 i = 0; i < 8; ++i)
	{
		if((std::to_underlying(desc.RenderTargetMask) & BIT(i)) != 0u)
		{
			TRAP_ASSERT(desc.SrcFactors[blendDescIndex] < RendererAPI::BlendConstant::MAX_BLEND_CONSTANTS, "UtilToBlendDesc(): Invalid SrcFactor!");
			TRAP_ASSERT(desc.DstFactors[blendDescIndex] < RendererAPI::BlendConstant::MAX_BLEND_CONSTANTS, "UtilToBlendDesc(): Invalid DstFactor!");
			TRAP_ASSERT(desc.SrcAlphaFactors[blendDescIndex] < RendererAPI::BlendConstant::MAX_BLEND_CONSTANTS, "UtilToBlendDesc(): Invalid SrcAlphaFactor!");
			TRAP_ASSERT(desc.DstAlphaFactors[blendDescIndex] < RendererAPI::BlendConstant::MAX_BLEND_CONSTANTS, "UtilToBlendDesc(): Invalid DstAlphaFactor!");
			TRAP_ASSERT(desc.BlendModes[blendDescIndex] < RendererAPI::BlendMode::MAX_BLEND_MODES, "UtilToBlendDesc(): Invalid BlendMode!");
			TRAP_ASSERT(desc.BlendAlphaModes[blendDescIndex] < RendererAPI::BlendMode::MAX_BLEND_MODES, "UtilToBlendDesc(): Invalid BlendAlphaMode!");
		}

		if (desc.IndependentBlend)
			++blendDescIndex;
	}

	blendDescIndex = 0;
#endif /*ENABLE_GRAPHICS_DEBUG*/

	for(u32 i = 0; i < 8; ++i)
	{
		const VkBool32 blendEnable = VkBool32
		(
			VkBlendConstantTranslator[std::to_underlying(desc.SrcFactors[blendDescIndex])] != VK_BLEND_FACTOR_ONE ||
			VkBlendConstantTranslator[std::to_underlying(desc.DstFactors[blendDescIndex])] != VK_BLEND_FACTOR_ZERO ||
			VkBlendConstantTranslator[std::to_underlying(desc.SrcAlphaFactors[blendDescIndex])] != VK_BLEND_FACTOR_ONE ||
			VkBlendConstantTranslator[std::to_underlying(desc.DstAlphaFactors[blendDescIndex])] != VK_BLEND_FACTOR_ZERO
		);

		attachments[i].blendEnable = static_cast<VkBool32>((blendEnable != VK_FALSE) && ((std::to_underlying(desc.RenderTargetMask) & BIT(i)) != 0u));
		attachments[i].colorWriteMask = static_cast<VkColorComponentFlags>(desc.Masks[blendDescIndex]);
		attachments[i].srcColorBlendFactor = VkBlendConstantTranslator[std::to_underlying(desc.SrcFactors[blendDescIndex])];
		attachments[i].dstColorBlendFactor = VkBlendConstantTranslator[std::to_underlying(desc.DstFactors[blendDescIndex])];
		attachments[i].colorBlendOp = VkBlendOpTranslator[std::to_underlying(desc.BlendModes[blendDescIndex])];
		attachments[i].srcAlphaBlendFactor = VkBlendConstantTranslator[std::to_underlying(desc.SrcAlphaFactors[blendDescIndex])];
		attachments[i].dstAlphaBlendFactor = VkBlendConstantTranslator[std::to_underlying(desc.DstAlphaFactors[blendDescIndex])];
		attachments[i].alphaBlendOp = VkBlendOpTranslator[std::to_underlying(desc.BlendAlphaModes[blendDescIndex])];

		if (desc.IndependentBlend)
			++blendDescIndex;
	}

	return VulkanInits::PipelineColorBlendStateCreateInfo(attachments);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkPipelineDepthStencilStateCreateInfo TRAP::Graphics::API::UtilToDepthDesc(const RendererAPI::DepthStateDesc& desc)
{
	TRAP_ASSERT(desc.DepthFunc < RendererAPI::CompareMode::MAX_COMPARE_MODES, "UtilToDepthDesc(): Invalid DepthFunc!");
	TRAP_ASSERT(desc.StencilFrontFunc < RendererAPI::CompareMode::MAX_COMPARE_MODES, "UtilToDepthDesc(): Invalid StencilFrontFunc!");
	TRAP_ASSERT(desc.StencilFrontFail < RendererAPI::StencilOp::MAX_STENCIL_OPS, "UtilToDepthDesc(): Invalid StencilFrontFail!");
	TRAP_ASSERT(desc.DepthFrontFail < RendererAPI::StencilOp::MAX_STENCIL_OPS, "UtilToDepthDesc(): Invalid DepthFrontFail!");
	TRAP_ASSERT(desc.StencilFrontPass < RendererAPI::StencilOp::MAX_STENCIL_OPS, "UtilToDepthDesc(): Invalid StencilFrontPass!");
	TRAP_ASSERT(desc.StencilBackFunc < RendererAPI::CompareMode::MAX_COMPARE_MODES, "UtilToDepthDesc(): Invalid StencilBackFunc!");
	TRAP_ASSERT(desc.StencilBackFail < RendererAPI::StencilOp::MAX_STENCIL_OPS, "UtilToDepthDesc(): Invalid StencilBackFail!");
	TRAP_ASSERT(desc.DepthBackFail < RendererAPI::StencilOp::MAX_STENCIL_OPS, "UtilToDepthDesc(): Invalid DepthBackFail!");
	TRAP_ASSERT(desc.StencilBackPass < RendererAPI::StencilOp::MAX_STENCIL_OPS, "UtilToDepthDesc(): Invalid StencilBackPass!");

	VkPipelineDepthStencilStateCreateInfo ds{};
	ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	ds.pNext = nullptr;
	ds.flags = 0;
	ds.depthTestEnable = NumericCast<VkBool32>(desc.DepthTest);
	ds.depthWriteEnable = NumericCast<VkBool32>(desc.DepthWrite);
	ds.depthCompareOp = VkComparisonFuncTranslator[std::to_underlying(desc.DepthFunc)];
	ds.depthBoundsTestEnable = VK_FALSE;
	ds.stencilTestEnable = NumericCast<VkBool32>(desc.StencilTest);

	ds.front.failOp = VkStencilOpTranslator[std::to_underlying(desc.StencilFrontFail)];
	ds.front.passOp = VkStencilOpTranslator[std::to_underlying(desc.StencilFrontPass)];
	ds.front.depthFailOp = VkStencilOpTranslator[std::to_underlying(desc.DepthFrontFail)];
	ds.front.compareOp = VkComparisonFuncTranslator[std::to_underlying(desc.StencilFrontFunc)];
	ds.front.compareMask = desc.StencilReadMask;
	ds.front.writeMask = desc.StencilWriteMask;
	ds.front.reference = 0;

	ds.back.failOp = VkStencilOpTranslator[std::to_underlying(desc.StencilBackFail)];
	ds.back.passOp = VkStencilOpTranslator[std::to_underlying(desc.StencilBackPass)];
	ds.back.depthFailOp = VkStencilOpTranslator[std::to_underlying(desc.DepthBackFail)];
	ds.back.compareOp = VkComparisonFuncTranslator[std::to_underlying(desc.StencilBackFail)];
	ds.back.compareMask = desc.StencilReadMask;
	ds.back.writeMask = desc.StencilWriteMask;
	ds.back.reference = 0;

	ds.minDepthBounds = 0.0f;
	ds.maxDepthBounds = 1.0f;

	return ds;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkPipelineRasterizationStateCreateInfo TRAP::Graphics::API::UtilToRasterizerDesc(const RendererAPI::RasterizerStateDesc& desc)
{
	TRAP_ASSERT(desc.FillMode < RendererAPI::FillMode::MAX_FILL_MODES, "UtilToRasterizerDesc(): Invalid FillMode!");
	TRAP_ASSERT(desc.CullMode < RendererAPI::CullMode::MAX_CULL_MODES, "UtilToRasterizerDesc(): Invalid CullMode!");

	VkPipelineRasterizationStateCreateInfo rs{};
	rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rs.pNext = nullptr;
	rs.flags = 0;
	rs.depthClampEnable = desc.DepthClampEnable ? VK_TRUE : VK_FALSE;
	rs.rasterizerDiscardEnable = VK_FALSE;
	rs.polygonMode = VkFillModeTranslator[std::to_underlying(desc.FillMode)];
	rs.cullMode = VkCullModeTranslator[std::to_underlying(desc.CullMode)];
	rs.frontFace = VkFrontFaceTranslator[std::to_underlying(desc.FrontFace)];
	rs.depthBiasEnable = (desc.DepthBias != 0) ? VK_TRUE : VK_FALSE;
	rs.depthBiasConstantFactor = NumericCast<f32>(desc.DepthBias);
	rs.depthBiasClamp = 0.0f;
	rs.depthBiasSlopeFactor = desc.SlopeScaledDepthBias;
	rs.lineWidth = 1.0f;

	return rs;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::UtilGetPlanarVkImageMemoryRequirement(VkDevice device, VkImage image,
																const u32 planesCount,
                                                                VkMemoryRequirements& memReq,
                                                                std::vector<u64>& planesOffsets)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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

	for(u32 i = 0; i < planesCount; ++i)
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

[[nodiscard]] VkFragmentShadingRateCombinerOpKHR TRAP::Graphics::API::ShadingRateCombinerToVkFragmentShadingRateCombinerOpKHR(const RendererAPI::ShadingRateCombiner& combiner)
{
	switch(combiner)
	{
	case RendererAPI::ShadingRateCombiner::Passthrough:
		return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR;
	case RendererAPI::ShadingRateCombiner::Override:
		return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_KHR;
	case RendererAPI::ShadingRateCombiner::Min:
		return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN_KHR;
	case RendererAPI::ShadingRateCombiner::Max:
		return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX_KHR;
	case RendererAPI::ShadingRateCombiner::Sum:
		return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL_KHR;

	default:
		TRAP_ASSERT(false, "ShadingRateCombinerToVkFragmentShadingRateCombinerOpKHR(): Invalid shading rate combiner type");
		return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkExtent2D TRAP::Graphics::API::ShadingRateToVkExtent2D(const RendererAPI::ShadingRate& rate)
{
	switch(rate)
	{
	case RendererAPI::ShadingRate::Full:
		return VkExtent2D{ 1, 1 };
	case RendererAPI::ShadingRate::Half:
		return VkExtent2D{ 2, 2 };
	case RendererAPI::ShadingRate::Quarter:
		return VkExtent2D{ 4, 4 };
	case RendererAPI::ShadingRate::Eighth:
		return VkExtent2D{ 8, 8 };
	case RendererAPI::ShadingRate::OneXTwo:
		return VkExtent2D{ 1, 2 };
	case RendererAPI::ShadingRate::TwoXOne:
		return VkExtent2D{ 2, 1 };
	case RendererAPI::ShadingRate::TwoXFour:
		return VkExtent2D{ 2, 4 };
	case RendererAPI::ShadingRate::FourXTwo:
		return VkExtent2D{ 4, 2 };

	default:
		TRAP_ASSERT(false, "ShadingRateToVkExtent2D(): Invalid shading rate");
		return VkExtent2D{ 1, 1 };
	}
}
