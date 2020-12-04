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

	void VkSetObjectName(VkDevice device, uint64_t handle, VkObjectType type, const char* name);

	inline static constexpr std::array<VkAttachmentLoadOp, static_cast<uint32_t>(RendererAPI::LoadActionType::MAX_LOAD_ACTION_TYPE)> VkAttachmentLoadOpTranslator =
	{
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_LOAD_OP_CLEAR
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