#include "TRAPPCH.h"
#include "VulkanCommon.h"

#include "VulkanRenderer.h"
#include "Graphics/RenderCommand.h"
#include "Objects/VulkanInits.h"

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

//-------------------------------------------------------------------------------------------------------------------//

VkSampleCountFlagBits TRAP::Graphics::API::SampleCountToVkSampleCount(const RendererAPI::SampleCount sampleCount)
{
	VkSampleCountFlagBits result = VK_SAMPLE_COUNT_1_BIT;

	switch(sampleCount)
	{
	case RendererAPI::SampleCount::SampleCount1:
		result = VK_SAMPLE_COUNT_1_BIT;
		break;

	case RendererAPI::SampleCount::SampleCount2:
		result = VK_SAMPLE_COUNT_2_BIT;
		break;

	case RendererAPI::SampleCount::SampleCount4:
		result = VK_SAMPLE_COUNT_4_BIT;
		break;

	case RendererAPI::SampleCount::SampleCount8:
		result = VK_SAMPLE_COUNT_8_BIT;
		break;

	case RendererAPI::SampleCount::SampleCount16:
		result = VK_SAMPLE_COUNT_16_BIT;
		break;
		
	default:
		break;
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

VkFormat TRAP::Graphics::API::ImageFormatToVkFormat(const RendererAPI::ImageFormat imageFormat)
{
	switch(imageFormat)
	{
	case RendererAPI::ImageFormat::Undefined: return VK_FORMAT_UNDEFINED;
	case RendererAPI::ImageFormat::G4R4_UNORM: return VK_FORMAT_R4G4_UNORM_PACK8;
	case RendererAPI::ImageFormat::A4B4G4R4_UNORM: return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
	case RendererAPI::ImageFormat::A4R4G4B4_UNORM: return VK_FORMAT_B4G4R4A4_UNORM_PACK16;
	case RendererAPI::ImageFormat::R5G6B5_UNORM: return VK_FORMAT_B5G6R5_UNORM_PACK16;
	case RendererAPI::ImageFormat::B5G6R5_UNORM: return VK_FORMAT_R5G6B5_UNORM_PACK16;
	case RendererAPI::ImageFormat::A1B5G5R5_UNORM: return VK_FORMAT_R5G5B5A1_UNORM_PACK16;
	case RendererAPI::ImageFormat::A1R5G5B5_UNORM: return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
	case RendererAPI::ImageFormat::B5G5R5A1_UNORM: return VK_FORMAT_A1R5G5B5_UNORM_PACK16;
	case RendererAPI::ImageFormat::A2B10G10R10_UNORM: return VK_FORMAT_A2B10G10R10_UNORM_PACK32;

	case RendererAPI::ImageFormat::R8_UNORM: return VK_FORMAT_R8_UNORM;
	case RendererAPI::ImageFormat::R8_SNORM: return VK_FORMAT_R8_SNORM;
	case RendererAPI::ImageFormat::R8_UINT: return VK_FORMAT_R8_UINT;
	case RendererAPI::ImageFormat::R8_SINT: return VK_FORMAT_R8_SINT;
	case RendererAPI::ImageFormat::R8_SRGB: return VK_FORMAT_R8_SRGB;
	case RendererAPI::ImageFormat::R8G8_UNORM: return VK_FORMAT_R8G8_UNORM;
	case RendererAPI::ImageFormat::R8G8_SNORM: return VK_FORMAT_R8G8_SNORM;
	case RendererAPI::ImageFormat::R8G8_UINT: return VK_FORMAT_R8G8_UINT;
	case RendererAPI::ImageFormat::R8G8_SINT: return VK_FORMAT_R8G8_SINT;
	case RendererAPI::ImageFormat::R8G8_SRGB: return VK_FORMAT_R8G8_SRGB;
	case RendererAPI::ImageFormat::R8G8B8_UNORM: return VK_FORMAT_R8G8B8_UNORM;
	case RendererAPI::ImageFormat::R8G8B8_SNORM: return VK_FORMAT_R8G8B8_SNORM;
	case RendererAPI::ImageFormat::R8G8B8_UINT: return VK_FORMAT_R8G8B8_UINT;
	case RendererAPI::ImageFormat::R8G8B8_SINT: return VK_FORMAT_R8G8B8_SINT;
	case RendererAPI::ImageFormat::R8G8B8_SRGB: return VK_FORMAT_R8G8B8_SRGB;
	case RendererAPI::ImageFormat::B8G8R8_UNORM: return VK_FORMAT_B8G8R8_UNORM;
	case RendererAPI::ImageFormat::B8G8R8_SNORM: return VK_FORMAT_B8G8R8_SNORM;
	case RendererAPI::ImageFormat::B8G8R8_UINT: return VK_FORMAT_B8G8R8_UINT;
	case RendererAPI::ImageFormat::B8G8R8_SINT: return VK_FORMAT_B8G8R8_SINT;
	case RendererAPI::ImageFormat::B8G8R8_SRGB: return VK_FORMAT_B8G8R8_SRGB;
	case RendererAPI::ImageFormat::R8G8B8A8_UNORM: return VK_FORMAT_R8G8B8A8_UNORM;
	case RendererAPI::ImageFormat::R8G8B8A8_SNORM: return VK_FORMAT_R8G8B8A8_SNORM;
	case RendererAPI::ImageFormat::R8G8B8A8_UINT: return VK_FORMAT_R8G8B8A8_UINT;
	case RendererAPI::ImageFormat::R8G8B8A8_SINT: return VK_FORMAT_R8G8B8A8_SINT;
	case RendererAPI::ImageFormat::R8G8B8A8_SRGB: return VK_FORMAT_R8G8B8A8_SRGB;
	case RendererAPI::ImageFormat::B8G8R8A8_UNORM: return VK_FORMAT_B8G8R8A8_UNORM;
	case RendererAPI::ImageFormat::B8G8R8A8_SNORM: return VK_FORMAT_B8G8R8A8_SNORM;
	case RendererAPI::ImageFormat::B8G8R8A8_UINT: return VK_FORMAT_B8G8R8A8_UINT;
	case RendererAPI::ImageFormat::B8G8R8A8_SINT: return VK_FORMAT_B8G8R8A8_SINT;
	case RendererAPI::ImageFormat::B8G8R8A8_SRGB: return VK_FORMAT_B8G8R8A8_SRGB;
	case RendererAPI::ImageFormat::R16_UNORM: return VK_FORMAT_R16_UNORM;
	case RendererAPI::ImageFormat::R16_SNORM: return VK_FORMAT_R16_SNORM;
	case RendererAPI::ImageFormat::R16_UINT: return VK_FORMAT_R16_UINT;
	case RendererAPI::ImageFormat::R16_SINT: return VK_FORMAT_R16_SINT;
	case RendererAPI::ImageFormat::R16_SFLOAT: return VK_FORMAT_R16_SFLOAT;
	case RendererAPI::ImageFormat::R16G16_UNORM: return VK_FORMAT_R16G16_UNORM;
	case RendererAPI::ImageFormat::R16G16_SNORM: return VK_FORMAT_R16G16_SNORM;
	case RendererAPI::ImageFormat::R16G16_UINT: return VK_FORMAT_R16G16_UINT;
	case RendererAPI::ImageFormat::R16G16_SINT: return VK_FORMAT_R16G16_SINT;
	case RendererAPI::ImageFormat::R16G16_SFLOAT: return VK_FORMAT_R16G16_SFLOAT;
	case RendererAPI::ImageFormat::R16G16B16_UNORM: return VK_FORMAT_R16G16B16_UNORM;
	case RendererAPI::ImageFormat::R16G16B16_SNORM: return VK_FORMAT_R16G16B16_SNORM;
	case RendererAPI::ImageFormat::R16G16B16_UINT: return VK_FORMAT_R16G16B16_UINT;
	case RendererAPI::ImageFormat::R16G16B16_SINT: return VK_FORMAT_R16G16B16_SINT;
	case RendererAPI::ImageFormat::R16G16B16_SFLOAT: return VK_FORMAT_R16G16B16_SFLOAT;
	case RendererAPI::ImageFormat::R16G16B16A16_UNORM: return VK_FORMAT_R16G16B16A16_UNORM;
	case RendererAPI::ImageFormat::R16G16B16A16_SNORM: return VK_FORMAT_R16G16B16A16_SNORM;
	case RendererAPI::ImageFormat::R16G16B16A16_UINT: return VK_FORMAT_R16G16B16A16_UINT;
	case RendererAPI::ImageFormat::R16G16B16A16_SINT: return VK_FORMAT_R16G16B16A16_SINT;
	case RendererAPI::ImageFormat::R16G16B16A16_SFLOAT: return VK_FORMAT_R16G16B16A16_SFLOAT;
	case RendererAPI::ImageFormat::R32_UINT: return VK_FORMAT_R32_UINT;
	case RendererAPI::ImageFormat::R32_SINT: return VK_FORMAT_R32_SINT;
	case RendererAPI::ImageFormat::R32_SFLOAT: return VK_FORMAT_R32_SFLOAT;
	case RendererAPI::ImageFormat::R32G32_UINT: return VK_FORMAT_R32G32_UINT;
	case RendererAPI::ImageFormat::R32G32_SINT: return VK_FORMAT_R32G32_SINT;
	case RendererAPI::ImageFormat::R32G32_SFLOAT: return VK_FORMAT_R32G32_SFLOAT;
	case RendererAPI::ImageFormat::R32G32B32_UINT: return VK_FORMAT_R32G32B32_UINT;
	case RendererAPI::ImageFormat::R32G32B32_SINT: return VK_FORMAT_R32G32B32_SINT;
	case RendererAPI::ImageFormat::R32G32B32_SFLOAT: return VK_FORMAT_R32G32B32_SFLOAT;
	case RendererAPI::ImageFormat::R32G32B32A32_UINT: return VK_FORMAT_R32G32B32A32_UINT;
	case RendererAPI::ImageFormat::R32G32B32A32_SINT: return VK_FORMAT_R32G32B32A32_SINT;
	case RendererAPI::ImageFormat::R32G32B32A32_SFLOAT: return VK_FORMAT_R32G32B32A32_SFLOAT;
	case RendererAPI::ImageFormat::R64_UINT: return VK_FORMAT_R64_UINT;
	case RendererAPI::ImageFormat::R64_SINT: return VK_FORMAT_R64_SINT;
	case RendererAPI::ImageFormat::R64_SFLOAT: return VK_FORMAT_R64_SFLOAT;
	case RendererAPI::ImageFormat::R64G64_UINT: return VK_FORMAT_R64G64_UINT;
	case RendererAPI::ImageFormat::R64G64_SINT: return VK_FORMAT_R64G64_SINT;
	case RendererAPI::ImageFormat::R64G64_SFLOAT: return VK_FORMAT_R64G64_SFLOAT;
	case RendererAPI::ImageFormat::R64G64B64_UINT: return VK_FORMAT_R64G64B64_UINT;
	case RendererAPI::ImageFormat::R64G64B64_SINT: return VK_FORMAT_R64G64B64_SINT;
	case RendererAPI::ImageFormat::R64G64B64_SFLOAT: return VK_FORMAT_R64G64B64_SFLOAT;
	case RendererAPI::ImageFormat::R64G64B64A64_UINT: return VK_FORMAT_R64G64B64A64_UINT;
	case RendererAPI::ImageFormat::R64G64B64A64_SINT: return VK_FORMAT_R64G64B64A64_SINT;
	case RendererAPI::ImageFormat::R64G64B64A64_SFLOAT: return VK_FORMAT_R64G64B64A64_SFLOAT;

	case RendererAPI::ImageFormat::B10G10R10A2_UNORM: return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
	case RendererAPI::ImageFormat::B10G10R10A2_UINT: return VK_FORMAT_A2R10G10B10_UINT_PACK32;
	case RendererAPI::ImageFormat::R10G10B10A2_UNORM: return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
	case RendererAPI::ImageFormat::R10G10B10A2_UINT: return VK_FORMAT_A2B10G10R10_UINT_PACK32;

	case RendererAPI::ImageFormat::B10G11R11_UFLOAT: return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
	case RendererAPI::ImageFormat::E5B9G9R9_UFLOAT: return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;

	case RendererAPI::ImageFormat::D16_UNORM: return VK_FORMAT_D16_UNORM;
	case RendererAPI::ImageFormat::X8_D24_UNORM: return VK_FORMAT_X8_D24_UNORM_PACK32;
	case RendererAPI::ImageFormat::D32_SFLOAT: return VK_FORMAT_D32_SFLOAT;
	case RendererAPI::ImageFormat::S8_UINT: return VK_FORMAT_S8_UINT;
	case RendererAPI::ImageFormat::D16_UNORM_S8_UINT: return VK_FORMAT_D16_UNORM_S8_UINT;
	case RendererAPI::ImageFormat::D24_UNORM_S8_UINT: return VK_FORMAT_D24_UNORM_S8_UINT;
	case RendererAPI::ImageFormat::D32_SFLOAT_S8_UINT: return VK_FORMAT_D32_SFLOAT_S8_UINT;
	case RendererAPI::ImageFormat::DXBC1_RGB_UNORM: return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
	case RendererAPI::ImageFormat::DXBC1_RGB_SRGB: return VK_FORMAT_BC1_RGB_SRGB_BLOCK;
	case RendererAPI::ImageFormat::DXBC1_RGBA_UNORM: return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
	case RendererAPI::ImageFormat::DXBC1_RGBA_SRGB: return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
	case RendererAPI::ImageFormat::DXBC2_UNORM: return VK_FORMAT_BC2_UNORM_BLOCK;
	case RendererAPI::ImageFormat::DXBC2_SRGB: return VK_FORMAT_BC2_SRGB_BLOCK;
	case RendererAPI::ImageFormat::DXBC3_UNORM: return VK_FORMAT_BC3_UNORM_BLOCK;
	case RendererAPI::ImageFormat::DXBC3_SRGB: return VK_FORMAT_BC3_SRGB_BLOCK;
	case RendererAPI::ImageFormat::DXBC4_UNORM: return VK_FORMAT_BC4_UNORM_BLOCK;
	case RendererAPI::ImageFormat::DXBC4_SNORM: return VK_FORMAT_BC4_SNORM_BLOCK;
	case RendererAPI::ImageFormat::DXBC5_UNORM: return VK_FORMAT_BC5_UNORM_BLOCK;
	case RendererAPI::ImageFormat::DXBC5_SNORM: return VK_FORMAT_BC5_SNORM_BLOCK;
	case RendererAPI::ImageFormat::DXBC6H_UFLOAT: return VK_FORMAT_BC6H_UFLOAT_BLOCK;
	case RendererAPI::ImageFormat::DXBC6H_SFLOAT: return VK_FORMAT_BC6H_SFLOAT_BLOCK;
	case RendererAPI::ImageFormat::DXBC7_UNORM: return VK_FORMAT_BC7_UNORM_BLOCK;
	case RendererAPI::ImageFormat::DXBC7_SRGB: return VK_FORMAT_BC7_SRGB_BLOCK;
	case RendererAPI::ImageFormat::PVRTC1_2BPP_UNORM: return VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG;
	case RendererAPI::ImageFormat::PVRTC1_4BPP_UNORM: return VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG;
	case RendererAPI::ImageFormat::PVRTC1_2BPP_SRGB: return VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG;
	case RendererAPI::ImageFormat::PVRTC1_4BPP_SRGB: return VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG;
	case RendererAPI::ImageFormat::ETC2_R8G8B8_UNORM: return VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
	case RendererAPI::ImageFormat::ETC2_R8G8B8A1_UNORM: return VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK;
	case RendererAPI::ImageFormat::ETC2_R8G8B8A8_UNORM: return VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
	case RendererAPI::ImageFormat::ETC2_R8G8B8_SRGB: return VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK;
	case RendererAPI::ImageFormat::ETC2_R8G8B8A1_SRGB: return VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK;
	case RendererAPI::ImageFormat::ETC2_R8G8B8A8_SRGB: return VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK;
	case RendererAPI::ImageFormat::ETC2_EAC_R11_UNORM: return VK_FORMAT_EAC_R11_UNORM_BLOCK;
	case RendererAPI::ImageFormat::ETC2_EAC_R11G11_UNORM: return VK_FORMAT_EAC_R11G11_UNORM_BLOCK;
	case RendererAPI::ImageFormat::ETC2_EAC_R11_SNORM: return VK_FORMAT_EAC_R11_SNORM_BLOCK;
	case RendererAPI::ImageFormat::ETC2_EAC_R11G11_SNORM: return VK_FORMAT_EAC_R11G11_SNORM_BLOCK;
	case RendererAPI::ImageFormat::ASTC_4x4_UNORM: return VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
	case RendererAPI::ImageFormat::ASTC_4x4_SRGB: return VK_FORMAT_ASTC_4x4_SRGB_BLOCK;
	case RendererAPI::ImageFormat::ASTC_5x4_UNORM: return VK_FORMAT_ASTC_5x4_UNORM_BLOCK;
	case RendererAPI::ImageFormat::ASTC_5x4_SRGB: return VK_FORMAT_ASTC_5x4_SRGB_BLOCK;
	case RendererAPI::ImageFormat::ASTC_5x5_UNORM: return VK_FORMAT_ASTC_5x5_UNORM_BLOCK;
	case RendererAPI::ImageFormat::ASTC_5x5_SRGB: return VK_FORMAT_ASTC_5x5_SRGB_BLOCK;
	case RendererAPI::ImageFormat::ASTC_6x5_UNORM: return VK_FORMAT_ASTC_6x5_UNORM_BLOCK;
	case RendererAPI::ImageFormat::ASTC_6x5_SRGB: return VK_FORMAT_ASTC_6x5_SRGB_BLOCK;
	case RendererAPI::ImageFormat::ASTC_6x6_UNORM: return VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
	case RendererAPI::ImageFormat::ASTC_6x6_SRGB: return VK_FORMAT_ASTC_6x6_SRGB_BLOCK;
	case RendererAPI::ImageFormat::ASTC_8x5_UNORM: return VK_FORMAT_ASTC_8x5_UNORM_BLOCK;
	case RendererAPI::ImageFormat::ASTC_8x5_SRGB: return VK_FORMAT_ASTC_8x5_SRGB_BLOCK;
	case RendererAPI::ImageFormat::ASTC_8x6_UNORM: return VK_FORMAT_ASTC_8x6_UNORM_BLOCK;
	case RendererAPI::ImageFormat::ASTC_8x6_SRGB: return VK_FORMAT_ASTC_8x6_SRGB_BLOCK;
	case RendererAPI::ImageFormat::ASTC_8x8_UNORM: return VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
	case RendererAPI::ImageFormat::ASTC_8x8_SRGB: return VK_FORMAT_ASTC_8x8_SRGB_BLOCK;
	case RendererAPI::ImageFormat::ASTC_10x5_UNORM: return VK_FORMAT_ASTC_10x5_UNORM_BLOCK;
	case RendererAPI::ImageFormat::ASTC_10x5_SRGB: return VK_FORMAT_ASTC_10x5_SRGB_BLOCK;
	case RendererAPI::ImageFormat::ASTC_10x6_UNORM: return VK_FORMAT_ASTC_10x6_UNORM_BLOCK;
	case RendererAPI::ImageFormat::ASTC_10x6_SRGB: return VK_FORMAT_ASTC_10x6_SRGB_BLOCK;
	case RendererAPI::ImageFormat::ASTC_10x8_UNORM: return VK_FORMAT_ASTC_10x8_UNORM_BLOCK;
	case RendererAPI::ImageFormat::ASTC_10x8_SRGB: return VK_FORMAT_ASTC_10x8_SRGB_BLOCK;
	case RendererAPI::ImageFormat::ASTC_10x10_UNORM: return VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
	case RendererAPI::ImageFormat::ASTC_10x10_SRGB: return VK_FORMAT_ASTC_10x10_SRGB_BLOCK;
	case RendererAPI::ImageFormat::ASTC_12x10_UNORM: return VK_FORMAT_ASTC_12x10_UNORM_BLOCK;
	case RendererAPI::ImageFormat::ASTC_12x10_SRGB: return VK_FORMAT_ASTC_12x10_SRGB_BLOCK;
	case RendererAPI::ImageFormat::ASTC_12x12_UNORM: return VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
	case RendererAPI::ImageFormat::ASTC_12x12_SRGB: return VK_FORMAT_ASTC_12x12_SRGB_BLOCK;
	case RendererAPI::ImageFormat::PVRTC2_2BPP_UNORM: return VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG;
	case RendererAPI::ImageFormat::PVRTC2_4BPP_UNORM: return VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG;
	case RendererAPI::ImageFormat::PVRTC2_2BPP_SRGB: return VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG;
	case RendererAPI::ImageFormat::PVRTC2_4BPP_SRGB: return VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG;
		
	default:
		return VK_FORMAT_UNDEFINED;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VkImageAspectFlags TRAP::Graphics::API::DetermineAspectMask(const VkFormat format, const bool includeStencilBit)
{
	VkImageAspectFlags result;
	switch(format)
	{
		//Depth
	case VK_FORMAT_D16_UNORM:
	case VK_FORMAT_X8_D24_UNORM_PACK32:
	case VK_FORMAT_D32_SFLOAT:
		result = VK_IMAGE_ASPECT_DEPTH_BIT;
		break;

		//Stencil
	case VK_FORMAT_S8_UINT:
		result = VK_IMAGE_ASPECT_STENCIL_BIT;
		break;

		//Depth/Stencil
	case VK_FORMAT_D16_UNORM_S8_UINT:
	case VK_FORMAT_D24_UNORM_S8_UINT:
	case VK_FORMAT_D32_SFLOAT_S8_UINT:
		result = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (includeStencilBit)
			result |= VK_IMAGE_ASPECT_STENCIL_BIT;
		break;

		//Assume everything else is Color
	default:
		result = VK_IMAGE_ASPECT_COLOR_BIT;
		break;
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

VkImageUsageFlags TRAP::Graphics::API::DescriptorTypeToVkImageUsage(const RendererAPI::DescriptorType type)
{
	VkImageUsageFlags result = 0;

	if ((static_cast<uint32_t>(type & RendererAPI::DescriptorType::Texture)))
		result |= VK_IMAGE_USAGE_SAMPLED_BIT;
	if ((static_cast<uint32_t>(type & RendererAPI::DescriptorType::RWTexture)))
		result |= VK_IMAGE_USAGE_STORAGE_BIT;
	
	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

VkFormatFeatureFlags TRAP::Graphics::API::VkImageUsageToFormatFeatures(const VkImageUsageFlags usage)
{
	VkFormatFeatureFlags result = static_cast<VkFormatFeatureFlags>(0);
	if ((usage & VK_IMAGE_USAGE_SAMPLED_BIT) == VK_IMAGE_USAGE_SAMPLED_BIT)
		result |= VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
	if ((usage & VK_IMAGE_USAGE_STORAGE_BIT) == VK_IMAGE_USAGE_STORAGE_BIT)
		result |= VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
	if ((usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) == VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
		result |= VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
	if ((usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
		result |= VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

VkBufferUsageFlags TRAP::Graphics::API::DescriptorTypeToVkBufferUsage(const RendererAPI::DescriptorType usage, const bool typed)
{
	VkBufferUsageFlags result = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	if (static_cast<uint32_t>(usage & RendererAPI::DescriptorType::UniformBuffer))
		result |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	if (static_cast<uint32_t>(usage & RendererAPI::DescriptorType::RWBuffer))
	{
		result |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		if (typed)
			result |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
	}
	if (static_cast<uint32_t>(usage & RendererAPI::DescriptorType::Buffer))
	{
		result |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		if (typed)
			result |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
	}
	if (static_cast<uint32_t>(usage & RendererAPI::DescriptorType::IndexBuffer))
		result |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	if (static_cast<uint32_t>(usage & RendererAPI::DescriptorType::VertexBuffer))
		result |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	if (static_cast<uint32_t>(usage & RendererAPI::DescriptorType::IndirectBuffer))
		result |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
	if (static_cast<uint32_t>(usage & RendererAPI::DescriptorType::RayTracing))
		result |= VK_BUFFER_USAGE_RAY_TRACING_BIT_KHR;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

VkFilter TRAP::Graphics::API::FilterTypeToVkFilter(const RendererAPI::FilterType filter)
{
	switch(filter)
	{
	case RendererAPI::FilterType::Nearest:
		return VK_FILTER_NEAREST;

	case RendererAPI::FilterType::Linear:
		return VK_FILTER_LINEAR;
		
	default:
		return VK_FILTER_LINEAR;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VkSamplerMipmapMode TRAP::Graphics::API::MipMapModeToVkMipMapMode(const RendererAPI::MipMapMode mipMapMode)
{
	switch (mipMapMode)
	{
	case RendererAPI::MipMapMode::Nearest:
		return VK_SAMPLER_MIPMAP_MODE_NEAREST;

	case RendererAPI::MipMapMode::Linear:
		return VK_SAMPLER_MIPMAP_MODE_LINEAR;

	default:
		TRAP_ASSERT(false, "Invalid Mip Map Mode");
		return VK_SAMPLER_MIPMAP_MODE_MAX_ENUM;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VkSamplerAddressMode TRAP::Graphics::API::AddressModeToVkAddressMode(const RendererAPI::AddressMode addressMode)
{
	switch(addressMode)
	{
	case RendererAPI::AddressMode::Mirror:
		return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;

	case RendererAPI::AddressMode::Repeat:
		return VK_SAMPLER_ADDRESS_MODE_REPEAT;

	case RendererAPI::AddressMode::ClampToEdge:
		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

	case RendererAPI::AddressMode::ClampToBorder:
		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		
	default:
		return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VkSetObjectName(VkDevice device, const uint64_t handle, const VkObjectType type, const char* name)
{
#if defined(ENABLE_GRAPHICS_DEBUG)
	if (VulkanRenderer::s_debugUtilsExtension)
	{
		VkDebugUtilsObjectNameInfoEXT nameInfo = VulkanInits::DebugUtilsObjectNameInfo(type, handle, name);
		VkCall(vkSetDebugUtilsObjectNameEXT(device, &nameInfo));
	}
#endif
}