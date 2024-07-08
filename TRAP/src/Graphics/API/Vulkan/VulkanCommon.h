// MIT License

// Copyright (c) 2019 DeanoC

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// The license above only applies to the following functions:
// - ImageFormatToVkFormat
// - ImageFormatFromVkFormat
//
// Modified by Jan "GamesTrap" Schuerkamp

#ifndef TRAP_VULKANCOMMON_H
#define TRAP_VULKANCOMMON_H

#include "Core/PlatformDetection.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/Objects/VulkanInits.h"
#include "Utils/ConstexprMap.h"

namespace TRAP::Graphics::API
{
	class VulkanQueue;
	class VulkanPhysicalDevice;

	/// @brief Check the if given VkResult contains an error.
	/// If the given VkResult contains an error, the function will log information about the error.
	/// @param result VkResult to check.
	/// @param sourceLoc Source location of the function that called the error checker.
	/// @return True for non error codes, otherwise false.
	constexpr bool ErrorCheck(VkResult result, const std::source_location& sourceLoc);
#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
	/// @brief Check the if given NvLL_VK_Status contains an error.
	/// If the given NvLL_VK_Status contains an error, the function will log information about the error.
	/// @param result NvLL_VK_Status to check.
	/// @param sourceLoc Source location of the function that called the error checker.
	/// @return True for non error codes, otherwise false.
	/// @remark @headless This function is not available in headless mode.
	/// @remark This function is only available when NVIDIA Reflex SDK is provided.
	constexpr bool ReflexErrorCheck(NvLL_VK_Status result, const std::source_location& sourceLoc);
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/
	/// @brief Convert the RendererAPI::QueueType to VkQueueFlags.
	/// @param queueType QueueType to convert.
	/// @return Converted VkQueueFlags.
	[[nodiscard]] constexpr VkQueueFlags QueueTypeToVkQueueFlags(RendererAPI::QueueType queueType) noexcept;

	/// @brief Convert the RendererAPI::SampleCount to VkSampleCountFlagBits.
	/// @param sampleCount SampleCount to convert.
	/// @return Converted VkSampleCountFlagBits.
	[[nodiscard]] constexpr VkSampleCountFlagBits SampleCountToVkSampleCount(RendererAPI::SampleCount sampleCount) noexcept;
	/// @brief Convert the ImageFormat to VkFormat.
	/// @param imageFormat ImageFormat to convert.
	/// @return Converted VkFormat.
	[[nodiscard]] constexpr VkFormat ImageFormatToVkFormat(TRAP::Graphics::API::ImageFormat imageFormat) noexcept;
	/// @brief Determine the aspect mask for an image.
	/// @param format ImageFormat.
	/// @param includeStencilBit Whether to include the stencil bit in the aspect mask.
	/// @return VkImageAspectFlags.
	[[nodiscard]] constexpr VkImageAspectFlags DetermineAspectMask(VkFormat format, bool includeStencilBit) noexcept;
	/// @brief Convert the RendererAPI::DescriptorType to VkImageUsageFlags.
	/// @param type DescriptorType to convert.
	/// @return Converted VkImageUsageFlags.
	[[nodiscard]] constexpr VkImageUsageFlags DescriptorTypeToVkImageUsage(RendererAPI::DescriptorType type) noexcept;
	/// @brief Convert the VkImageUsageFlags to VkFormatFeatureFlags.
	/// @param usage VkImageUsageFlags to convert.
	/// @return Converted VkFormatFeatureFlags.
	[[nodiscard]] constexpr VkFormatFeatureFlags VkImageUsageToFormatFeatures(VkImageUsageFlags usage) noexcept;
	/// @brief Convert the RendererAPI::DescriptorType to VkBufferUsage.
	/// @param usage DescriptorType to convert.
	/// @param typed Whether the buffer is a typed buffer.
	/// @return Converted VkBufferUsageFlags.
	[[nodiscard]] constexpr VkBufferUsageFlags DescriptorTypeToVkBufferUsage(RendererAPI::DescriptorType usage, bool typed) noexcept;
	/// @brief Convert the RendererAPI::FilterType to VkFilter.
	/// @param filter FilterType to convert.
	/// @return Converted VkFilter.
	[[nodiscard]] constexpr VkFilter FilterTypeToVkFilter(RendererAPI::FilterType filter) noexcept;
	/// @brief Convert the RendererAPI::MipMapMode to VkSamplerMipmapMode.
	/// @param mipMapMode MipMapMode to convert.
	/// @return Converted VkSamplerMipmapMode.
	[[nodiscard]] constexpr VkSamplerMipmapMode MipMapModeToVkMipMapMode(RendererAPI::MipMapMode mipMapMode) noexcept;
	/// @brief Convert the RendererAPI::AddressMode to VkSamplerAddressMode.
	/// @param addressMode AddressMode to convert.
	/// @return Converted VkSamplerAddressMode.
	[[nodiscard]] constexpr VkSamplerAddressMode AddressModeToVkAddressMode(RendererAPI::AddressMode addressMode) noexcept;
	/// @brief Convert the RendererAPI::DescriptorType to VkDescriptorType.
	/// @param type DescriptorType to convert.
	/// @return Converted VkDescriptorType.
	[[nodiscard]] constexpr VkDescriptorType DescriptorTypeToVkDescriptorType(RendererAPI::DescriptorType type) noexcept;
	/// @brief Convert the RendererAPI::ShaderStage to VkShaderStageFlags.
	/// @param stages ShaderStage(s) to convert.
	/// @return Converted VkShaderStageFlags.
	/// @note This returns all found VkShaderStageFlagBits in stages as a single VkShaderStageFlags value.
	[[nodiscard]] constexpr VkShaderStageFlags ShaderStageToVkShaderStageFlags(RendererAPI::ShaderStage stages) noexcept;
	/// @brief Convert the RendererAPI::ShaderStage to VkShaderStageFlagBits.
	/// @param stages ShaderStage(s) to convert.
	/// @return Converted VkShaderStageFlagBits.
	/// @note This returns the first found VkShaderStageFlagBits.
	[[nodiscard]] constexpr VkShaderStageFlagBits ShaderStageToVkShaderStageFlagBits(RendererAPI::ShaderStage stage) noexcept;
	/// @brief Convert the RendererAPI::PipelineCacheFlags to VkPipelineCacheCreateFlags.
	/// @param flags PipelineCacheFlag to convert.
	/// @return Converted VkPipelineCacheCreateFlags.
	[[nodiscard]] constexpr VkPipelineCacheCreateFlags PipelineCacheFlagsToVkPipelineCacheCreateFlags(RendererAPI::PipelineCacheFlags flags) noexcept;
	/// @brief Convert the RendererAPI::ResourceState to VkAccessFlags.
	/// @param state ResourceState to convert.
	/// @return Converted VkAccessFlags.
	[[nodiscard]] constexpr VkAccessFlags ResourceStateToVkAccessFlags(RendererAPI::ResourceState state) noexcept;
	/// @brief Convert the RendererAPI::ResourceState to VkImageLayout.
	/// @param usage ResourceState to convert.
	/// @return Converted VkImageLayout.
	[[nodiscard]] constexpr VkImageLayout ResourceStateToVkImageLayout(RendererAPI::ResourceState usage) noexcept;
	/// @brief Determine the VkPipelineStageFlags from VkAccessFlags and RendererAPI::QueueType.
	/// @param physicalDevice Vulkan physical device.
	/// @param accessFlags VkAccessFlags.
	/// @param queueType QueueType.
	/// @return Determined VkPipelineStageFlags.
	[[nodiscard]] VkPipelineStageFlags DetermineVkPipelineStageFlags(const VulkanPhysicalDevice& physicalDevice, VkAccessFlags accessFlags, RendererAPI::QueueType queueType) noexcept;
	/// @brief Convert the RendererAPI::QueryType to VkQueryType.
	/// @param type QueryType to convert.
	/// @return Converted VkQueryType.
	[[nodiscard]] constexpr VkQueryType QueryTypeToVkQueryType(RendererAPI::QueryType type) noexcept;
	/// @brief Convert the RendererAPI::PrimitiveTopology to VkPrimitiveTopology.
	/// @param primitiveTopology Primitive topology to convert.
	/// @return Converted VkPrimitiveTopology.
	[[nodiscard]] constexpr VkPrimitiveTopology PrimitiveTopologyToVkPrimitiveTopology(RendererAPI::PrimitiveTopology primitiveToplogy) noexcept;

	/// @brief Utility to create the VkPipelineColorBlendStateCreateInfo struct from
	/// a RendererAPI::BlendStateDesc and color blend attachments.
	/// @param desc BlendStateDesc.
	/// @param attachments Color blend attachments.
	/// @return Created VkPipelineColorBlendStateCreateInfo.
	[[nodiscard]] constexpr VkPipelineColorBlendStateCreateInfo UtilToBlendDesc(const RendererAPI::BlendStateDesc& desc,
	                                                                            std::vector<VkPipelineColorBlendAttachmentState>& attachments) ;
	/// @brief Utility to create the VkPipelineDepthStencilStateCreateInfo struct from
	/// a RendererAPI::DepthStateDesc.
	/// @param desc DepthStateDesc.
	/// @return Created VkPipelineDepthStencilStateCreateInfo.
	[[nodiscard]] constexpr VkPipelineDepthStencilStateCreateInfo UtilToDepthDesc(const RendererAPI::DepthStateDesc& desc);
	/// @brief Utility to create the VkPipelineRasterizationStateCreateInfo struct from
	/// a RendererAPI::RasterizerStateDesc.
	/// @param desc RasterizerStateDesc.
	/// @return Created VkPipelineRasterizationStateCreateInfo.
	[[nodiscard]] constexpr VkPipelineRasterizationStateCreateInfo UtilToRasterizerDesc(const RendererAPI::RasterizerStateDesc& desc);
	/// @brief Utility to retrieve the planar image memory requirement for Vulkan.
	/// @param device Vulkan device.
	/// @param image Vulkan image.
	/// @param planesCount Number of planes.
	/// @param memReq Output Vulkan memory requirement.
	/// @param planesOffsets Output plane offsets.
	void UtilGetPlanarVkImageMemoryRequirement(VkDevice device, VkImage image, u32 planesCount,
	                                           VkMemoryRequirements& memReq, std::vector<u64>& planesOffsets);
	/// @brief Utility to create the VkFragmentShadingRateCombinerOpKHR from
	/// a RendererAPI::ShadingRateCombiner.
	/// @param combiner ShadingRateCombiner.
	/// @return Converted VkFragmentShadingRateCombinerOpKHR.
	[[nodiscard]] constexpr VkFragmentShadingRateCombinerOpKHR ShadingRateCombinerToVkFragmentShadingRateCombinerOpKHR(RendererAPI::ShadingRateCombiner combiner);
	/// @brief Utility to create the VkExtent2D (fragment size) from
	/// a RendererAPI::ShadingRate.
	/// @param rate ShadingRate.
	/// @return Converted VkExtent2D.
	[[nodiscard]] constexpr VkExtent2D ShadingRateToVkExtent2D(RendererAPI::ShadingRate rate);

