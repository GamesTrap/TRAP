#include "TRAPPCH.h"
#include "VulkanCommon.h"

#include "Objects/VulkanQueue.h"
#include "VulkanRenderer.h"
#include "Graphics/RenderCommand.h"
#include "Objects/VulkanInits.h"
#include "Objects/VulkanTexture.h"

bool TRAP::Graphics::API::ErrorCheck(const VkResult result, const char* function, const char* file,
                                     const int32_t line)
{
	if(result >= 0)
		return true;

	switch (result)
	{
	case VK_ERROR_OUT_OF_HOST_MEMORY:
		TP_ERROR(Log::RendererVulkanPrefix, "VK_ERROR_OUT_OF_HOST_MEMORY: ", function, " @[", file, ':', line, ']');
		break;
	case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		TP_ERROR(Log::RendererVulkanPrefix, "VK_ERROR_OUT_OF_DEVICE_MEMORY: ", function, " @[", file, ':', line,
		         ']');
		break;
	case VK_ERROR_INITIALIZATION_FAILED:
		TP_ERROR(Log::RendererVulkanPrefix, "VK_ERROR_INITIALIZATION_FAILED: ", function, " @[", file, ':', line,
		         ']');
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
		TP_ERROR(Log::RendererVulkanPrefix, "VK_ERROR_EXTENSION_NOT_PRESENT: ", function, " @[", file, ':', line,
		         ']');
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
		TP_ERROR(Log::RendererVulkanPrefix, "VK_ERROR_FORMAT_NOT_SUPPORTED: ", function, " @[", file, ':', line,
		         ']');
		break;
	case VK_ERROR_SURFACE_LOST_KHR:
		TP_ERROR(Log::RendererVulkanPrefix, "VK_ERROR_SURFACE_LOST_KHR: ", function, " @[", file, ':', line, ']');
		break;
	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
		TP_ERROR(Log::RendererVulkanPrefix, "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: ", function, " @[", file, ':', line,
		         ']');
		break;
	case VK_SUBOPTIMAL_KHR:
		TP_ERROR(Log::RendererVulkanPrefix, "VK_SUBOPTIMAL_KHR: ", function, " @[", file, ':', line, ']');
		break;
	case VK_ERROR_OUT_OF_DATE_KHR:
		TP_ERROR(Log::RendererVulkanPrefix, "VK_ERROR_OUT_OF_DATE_KHR: ", function, " @[", file, ':', line, ']');
		break;
	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
		TP_ERROR(Log::RendererVulkanPrefix, "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: ", function, " @[", file, ':', line,
		         ']');
		break;
	case VK_ERROR_VALIDATION_FAILED_EXT:
		TP_ERROR(Log::RendererVulkanPrefix, "VK_ERROR_VALIDATION_FAILED_EXT: ", function, " @[", file, ':', line,
		         ']');
		break;

	default:
		TP_ERROR(Log::RendererVulkanPrefix, "Unknown error", function, " @[", file, ':', line, ']');
		break;
	}

	return false;
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
	switch(sampleCount)
	{
	case RendererAPI::SampleCount::SampleCount1:
		return VK_SAMPLE_COUNT_1_BIT;

	case RendererAPI::SampleCount::SampleCount2:
		return VK_SAMPLE_COUNT_2_BIT;

	case RendererAPI::SampleCount::SampleCount4:
		return VK_SAMPLE_COUNT_4_BIT;

	case RendererAPI::SampleCount::SampleCount8:
		return VK_SAMPLE_COUNT_8_BIT;

	case RendererAPI::SampleCount::SampleCount16:
		return VK_SAMPLE_COUNT_16_BIT;

	default:
		return VK_SAMPLE_COUNT_1_BIT;
	}
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
	switch(format)
	{
		//Depth
	case VK_FORMAT_D16_UNORM:
	case VK_FORMAT_X8_D24_UNORM_PACK32:
	case VK_FORMAT_D32_SFLOAT:
		return VK_IMAGE_ASPECT_DEPTH_BIT;

		//Stencil
	case VK_FORMAT_S8_UINT:
		return VK_IMAGE_ASPECT_STENCIL_BIT;

		//Depth/Stencil
	case VK_FORMAT_D16_UNORM_S8_UINT:
	case VK_FORMAT_D24_UNORM_S8_UINT:
	case VK_FORMAT_D32_SFLOAT_S8_UINT:
	{
		VkImageAspectFlags res = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (includeStencilBit)
			res |= VK_IMAGE_ASPECT_STENCIL_BIT;
		return res;
	}

	default:
		//Assume everything else is Color
		return VK_IMAGE_ASPECT_COLOR_BIT;
	}
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
	VkFormatFeatureFlags result = 0;

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

VkBufferUsageFlags TRAP::Graphics::API::DescriptorTypeToVkBufferUsage(const RendererAPI::DescriptorType usage,
                                                                      const bool typed)
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
		result |= VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR;

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

void TRAP::Graphics::API::VkSetObjectName(VkDevice device, const uint64_t handle, const VkObjectType type,
                                          const std::string& name)
{
#if defined(ENABLE_GRAPHICS_DEBUG)
	if (VulkanRenderer::s_debugUtilsExtension)
	{
		VkDebugUtilsObjectNameInfoEXT nameInfo = VulkanInits::DebugUtilsObjectNameInfo(type, handle, name);
		VkCall(vkSetDebugUtilsObjectNameEXT(device, &nameInfo));
	}
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

VkDescriptorType TRAP::Graphics::API::DescriptorTypeToVkDescriptorType(const RendererAPI::DescriptorType type)
{
	switch(type)
	{
	case RendererAPI::DescriptorType::Undefined:
		TRAP_ASSERT("Invalid DescriptorInfo Type");
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

	case RendererAPI::DescriptorType::RayTracing:
		return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;

	default:
		TRAP_ASSERT("Invalid DescriptorInfo Type");
		return VK_DESCRIPTOR_TYPE_MAX_ENUM;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VkShaderStageFlags TRAP::Graphics::API::ShaderStageToVkShaderStageFlags(const RendererAPI::ShaderStage stages)
{
	VkShaderStageFlags res = 0;

	if (static_cast<uint32_t>(stages & RendererAPI::ShaderStage::AllGraphics))
		return VK_SHADER_STAGE_ALL_GRAPHICS;

	if (static_cast<uint32_t>(stages & RendererAPI::ShaderStage::Vertex))
		res |= VK_SHADER_STAGE_VERTEX_BIT;
	if (static_cast<uint32_t>(stages & RendererAPI::ShaderStage::Geometry))
		res |= VK_SHADER_STAGE_GEOMETRY_BIT;
	if (static_cast<uint32_t>(stages & RendererAPI::ShaderStage::TessellationEvaluation))
		res |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	if (static_cast<uint32_t>(stages & RendererAPI::ShaderStage::TessellationControl))
		res |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	if (static_cast<uint32_t>(stages & RendererAPI::ShaderStage::Compute))
		res |= VK_SHADER_STAGE_COMPUTE_BIT;
	if (static_cast<uint32_t>(stages & RendererAPI::ShaderStage::RayTracing))
		res |= (VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR |
			VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR |
			VK_SHADER_STAGE_INTERSECTION_BIT_KHR | VK_SHADER_STAGE_CALLABLE_BIT_KHR);

	TRAP_ASSERT(res != 0);
	return res;
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineCacheCreateFlags TRAP::Graphics::API::PipelineCacheFlagsToVkPipelineCacheCreateFlags(const RendererAPI::PipelineCacheFlags flags)
{
	VkPipelineCacheCreateFlags ret = 0;

	if (static_cast<uint32_t>(flags & RendererAPI::PipelineCacheFlags::ExternallySynchronized))
		ret |= VK_PIPELINE_CACHE_CREATE_EXTERNALLY_SYNCHRONIZED_BIT_EXT;

	return ret;
}

//-------------------------------------------------------------------------------------------------------------------//

VkAccessFlags TRAP::Graphics::API::ResourceStateToVkAccessFlags(const RendererAPI::ResourceState state)
{
	VkAccessFlags ret = 0;

	if (static_cast<uint32_t>(state & RendererAPI::ResourceState::CopySource))
		ret |= VK_ACCESS_TRANSFER_READ_BIT;
	if (static_cast<uint32_t>(state & RendererAPI::ResourceState::CopyDestination))
		ret |= VK_ACCESS_TRANSFER_WRITE_BIT;
	if (static_cast<uint32_t>(state & RendererAPI::ResourceState::VertexAndConstantBuffer))
		ret |= VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
	if (static_cast<uint32_t>(state & RendererAPI::ResourceState::IndexBuffer))
		ret |= VK_ACCESS_INDEX_READ_BIT;
	if (static_cast<uint32_t>(state & RendererAPI::ResourceState::UnorderedAccess))
		ret |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
	if (static_cast<uint32_t>(state & RendererAPI::ResourceState::IndirectArgument))
		ret |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
	if (static_cast<uint32_t>(state & RendererAPI::ResourceState::RenderTarget))
		ret |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	if (static_cast<uint32_t>(state & RendererAPI::ResourceState::DepthWrite))
		ret |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	if (static_cast<uint32_t>(state & RendererAPI::ResourceState::ShaderResource))
		ret |= VK_ACCESS_SHADER_READ_BIT;
	if (static_cast<uint32_t>(state & RendererAPI::ResourceState::Present))
		ret |= VK_ACCESS_MEMORY_READ_BIT;
	if (static_cast<uint32_t>(state & RendererAPI::ResourceState::RayTracingAccelerationStructure))
		ret |= VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR | VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;

	return ret;
}

//-------------------------------------------------------------------------------------------------------------------//

VkImageLayout TRAP::Graphics::API::ResourceStateToVkImageLayout(const RendererAPI::ResourceState usage)
{
	if (static_cast<uint32_t>(usage & RendererAPI::ResourceState::CopySource))
		return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

	if (static_cast<uint32_t>(usage & RendererAPI::ResourceState::CopyDestination))
		return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

	if (static_cast<uint32_t>(usage & RendererAPI::ResourceState::RenderTarget))
		return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	if (static_cast<uint32_t>(usage & RendererAPI::ResourceState::DepthWrite))
		return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	if (static_cast<uint32_t>(usage & RendererAPI::ResourceState::UnorderedAccess))
		return VK_IMAGE_LAYOUT_GENERAL;

	if (static_cast<uint32_t>(usage & RendererAPI::ResourceState::ShaderResource))
		return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	if (static_cast<uint32_t>(usage & RendererAPI::ResourceState::Present))
		return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	if (static_cast<uint32_t>(usage & RendererAPI::ResourceState::Common))
		return VK_IMAGE_LAYOUT_GENERAL;

	return VK_IMAGE_LAYOUT_UNDEFINED;
}

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
			if (VulkanRenderer::s_raytracingExtension)
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

VkQueryType TRAP::Graphics::API::QueryTypeToVkQueryType(const RendererAPI::QueryType type)
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
		TRAP_ASSERT(false, "Invalid query heap type");
		return VK_QUERY_TYPE_MAX_ENUM;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VkImageAspectFlags TRAP::Graphics::API::ClearFlagsToVKImageAspectFlags(const RendererAPI::ClearFlags flags)
{
	switch(flags)
	{
	case RendererAPI::ClearFlags::Color:
		return VK_IMAGE_ASPECT_COLOR_BIT;

	case RendererAPI::ClearFlags::Depth:
		return VK_IMAGE_ASPECT_DEPTH_BIT;

	case RendererAPI::ClearFlags::Stencil:
		return VK_IMAGE_ASPECT_STENCIL_BIT;

	default:
		return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineColorBlendStateCreateInfo TRAP::Graphics::API::UtilToBlendDesc(const RendererAPI::BlendStateDesc& desc,
	                                                                     std::vector<VkPipelineColorBlendAttachmentState>& attachments)
{
	int32_t blendDescIndex = 0;

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
	imagePlaneMemReqInfo.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2;
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

TRAP::Graphics::RendererAPI::ImageFormat TRAP::Graphics::API::VulkanGetRecommendedSwapchainFormat(const bool)
{
	return RendererAPI::ImageFormat::B8G8R8A8_UNORM; //TODO HDR support
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::ImageFormat TRAP::Graphics::API::ImageFormatFromVkFormat(VkFormat format)
{
	switch(format)
	{
	case VK_FORMAT_UNDEFINED: return RendererAPI::ImageFormat::Undefined;
	case VK_FORMAT_R4G4_UNORM_PACK8: return RendererAPI::ImageFormat::G4R4_UNORM;
	case VK_FORMAT_R4G4B4A4_UNORM_PACK16: return RendererAPI::ImageFormat::A4B4G4R4_UNORM;
	case VK_FORMAT_B4G4R4A4_UNORM_PACK16: return RendererAPI::ImageFormat::A4R4G4B4_UNORM;
	case VK_FORMAT_R5G6B5_UNORM_PACK16: return RendererAPI::ImageFormat::B5G6R5_UNORM;
	case VK_FORMAT_B5G6R5_UNORM_PACK16: return RendererAPI::ImageFormat::R5G6B5_UNORM;
	case VK_FORMAT_R5G5B5A1_UNORM_PACK16: return RendererAPI::ImageFormat::A1B5G5R5_UNORM;
	case VK_FORMAT_B5G5R5A1_UNORM_PACK16: return RendererAPI::ImageFormat::A1R5G5B5_UNORM;
	case VK_FORMAT_A1R5G5B5_UNORM_PACK16: return RendererAPI::ImageFormat::B5G5R5A1_UNORM;
	case VK_FORMAT_R8_UNORM: return RendererAPI::ImageFormat::R8_UNORM;
	case VK_FORMAT_R8_SNORM: return RendererAPI::ImageFormat::R8_SNORM;
	case VK_FORMAT_R8_UINT: return RendererAPI::ImageFormat::R8_UINT;
	case VK_FORMAT_R8_SINT: return RendererAPI::ImageFormat::R8_SINT;
	case VK_FORMAT_R8_SRGB: return RendererAPI::ImageFormat::R8_SRGB;
	case VK_FORMAT_R8G8_UNORM: return RendererAPI::ImageFormat::R8G8_UNORM;
	case VK_FORMAT_R8G8_SNORM: return RendererAPI::ImageFormat::R8G8_SNORM;
	case VK_FORMAT_R8G8_UINT: return RendererAPI::ImageFormat::R8G8_UINT;
	case VK_FORMAT_R8G8_SINT: return RendererAPI::ImageFormat::R8G8_SINT;
	case VK_FORMAT_R8G8_SRGB: return RendererAPI::ImageFormat::R8G8_SRGB;
	case VK_FORMAT_R8G8B8_UNORM: return RendererAPI::ImageFormat::R8G8B8_UNORM;
	case VK_FORMAT_R8G8B8_SNORM: return RendererAPI::ImageFormat::R8G8B8_SNORM;
	case VK_FORMAT_R8G8B8_UINT: return RendererAPI::ImageFormat::R8G8B8_UINT;
	case VK_FORMAT_R8G8B8_SINT: return RendererAPI::ImageFormat::R8G8B8_SINT;
	case VK_FORMAT_R8G8B8_SRGB: return RendererAPI::ImageFormat::R8G8B8_SRGB;
	case VK_FORMAT_B8G8R8_UNORM: return RendererAPI::ImageFormat::B8G8R8_UNORM;
	case VK_FORMAT_B8G8R8_SNORM: return RendererAPI::ImageFormat::B8G8R8_SNORM;
	case VK_FORMAT_B8G8R8_UINT: return RendererAPI::ImageFormat::B8G8R8_UINT;
	case VK_FORMAT_B8G8R8_SINT: return RendererAPI::ImageFormat::B8G8R8_SINT;
	case VK_FORMAT_B8G8R8_SRGB: return RendererAPI::ImageFormat::B8G8R8_SRGB;
	case VK_FORMAT_R8G8B8A8_UNORM: return RendererAPI::ImageFormat::R8G8B8A8_UNORM;
	case VK_FORMAT_R8G8B8A8_SNORM: return RendererAPI::ImageFormat::R8G8B8A8_SNORM;
	case VK_FORMAT_R8G8B8A8_UINT: return RendererAPI::ImageFormat::R8G8B8A8_UINT;
	case VK_FORMAT_R8G8B8A8_SINT: return RendererAPI::ImageFormat::R8G8B8A8_SINT;
	case VK_FORMAT_R8G8B8A8_SRGB: return RendererAPI::ImageFormat::R8G8B8A8_SRGB;
	case VK_FORMAT_B8G8R8A8_UNORM: return RendererAPI::ImageFormat::B8G8R8A8_UNORM;
	case VK_FORMAT_B8G8R8A8_SNORM: return RendererAPI::ImageFormat::B8G8R8A8_SNORM;
	case VK_FORMAT_B8G8R8A8_UINT: return RendererAPI::ImageFormat::B8G8R8A8_UINT;
	case VK_FORMAT_B8G8R8A8_SINT: return RendererAPI::ImageFormat::B8G8R8A8_SINT;
	case VK_FORMAT_B8G8R8A8_SRGB: return RendererAPI::ImageFormat::B8G8R8A8_SRGB;
	case VK_FORMAT_A2R10G10B10_UNORM_PACK32: return RendererAPI::ImageFormat::A2R10G10B10_UNORM;
	case VK_FORMAT_A2R10G10B10_UINT_PACK32: return RendererAPI::ImageFormat::A2R10G10B10_UINT;
	case VK_FORMAT_A2B10G10R10_UNORM_PACK32: return RendererAPI::ImageFormat::A2B10G10R10_UNORM;
	case VK_FORMAT_A2B10G10R10_UINT_PACK32: return RendererAPI::ImageFormat::A2B10G10R10_UINT;
	case VK_FORMAT_R16_UNORM: return RendererAPI::ImageFormat::R16_UNORM;
	case VK_FORMAT_R16_SNORM: return RendererAPI::ImageFormat::R16_SNORM;
	case VK_FORMAT_R16_UINT: return RendererAPI::ImageFormat::R16_UINT;
	case VK_FORMAT_R16_SINT: return RendererAPI::ImageFormat::R16_SINT;
	case VK_FORMAT_R16_SFLOAT: return RendererAPI::ImageFormat::R16_SFLOAT;
	case VK_FORMAT_R16G16_UNORM: return RendererAPI::ImageFormat::R16G16_UNORM;
	case VK_FORMAT_R16G16_SNORM: return RendererAPI::ImageFormat::R16G16_SNORM;
	case VK_FORMAT_R16G16_UINT: return RendererAPI::ImageFormat::R16G16_UINT;
	case VK_FORMAT_R16G16_SINT: return RendererAPI::ImageFormat::R16G16_SINT;
	case VK_FORMAT_R16G16_SFLOAT: return RendererAPI::ImageFormat::R16G16_SFLOAT;
	case VK_FORMAT_R16G16B16_UNORM: return RendererAPI::ImageFormat::R16G16B16_UNORM;
	case VK_FORMAT_R16G16B16_SNORM: return RendererAPI::ImageFormat::R16G16B16_SNORM;
	case VK_FORMAT_R16G16B16_UINT: return RendererAPI::ImageFormat::R16G16B16_UINT;
	case VK_FORMAT_R16G16B16_SINT: return RendererAPI::ImageFormat::R16G16B16_SINT;
	case VK_FORMAT_R16G16B16_SFLOAT: return RendererAPI::ImageFormat::R16G16B16_SFLOAT;
	case VK_FORMAT_R16G16B16A16_UNORM: return RendererAPI::ImageFormat::R16G16B16A16_UNORM;
	case VK_FORMAT_R16G16B16A16_SNORM: return RendererAPI::ImageFormat::R16G16B16A16_SNORM;
	case VK_FORMAT_R16G16B16A16_UINT: return RendererAPI::ImageFormat::R16G16B16A16_UINT;
	case VK_FORMAT_R16G16B16A16_SINT: return RendererAPI::ImageFormat::R16G16B16A16_SINT;
	case VK_FORMAT_R16G16B16A16_SFLOAT: return RendererAPI::ImageFormat::R16G16B16A16_SFLOAT;
	case VK_FORMAT_R32_UINT: return RendererAPI::ImageFormat::R32_UINT;
	case VK_FORMAT_R32_SINT: return RendererAPI::ImageFormat::R32_SINT;
	case VK_FORMAT_R32_SFLOAT: return RendererAPI::ImageFormat::R32_SFLOAT;
	case VK_FORMAT_R32G32_UINT: return RendererAPI::ImageFormat::R32G32_UINT;
	case VK_FORMAT_R32G32_SINT: return RendererAPI::ImageFormat::R32G32_SINT;
	case VK_FORMAT_R32G32_SFLOAT: return RendererAPI::ImageFormat::R32G32_SFLOAT;
	case VK_FORMAT_R32G32B32_UINT: return RendererAPI::ImageFormat::R32G32B32_UINT;
	case VK_FORMAT_R32G32B32_SINT: return RendererAPI::ImageFormat::R32G32B32_SINT;
	case VK_FORMAT_R32G32B32_SFLOAT: return RendererAPI::ImageFormat::R32G32B32_SFLOAT;
	case VK_FORMAT_R32G32B32A32_UINT: return RendererAPI::ImageFormat::R32G32B32A32_UINT;
	case VK_FORMAT_R32G32B32A32_SINT: return RendererAPI::ImageFormat::R32G32B32A32_SINT;
	case VK_FORMAT_R32G32B32A32_SFLOAT: return RendererAPI::ImageFormat::R32G32B32A32_SFLOAT;
	case VK_FORMAT_R64_UINT: return RendererAPI::ImageFormat::R64_UINT;
	case VK_FORMAT_R64_SINT: return RendererAPI::ImageFormat::R64_SINT;
	case VK_FORMAT_R64_SFLOAT: return RendererAPI::ImageFormat::R64_SFLOAT;
	case VK_FORMAT_R64G64_UINT: return RendererAPI::ImageFormat::R64G64_UINT;
	case VK_FORMAT_R64G64_SINT: return RendererAPI::ImageFormat::R64G64_SINT;
	case VK_FORMAT_R64G64_SFLOAT: return RendererAPI::ImageFormat::R64G64_SFLOAT;
	case VK_FORMAT_R64G64B64_UINT: return RendererAPI::ImageFormat::R64G64B64_UINT;
	case VK_FORMAT_R64G64B64_SINT: return RendererAPI::ImageFormat::R64G64B64_SINT;
	case VK_FORMAT_R64G64B64_SFLOAT: return RendererAPI::ImageFormat::R64G64B64_SFLOAT;
	case VK_FORMAT_R64G64B64A64_UINT: return RendererAPI::ImageFormat::R64G64B64A64_UINT;
	case VK_FORMAT_R64G64B64A64_SINT: return RendererAPI::ImageFormat::R64G64B64A64_SINT;
	case VK_FORMAT_R64G64B64A64_SFLOAT: return RendererAPI::ImageFormat::R64G64B64A64_SFLOAT;
	case VK_FORMAT_B10G11R11_UFLOAT_PACK32: return RendererAPI::ImageFormat::B10G11R11_UFLOAT;
	case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32: return RendererAPI::ImageFormat::E5B9G9R9_UFLOAT;
	case VK_FORMAT_D16_UNORM: return RendererAPI::ImageFormat::D16_UNORM;
	case VK_FORMAT_X8_D24_UNORM_PACK32: return RendererAPI::ImageFormat::X8_D24_UNORM;
	case VK_FORMAT_D32_SFLOAT: return RendererAPI::ImageFormat::D32_SFLOAT;
	case VK_FORMAT_S8_UINT: return RendererAPI::ImageFormat::S8_UINT;
	case VK_FORMAT_D16_UNORM_S8_UINT: return RendererAPI::ImageFormat::D16_UNORM_S8_UINT;
	case VK_FORMAT_D24_UNORM_S8_UINT: return RendererAPI::ImageFormat::D24_UNORM_S8_UINT;
	case VK_FORMAT_D32_SFLOAT_S8_UINT: return RendererAPI::ImageFormat::D32_SFLOAT_S8_UINT;
	case VK_FORMAT_BC1_RGB_UNORM_BLOCK: return RendererAPI::ImageFormat::DXBC1_RGB_UNORM;
	case VK_FORMAT_BC1_RGB_SRGB_BLOCK: return RendererAPI::ImageFormat::DXBC1_RGB_SRGB;
	case VK_FORMAT_BC1_RGBA_UNORM_BLOCK: return RendererAPI::ImageFormat::DXBC1_RGBA_UNORM;
	case VK_FORMAT_BC1_RGBA_SRGB_BLOCK: return RendererAPI::ImageFormat::DXBC1_RGBA_SRGB;
	case VK_FORMAT_BC2_UNORM_BLOCK: return RendererAPI::ImageFormat::DXBC2_UNORM;
	case VK_FORMAT_BC2_SRGB_BLOCK: return RendererAPI::ImageFormat::DXBC2_SRGB;
	case VK_FORMAT_BC3_UNORM_BLOCK: return RendererAPI::ImageFormat::DXBC3_UNORM;
	case VK_FORMAT_BC3_SRGB_BLOCK: return RendererAPI::ImageFormat::DXBC3_SRGB;
	case VK_FORMAT_BC4_UNORM_BLOCK: return RendererAPI::ImageFormat::DXBC4_UNORM;
	case VK_FORMAT_BC4_SNORM_BLOCK: return RendererAPI::ImageFormat::DXBC4_SNORM;
	case VK_FORMAT_BC5_UNORM_BLOCK: return RendererAPI::ImageFormat::DXBC5_UNORM;
	case VK_FORMAT_BC5_SNORM_BLOCK: return RendererAPI::ImageFormat::DXBC5_SNORM;
	case VK_FORMAT_BC6H_UFLOAT_BLOCK: return RendererAPI::ImageFormat::DXBC6H_UFLOAT;
	case VK_FORMAT_BC6H_SFLOAT_BLOCK: return RendererAPI::ImageFormat::DXBC6H_SFLOAT;
	case VK_FORMAT_BC7_UNORM_BLOCK: return RendererAPI::ImageFormat::DXBC7_UNORM;
	case VK_FORMAT_BC7_SRGB_BLOCK: return RendererAPI::ImageFormat::DXBC7_SRGB;
	case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK: return RendererAPI::ImageFormat::ETC2_R8G8B8_UNORM;
	case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK: return RendererAPI::ImageFormat::ETC2_R8G8B8_SRGB;
	case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK: return RendererAPI::ImageFormat::ETC2_R8G8B8A1_UNORM;
	case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK: return RendererAPI::ImageFormat::ETC2_R8G8B8A1_SRGB;
	case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK: return RendererAPI::ImageFormat::ETC2_R8G8B8A8_UNORM;
	case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK: return RendererAPI::ImageFormat::ETC2_R8G8B8A8_SRGB;
	case VK_FORMAT_EAC_R11_UNORM_BLOCK: return RendererAPI::ImageFormat::ETC2_EAC_R11_UNORM;
	case VK_FORMAT_EAC_R11_SNORM_BLOCK: return RendererAPI::ImageFormat::ETC2_EAC_R11_SNORM;
	case VK_FORMAT_EAC_R11G11_UNORM_BLOCK: return RendererAPI::ImageFormat::ETC2_EAC_R11G11_UNORM;
	case VK_FORMAT_EAC_R11G11_SNORM_BLOCK: return RendererAPI::ImageFormat::ETC2_EAC_R11G11_SNORM;
	case VK_FORMAT_ASTC_4x4_UNORM_BLOCK: return RendererAPI::ImageFormat::ASTC_4x4_UNORM;
	case VK_FORMAT_ASTC_4x4_SRGB_BLOCK: return RendererAPI::ImageFormat::ASTC_4x4_SRGB;
	case VK_FORMAT_ASTC_5x4_UNORM_BLOCK: return RendererAPI::ImageFormat::ASTC_5x4_UNORM;
	case VK_FORMAT_ASTC_5x4_SRGB_BLOCK: return RendererAPI::ImageFormat::ASTC_5x4_SRGB;
	case VK_FORMAT_ASTC_5x5_UNORM_BLOCK: return RendererAPI::ImageFormat::ASTC_5x5_UNORM;
	case VK_FORMAT_ASTC_5x5_SRGB_BLOCK: return RendererAPI::ImageFormat::ASTC_5x5_SRGB;
	case VK_FORMAT_ASTC_6x5_UNORM_BLOCK: return RendererAPI::ImageFormat::ASTC_6x5_UNORM;
	case VK_FORMAT_ASTC_6x5_SRGB_BLOCK: return RendererAPI::ImageFormat::ASTC_6x5_SRGB;
	case VK_FORMAT_ASTC_6x6_UNORM_BLOCK: return RendererAPI::ImageFormat::ASTC_6x6_UNORM;
	case VK_FORMAT_ASTC_6x6_SRGB_BLOCK: return RendererAPI::ImageFormat::ASTC_6x6_SRGB;
	case VK_FORMAT_ASTC_8x5_UNORM_BLOCK: return RendererAPI::ImageFormat::ASTC_8x5_UNORM;
	case VK_FORMAT_ASTC_8x5_SRGB_BLOCK: return RendererAPI::ImageFormat::ASTC_8x5_SRGB;
	case VK_FORMAT_ASTC_8x6_UNORM_BLOCK: return RendererAPI::ImageFormat::ASTC_8x6_UNORM;
	case VK_FORMAT_ASTC_8x6_SRGB_BLOCK: return RendererAPI::ImageFormat::ASTC_8x6_SRGB;
	case VK_FORMAT_ASTC_8x8_UNORM_BLOCK: return RendererAPI::ImageFormat::ASTC_8x8_UNORM;
	case VK_FORMAT_ASTC_8x8_SRGB_BLOCK: return RendererAPI::ImageFormat::ASTC_8x8_SRGB;
	case VK_FORMAT_ASTC_10x5_UNORM_BLOCK: return RendererAPI::ImageFormat::ASTC_10x5_UNORM;
	case VK_FORMAT_ASTC_10x5_SRGB_BLOCK: return RendererAPI::ImageFormat::ASTC_10x5_SRGB;
	case VK_FORMAT_ASTC_10x6_UNORM_BLOCK: return RendererAPI::ImageFormat::ASTC_10x6_UNORM;
	case VK_FORMAT_ASTC_10x6_SRGB_BLOCK: return RendererAPI::ImageFormat::ASTC_10x6_SRGB;
	case VK_FORMAT_ASTC_10x8_UNORM_BLOCK: return RendererAPI::ImageFormat::ASTC_10x8_UNORM;
	case VK_FORMAT_ASTC_10x8_SRGB_BLOCK: return RendererAPI::ImageFormat::ASTC_10x8_SRGB;
	case VK_FORMAT_ASTC_10x10_UNORM_BLOCK: return RendererAPI::ImageFormat::ASTC_10x10_UNORM;
	case VK_FORMAT_ASTC_10x10_SRGB_BLOCK: return RendererAPI::ImageFormat::ASTC_10x10_SRGB;
	case VK_FORMAT_ASTC_12x10_UNORM_BLOCK: return RendererAPI::ImageFormat::ASTC_12x10_UNORM;
	case VK_FORMAT_ASTC_12x10_SRGB_BLOCK: return RendererAPI::ImageFormat::ASTC_12x10_SRGB;
	case VK_FORMAT_ASTC_12x12_UNORM_BLOCK: return RendererAPI::ImageFormat::ASTC_12x12_UNORM;
	case VK_FORMAT_ASTC_12x12_SRGB_BLOCK: return RendererAPI::ImageFormat::ASTC_12x12_SRGB;

	case VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG: return RendererAPI::ImageFormat::PVRTC1_2BPP_UNORM;
	case VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG: return RendererAPI::ImageFormat::PVRTC1_4BPP_UNORM;
	case VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG: return RendererAPI::ImageFormat::PVRTC1_2BPP_SRGB;
	case VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG: return RendererAPI::ImageFormat::PVRTC1_4BPP_SRGB;

	case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
	case VK_FORMAT_R8_USCALED:
	case VK_FORMAT_R8_SSCALED:
	case VK_FORMAT_R8G8_USCALED:
	case VK_FORMAT_R8G8_SSCALED:
	case VK_FORMAT_R8G8B8_USCALED:
	case VK_FORMAT_R8G8B8_SSCALED:
	case VK_FORMAT_B8G8R8_USCALED:
	case VK_FORMAT_B8G8R8_SSCALED:
	case VK_FORMAT_R8G8B8A8_USCALED:
	case VK_FORMAT_R8G8B8A8_SSCALED:
	case VK_FORMAT_B8G8R8A8_USCALED:
	case VK_FORMAT_B8G8R8A8_SSCALED:
	case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
	case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
	case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
	case VK_FORMAT_A8B8G8R8_UINT_PACK32:
	case VK_FORMAT_A8B8G8R8_SINT_PACK32:
	case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
	case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
	case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
	case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
	case VK_FORMAT_A2R10G10B10_SINT_PACK32:
	case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
	case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
	case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
	case VK_FORMAT_A2B10G10R10_SINT_PACK32:
	case VK_FORMAT_R16_USCALED:
	case VK_FORMAT_R16_SSCALED:
	case VK_FORMAT_R16G16_USCALED:
	case VK_FORMAT_R16G16_SSCALED:
	case VK_FORMAT_R16G16B16_USCALED:
	case VK_FORMAT_R16G16B16_SSCALED:
	case VK_FORMAT_R16G16B16A16_USCALED:
	case VK_FORMAT_R16G16B16A16_SSCALED:
	case VK_FORMAT_R10X6_UNORM_PACK16:
	case VK_FORMAT_R10X6G10X6_UNORM_2PACK16:
	case VK_FORMAT_R12X4_UNORM_PACK16:
	case VK_FORMAT_R12X4G12X4_UNORM_2PACK16:
	case VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG:
	case VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG:
	case VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG:
	case VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG:
		return RendererAPI::ImageFormat::Undefined;

	default:
		return RendererAPI::ImageFormat::Undefined;
	}
}