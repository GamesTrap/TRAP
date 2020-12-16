#ifndef _TRAP_VULKANCOMMON_H_
#define _TRAP_VULKANCOMMON_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics::API
{
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
	
	void VkSetObjectName(VkDevice device, uint64_t handle, VkObjectType type, const char* name);

	inline static constexpr std::array<VkAttachmentLoadOp, static_cast<uint32_t>(RendererAPI::LoadActionType::MAX_LOAD_ACTION_TYPE)> VkAttachmentLoadOpTranslator =
	{
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_LOAD_OP_CLEAR
	};

	inline static constexpr std::array<VkCompareOp, static_cast<uint32_t>(RendererAPI::CompareMode::MAX_COMPARE_MODES)> VkComparisonFuncTranslator =
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

	inline static constexpr std::array<VkPipelineBindPoint, static_cast<uint32_t>(RendererAPI::PipelineType::PIPELINE_TYPE_COUNT)> VkPipelineBindPointTranslator =
	{
		VK_PIPELINE_BIND_POINT_MAX_ENUM,
		VK_PIPELINE_BIND_POINT_COMPUTE,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR
	};
}

//-------------------------------------------------------------------------------------------------------------------//

namespace TRAP::Graphics
{
#ifdef TRAP_DEBUG
#if __cplusplus > 201703L
	#define VkCall(x) std::source_location loc = std::source_location::current(); ::TRAP::Graphics::API::ErrorCheck(x, #x, loc.file_name, loc.line);
#else
	#define VkCall(x) ::TRAP::Graphics::API::ErrorCheck(x, #x, __FILE__, __LINE__);
#endif
#else
	#define VkCall(x) x;
#endif
}

#endif /*_TRAP_VULKANCOMMON_H_*/