	/// @brief Retrieve the recommended swapchain format for Vulkan.
	/// @param HDR True if HDR is desired.
	/// @param SRGB True if SRGB is desired.
	/// @return Recommended swapchain format.
	[[nodiscard]] constexpr TRAP::Graphics::API::ImageFormat VulkanGetRecommendedSwapchainFormat(bool HDR, bool SRGB) noexcept;

	/// @brief Convert the VkFormat to ImageFormat.
	/// @param format VkFormat to convert.
	/// @return Converted ImageFormat.
	[[nodiscard]] constexpr TRAP::Graphics::API::ImageFormat ImageFormatFromVkFormat(VkFormat format) noexcept;

	template<typename T>
	requires (std::same_as<decltype(T::sType), VkStructureType>) && requires {T::pNext;}
	/// @brief Links toLink to the pNext member of base and updates base pointer to point to toLink::pNext.
	/// @param base Vulkan structure to set pNext member for.
	/// @param toLink Vulkan structure to link to pNext member of base.
	constexpr void LinkVulkanStruct(VkBaseOutStructure*& base, T& toLink);

#ifdef ENABLE_GRAPHICS_DEBUG
	/// @brief Set a name for a Vulkan object.
	/// @param device Vulkan device.
	/// @param handle Vulkan object.
	/// @param type Vulkan object type.
	/// @param name Name to set.
	void VkSetObjectName(const VulkanDevice& device, u64 handle, VkObjectType type, std::string_view name);
#endif /*ENABLE_GRAPHICS_DEBUG*/

	[[nodiscard]] constexpr VkAttachmentLoadOp VkAttachmentLoadOpTranslator(const RendererAPI::LoadActionType loadActionType)
	{
		using enum TRAP::Graphics::RendererAPI::LoadActionType;

		switch(loadActionType)
		{
		case Load:
			return VK_ATTACHMENT_LOAD_OP_LOAD;
		case Clear:
			return VK_ATTACHMENT_LOAD_OP_CLEAR;
		case DontCare:
			return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		}

		TRAP_ASSERT(false, "VkAttachmentLoadOpTranslator(): Unknown RendererAPI::LoadActionType!");
		return VK_ATTACHMENT_LOAD_OP_MAX_ENUM;
	};


	[[nodiscard]] constexpr VkAttachmentStoreOp VkAttachmentStoreOpTranslator(const RendererAPI::StoreActionType storeActionType)
	{
		using enum TRAP::Graphics::RendererAPI::StoreActionType;

		switch(storeActionType)
		{
		case Store:
			return VK_ATTACHMENT_STORE_OP_STORE;
		case DontCare:
			return VK_ATTACHMENT_STORE_OP_DONT_CARE;
		//Store op none is treated as dont care in most drivers
		case None:
			return VK_ATTACHMENT_STORE_OP_DONT_CARE;
		}

		TRAP_ASSERT(false, "VkAttachmentStoreOpTranslator(): Unknown RendererAPI::StoreActionType!");
		return VK_ATTACHMENT_STORE_OP_MAX_ENUM;
	};

	[[nodiscard]] constexpr VkCompareOp VkComparisonFuncTranslator(const RendererAPI::CompareMode compareMode)
	{
		using enum TRAP::Graphics::RendererAPI::CompareMode;

		switch(compareMode)
		{
		case Never:
			return VK_COMPARE_OP_NEVER;
		case Less:
			return VK_COMPARE_OP_LESS;
		case Equal:
			return VK_COMPARE_OP_EQUAL;
		case LessOrEqual:
			return VK_COMPARE_OP_LESS_OR_EQUAL;
		case Greater:
			return VK_COMPARE_OP_GREATER;
		case NotEqual:
			return VK_COMPARE_OP_NOT_EQUAL;
		case GreaterOrEqual:
			return VK_COMPARE_OP_GREATER_OR_EQUAL;
		case Always:
			return VK_COMPARE_OP_ALWAYS;
		}

		TRAP_ASSERT(false, "VkComparisonFuncTranslator(): Unknown RendererAPI::CompareMode!");
		return VK_COMPARE_OP_MAX_ENUM;
	}

	[[nodiscard]] constexpr VkPipelineBindPoint VkPipelineBindPointTranslator(const RendererAPI::PipelineType pipelineType)
	{
		using enum TRAP::Graphics::RendererAPI::PipelineType;

		switch(pipelineType)
		{
		case Compute:
			return VK_PIPELINE_BIND_POINT_COMPUTE;
		case Graphics:
			return VK_PIPELINE_BIND_POINT_GRAPHICS;
		case RayTracing:
			return VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
		}

		TRAP_ASSERT(false, "VkPipelineBindPointTranslator(): Unknown RendererAPI::PipelineType!");
		return VK_PIPELINE_BIND_POINT_MAX_ENUM;
	}

	[[nodiscard]] constexpr VkBlendFactor VkBlendConstantTranslator(const RendererAPI::BlendConstant blendConstant)
	{
		using enum TRAP::Graphics::RendererAPI::BlendConstant;

		switch(blendConstant)
		{
		case Zero:
			return VK_BLEND_FACTOR_ZERO;
		case One:
			return VK_BLEND_FACTOR_ONE;
		case SrcColor:
			return VK_BLEND_FACTOR_SRC_COLOR;
		case OneMinusSrcColor:
			return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
		case DstColor:
			return VK_BLEND_FACTOR_DST_COLOR;
		case OneMinusDstColor:
			return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
		case SrcAlpha:
			return VK_BLEND_FACTOR_SRC_ALPHA;
		case OneMinusSrcAlpha:
			return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		case DstAlpha:
			return VK_BLEND_FACTOR_DST_ALPHA;
		case OneMinusDstAlpha:
			return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
		case SrcAlphaSaturate:
			return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
		case BlendFactor:
			return VK_BLEND_FACTOR_CONSTANT_COLOR;
		case OneMinusBlendFactor:
			return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
		}

		TRAP_ASSERT(false, "VkBlendConstantTranslator(): Unknown RendererAPI::BlendConstant!");
		return VK_BLEND_FACTOR_MAX_ENUM;
	}

	[[nodiscard]] constexpr VkBlendOp VkBlendOpTranslator(const RendererAPI::BlendMode blendMode)
	{
		using enum RendererAPI::BlendMode;

		switch(blendMode)
		{
		case Add:
			return VK_BLEND_OP_ADD;
		case Subtract:
			return VK_BLEND_OP_SUBTRACT;
		case ReverseSubtract:
			return VK_BLEND_OP_REVERSE_SUBTRACT;
		case Min:
			return VK_BLEND_OP_MIN;
		case Max:
			return VK_BLEND_OP_MAX;
		}

		TRAP_ASSERT(false, "VkBlendOpTranslator(): Unknown RendererAPI::BlendMode!");
		return VK_BLEND_OP_MAX_ENUM;
	}

	[[nodiscard]] constexpr VkStencilOp VkStencilOpTranslator(const RendererAPI::StencilOp stencilOp)
	{
		using enum RendererAPI::StencilOp;

		switch(stencilOp)
		{
		case Keep:
			return VK_STENCIL_OP_KEEP;
		case SetZero:
			return VK_STENCIL_OP_ZERO;
		case Replace:
			return VK_STENCIL_OP_REPLACE;
		case Invert:
			return VK_STENCIL_OP_INVERT;
		case Increment:
			return VK_STENCIL_OP_INCREMENT_AND_WRAP;
		case Decrement:
			return VK_STENCIL_OP_DECREMENT_AND_WRAP;
		case IncrementSaturation:
			return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
		case DecrementSaturation:
			return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
		}

		TRAP_ASSERT(false, "VkStencilOpTranslator(): Unknown RendererAPI::StencilOp!");
		return VK_STENCIL_OP_MAX_ENUM;
	}

	[[nodiscard]] constexpr VkCullModeFlags VkCullModeTranslator(const RendererAPI::CullMode cullMode)
	{
		using enum RendererAPI::CullMode;

		switch(cullMode)
		{
		case None:
			return VK_CULL_MODE_NONE;
		case Back:
			return VK_CULL_MODE_BACK_BIT;
		case Front:
			return VK_CULL_MODE_FRONT_BIT;
		case Both:
			return VK_CULL_MODE_FRONT_AND_BACK;
		}

		TRAP_ASSERT(false, "VkCullModeTranslator(): Unknown RendererAPI::CullMode!");
		return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
	}

	[[nodiscard]] constexpr VkPolygonMode VkFillModeTranslator(const RendererAPI::FillMode fillMode)
	{
		using enum RendererAPI::FillMode;

		switch(fillMode)
		{
		case Solid:
			return VK_POLYGON_MODE_FILL;
		case Line:
			return VK_POLYGON_MODE_LINE;
		case Point:
			return VK_POLYGON_MODE_POINT;
		}

		TRAP_ASSERT(false, "VkFillModeTranslator(): Unknown RendererAPI::FillMode!");
		return VK_POLYGON_MODE_MAX_ENUM;
	}

	[[nodiscard]] constexpr VkFrontFace VkFrontFaceTranslator(const RendererAPI::FrontFace frontFace)
	{
		using enum RendererAPI::FrontFace;

		switch(frontFace)
		{
		case CounterClockwise:
			return VK_FRONT_FACE_COUNTER_CLOCKWISE;
		case Clockwise:
			return VK_FRONT_FACE_CLOCKWISE;
		}

		TRAP_ASSERT(false, "VkFrontFaceTranslator(): Unknown RendererAPI::FrontFace!");
		return VK_FRONT_FACE_MAX_ENUM;
	}

