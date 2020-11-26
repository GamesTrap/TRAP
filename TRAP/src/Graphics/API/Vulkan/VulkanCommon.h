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