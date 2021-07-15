#ifndef _TRAP_VULKANCOMMON_H_
#define _TRAP_VULKANCOMMON_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics::API
{
	class VulkanQueue;
	class VulkanPhysicalDevice;
	class VulkanTexture;

	bool ErrorCheck(VkResult result, const char* function, const char* file, int32_t line);
	VkQueueFlags QueueTypeToVkQueueFlags(RendererAPI::QueueType queueType);

	VkBlendOp RendererBlendEquationToVkBlendOp(RendererBlendEquation blendEquation);
	VkBlendFactor RendererBlendFunctionToVkBlendFactor(RendererBlendFunction blendFunction);
	VkCompareOp RendererFunctionToVkCompareOp(RendererFunction func);
	VkStencilOp RendererOperationToVkStencilOp(RendererOperation op);
	VkCullModeFlagBits RendererCullModeToVkCullModeFlagBits(RendererCullMode cullMode);
	VkFrontFace RendererFrontFaceToVkFrontFace(RendererFrontFace frontFace);

	VkSampleCountFlagBits SampleCountToVkSampleCount(RendererAPI::SampleCount sampleCount);
	VkFormat ImageFormatToVkFormat(RendererAPI::ImageFormat imageFormat);
	VkImageAspectFlags DetermineAspectMask(VkFormat format, bool includeStencilBit);
	VkImageUsageFlags DescriptorTypeToVkImageUsage(RendererAPI::DescriptorType type);
	VkFormatFeatureFlags VkImageUsageToFormatFeatures(VkImageUsageFlags usage);
	VkBufferUsageFlags DescriptorTypeToVkBufferUsage(RendererAPI::DescriptorType usage, bool typed);
	VkFilter FilterTypeToVkFilter(RendererAPI::FilterType filter);
	VkSamplerMipmapMode MipMapModeToVkMipMapMode(RendererAPI::MipMapMode mipMapMode);
	VkSamplerAddressMode AddressModeToVkAddressMode(RendererAPI::AddressMode addressMode);
	VkDescriptorType DescriptorTypeToVkDescriptorType(RendererAPI::DescriptorType type);
	VkShaderStageFlags ShaderStageToVkShaderStageFlags(RendererAPI::ShaderStage stages);
	VkPipelineCacheCreateFlags PipelineCacheFlagsToVkPipelineCacheCreateFlags(RendererAPI::PipelineCacheFlags flags);
	VkAccessFlags ResourceStateToVkAccessFlags(RendererAPI::ResourceState state);
	VkImageLayout ResourceStateToVkImageLayout(RendererAPI::ResourceState usage);
	VkPipelineStageFlags DetermineVkPipelineStageFlags(VkAccessFlags accessFlags, RendererAPI::QueueType queueType);
	VkQueryType QueryTypeToVkQueryType(RendererAPI::QueryType type);
	VkImageAspectFlags ClearFlagsToVKImageAspectFlags(RendererAPI::ClearFlags flags);

	VkPipelineColorBlendStateCreateInfo UtilToBlendDesc(const RendererAPI::BlendStateDesc& desc,
	                                                    std::vector<VkPipelineColorBlendAttachmentState>& attachments);
	VkPipelineDepthStencilStateCreateInfo UtilToDepthDesc(const RendererAPI::DepthStateDesc& desc);
	VkPipelineRasterizationStateCreateInfo UtilToRasterizerDesc(const RendererAPI::RasterizerStateDesc& desc);
	void UtilGetPlanarVkImageMemoryRequirement(VkDevice device, VkImage image, uint32_t planesCount,
	                                           VkMemoryRequirements& memReq, std::vector<uint64_t>& planesOffsets);

	RendererAPI::ImageFormat VulkanGetRecommendedSwapchainFormat(bool HDR);

	RendererAPI::ImageFormat ImageFormatFromVkFormat(VkFormat format);

	void VkSetObjectName(VkDevice device, uint64_t handle, VkObjectType type, const std::string& name);

	inline static constexpr std::array<VkAttachmentLoadOp,
	                                   static_cast<uint32_t>(RendererAPI::LoadActionType::MAX_LOAD_ACTION_TYPE)> VkAttachmentLoadOpTranslator =
	{
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_LOAD_OP_CLEAR
	};

	inline static constexpr std::array<VkCompareOp,
	                                   static_cast<uint32_t>(RendererAPI::CompareMode::MAX_COMPARE_MODES)> VkComparisonFuncTranslator =
	{
		VK_COMPARE_OP_NEVER,
		VK_COMPARE_OP_LESS,
		VK_COMPARE_OP_EQUAL,
		VK_COMPARE_OP_LESS_OR_EQUAL,
		VK_COMPARE_OP_GREATER,
		VK_COMPARE_OP_NOT_EQUAL,
		VK_COMPARE_OP_GREATER_OR_EQUAL,
		VK_COMPARE_OP_ALWAYS
	};

	inline static constexpr std::array<VkPipelineBindPoint,
	                                   static_cast<uint32_t>(RendererAPI::PipelineType::PIPELINE_TYPE_COUNT)> VkPipelineBindPointTranslator =
	{
		VK_PIPELINE_BIND_POINT_MAX_ENUM,
		VK_PIPELINE_BIND_POINT_COMPUTE,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR
	};

	inline static constexpr std::array<VkBlendFactor,
	                                   static_cast<uint32_t>(RendererAPI::BlendConstant::MAX_BLEND_CONSTANTS)> VkBlendConstantTranslator =
	{
		VK_BLEND_FACTOR_ZERO,
		VK_BLEND_FACTOR_ONE,
		VK_BLEND_FACTOR_SRC_COLOR,
		VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
		VK_BLEND_FACTOR_DST_COLOR,
		VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR,
		VK_BLEND_FACTOR_SRC_ALPHA,
		VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		VK_BLEND_FACTOR_DST_ALPHA,
		VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
		VK_BLEND_FACTOR_SRC_ALPHA_SATURATE,
		VK_BLEND_FACTOR_CONSTANT_COLOR,
		VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR
	};

	inline static constexpr std::array<VkBlendOp,
	                                   static_cast<uint32_t>(RendererAPI::BlendMode::MAX_BLEND_MODES)> VkBlendOpTranslator =
	{
		VK_BLEND_OP_ADD,
		VK_BLEND_OP_SUBTRACT,
		VK_BLEND_OP_REVERSE_SUBTRACT,
		VK_BLEND_OP_MIN,
		VK_BLEND_OP_MAX
	};

	inline static constexpr std::array<VkStencilOp,
	                                   static_cast<uint32_t>(RendererAPI::StencilOp::MAX_STENCIL_OPS)> VkStencilOpTranslator =
	{
		VK_STENCIL_OP_KEEP,
		VK_STENCIL_OP_ZERO,
		VK_STENCIL_OP_REPLACE,
		VK_STENCIL_OP_INVERT,
		VK_STENCIL_OP_INCREMENT_AND_WRAP,
		VK_STENCIL_OP_DECREMENT_AND_WRAP,
		VK_STENCIL_OP_INCREMENT_AND_CLAMP,
		VK_STENCIL_OP_DECREMENT_AND_CLAMP
	};

	inline static constexpr std::array<VkCullModeFlagBits,
	                                   static_cast<uint32_t>(RendererAPI::CullMode::MAX_CULL_MODES)> VkCullModeTranslator =
	{
		VK_CULL_MODE_NONE,
		VK_CULL_MODE_BACK_BIT,
		VK_CULL_MODE_FRONT_BIT
	};

	inline static constexpr std::array<VkPolygonMode,
	                                   static_cast<uint32_t>(RendererAPI::FillMode::MAX_FILL_MODES)> VkFillModeTranslator =
	{
		VK_POLYGON_MODE_FILL,
		VK_POLYGON_MODE_LINE
	};

	inline static constexpr std::array<VkFrontFace, 2> VkFrontFaceTranslator =
	{
		VK_FRONT_FACE_COUNTER_CLOCKWISE,
		VK_FRONT_FACE_CLOCKWISE
	};
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_DEBUG
#if __cplusplus > 201703L
	#define VkCall(x) std::source_location loc = std::source_location::current();\
	                  ::TRAP::Graphics::API::ErrorCheck(x, #x, loc.file_name, loc.line);
#else
	#define VkCall(x) ::TRAP::Graphics::API::ErrorCheck(x, #x, __FILE__, __LINE__);
#endif
#else
	constexpr void VkCall(VkResult)
	{}
#endif

#endif /*_TRAP_VULKANCOMMON_H_*/