#include "TRAPPCH.h"
#include "VulkanCommon.h"

bool TRAP::Graphics::API::ErrorCheck(const VkResult result, const char* function, const char* file, const int32_t line)
{
	if (result < 0)
	{
		switch (result)
		{
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_OUT_OF_HOST_MEMORY: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_OUT_OF_DEVICE_MEMORY: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_INITIALIZATION_FAILED:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_INITIALIZATION_FAILED: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_DEVICE_LOST:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_DEVICE_LOST: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_MEMORY_MAP_FAILED:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_MEMORY_MAP_FAILED: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_LAYER_NOT_PRESENT:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_LAYER_NOT_PRESENT: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_EXTENSION_NOT_PRESENT:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_EXTENSION_NOT_PRESENT: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_FEATURE_NOT_PRESENT:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_FEATURE_NOT_PRESENT: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_INCOMPATIBLE_DRIVER:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_INCOMPATIBLE_DRIVER: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_TOO_MANY_OBJECTS:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_TOO_MANY_OBJECTS: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_FORMAT_NOT_SUPPORTED:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_FORMAT_NOT_SUPPORTED: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_SURFACE_LOST_KHR:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_SURFACE_LOST_KHR: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: ", function, " @[", file, ':', line, ']');
			break;
		case VK_SUBOPTIMAL_KHR:
			TP_ERROR("[Renderer][Vulkan] VK_SUBOPTIMAL_KHR: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_OUT_OF_DATE_KHR: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_VALIDATION_FAILED_EXT:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_VALIDATION_FAILED_EXT: ", function, " @[", file, ':', line, ']');
			break;

		default:
			TP_ERROR("[Renderer][Vulkan] Unknown error", function, " @[", file, ':', line, ']');
			break;
		}

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::API::VkShaderStageFlagBitsToString(const VkShaderStageFlagBits stage)
{
	switch (stage)
	{
	case VK_SHADER_STAGE_VERTEX_BIT:
		return "VertexShader";

	case VK_SHADER_STAGE_FRAGMENT_BIT:
		return "FragmentShader";

	case VK_SHADER_STAGE_GEOMETRY_BIT:
		return "GeometryShader";

	case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
		return "TessellationControllerShader";

	case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
		return "TessellationEvaluationShader";

	case VK_SHADER_STAGE_COMPUTE_BIT:
		return "ComputeShader";

	default:
		return "";
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::API::BaseTypeToString(const spirv_cross::SPIRType::BaseType type)
{
	switch (type)
	{
	case spirv_cross::SPIRType::Unknown:
		return "Unknown";
		
	case spirv_cross::SPIRType::Void:
		return "Void";
		
	case spirv_cross::SPIRType::Boolean:
		return "Boolean";
		
	case spirv_cross::SPIRType::Char:
		return "Char";
		
	case spirv_cross::SPIRType::SByte:
		return "SByte";
		
	case spirv_cross::SPIRType::UByte:
		return "UByte";
		
	case spirv_cross::SPIRType::Short:
		return "Short";
		
	case spirv_cross::SPIRType::UShort:
		return "UShort";
		
	case spirv_cross::SPIRType::Int:
		return "Int";
		
	case spirv_cross::SPIRType::UInt:
		return "UInt";
		
	case spirv_cross::SPIRType::Int64:
		return "Int64";
		
	case spirv_cross::SPIRType::UInt64:
		return "UInt64";
		
	case spirv_cross::SPIRType::AtomicCounter:
		return "AtomicCounter";
		
	case spirv_cross::SPIRType::Half:
		return "Half";
		
	case spirv_cross::SPIRType::Float:
		return "Float";
		
	case spirv_cross::SPIRType::Double:
		return "Double";
		
	case spirv_cross::SPIRType::Struct:
		return "Struct";
		
	case spirv_cross::SPIRType::Image:
		return "Image";
		
	case spirv_cross::SPIRType::SampledImage:
		return "SampledImage";
		
	case spirv_cross::SPIRType::Sampler:
		return "Sampler";
		
	case spirv_cross::SPIRType::ControlPointArray:
		return "ControlPointArray";
		
	case spirv_cross::SPIRType::AccelerationStructure:
		return "AccelerationStructure";

	default:
		return "BaseTypeToString(unknown)";
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::API::StorageClassToString(const spv::StorageClass sc)
{
	switch (sc) 
	{
	case spv::StorageClassUniformConstant:
		return "UniformConstant";
		
	case spv::StorageClassInput:
		return "Input";
		
	case spv::StorageClassUniform:
		return "Uniform";
		
	case spv::StorageClassOutput:
		return "Output";
		
	case spv::StorageClassWorkgroup:
		return "Workgroup";
		
	case spv::StorageClassCrossWorkgroup:
		return "CrossWorkgroup";
		
	case spv::StorageClassPrivate:
		return "Private";
		
	case spv::StorageClassFunction:
		return "Function";
		
	case spv::StorageClassGeneric:
		return "Generic";
		
	case spv::StorageClassPushConstant:
		return "PushConstant";
		
	case spv::StorageClassAtomicCounter:
		return "AtomicCounter";
		
	case spv::StorageClassImage:
		return "Image";
		
	case spv::StorageClassStorageBuffer:
		return "StorageBuffer";

	case spv::StorageClassCallableDataKHR:
		return "StorageClassCallableDataKHR";
		
	case spv::StorageClassIncomingCallableDataKHR:
		return "StorageClassIncomingCallableDataKHR";
		
	case spv::StorageClassRayPayloadKHR:
		return "StorageClassRayPayloadKHR";
		
	case spv::StorageClassHitAttributeKHR:
		return "StorageClassHitAttributeKHR";
		
	case spv::StorageClassIncomingRayPayloadKHR:
		return "StorageClassIncomingRayPayloadKHR";
		
	case spv::StorageClassShaderRecordBufferKHR:
		return "StorageClassShaderRecordBufferKHR";
		
	case spv::StorageClassPhysicalStorageBuffer:
		return "StorageClassPhysicalStorageBuffer";
		
	case spv::StorageClassMax:
		return "";

	default:
		return "StorageClassToString(unknown)";
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::API::DecorationToString(const spv::Decoration dec)
{
	switch (dec)
	{
	case spv::DecorationRelaxedPrecision:
	case spv::DecorationSpecId:
		return "SpecId";
		
	case spv::DecorationBlock:
		return "Block";
		
	case spv::DecorationBufferBlock:
		return "BufferBlock";
		
	case spv::DecorationRowMajor:
		return "RowMajor";
		
	case spv::DecorationColMajor:
		return "ColMajor";
		
	case spv::DecorationArrayStride:
		return "ArrayStride";
		
	case spv::DecorationMatrixStride:
		return "MatrixStride";
		
	case spv::DecorationGLSLShared:
		return "GLSLShared";
		
	case spv::DecorationGLSLPacked:
		return "GLSLPacked";
		
	case spv::DecorationCPacked:
		return "CPacked";
		
	case spv::DecorationBuiltIn:
		return "BuiltIn";
		
	case spv::DecorationNoPerspective:
		return "NoPerspective";
		
	case spv::DecorationFlat:
		return "Flat";
		
	case spv::DecorationPatch:
		return "Patch";
		
	case spv::DecorationCentroid:
		return "Centroid";
		
	case spv::DecorationSample:
		return "Sample";
		
	case spv::DecorationInvariant:
		return "Invariant";
		
	case spv::DecorationRestrict:
		return "Restrict";
		
	case spv::DecorationAliased:
		return "Aliased";
		
	case spv::DecorationVolatile:
		return "Volatile";
		
	case spv::DecorationConstant:
		return "Constant";
		
	case spv::DecorationCoherent:
		return "Coherent";
		
	case spv::DecorationNonWritable:
		return "NonWritable";
		
	case spv::DecorationNonReadable:
		return "NonReadable";
		
	case spv::DecorationUniform:
		return "Uniform";
		
	case spv::DecorationUniformId:
		return "UniformId";
		
	case spv::DecorationSaturatedConversion:
		return "SaturatedConversion";
		
	case spv::DecorationStream:
		return "Stream";
		
	case spv::DecorationLocation:
		return "Location";
		
	case spv::DecorationComponent:
		return "Component";
		
	case spv::DecorationIndex:
		return "Index";
		
	case spv::DecorationBinding:
		return "Binding";
		
	case spv::DecorationDescriptorSet:
		return "DescriptorSet";
		
	case spv::DecorationOffset:
		return "Offset";
		
	case spv::DecorationXfbBuffer:
		return "XfbBuffer";
		
	case spv::DecorationXfbStride:
		return "XfbStride";
		
	case spv::DecorationFuncParamAttr:
		return "FuncParamAttr";
		
	case spv::DecorationFPRoundingMode:
		return "FPRoundingMode";
		
	case spv::DecorationFPFastMathMode:
		return "FPFastMathMode";
		
	case spv::DecorationLinkageAttributes:
		return "LinkageAttributes";
		
	case spv::DecorationNoContraction:
		return "NoContraction";
		
	case spv::DecorationInputAttachmentIndex:
		return "InputAttachmentIndex";
		
	case spv::DecorationAlignment:
		return "Alignment";
		
	case spv::DecorationMaxByteOffset:
		return "MaxByteOffset";
		
	case spv::DecorationAlignmentId:
		return "AlignmentId";
		
	case spv::DecorationMaxByteOffsetId:
		return "MaxByteOffsetId";
		
	case spv::DecorationExplicitInterpAMD:
		return "ExplicitInterpAMD";
		
	case spv::DecorationOverrideCoverageNV:
		return "OverrideCoverageNV";
		
	case spv::DecorationPassthroughNV:
		return "PassthroughNV";
		
	case spv::DecorationViewportRelativeNV:
		return "ViewportRelativeNV";
		
	case spv::DecorationSecondaryViewportRelativeNV:
		return "SecondaryViewportRelativeNV";
		
	case spv::DecorationNoSignedWrap:
		return "DecorationNoSignedWrap";
		
	case spv::DecorationNoUnsignedWrap:
		return "DecorationNoUnsignedWrap";
		
	case spv::DecorationPerPrimitiveNV:
		return "DecorationPerPrimitiveNV";
		
	case spv::DecorationPerViewNV:
		return "DecorationPerViewNV";
		
	case spv::DecorationPerTaskNV:
		return "DecorationPerTaskNV";
		
	case spv::DecorationPerVertexNV:
		return "DecorationPerVertexNV";
		
	case spv::DecorationNonUniform:
		return "DecorationNonUniform";
		
	case spv::DecorationRestrictPointer:
		return "DecorationRestrictPointer";
		
	case spv::DecorationAliasedPointer:
		return "DecorationAliasedPointer";
		
	case spv::DecorationCounterBuffer:
		return "DecorationCounterBuffer";
		
	case spv::DecorationUserSemantic:
		return "DecorationUserSemantic";
		
	case spv::DecorationUserTypeGOOGLE:
		return "DecorationUserTypeGOOGLE";
		
	case spv::DecorationMax:
		return "";

	default:
		return "DecorationToString(unknown)";
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::API::VkDescriptorTypeToString(const VkDescriptorType type)
{
	switch(type)
	{
	case VK_DESCRIPTOR_TYPE_SAMPLER:
		return "Sampler";

	case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
		return "ImageSampler";

	case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
		return "SampledImage";

	case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
		return "StorageImage";

	case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
		return "UniformTexelBuffer";

	case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
		return "StorageTexelBuffer";

	case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
		return "UniformBuffer";

	case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
		return "StorageBuffer";

	case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
		return "UniformBufferDynamic";

	case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
		return "StorageBufferDynamic";

	case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
		return "InputAttachment";

	case VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT:
		return "InlineUniformBlockEXT";

	case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
		return "AccelerationStructureKHR";
		
	default:
		return "";
	}
}