	[[nodiscard]] constexpr VkVertexInputRate VkVertexInputRateTranslator(const RendererAPI::VertexAttributeRate vertexAttributeRate)
	{
		using enum RendererAPI::VertexAttributeRate;

		switch(vertexAttributeRate)
		{
		case Vertex:
			return VK_VERTEX_INPUT_RATE_VERTEX;
		case Instance:
			return VK_VERTEX_INPUT_RATE_INSTANCE;
		}

		TRAP_ASSERT(false, "VkVertexInputRateTranslator(): Unknown RendererAPI::VertexAttributeRate!");
		return VK_VERTEX_INPUT_RATE_MAX_ENUM;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_DEBUG
 	/// @brief Utility to check VkResult for errors and log them.
	/// @param result Vulkan result to check.
	/// @param loc Source location where VkCall() got called from.
	constexpr void VkCall(const VkResult result, const std::source_location& loc = std::source_location::current())
	{
		::TRAP::Graphics::API::ErrorCheck(result, loc);
	}
#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
	constexpr void VkReflexCall(const NvLL_VK_Status status, const std::source_location& loc = std::source_location::current())
	{
		::TRAP::Graphics::API::ReflexErrorCheck(status, loc);
	}
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/
#else
	/// @brief Utility to check VkResult for errors and log them.
	constexpr void VkCall(VkResult)
	{}
#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
	constexpr void VkReflexCall(NvLL_VK_Status)
	{}
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/
#endif /*TRAP_DEBUG*/

//-------------------------------------------------------------------------------------------------------------------//

namespace TRAP::INTERNAL
{
	constexpr Utils::ConstexprMap<VkResult, std::string_view, 37> VkResultToString
	{
		{
			{
				{VK_ERROR_OUT_OF_HOST_MEMORY, "VK_ERROR_OUT_OF_HOST_MEMORY"},
				{VK_ERROR_OUT_OF_DEVICE_MEMORY, "VK_ERROR_OUT_OF_DEVICE_MEMORY"},
				{VK_ERROR_INITIALIZATION_FAILED, "VK_ERROR_INITIALIZATION_FAILED"},
				{VK_ERROR_DEVICE_LOST, "VK_ERROR_DEVICE_LOST"},
				{VK_ERROR_MEMORY_MAP_FAILED, "VK_ERROR_MEMORY_MAP_FAILED"},
				{VK_ERROR_LAYER_NOT_PRESENT, "VK_ERROR_LAYER_NOT_PRESENT"},
				{VK_ERROR_EXTENSION_NOT_PRESENT, "VK_ERROR_EXTENSION_NOT_PRESENT"},
				{VK_ERROR_FEATURE_NOT_PRESENT, "VK_ERROR_FEATURE_NOT_PRESENT"},
				{VK_ERROR_INCOMPATIBLE_DRIVER, "VK_ERROR_INCOMPATIBLE_DRIVER"},
				{VK_ERROR_TOO_MANY_OBJECTS, "VK_ERROR_TOO_MANY_OBJECTS"},
				{VK_ERROR_FORMAT_NOT_SUPPORTED, "VK_ERROR_FORMAT_NOT_SUPPORTED"},
				{VK_ERROR_FRAGMENTED_POOL, "VK_ERROR_FRAGMENTED_POOL"},
				{VK_ERROR_UNKNOWN, "VK_ERROR_UNKNOWN"},
				{VK_ERROR_OUT_OF_POOL_MEMORY, "VK_ERROR_OUT_OF_POOL_MEMORY"},
				{VK_ERROR_INVALID_EXTERNAL_HANDLE, "VK_ERROR_INVALID_EXTERNAL_HANDLE"},
				{VK_ERROR_FRAGMENTATION, "VK_ERROR_FRAGMENTATION"},
				{VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS, "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS"},
				{VK_PIPELINE_COMPILE_REQUIRED, "VK_PIPELINE_COMPILE_REQUIRED"},
				{VK_ERROR_SURFACE_LOST_KHR, "VK_ERROR_SURFACE_LOST_KHR"},
				{VK_ERROR_NATIVE_WINDOW_IN_USE_KHR, "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR"},
				{VK_SUBOPTIMAL_KHR, "VK_SUBOPTIMAL_KHR"},
				{VK_ERROR_OUT_OF_DATE_KHR, "VK_ERROR_OUT_OF_DATE_KHR"},
				{VK_ERROR_INCOMPATIBLE_DISPLAY_KHR, "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR"},
				{VK_ERROR_VALIDATION_FAILED_EXT, "VK_ERROR_VALIDATION_FAILED_EXT"},
				{VK_ERROR_INVALID_SHADER_NV, "VK_ERROR_INVALID_SHADER_NV"},
				{VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR, "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR"},
				{VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR, "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR"},
				{VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR, "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR"},
				{VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR, "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR"},
				{VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR, "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR"},
				{VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR, "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR"},
				{VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT, "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT"},
				{VK_ERROR_NOT_PERMITTED_KHR, "VK_ERROR_NOT_PERMITTED_KHR"},
				{VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT, "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT"},
				{VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR, "VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR"},
				{VK_ERROR_COMPRESSION_EXHAUSTED_EXT, "VK_ERROR_COMPRESSION_EXHAUSTED_EXT"},
				{VK_ERROR_INCOMPATIBLE_SHADER_BINARY_EXT, "VK_ERROR_INCOMPATIBLE_SHADER_BINARY_EXT"}
			}
		}
	};

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
	constexpr Utils::ConstexprMap<NvLL_VK_Status, std::string_view, 11> NvLLVKStatusToString
	{
		{
			{
				{NVLL_VK_ERROR, "NVLL_VK_ERROR"},
				{NVLL_VK_LIBRARY_NOT_FOUND, "NVLL_VK_LIBRARY_NOT_FOUND"},
				{NVLL_VK_NO_IMPLEMENTATION, "NVLL_VK_NO_IMPLEMENTATION"},
				{NVLL_VK_API_NOT_INITIALIZED, "NVLL_VK_API_NOT_INITIALIZED"},
				{NVLL_VK_INVALID_ARGUMENT, "NVLL_VK_INVALID_ARGUMENT"},
				{NVLL_VK_INVALID_HANDLE, "NVLL_VK_INVALID_HANDLE"},
				{NVLL_VK_INCOMPATIBLE_STRUCT_VERSION, "NVLL_VK_INCOMPATIBLE_STRUCT_VERSION"},
				{NVLL_VK_INVALID_POINTER, "NVLL_VK_INVALID_POINTER"},
				{NVLL_VK_OUT_OF_MEMORY, "NVLL_VK_OUT_OF_MEMORY"},
				{NVLL_VK_API_IN_USE, "NVLL_VK_API_IN_USE"},
				{NVLL_VK_NO_VULKAN, "NVLL_VK_NO_VULKAN"}
			}
		}
	};
#endif /*defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)*/
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool TRAP::Graphics::API::ErrorCheck(const VkResult result, const std::source_location& sourceLoc)
{
	if(result >= 0)
		return true;

	TP_ERROR(Log::RendererVulkanPrefix, INTERNAL::VkResultToString.at(result).value_or("Unknown error"), ": ",
	         sourceLoc);

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
constexpr bool TRAP::Graphics::API::ReflexErrorCheck(const NvLL_VK_Status result, const std::source_location& sourceLoc)
{
	if(result == NVLL_VK_OK)
		return true;

	TP_ERROR(Log::RendererVulkanReflexPrefix, INTERNAL::NvLLVKStatusToString.at(result).value_or("Unknown error"),
	         ": ", sourceLoc);

	return false;
}
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkQueueFlags TRAP::Graphics::API::QueueTypeToVkQueueFlags(const RendererAPI::QueueType queueType) noexcept
{
	switch(queueType)
	{
	case RendererAPI::QueueType::Graphics:
		return VK_QUEUE_GRAPHICS_BIT;

	case RendererAPI::QueueType::Transfer:
		return VK_QUEUE_TRANSFER_BIT;

	case RendererAPI::QueueType::Compute:
		return VK_QUEUE_COMPUTE_BIT;
	}

	TRAP_ASSERT(false, "QueueTypeToVkQueueFlags(): Unknown RendererAPI::QueueType!");
	return VK_QUEUE_FLAG_BITS_MAX_ENUM;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkSampleCountFlagBits TRAP::Graphics::API::SampleCountToVkSampleCount(const RendererAPI::SampleCount sampleCount) noexcept
{
	switch(sampleCount)
	{
	case RendererAPI::SampleCount::One:
		return VK_SAMPLE_COUNT_1_BIT;

	case RendererAPI::SampleCount::Two:
		return VK_SAMPLE_COUNT_2_BIT;

	case RendererAPI::SampleCount::Four:
		return VK_SAMPLE_COUNT_4_BIT;

	case RendererAPI::SampleCount::Eight:
		return VK_SAMPLE_COUNT_8_BIT;

	case RendererAPI::SampleCount::Sixteen:
		return VK_SAMPLE_COUNT_16_BIT;
	}

	TRAP_ASSERT(false, "SampleCountToVkSampleCount(): Unknown RendererAPI::SampleCount!");
	return VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkFormat TRAP::Graphics::API::ImageFormatToVkFormat(const TRAP::Graphics::API::ImageFormat imageFormat) noexcept
{
	switch(imageFormat)
	{
	case ImageFormat::Undefined: return VK_FORMAT_UNDEFINED;
	case ImageFormat::G4R4_UNORM: return VK_FORMAT_R4G4_UNORM_PACK8;
	case ImageFormat::A4B4G4R4_UNORM: return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
	case ImageFormat::A4R4G4B4_UNORM: return VK_FORMAT_B4G4R4A4_UNORM_PACK16;
	case ImageFormat::R5G6B5_UNORM: return VK_FORMAT_B5G6R5_UNORM_PACK16;
	case ImageFormat::B5G6R5_UNORM: return VK_FORMAT_R5G6B5_UNORM_PACK16;
	case ImageFormat::A1B5G5R5_UNORM: return VK_FORMAT_R5G5B5A1_UNORM_PACK16;
	case ImageFormat::A1R5G5B5_UNORM: return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
	case ImageFormat::B5G5R5A1_UNORM: return VK_FORMAT_A1R5G5B5_UNORM_PACK16;
	case ImageFormat::A2B10G10R10_UNORM: return VK_FORMAT_A2B10G10R10_UNORM_PACK32;

	case ImageFormat::R8_UNORM: return VK_FORMAT_R8_UNORM;
	case ImageFormat::R8_SNORM: return VK_FORMAT_R8_SNORM;
	case ImageFormat::R8_UINT: return VK_FORMAT_R8_UINT;
	case ImageFormat::R8_SINT: return VK_FORMAT_R8_SINT;
	case ImageFormat::R8_SRGB: return VK_FORMAT_R8_SRGB;
	case ImageFormat::R8G8_UNORM: return VK_FORMAT_R8G8_UNORM;
	case ImageFormat::R8G8_SNORM: return VK_FORMAT_R8G8_SNORM;
	case ImageFormat::R8G8_UINT: return VK_FORMAT_R8G8_UINT;
	case ImageFormat::R8G8_SINT: return VK_FORMAT_R8G8_SINT;
	case ImageFormat::R8G8_SRGB: return VK_FORMAT_R8G8_SRGB;
	case ImageFormat::R8G8B8_UNORM: return VK_FORMAT_R8G8B8_UNORM;
	case ImageFormat::R8G8B8_SNORM: return VK_FORMAT_R8G8B8_SNORM;
	case ImageFormat::R8G8B8_UINT: return VK_FORMAT_R8G8B8_UINT;
	case ImageFormat::R8G8B8_SINT: return VK_FORMAT_R8G8B8_SINT;
	case ImageFormat::R8G8B8_SRGB: return VK_FORMAT_R8G8B8_SRGB;
	case ImageFormat::B8G8R8_UNORM: return VK_FORMAT_B8G8R8_UNORM;
	case ImageFormat::B8G8R8_SNORM: return VK_FORMAT_B8G8R8_SNORM;
	case ImageFormat::B8G8R8_UINT: return VK_FORMAT_B8G8R8_UINT;
	case ImageFormat::B8G8R8_SINT: return VK_FORMAT_B8G8R8_SINT;
	case ImageFormat::B8G8R8_SRGB: return VK_FORMAT_B8G8R8_SRGB;
	case ImageFormat::R8G8B8A8_UNORM: return VK_FORMAT_R8G8B8A8_UNORM;
	case ImageFormat::R8G8B8A8_SNORM: return VK_FORMAT_R8G8B8A8_SNORM;
	case ImageFormat::R8G8B8A8_UINT: return VK_FORMAT_R8G8B8A8_UINT;
	case ImageFormat::R8G8B8A8_SINT: return VK_FORMAT_R8G8B8A8_SINT;
	case ImageFormat::R8G8B8A8_SRGB: return VK_FORMAT_R8G8B8A8_SRGB;
	case ImageFormat::B8G8R8A8_UNORM: return VK_FORMAT_B8G8R8A8_UNORM;
	case ImageFormat::B8G8R8A8_SNORM: return VK_FORMAT_B8G8R8A8_SNORM;
	case ImageFormat::B8G8R8A8_UINT: return VK_FORMAT_B8G8R8A8_UINT;
	case ImageFormat::B8G8R8A8_SINT: return VK_FORMAT_B8G8R8A8_SINT;
	case ImageFormat::B8G8R8A8_SRGB: return VK_FORMAT_B8G8R8A8_SRGB;
	case ImageFormat::R16_UNORM: return VK_FORMAT_R16_UNORM;
	case ImageFormat::R16_SNORM: return VK_FORMAT_R16_SNORM;
	case ImageFormat::R16_UINT: return VK_FORMAT_R16_UINT;
	case ImageFormat::R16_SINT: return VK_FORMAT_R16_SINT;
	case ImageFormat::R16_SFLOAT: return VK_FORMAT_R16_SFLOAT;
	case ImageFormat::R16G16_UNORM: return VK_FORMAT_R16G16_UNORM;
	case ImageFormat::R16G16_SNORM: return VK_FORMAT_R16G16_SNORM;
	case ImageFormat::R16G16_UINT: return VK_FORMAT_R16G16_UINT;
	case ImageFormat::R16G16_SINT: return VK_FORMAT_R16G16_SINT;
	case ImageFormat::R16G16_SFLOAT: return VK_FORMAT_R16G16_SFLOAT;
	case ImageFormat::R16G16B16_UNORM: return VK_FORMAT_R16G16B16_UNORM;
	case ImageFormat::R16G16B16_SNORM: return VK_FORMAT_R16G16B16_SNORM;
	case ImageFormat::R16G16B16_UINT: return VK_FORMAT_R16G16B16_UINT;
	case ImageFormat::R16G16B16_SINT: return VK_FORMAT_R16G16B16_SINT;
	case ImageFormat::R16G16B16_SFLOAT: return VK_FORMAT_R16G16B16_SFLOAT;
	case ImageFormat::R16G16B16A16_UNORM: return VK_FORMAT_R16G16B16A16_UNORM;
	case ImageFormat::R16G16B16A16_SNORM: return VK_FORMAT_R16G16B16A16_SNORM;
	case ImageFormat::R16G16B16A16_UINT: return VK_FORMAT_R16G16B16A16_UINT;
	case ImageFormat::R16G16B16A16_SINT: return VK_FORMAT_R16G16B16A16_SINT;
	case ImageFormat::R16G16B16A16_SFLOAT: return VK_FORMAT_R16G16B16A16_SFLOAT;
	case ImageFormat::R32_UINT: return VK_FORMAT_R32_UINT;
	case ImageFormat::R32_SINT: return VK_FORMAT_R32_SINT;
	case ImageFormat::R32_SFLOAT: return VK_FORMAT_R32_SFLOAT;
	case ImageFormat::R32G32_UINT: return VK_FORMAT_R32G32_UINT;
	case ImageFormat::R32G32_SINT: return VK_FORMAT_R32G32_SINT;
	case ImageFormat::R32G32_SFLOAT: return VK_FORMAT_R32G32_SFLOAT;
	case ImageFormat::R32G32B32_UINT: return VK_FORMAT_R32G32B32_UINT;
	case ImageFormat::R32G32B32_SINT: return VK_FORMAT_R32G32B32_SINT;
	case ImageFormat::R32G32B32_SFLOAT: return VK_FORMAT_R32G32B32_SFLOAT;
	case ImageFormat::R32G32B32A32_UINT: return VK_FORMAT_R32G32B32A32_UINT;
	case ImageFormat::R32G32B32A32_SINT: return VK_FORMAT_R32G32B32A32_SINT;
	case ImageFormat::R32G32B32A32_SFLOAT: return VK_FORMAT_R32G32B32A32_SFLOAT;
	case ImageFormat::R64_UINT: return VK_FORMAT_R64_UINT;
	case ImageFormat::R64_SINT: return VK_FORMAT_R64_SINT;
	case ImageFormat::R64_SFLOAT: return VK_FORMAT_R64_SFLOAT;
	case ImageFormat::R64G64_UINT: return VK_FORMAT_R64G64_UINT;
	case ImageFormat::R64G64_SINT: return VK_FORMAT_R64G64_SINT;
	case ImageFormat::R64G64_SFLOAT: return VK_FORMAT_R64G64_SFLOAT;
	case ImageFormat::R64G64B64_UINT: return VK_FORMAT_R64G64B64_UINT;
	case ImageFormat::R64G64B64_SINT: return VK_FORMAT_R64G64B64_SINT;
	case ImageFormat::R64G64B64_SFLOAT: return VK_FORMAT_R64G64B64_SFLOAT;
	case ImageFormat::R64G64B64A64_UINT: return VK_FORMAT_R64G64B64A64_UINT;
	case ImageFormat::R64G64B64A64_SINT: return VK_FORMAT_R64G64B64A64_SINT;
	case ImageFormat::R64G64B64A64_SFLOAT: return VK_FORMAT_R64G64B64A64_SFLOAT;

	case ImageFormat::B10G10R10A2_UNORM: return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
	case ImageFormat::B10G10R10A2_UINT: return VK_FORMAT_A2R10G10B10_UINT_PACK32;
	case ImageFormat::R10G10B10A2_UNORM: return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
	case ImageFormat::R10G10B10A2_UINT: return VK_FORMAT_A2B10G10R10_UINT_PACK32;

	case ImageFormat::B10G11R11_UFLOAT: return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
	case ImageFormat::E5B9G9R9_UFLOAT: return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;

	case ImageFormat::G16B16G16R16_422_UNORM: return VK_FORMAT_G16B16G16R16_422_UNORM;
	case ImageFormat::B16G16R16G16_422_UNORM: return VK_FORMAT_B16G16R16G16_422_UNORM;
	case ImageFormat::R12X4G12X4B12X4A12X4_UNORM_4PACK16: return VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16;
	case ImageFormat::G12X4B12X4G12X4R12X4_422_UNORM_4PACK16: return VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16;
	case ImageFormat::B12X4G12X4R12X4G12X4_422_UNORM_4PACK16: return VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16;
	case ImageFormat::R10X6G10X6B10X6A10X6_UNORM_4PACK16: return VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16;
	case ImageFormat::G10X6B10X6G10X6R10X6_422_UNORM_4PACK16: return VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16;
	case ImageFormat::B10X6G10X6R10X6G10X6_422_UNORM_4PACK16: return VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16;
	case ImageFormat::G8B8G8R8_422_UNORM: return VK_FORMAT_G8B8G8R8_422_UNORM;
	case ImageFormat::B8G8R8G8_422_UNORM: return VK_FORMAT_B8G8R8G8_422_UNORM;
	case ImageFormat::G8_B8_R8_3PLANE_420_UNORM: return VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM;
	case ImageFormat::G8_B8R8_2PLANE_420_UNORM: return VK_FORMAT_G8_B8R8_2PLANE_420_UNORM;
	case ImageFormat::G8_B8_R8_3PLANE_422_UNORM: return VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM;
	case ImageFormat::G8_B8R8_2PLANE_422_UNORM: return VK_FORMAT_G8_B8R8_2PLANE_422_UNORM;
	case ImageFormat::G8_B8_R8_3PLANE_444_UNORM: return VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM;
	case ImageFormat::G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16: return VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16;
	case ImageFormat::G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16: return VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16;
	case ImageFormat::G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16: return VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16;
	case ImageFormat::G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16: return VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16;
	case ImageFormat::G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16: return VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16;
	case ImageFormat::G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16: return VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16;
	case ImageFormat::G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16: return VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16;
	case ImageFormat::G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16: return VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16;
	case ImageFormat::G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16: return VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16;
	case ImageFormat::G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16: return VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16;
	case ImageFormat::G16_B16_R16_3PLANE_420_UNORM: return VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM;
	case ImageFormat::G16_B16_R16_3PLANE_422_UNORM: return VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM;
	case ImageFormat::G16_B16_R16_3PLANE_444_UNORM: return VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM;
	case ImageFormat::G16_B16R16_2PLANE_420_UNORM: return VK_FORMAT_G16_B16R16_2PLANE_420_UNORM;
	case ImageFormat::G16_B16R16_2PLANE_422_UNORM: return VK_FORMAT_G16_B16R16_2PLANE_422_UNORM;

	case ImageFormat::D16_UNORM: return VK_FORMAT_D16_UNORM;
	case ImageFormat::X8_D24_UNORM: return VK_FORMAT_X8_D24_UNORM_PACK32;
	case ImageFormat::D32_SFLOAT: return VK_FORMAT_D32_SFLOAT;
	case ImageFormat::S8_UINT: return VK_FORMAT_S8_UINT;
	case ImageFormat::D16_UNORM_S8_UINT: return VK_FORMAT_D16_UNORM_S8_UINT;
	case ImageFormat::D24_UNORM_S8_UINT: return VK_FORMAT_D24_UNORM_S8_UINT;
	case ImageFormat::D32_SFLOAT_S8_UINT: return VK_FORMAT_D32_SFLOAT_S8_UINT;
	case ImageFormat::DXBC1_RGB_UNORM: return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
	case ImageFormat::DXBC1_RGB_SRGB: return VK_FORMAT_BC1_RGB_SRGB_BLOCK;
	case ImageFormat::DXBC1_RGBA_UNORM: return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
	case ImageFormat::DXBC1_RGBA_SRGB: return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
	case ImageFormat::DXBC2_UNORM: return VK_FORMAT_BC2_UNORM_BLOCK;
	case ImageFormat::DXBC2_SRGB: return VK_FORMAT_BC2_SRGB_BLOCK;
	case ImageFormat::DXBC3_UNORM: return VK_FORMAT_BC3_UNORM_BLOCK;
	case ImageFormat::DXBC3_SRGB: return VK_FORMAT_BC3_SRGB_BLOCK;
	case ImageFormat::DXBC4_UNORM: return VK_FORMAT_BC4_UNORM_BLOCK;
	case ImageFormat::DXBC4_SNORM: return VK_FORMAT_BC4_SNORM_BLOCK;
	case ImageFormat::DXBC5_UNORM: return VK_FORMAT_BC5_UNORM_BLOCK;
	case ImageFormat::DXBC5_SNORM: return VK_FORMAT_BC5_SNORM_BLOCK;
	case ImageFormat::DXBC6H_UFLOAT: return VK_FORMAT_BC6H_UFLOAT_BLOCK;
	case ImageFormat::DXBC6H_SFLOAT: return VK_FORMAT_BC6H_SFLOAT_BLOCK;
	case ImageFormat::DXBC7_UNORM: return VK_FORMAT_BC7_UNORM_BLOCK;
	case ImageFormat::DXBC7_SRGB: return VK_FORMAT_BC7_SRGB_BLOCK;
	// case ImageFormat::PVRTC1_2BPP_UNORM: return VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG;
	// case ImageFormat::PVRTC1_4BPP_UNORM: return VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG;
	// case ImageFormat::PVRTC1_2BPP_SRGB: return VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG;
	// case ImageFormat::PVRTC1_4BPP_SRGB: return VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG;
	case ImageFormat::ETC2_R8G8B8_UNORM: return VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
	case ImageFormat::ETC2_R8G8B8A1_UNORM: return VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK;
	case ImageFormat::ETC2_R8G8B8A8_UNORM: return VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
	case ImageFormat::ETC2_R8G8B8_SRGB: return VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK;
	case ImageFormat::ETC2_R8G8B8A1_SRGB: return VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK;
	case ImageFormat::ETC2_R8G8B8A8_SRGB: return VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK;
	case ImageFormat::ETC2_EAC_R11_UNORM: return VK_FORMAT_EAC_R11_UNORM_BLOCK;
	case ImageFormat::ETC2_EAC_R11G11_UNORM: return VK_FORMAT_EAC_R11G11_UNORM_BLOCK;
	case ImageFormat::ETC2_EAC_R11_SNORM: return VK_FORMAT_EAC_R11_SNORM_BLOCK;
	case ImageFormat::ETC2_EAC_R11G11_SNORM: return VK_FORMAT_EAC_R11G11_SNORM_BLOCK;
	case ImageFormat::ASTC_4x4_UNORM: return VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
	case ImageFormat::ASTC_4x4_SRGB: return VK_FORMAT_ASTC_4x4_SRGB_BLOCK;
	case ImageFormat::ASTC_5x4_UNORM: return VK_FORMAT_ASTC_5x4_UNORM_BLOCK;
	case ImageFormat::ASTC_5x4_SRGB: return VK_FORMAT_ASTC_5x4_SRGB_BLOCK;
	case ImageFormat::ASTC_5x5_UNORM: return VK_FORMAT_ASTC_5x5_UNORM_BLOCK;
	case ImageFormat::ASTC_5x5_SRGB: return VK_FORMAT_ASTC_5x5_SRGB_BLOCK;
	case ImageFormat::ASTC_6x5_UNORM: return VK_FORMAT_ASTC_6x5_UNORM_BLOCK;
	case ImageFormat::ASTC_6x5_SRGB: return VK_FORMAT_ASTC_6x5_SRGB_BLOCK;
	case ImageFormat::ASTC_6x6_UNORM: return VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
	case ImageFormat::ASTC_6x6_SRGB: return VK_FORMAT_ASTC_6x6_SRGB_BLOCK;
	case ImageFormat::ASTC_8x5_UNORM: return VK_FORMAT_ASTC_8x5_UNORM_BLOCK;
	case ImageFormat::ASTC_8x5_SRGB: return VK_FORMAT_ASTC_8x5_SRGB_BLOCK;
	case ImageFormat::ASTC_8x6_UNORM: return VK_FORMAT_ASTC_8x6_UNORM_BLOCK;
	case ImageFormat::ASTC_8x6_SRGB: return VK_FORMAT_ASTC_8x6_SRGB_BLOCK;
	case ImageFormat::ASTC_8x8_UNORM: return VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
	case ImageFormat::ASTC_8x8_SRGB: return VK_FORMAT_ASTC_8x8_SRGB_BLOCK;
	case ImageFormat::ASTC_10x5_UNORM: return VK_FORMAT_ASTC_10x5_UNORM_BLOCK;
	case ImageFormat::ASTC_10x5_SRGB: return VK_FORMAT_ASTC_10x5_SRGB_BLOCK;
	case ImageFormat::ASTC_10x6_UNORM: return VK_FORMAT_ASTC_10x6_UNORM_BLOCK;
	case ImageFormat::ASTC_10x6_SRGB: return VK_FORMAT_ASTC_10x6_SRGB_BLOCK;
	case ImageFormat::ASTC_10x8_UNORM: return VK_FORMAT_ASTC_10x8_UNORM_BLOCK;
	case ImageFormat::ASTC_10x8_SRGB: return VK_FORMAT_ASTC_10x8_SRGB_BLOCK;
	case ImageFormat::ASTC_10x10_UNORM: return VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
	case ImageFormat::ASTC_10x10_SRGB: return VK_FORMAT_ASTC_10x10_SRGB_BLOCK;
	case ImageFormat::ASTC_12x10_UNORM: return VK_FORMAT_ASTC_12x10_UNORM_BLOCK;
	case ImageFormat::ASTC_12x10_SRGB: return VK_FORMAT_ASTC_12x10_SRGB_BLOCK;
	case ImageFormat::ASTC_12x12_UNORM: return VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
	case ImageFormat::ASTC_12x12_SRGB: return VK_FORMAT_ASTC_12x12_SRGB_BLOCK;
	// case ImageFormat::PVRTC2_2BPP_UNORM: return VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG;
	// case ImageFormat::PVRTC2_4BPP_UNORM: return VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG;
	// case ImageFormat::PVRTC2_2BPP_SRGB: return VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG;
	// case ImageFormat::PVRTC2_4BPP_SRGB: return VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG;

	default: return VK_FORMAT_UNDEFINED;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkImageAspectFlags TRAP::Graphics::API::DetermineAspectMask(const VkFormat format, const bool includeStencilBit) noexcept
{
	switch(format)
	{
		//Depth
	case VK_FORMAT_D16_UNORM:
		[[fallthrough]];
	case VK_FORMAT_X8_D24_UNORM_PACK32:
		[[fallthrough]];
	case VK_FORMAT_D32_SFLOAT:
		return VK_IMAGE_ASPECT_DEPTH_BIT;

		//Stencil
	case VK_FORMAT_S8_UINT:
		return VK_IMAGE_ASPECT_STENCIL_BIT;

		//Depth/Stencil
	case VK_FORMAT_D16_UNORM_S8_UINT:
		[[fallthrough]];
	case VK_FORMAT_D24_UNORM_S8_UINT:
		[[fallthrough]];
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

[[nodiscard]] constexpr VkImageUsageFlags TRAP::Graphics::API::DescriptorTypeToVkImageUsage(const RendererAPI::DescriptorType type) noexcept
{
	VkImageUsageFlags result = 0;

	if ((type & RendererAPI::DescriptorType::Texture) != RendererAPI::DescriptorType::Undefined)
		result |= VK_IMAGE_USAGE_SAMPLED_BIT;
	if ((type & RendererAPI::DescriptorType::RWTexture) != RendererAPI::DescriptorType::Undefined)
		result |= VK_IMAGE_USAGE_STORAGE_BIT;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkFormatFeatureFlags TRAP::Graphics::API::VkImageUsageToFormatFeatures(const VkImageUsageFlags usage) noexcept
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

[[nodiscard]] constexpr VkBufferUsageFlags TRAP::Graphics::API::DescriptorTypeToVkBufferUsage(const RendererAPI::DescriptorType usage,
                                                                                              const bool typed) noexcept
{
	VkBufferUsageFlags result = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	if ((usage & RendererAPI::DescriptorType::UniformBuffer) != RendererAPI::DescriptorType::Undefined)
		result |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	if ((usage & RendererAPI::DescriptorType::RWBuffer) != RendererAPI::DescriptorType::Undefined)
	{
		result |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		if (typed)
			result |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
	}
	if ((usage & RendererAPI::DescriptorType::Buffer) != RendererAPI::DescriptorType::Undefined)
	{
		result |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		if (typed)
			result |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
	}
	if ((usage & RendererAPI::DescriptorType::IndexBuffer) != RendererAPI::DescriptorType::Undefined)
		result |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	if ((usage & RendererAPI::DescriptorType::VertexBuffer) != RendererAPI::DescriptorType::Undefined)
		result |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	if ((usage & RendererAPI::DescriptorType::IndirectBuffer) != RendererAPI::DescriptorType::Undefined)
		result |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;

	//RayTracing
	if ((usage & RendererAPI::DescriptorType::AccelerationStructure) != RendererAPI::DescriptorType::Undefined)
		result |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;
	if ((usage & RendererAPI::DescriptorType::AccelerationStructureBuildInput) != RendererAPI::DescriptorType::Undefined)
		result |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
	if ((usage & RendererAPI::DescriptorType::ShaderDeviceAddress) != RendererAPI::DescriptorType::Undefined)
		result |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR;
	if ((usage & RendererAPI::DescriptorType::ShaderBindingTable) != RendererAPI::DescriptorType::Undefined)
		result |= VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkFilter TRAP::Graphics::API::FilterTypeToVkFilter(const RendererAPI::FilterType filter) noexcept
{
	switch(filter)
	{
	case RendererAPI::FilterType::Nearest:
		return VK_FILTER_NEAREST;

	case RendererAPI::FilterType::Linear:
		return VK_FILTER_LINEAR;
	}

	TRAP_ASSERT(false, "FilterTypeToVkFilter(): Unknown RendererAPI::FilterType!");
	return VK_FILTER_MAX_ENUM;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkSamplerMipmapMode TRAP::Graphics::API::MipMapModeToVkMipMapMode(const RendererAPI::MipMapMode mipMapMode) noexcept
{
	switch (mipMapMode)
	{
	case RendererAPI::MipMapMode::Nearest:
		return VK_SAMPLER_MIPMAP_MODE_NEAREST;

	case RendererAPI::MipMapMode::Linear:
		return VK_SAMPLER_MIPMAP_MODE_LINEAR;
	}

	TRAP_ASSERT(false, "MipMapModeToVkMipMapMode(): Unknown RendererAPI::MipMapMode!");
	return VK_SAMPLER_MIPMAP_MODE_MAX_ENUM;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkSamplerAddressMode TRAP::Graphics::API::AddressModeToVkAddressMode(const RendererAPI::AddressMode addressMode) noexcept
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
	}

	TRAP_ASSERT(false, "AddressModeToVkAddressMode(): Unknown RendererAPI::AddressMode!");
	return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkDescriptorType TRAP::Graphics::API::DescriptorTypeToVkDescriptorType(const RendererAPI::DescriptorType type) noexcept
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
		[[fallthrough]];
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

[[nodiscard]] constexpr VkShaderStageFlags TRAP::Graphics::API::ShaderStageToVkShaderStageFlags(const RendererAPI::ShaderStage stages) noexcept
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

[[nodiscard]] constexpr VkShaderStageFlagBits TRAP::Graphics::API::ShaderStageToVkShaderStageFlagBits(const RendererAPI::ShaderStage stage) noexcept
{
	VkShaderStageFlagBits res{};

	//Graphics
	if (stage == RendererAPI::ShaderStage::Vertex)
		return VK_SHADER_STAGE_VERTEX_BIT;
	if (stage == RendererAPI::ShaderStage::Geometry)
		return VK_SHADER_STAGE_GEOMETRY_BIT;
	if (stage == RendererAPI::ShaderStage::TessellationEvaluation)
		return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	if (stage == RendererAPI::ShaderStage::TessellationControl)
		return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	if (stage == RendererAPI::ShaderStage::Fragment)
		return VK_SHADER_STAGE_FRAGMENT_BIT;

	//Compute
	if (stage == RendererAPI::ShaderStage::Compute)
		return VK_SHADER_STAGE_COMPUTE_BIT;

	//RayTracing
	// if (stage == RendererAPI::ShaderStage::RayTracing)
	// {
	// 	return (VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR |
	// 		    VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR |
	// 		    VK_SHADER_STAGE_INTERSECTION_BIT_KHR | VK_SHADER_STAGE_CALLABLE_BIT_KHR);
	// }

	TRAP_ASSERT(false, "ShaderStageToVkShaderStageFlagBits(): Invalid ShaderStage provided!");
	return res;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkPipelineCacheCreateFlags TRAP::Graphics::API::PipelineCacheFlagsToVkPipelineCacheCreateFlags(const RendererAPI::PipelineCacheFlags flags) noexcept
{
	VkPipelineCacheCreateFlags ret = 0;

	if ((flags & RendererAPI::PipelineCacheFlags::ExternallySynchronized) != RendererAPI::PipelineCacheFlags::None)
		ret |= VK_PIPELINE_CACHE_CREATE_EXTERNALLY_SYNCHRONIZED_BIT_EXT;

	return ret;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkAccessFlags TRAP::Graphics::API::ResourceStateToVkAccessFlags(const RendererAPI::ResourceState state) noexcept
{
	VkAccessFlags ret = 0;

	if ((state & RendererAPI::ResourceState::CopySource) != RendererAPI::ResourceState::Undefined)
		ret |= VK_ACCESS_TRANSFER_READ_BIT;
	if ((state & RendererAPI::ResourceState::CopyDestination) != RendererAPI::ResourceState::Undefined)
		ret |= VK_ACCESS_TRANSFER_WRITE_BIT;
	if ((state & RendererAPI::ResourceState::VertexAndConstantBuffer) != RendererAPI::ResourceState::Undefined)
		ret |= VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
	if ((state & RendererAPI::ResourceState::IndexBuffer) != RendererAPI::ResourceState::Undefined)
		ret |= VK_ACCESS_INDEX_READ_BIT;
	if ((state & RendererAPI::ResourceState::UnorderedAccess) != RendererAPI::ResourceState::Undefined)
		ret |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
	if ((state & RendererAPI::ResourceState::IndirectArgument) != RendererAPI::ResourceState::Undefined)
		ret |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
	if ((state & RendererAPI::ResourceState::RenderTarget) != RendererAPI::ResourceState::Undefined)
		ret |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	if ((state & RendererAPI::ResourceState::DepthWrite) != RendererAPI::ResourceState::Undefined)
		ret |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	if ((state & RendererAPI::ResourceState::ShaderResource) != RendererAPI::ResourceState::Undefined)
		ret |= VK_ACCESS_SHADER_READ_BIT;

	//RayTracing
	if ((state & RendererAPI::ResourceState::RayTracingAccelerationStructure) != RendererAPI::ResourceState::Undefined)
		ret |= VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR | VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;

	return ret;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkImageLayout TRAP::Graphics::API::ResourceStateToVkImageLayout(const RendererAPI::ResourceState usage) noexcept
{
	if ((usage & RendererAPI::ResourceState::CopySource) != RendererAPI::ResourceState::Undefined)
		return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

	if ((usage & RendererAPI::ResourceState::CopyDestination) != RendererAPI::ResourceState::Undefined)
		return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

	if ((usage & RendererAPI::ResourceState::RenderTarget) != RendererAPI::ResourceState::Undefined)
		return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	if ((usage & RendererAPI::ResourceState::DepthWrite) != RendererAPI::ResourceState::Undefined)
		return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	if ((usage & RendererAPI::ResourceState::UnorderedAccess) != RendererAPI::ResourceState::Undefined)
		return VK_IMAGE_LAYOUT_GENERAL;

	if ((usage & RendererAPI::ResourceState::ShaderResource) != RendererAPI::ResourceState::Undefined)
		return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	if ((usage & RendererAPI::ResourceState::Present) != RendererAPI::ResourceState::Undefined)
		return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	if ((usage & RendererAPI::ResourceState::Common) != RendererAPI::ResourceState::Undefined)
		return VK_IMAGE_LAYOUT_GENERAL;

	if ((usage & RendererAPI::ResourceState::ShadingRateSource) != RendererAPI::ResourceState::Undefined)
		return VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;

	return VK_IMAGE_LAYOUT_UNDEFINED;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkQueryType TRAP::Graphics::API::QueryTypeToVkQueryType(const RendererAPI::QueryType type) noexcept
{
	switch(type)
	{
	case RendererAPI::QueryType::Timestamp:
		return VK_QUERY_TYPE_TIMESTAMP;

	case RendererAPI::QueryType::PipelineStatistics:
		return VK_QUERY_TYPE_PIPELINE_STATISTICS;

	case RendererAPI::QueryType::Occlusion:
		return VK_QUERY_TYPE_OCCLUSION;
	}

	TRAP_ASSERT(false, "QueryTypeToVkQueryType(): Unknown RendererAPI::QueryType!");
	return VK_QUERY_TYPE_MAX_ENUM;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkPrimitiveTopology TRAP::Graphics::API::PrimitiveTopologyToVkPrimitiveTopology(const TRAP::Graphics::RendererAPI::PrimitiveTopology primitiveToplogy) noexcept
{
	using enum RendererAPI::PrimitiveTopology;

	switch(primitiveToplogy)
	{
	case PointList:
		return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
	case LineList:
		return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	case LineStrip:
		return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
	case TriangleList:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	case TriangleStrip:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
	case PatchList:
		return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
	}

	TRAP_ASSERT(false, "PrimitiveTopologyToVkPrimitiveTopology(): Unknown RendererAPI::PrimitiveTopology!");
	return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkPipelineColorBlendStateCreateInfo TRAP::Graphics::API::UtilToBlendDesc(const RendererAPI::BlendStateDesc& desc,
	                                                                                             std::vector<VkPipelineColorBlendAttachmentState>& attachments)
{
	u32 blendDescIndex = 0;

	attachments.resize(8);
	for(u32 i = 0; i < 8; ++i)
	{
		const VkBool32 blendEnable = VkBool32
		(
			VkBlendConstantTranslator(desc.SrcFactors[blendDescIndex]) != VK_BLEND_FACTOR_ONE ||
			VkBlendConstantTranslator(desc.DstFactors[blendDescIndex]) != VK_BLEND_FACTOR_ZERO ||
			VkBlendConstantTranslator(desc.SrcAlphaFactors[blendDescIndex]) != VK_BLEND_FACTOR_ONE ||
			VkBlendConstantTranslator(desc.DstAlphaFactors[blendDescIndex]) != VK_BLEND_FACTOR_ZERO
		);

		attachments[i].blendEnable = static_cast<VkBool32>((blendEnable != VK_FALSE) && ((std::to_underlying(desc.RenderTargetMask) & BIT(i)) != 0u));
		attachments[i].colorWriteMask = static_cast<VkColorComponentFlags>(desc.Masks[blendDescIndex]);
		attachments[i].srcColorBlendFactor = VkBlendConstantTranslator(desc.SrcFactors[blendDescIndex]);
		attachments[i].dstColorBlendFactor = VkBlendConstantTranslator(desc.DstFactors[blendDescIndex]);
		attachments[i].colorBlendOp = VkBlendOpTranslator(desc.BlendModes[blendDescIndex]);
		attachments[i].srcAlphaBlendFactor = VkBlendConstantTranslator(desc.SrcAlphaFactors[blendDescIndex]);
		attachments[i].dstAlphaBlendFactor = VkBlendConstantTranslator(desc.DstAlphaFactors[blendDescIndex]);
		attachments[i].alphaBlendOp = VkBlendOpTranslator(desc.BlendAlphaModes[blendDescIndex]);

		if (desc.IndependentBlend)
			++blendDescIndex;
	}

	return VulkanInits::PipelineColorBlendStateCreateInfo(attachments);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkPipelineDepthStencilStateCreateInfo TRAP::Graphics::API::UtilToDepthDesc(const RendererAPI::DepthStateDesc& desc)
{
	const VkStencilOpState frontStencilState
	{
		.failOp = VkStencilOpTranslator(desc.StencilFrontFail),
		.passOp = VkStencilOpTranslator(desc.StencilFrontPass),
		.depthFailOp = VkStencilOpTranslator(desc.DepthFrontFail),
		.compareOp = VkComparisonFuncTranslator(desc.StencilFrontFunc),
		.compareMask = desc.StencilReadMask,
		.writeMask = desc.StencilWriteMask,
		.reference = 0
	};

	const VkStencilOpState backStencilState
	{
		.failOp = VkStencilOpTranslator(desc.StencilBackFail),
		.passOp = VkStencilOpTranslator(desc.StencilBackPass),
		.depthFailOp = VkStencilOpTranslator(desc.DepthBackFail),
		.compareOp = VkComparisonFuncTranslator(desc.StencilBackFunc),
		.compareMask = desc.StencilReadMask,
		.writeMask = desc.StencilWriteMask,
		.reference = 0
	};

	return VkPipelineDepthStencilStateCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.depthTestEnable = NumericCast<VkBool32>(desc.DepthTest),
		.depthWriteEnable = NumericCast<VkBool32>(desc.DepthWrite),
		.depthCompareOp = VkComparisonFuncTranslator(desc.DepthFunc),
		.depthBoundsTestEnable = VK_FALSE,
		.stencilTestEnable = NumericCast<VkBool32>(desc.StencilTest),
		.front = frontStencilState,
		.back = backStencilState,
		.minDepthBounds = 0.0f,
		.maxDepthBounds = 1.0f
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkPipelineRasterizationStateCreateInfo TRAP::Graphics::API::UtilToRasterizerDesc(const RendererAPI::RasterizerStateDesc& desc)
{
	return VkPipelineRasterizationStateCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.depthClampEnable = desc.DepthClampEnable ? VK_TRUE : VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VkFillModeTranslator(desc.FillMode),
		.cullMode = VkCullModeTranslator(desc.CullMode),
		.frontFace = VkFrontFaceTranslator(desc.FrontFace),
		.depthBiasEnable = (desc.DepthBias != 0) ? VK_TRUE : VK_FALSE,
		.depthBiasConstantFactor = NumericCast<f32>(desc.DepthBias),
		.depthBiasClamp = 0.0f,
		.depthBiasSlopeFactor = desc.SlopeScaledDepthBias,
		.lineWidth = 1.0f
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::API::ImageFormat TRAP::Graphics::API::ImageFormatFromVkFormat(const VkFormat format) noexcept
{
	switch(format)
	{
	case VK_FORMAT_UNDEFINED: return ImageFormat::Undefined;
	case VK_FORMAT_R4G4_UNORM_PACK8: return ImageFormat::G4R4_UNORM;
	case VK_FORMAT_R4G4B4A4_UNORM_PACK16: return ImageFormat::A4B4G4R4_UNORM;
	case VK_FORMAT_B4G4R4A4_UNORM_PACK16: return ImageFormat::A4R4G4B4_UNORM;
	case VK_FORMAT_R5G6B5_UNORM_PACK16: return ImageFormat::B5G6R5_UNORM;
	case VK_FORMAT_B5G6R5_UNORM_PACK16: return ImageFormat::R5G6B5_UNORM;
	case VK_FORMAT_R5G5B5A1_UNORM_PACK16: return ImageFormat::A1B5G5R5_UNORM;
	case VK_FORMAT_B5G5R5A1_UNORM_PACK16: return ImageFormat::A1R5G5B5_UNORM;
	case VK_FORMAT_A1R5G5B5_UNORM_PACK16: return ImageFormat::B5G5R5A1_UNORM;
	case VK_FORMAT_R8_UNORM: return ImageFormat::R8_UNORM;
	case VK_FORMAT_R8_SNORM: return ImageFormat::R8_SNORM;
	case VK_FORMAT_R8_UINT: return ImageFormat::R8_UINT;
	case VK_FORMAT_R8_SINT: return ImageFormat::R8_SINT;
	case VK_FORMAT_R8_SRGB: return ImageFormat::R8_SRGB;
	case VK_FORMAT_R8G8_UNORM: return ImageFormat::R8G8_UNORM;
	case VK_FORMAT_R8G8_SNORM: return ImageFormat::R8G8_SNORM;
	case VK_FORMAT_R8G8_UINT: return ImageFormat::R8G8_UINT;
	case VK_FORMAT_R8G8_SINT: return ImageFormat::R8G8_SINT;
	case VK_FORMAT_R8G8_SRGB: return ImageFormat::R8G8_SRGB;
	case VK_FORMAT_R8G8B8_UNORM: return ImageFormat::R8G8B8_UNORM;
	case VK_FORMAT_R8G8B8_SNORM: return ImageFormat::R8G8B8_SNORM;
	case VK_FORMAT_R8G8B8_UINT: return ImageFormat::R8G8B8_UINT;
	case VK_FORMAT_R8G8B8_SINT: return ImageFormat::R8G8B8_SINT;
	case VK_FORMAT_R8G8B8_SRGB: return ImageFormat::R8G8B8_SRGB;
	case VK_FORMAT_B8G8R8_UNORM: return ImageFormat::B8G8R8_UNORM;
	case VK_FORMAT_B8G8R8_SNORM: return ImageFormat::B8G8R8_SNORM;
	case VK_FORMAT_B8G8R8_UINT: return ImageFormat::B8G8R8_UINT;
	case VK_FORMAT_B8G8R8_SINT: return ImageFormat::B8G8R8_SINT;
	case VK_FORMAT_B8G8R8_SRGB: return ImageFormat::B8G8R8_SRGB;
	case VK_FORMAT_R8G8B8A8_UNORM: return ImageFormat::R8G8B8A8_UNORM;
	case VK_FORMAT_R8G8B8A8_SNORM: return ImageFormat::R8G8B8A8_SNORM;
	case VK_FORMAT_R8G8B8A8_UINT: return ImageFormat::R8G8B8A8_UINT;
	case VK_FORMAT_R8G8B8A8_SINT: return ImageFormat::R8G8B8A8_SINT;
	case VK_FORMAT_R8G8B8A8_SRGB: return ImageFormat::R8G8B8A8_SRGB;
	case VK_FORMAT_B8G8R8A8_UNORM: return ImageFormat::B8G8R8A8_UNORM;
	case VK_FORMAT_B8G8R8A8_SNORM: return ImageFormat::B8G8R8A8_SNORM;
	case VK_FORMAT_B8G8R8A8_UINT: return ImageFormat::B8G8R8A8_UINT;
	case VK_FORMAT_B8G8R8A8_SINT: return ImageFormat::B8G8R8A8_SINT;
	case VK_FORMAT_B8G8R8A8_SRGB: return ImageFormat::B8G8R8A8_SRGB;
	case VK_FORMAT_A2R10G10B10_UNORM_PACK32: return ImageFormat::A2R10G10B10_UNORM;
	case VK_FORMAT_A2R10G10B10_UINT_PACK32: return ImageFormat::A2R10G10B10_UINT;
	case VK_FORMAT_A2B10G10R10_UNORM_PACK32: return ImageFormat::A2B10G10R10_UNORM;
	case VK_FORMAT_A2B10G10R10_UINT_PACK32: return ImageFormat::A2B10G10R10_UINT;
	case VK_FORMAT_R16_UNORM: return ImageFormat::R16_UNORM;
	case VK_FORMAT_R16_SNORM: return ImageFormat::R16_SNORM;
	case VK_FORMAT_R16_UINT: return ImageFormat::R16_UINT;
	case VK_FORMAT_R16_SINT: return ImageFormat::R16_SINT;
	case VK_FORMAT_R16_SFLOAT: return ImageFormat::R16_SFLOAT;
	case VK_FORMAT_R16G16_UNORM: return ImageFormat::R16G16_UNORM;
	case VK_FORMAT_R16G16_SNORM: return ImageFormat::R16G16_SNORM;
	case VK_FORMAT_R16G16_UINT: return ImageFormat::R16G16_UINT;
	case VK_FORMAT_R16G16_SINT: return ImageFormat::R16G16_SINT;
	case VK_FORMAT_R16G16_SFLOAT: return ImageFormat::R16G16_SFLOAT;
	case VK_FORMAT_R16G16B16_UNORM: return ImageFormat::R16G16B16_UNORM;
	case VK_FORMAT_R16G16B16_SNORM: return ImageFormat::R16G16B16_SNORM;
	case VK_FORMAT_R16G16B16_UINT: return ImageFormat::R16G16B16_UINT;
	case VK_FORMAT_R16G16B16_SINT: return ImageFormat::R16G16B16_SINT;
	case VK_FORMAT_R16G16B16_SFLOAT: return ImageFormat::R16G16B16_SFLOAT;
	case VK_FORMAT_R16G16B16A16_UNORM: return ImageFormat::R16G16B16A16_UNORM;
	case VK_FORMAT_R16G16B16A16_SNORM: return ImageFormat::R16G16B16A16_SNORM;
	case VK_FORMAT_R16G16B16A16_UINT: return ImageFormat::R16G16B16A16_UINT;
	case VK_FORMAT_R16G16B16A16_SINT: return ImageFormat::R16G16B16A16_SINT;
	case VK_FORMAT_R16G16B16A16_SFLOAT: return ImageFormat::R16G16B16A16_SFLOAT;
	case VK_FORMAT_R32_UINT: return ImageFormat::R32_UINT;
	case VK_FORMAT_R32_SINT: return ImageFormat::R32_SINT;
	case VK_FORMAT_R32_SFLOAT: return ImageFormat::R32_SFLOAT;
	case VK_FORMAT_R32G32_UINT: return ImageFormat::R32G32_UINT;
	case VK_FORMAT_R32G32_SINT: return ImageFormat::R32G32_SINT;
	case VK_FORMAT_R32G32_SFLOAT: return ImageFormat::R32G32_SFLOAT;
	case VK_FORMAT_R32G32B32_UINT: return ImageFormat::R32G32B32_UINT;
	case VK_FORMAT_R32G32B32_SINT: return ImageFormat::R32G32B32_SINT;
	case VK_FORMAT_R32G32B32_SFLOAT: return ImageFormat::R32G32B32_SFLOAT;
	case VK_FORMAT_R32G32B32A32_UINT: return ImageFormat::R32G32B32A32_UINT;
	case VK_FORMAT_R32G32B32A32_SINT: return ImageFormat::R32G32B32A32_SINT;
	case VK_FORMAT_R32G32B32A32_SFLOAT: return ImageFormat::R32G32B32A32_SFLOAT;
	case VK_FORMAT_R64_UINT: return ImageFormat::R64_UINT;
	case VK_FORMAT_R64_SINT: return ImageFormat::R64_SINT;
	case VK_FORMAT_R64_SFLOAT: return ImageFormat::R64_SFLOAT;
	case VK_FORMAT_R64G64_UINT: return ImageFormat::R64G64_UINT;
	case VK_FORMAT_R64G64_SINT: return ImageFormat::R64G64_SINT;
	case VK_FORMAT_R64G64_SFLOAT: return ImageFormat::R64G64_SFLOAT;
	case VK_FORMAT_R64G64B64_UINT: return ImageFormat::R64G64B64_UINT;
	case VK_FORMAT_R64G64B64_SINT: return ImageFormat::R64G64B64_SINT;
	case VK_FORMAT_R64G64B64_SFLOAT: return ImageFormat::R64G64B64_SFLOAT;
	case VK_FORMAT_R64G64B64A64_UINT: return ImageFormat::R64G64B64A64_UINT;
	case VK_FORMAT_R64G64B64A64_SINT: return ImageFormat::R64G64B64A64_SINT;
	case VK_FORMAT_R64G64B64A64_SFLOAT: return ImageFormat::R64G64B64A64_SFLOAT;
	case VK_FORMAT_B10G11R11_UFLOAT_PACK32: return ImageFormat::B10G11R11_UFLOAT;
	case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32: return ImageFormat::E5B9G9R9_UFLOAT;
	case VK_FORMAT_D16_UNORM: return ImageFormat::D16_UNORM;
	case VK_FORMAT_X8_D24_UNORM_PACK32: return ImageFormat::X8_D24_UNORM;
	case VK_FORMAT_D32_SFLOAT: return ImageFormat::D32_SFLOAT;
	case VK_FORMAT_S8_UINT: return ImageFormat::S8_UINT;
	case VK_FORMAT_D16_UNORM_S8_UINT: return ImageFormat::D16_UNORM_S8_UINT;
	case VK_FORMAT_D24_UNORM_S8_UINT: return ImageFormat::D24_UNORM_S8_UINT;
	case VK_FORMAT_D32_SFLOAT_S8_UINT: return ImageFormat::D32_SFLOAT_S8_UINT;
	case VK_FORMAT_BC1_RGB_UNORM_BLOCK: return ImageFormat::DXBC1_RGB_UNORM;
	case VK_FORMAT_BC1_RGB_SRGB_BLOCK: return ImageFormat::DXBC1_RGB_SRGB;
	case VK_FORMAT_BC1_RGBA_UNORM_BLOCK: return ImageFormat::DXBC1_RGBA_UNORM;
	case VK_FORMAT_BC1_RGBA_SRGB_BLOCK: return ImageFormat::DXBC1_RGBA_SRGB;
	case VK_FORMAT_BC2_UNORM_BLOCK: return ImageFormat::DXBC2_UNORM;
	case VK_FORMAT_BC2_SRGB_BLOCK: return ImageFormat::DXBC2_SRGB;
	case VK_FORMAT_BC3_UNORM_BLOCK: return ImageFormat::DXBC3_UNORM;
	case VK_FORMAT_BC3_SRGB_BLOCK: return ImageFormat::DXBC3_SRGB;
	case VK_FORMAT_BC4_UNORM_BLOCK: return ImageFormat::DXBC4_UNORM;
	case VK_FORMAT_BC4_SNORM_BLOCK: return ImageFormat::DXBC4_SNORM;
	case VK_FORMAT_BC5_UNORM_BLOCK: return ImageFormat::DXBC5_UNORM;
	case VK_FORMAT_BC5_SNORM_BLOCK: return ImageFormat::DXBC5_SNORM;
	case VK_FORMAT_BC6H_UFLOAT_BLOCK: return ImageFormat::DXBC6H_UFLOAT;
	case VK_FORMAT_BC6H_SFLOAT_BLOCK: return ImageFormat::DXBC6H_SFLOAT;
	case VK_FORMAT_BC7_UNORM_BLOCK: return ImageFormat::DXBC7_UNORM;
	case VK_FORMAT_BC7_SRGB_BLOCK: return ImageFormat::DXBC7_SRGB;
	case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK: return ImageFormat::ETC2_R8G8B8_UNORM;
	case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK: return ImageFormat::ETC2_R8G8B8_SRGB;
	case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK: return ImageFormat::ETC2_R8G8B8A1_UNORM;
	case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK: return ImageFormat::ETC2_R8G8B8A1_SRGB;
	case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK: return ImageFormat::ETC2_R8G8B8A8_UNORM;
	case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK: return ImageFormat::ETC2_R8G8B8A8_SRGB;
	case VK_FORMAT_EAC_R11_UNORM_BLOCK: return ImageFormat::ETC2_EAC_R11_UNORM;
	case VK_FORMAT_EAC_R11_SNORM_BLOCK: return ImageFormat::ETC2_EAC_R11_SNORM;
	case VK_FORMAT_EAC_R11G11_UNORM_BLOCK: return ImageFormat::ETC2_EAC_R11G11_UNORM;
	case VK_FORMAT_EAC_R11G11_SNORM_BLOCK: return ImageFormat::ETC2_EAC_R11G11_SNORM;
	case VK_FORMAT_ASTC_4x4_UNORM_BLOCK: return ImageFormat::ASTC_4x4_UNORM;
	case VK_FORMAT_ASTC_4x4_SRGB_BLOCK: return ImageFormat::ASTC_4x4_SRGB;
	case VK_FORMAT_ASTC_5x4_UNORM_BLOCK: return ImageFormat::ASTC_5x4_UNORM;
	case VK_FORMAT_ASTC_5x4_SRGB_BLOCK: return ImageFormat::ASTC_5x4_SRGB;
	case VK_FORMAT_ASTC_5x5_UNORM_BLOCK: return ImageFormat::ASTC_5x5_UNORM;
	case VK_FORMAT_ASTC_5x5_SRGB_BLOCK: return ImageFormat::ASTC_5x5_SRGB;
	case VK_FORMAT_ASTC_6x5_UNORM_BLOCK: return ImageFormat::ASTC_6x5_UNORM;
	case VK_FORMAT_ASTC_6x5_SRGB_BLOCK: return ImageFormat::ASTC_6x5_SRGB;
	case VK_FORMAT_ASTC_6x6_UNORM_BLOCK: return ImageFormat::ASTC_6x6_UNORM;
	case VK_FORMAT_ASTC_6x6_SRGB_BLOCK: return ImageFormat::ASTC_6x6_SRGB;
	case VK_FORMAT_ASTC_8x5_UNORM_BLOCK: return ImageFormat::ASTC_8x5_UNORM;
	case VK_FORMAT_ASTC_8x5_SRGB_BLOCK: return ImageFormat::ASTC_8x5_SRGB;
	case VK_FORMAT_ASTC_8x6_UNORM_BLOCK: return ImageFormat::ASTC_8x6_UNORM;
	case VK_FORMAT_ASTC_8x6_SRGB_BLOCK: return ImageFormat::ASTC_8x6_SRGB;
	case VK_FORMAT_ASTC_8x8_UNORM_BLOCK: return ImageFormat::ASTC_8x8_UNORM;
	case VK_FORMAT_ASTC_8x8_SRGB_BLOCK: return ImageFormat::ASTC_8x8_SRGB;
	case VK_FORMAT_ASTC_10x5_UNORM_BLOCK: return ImageFormat::ASTC_10x5_UNORM;
	case VK_FORMAT_ASTC_10x5_SRGB_BLOCK: return ImageFormat::ASTC_10x5_SRGB;
	case VK_FORMAT_ASTC_10x6_UNORM_BLOCK: return ImageFormat::ASTC_10x6_UNORM;
	case VK_FORMAT_ASTC_10x6_SRGB_BLOCK: return ImageFormat::ASTC_10x6_SRGB;
	case VK_FORMAT_ASTC_10x8_UNORM_BLOCK: return ImageFormat::ASTC_10x8_UNORM;
	case VK_FORMAT_ASTC_10x8_SRGB_BLOCK: return ImageFormat::ASTC_10x8_SRGB;
	case VK_FORMAT_ASTC_10x10_UNORM_BLOCK: return ImageFormat::ASTC_10x10_UNORM;
	case VK_FORMAT_ASTC_10x10_SRGB_BLOCK: return ImageFormat::ASTC_10x10_SRGB;
	case VK_FORMAT_ASTC_12x10_UNORM_BLOCK: return ImageFormat::ASTC_12x10_UNORM;
	case VK_FORMAT_ASTC_12x10_SRGB_BLOCK: return ImageFormat::ASTC_12x10_SRGB;
	case VK_FORMAT_ASTC_12x12_UNORM_BLOCK: return ImageFormat::ASTC_12x12_UNORM;
	case VK_FORMAT_ASTC_12x12_SRGB_BLOCK: return ImageFormat::ASTC_12x12_SRGB;

	// case VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG: return ImageFormat::PVRTC1_2BPP_UNORM;
	// case VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG: return ImageFormat::PVRTC1_4BPP_UNORM;
	// case VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG: return ImageFormat::PVRTC1_2BPP_SRGB;
	// case VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG: return ImageFormat::PVRTC1_4BPP_SRGB;

	case VK_FORMAT_G16B16G16R16_422_UNORM: return ImageFormat::G16B16G16R16_422_UNORM;
	case VK_FORMAT_B16G16R16G16_422_UNORM: return ImageFormat::B16G16R16G16_422_UNORM;
	case VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16: return ImageFormat::R12X4G12X4B12X4A12X4_UNORM_4PACK16;
	case VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16: return ImageFormat::G12X4B12X4G12X4R12X4_422_UNORM_4PACK16;
	case VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16: return ImageFormat::B12X4G12X4R12X4G12X4_422_UNORM_4PACK16;
	case VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16: return ImageFormat::R10X6G10X6B10X6A10X6_UNORM_4PACK16;
	case VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16: return ImageFormat::G10X6B10X6G10X6R10X6_422_UNORM_4PACK16;
	case VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16: return ImageFormat::B10X6G10X6R10X6G10X6_422_UNORM_4PACK16;
	case VK_FORMAT_G8B8G8R8_422_UNORM: return ImageFormat::G8B8G8R8_422_UNORM;
	case VK_FORMAT_B8G8R8G8_422_UNORM: return ImageFormat::B8G8R8G8_422_UNORM;
	case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM: return ImageFormat::G8_B8_R8_3PLANE_420_UNORM;
	case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM: return ImageFormat::G8_B8R8_2PLANE_420_UNORM;
	case VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM: return ImageFormat::G8_B8_R8_3PLANE_422_UNORM;
	case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM: return ImageFormat::G8_B8R8_2PLANE_422_UNORM;
	case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM: return ImageFormat::G8_B8_R8_3PLANE_444_UNORM;
	case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16: return ImageFormat::G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16;
	case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16: return ImageFormat::G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16;
	case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16: return ImageFormat::G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16;
	case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16: return ImageFormat::G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16;
	case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16: return ImageFormat::G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16;
	case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16: return ImageFormat::G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16;
	case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16: return ImageFormat::G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16;
	case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16: return ImageFormat::G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16;
	case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16: return ImageFormat::G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16;
	case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16: return ImageFormat::G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16;
	case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM: return ImageFormat::G16_B16_R16_3PLANE_420_UNORM;
	case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM: return ImageFormat::G16_B16_R16_3PLANE_422_UNORM;
	case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM: return ImageFormat::G16_B16_R16_3PLANE_444_UNORM;
	case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM: return ImageFormat::G16_B16R16_2PLANE_420_UNORM;
	case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM: return ImageFormat::G16_B16R16_2PLANE_422_UNORM;

	default:
		return TRAP::Graphics::API::ImageFormat::Undefined;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires (std::same_as<decltype(T::sType), VkStructureType>) && requires {T::pNext;}
constexpr void TRAP::Graphics::API::LinkVulkanStruct(VkBaseOutStructure*& base, T& toLink)
{
	TRAP_ASSERT(base, "LinkVulkanStruct(): base is nullptr!");

	base->pNext = reinterpret_cast<VkBaseOutStructure* const>(&toLink);
	base = base->pNext;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkFragmentShadingRateCombinerOpKHR TRAP::Graphics::API::ShadingRateCombinerToVkFragmentShadingRateCombinerOpKHR(const RendererAPI::ShadingRateCombiner combiner)
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
	}

	TRAP_ASSERT(false, "ShadingRateCombinerToVkFragmentShadingRateCombinerOpKHR(): Unknown RendererAPI::ShadingRateCombiner!");
	return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX_ENUM_KHR;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkExtent2D TRAP::Graphics::API::ShadingRateToVkExtent2D(const RendererAPI::ShadingRate rate)
{
	switch(rate)
	{
	case RendererAPI::ShadingRate::Full:
		[[fallthrough]];
	case RendererAPI::ShadingRate::NotSupported:
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
	}

	TRAP_ASSERT(false, "ShadingRateToVkExtent2D(): Unknown RendererAPI::ShadingRate!");
	return VkExtent2D{ 1, 1 };
}

//-------------------------------------------------------------------------------------------------------------------//

//TODO HDR
[[nodiscard]] constexpr TRAP::Graphics::API::ImageFormat TRAP::Graphics::API::VulkanGetRecommendedSwapchainFormat([[maybe_unused]] const bool HDR,
																						                          const bool SRGB) noexcept
{
#ifndef TRAP_PLATFORM_ANDROID
	if(SRGB)
		return TRAP::Graphics::API::ImageFormat::B8G8R8A8_SRGB;

	return TRAP::Graphics::API::ImageFormat::B8G8R8A8_UNORM;
#else
	if(SRGB)
		return TRAP::Graphics::API::ImageFormat::R8G8B8A8_SRGB;

	return TRAP::Graphics::API::ImageFormat::R8G8B8A8_UNORM;
#endif
}

#endif /*TRAP_VULKANCOMMON_H*/
