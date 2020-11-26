#include "TRAPPCH.h"
#include "VulkanCommon.h"

#include "Graphics/RenderCommand.h"

bool TRAP::Graphics::API::ErrorCheck(const VkResult result, const char* function, const char* file, const int32_t line)
{
	if (result < 0)
	{
		switch (result)
		{
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			TP_ERROR(Log::RendererVulkanPrefix, "VK_ERROR_OUT_OF_HOST_MEMORY: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			TP_ERROR(Log::RendererVulkanPrefix, "VK_ERROR_OUT_OF_DEVICE_MEMORY: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_INITIALIZATION_FAILED:
			TP_ERROR(Log::RendererVulkanPrefix, "VK_ERROR_INITIALIZATION_FAILED: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_DEVICE_LOST:
			TP_ERROR(Log::RendererVulkanPrefix, "VK_ERROR_DEVICE_LOST: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_MEMORY_MAP_FAILED:
			TP_ERROR(Log::RendererVulkanPrefix, "VK_ERROR_MEMORY_MAP_FAILED: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_LAYER_NOT_PRESENT:
			TP_ERROR(Log::RendererVulkanPrefix, "VK_ERROR_LAYER_NOT_PRESENT: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_EXTENSION_NOT_PRESENT:
			TP_ERROR(Log::RendererVulkanPrefix, "VK_ERROR_EXTENSION_NOT_PRESENT: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_FEATURE_NOT_PRESENT:
			TP_ERROR(Log::RendererVulkanPrefix, "VK_ERROR_FEATURE_NOT_PRESENT: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_INCOMPATIBLE_DRIVER:
			TP_ERROR(Log::RendererVulkanPrefix, "VK_ERROR_INCOMPATIBLE_DRIVER: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_TOO_MANY_OBJECTS:
			TP_ERROR(Log::RendererVulkanPrefix, "VK_ERROR_TOO_MANY_OBJECTS: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_FORMAT_NOT_SUPPORTED:
			TP_ERROR(Log::RendererVulkanPrefix, "VK_ERROR_FORMAT_NOT_SUPPORTED: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_SURFACE_LOST_KHR:
			TP_ERROR(Log::RendererVulkanPrefix, "VK_ERROR_SURFACE_LOST_KHR: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
			TP_ERROR(Log::RendererVulkanPrefix, "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: ", function, " @[", file, ':', line, ']');
			break;
		case VK_SUBOPTIMAL_KHR:
			TP_ERROR(Log::RendererVulkanPrefix, "VK_SUBOPTIMAL_KHR: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			TP_ERROR(Log::RendererVulkanPrefix, "VK_ERROR_OUT_OF_DATE_KHR: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
			TP_ERROR(Log::RendererVulkanPrefix, "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_VALIDATION_FAILED_EXT:
			TP_ERROR(Log::RendererVulkanPrefix, "VK_ERROR_VALIDATION_FAILED_EXT: ", function, " @[", file, ':', line, ']');
			break;

		default:
			TP_ERROR(Log::RendererVulkanPrefix, "Unknown error", function, " @[", file, ':', line, ']');
			break;
		}

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

VkQueueFlags TRAP::Graphics::API::QueueTypeToVkQueueFlags(const RendererAPI::QueueType queueType)
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
		TRAP_ASSERT(false, "Invalid Queue Type");
		return VK_QUEUE_FLAG_BITS_MAX_ENUM;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VkBlendOp TRAP::Graphics::API::RendererBlendEquationToVkBlendOp(const RendererBlendEquation blendEquation)
{
	switch(blendEquation)
	{
	case RendererBlendEquation::Add:
		return VK_BLEND_OP_ADD;

	case RendererBlendEquation::Subtract:
		return VK_BLEND_OP_SUBTRACT;

	case RendererBlendEquation::Reverse_Subtract:
		return VK_BLEND_OP_REVERSE_SUBTRACT;

	case RendererBlendEquation::Min:
		return VK_BLEND_OP_MIN;

	case RendererBlendEquation::Max:
		return VK_BLEND_OP_MAX;
		
	default:
		return VkBlendOp();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VkBlendFactor TRAP::Graphics::API::RendererBlendFunctionToVkBlendFactor(const RendererBlendFunction blendFunction)
{
	switch(blendFunction)
	{
	case RendererBlendFunction::Zero:
		return VK_BLEND_FACTOR_ZERO;

	case RendererBlendFunction::One:
		return VK_BLEND_FACTOR_ONE;

	case RendererBlendFunction::Source_Color:
		return VK_BLEND_FACTOR_SRC_COLOR;

	case RendererBlendFunction::One_Minus_Source_Color:
		return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;

	case RendererBlendFunction::Destination_Color:
		return VK_BLEND_FACTOR_DST_COLOR;

	case RendererBlendFunction::One_Minus_Destination_Color:
		return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;

	case RendererBlendFunction::Source_Alpha:
		return VK_BLEND_FACTOR_SRC_ALPHA;

	case RendererBlendFunction::One_Minus_Source_Alpha:
		return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

	case RendererBlendFunction::Destination_Alpha:
		return VK_BLEND_FACTOR_DST_ALPHA;

	case RendererBlendFunction::One_Minus_Destination_Alpha:
		return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;

	case RendererBlendFunction::Source_Alpha_Saturate:
		return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;

	case RendererBlendFunction::Constant_Color:
		return VK_BLEND_FACTOR_CONSTANT_COLOR;

	case RendererBlendFunction::One_Minus_Constant_Color:
		return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
		
	default:
		return VkBlendFactor();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VkCompareOp TRAP::Graphics::API::RendererFunctionToVkCompareOp(const RendererFunction func)
{
	switch(func)
	{
	case RendererFunction::Never:
		return VK_COMPARE_OP_NEVER;

	case RendererFunction::Less:
		return VK_COMPARE_OP_LESS;

	case RendererFunction::Equal:
		return VK_COMPARE_OP_EQUAL;

	case RendererFunction::Less_Or_Equal:
		return VK_COMPARE_OP_LESS_OR_EQUAL;

	case RendererFunction::Greater:
		return VK_COMPARE_OP_GREATER;

	case RendererFunction::Not_Equal:
		return VK_COMPARE_OP_NOT_EQUAL;

	case RendererFunction::Greater_Or_Equal:
		return VK_COMPARE_OP_GREATER_OR_EQUAL;

	case RendererFunction::Always:
		return VK_COMPARE_OP_ALWAYS;
		
	default:
		return VkCompareOp();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VkStencilOp TRAP::Graphics::API::RendererOperationToVkStencilOp(const RendererOperation op)
{
	switch(op)
	{
	case RendererOperation::Keep:
		return VK_STENCIL_OP_KEEP;

	case RendererOperation::Zero:
		return VK_STENCIL_OP_ZERO;

	case RendererOperation::Replace:
		return VK_STENCIL_OP_REPLACE;

	case RendererOperation::Invert:
		return VK_STENCIL_OP_INVERT;

	case RendererOperation::Increment_And_Wrap:
		return VK_STENCIL_OP_INCREMENT_AND_WRAP;

	case RendererOperation::Decrement_And_Wrap:
		return VK_STENCIL_OP_DECREMENT_AND_WRAP;

	case RendererOperation::Increment_And_Clamp:
		return VK_STENCIL_OP_INCREMENT_AND_CLAMP;

	case RendererOperation::Decrement_And_Clamp:
		return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
		
	default:
		return VkStencilOp();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VkCullModeFlagBits TRAP::Graphics::API::RendererCullModeToVkCullModeFlagBits(const RendererCullMode cullMode)
{
	switch(cullMode)
	{
	case RendererCullMode::Back:
		return VK_CULL_MODE_BACK_BIT;

	case RendererCullMode::Front:
		return VK_CULL_MODE_FRONT_BIT;
		
	default:
		return VkCullModeFlagBits();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VkFrontFace TRAP::Graphics::API::RendererFrontFaceToVkFrontFace(const RendererFrontFace frontFace)
{
	switch(frontFace)
	{
	case RendererFrontFace::Counter_Clockwise:
		return VK_FRONT_FACE_COUNTER_CLOCKWISE;

	case RendererFrontFace::Clockwise:
		return VK_FRONT_FACE_CLOCKWISE;
		
	default:
		return VkFrontFace();
	}
}