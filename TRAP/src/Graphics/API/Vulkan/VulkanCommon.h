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

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics::API
{
	class VulkanQueue;
	class VulkanPhysicalDevice;

	/// <summary>
	/// Check the if given VkResult contains an error.
	/// If the given VkResult contains an error, the function will log information about the error.
	/// </summary>
	/// <param name="result">VkResult to check.</param>
	/// <param name="function">Name of the function that called the error checker.</param>
	/// <param name="file">Name of the file where the function is in that called the error checker.</param>
	/// <param name="line">Line number of the error check call.</param>
	/// <returns>True for non error codes, otherwise false.</returns>
	constexpr bool ErrorCheck(VkResult result, std::string_view function, std::string_view file, int32_t line);
#ifdef NVIDIA_REFLEX_AVAILABLE
	/// <summary>
	/// Check the if given NvLL_VK_Status contains an error.
	/// If the given NvLL_VK_Status contains an error, the function will log information about the error.
	/// </summary>
	/// <param name="result">NvLL_VK_Status to check.</param>
	/// <param name="function">Name of the function that called the error checker.</param>
	/// <param name="file">Name of the file where the function is in that called the error checker.</param>
	/// <param name="line">Line number of the error check call.</param>
	/// <returns>True for non error codes, otherwise false.</returns>
	constexpr bool ReflexErrorCheck(NvLL_VK_Status result, std::string_view function, std::string_view file, int32_t line);
#endif /*NVIDIA_REFLEX_AVAILABLE*/
	/// <summary>
	/// Convert the RendererAPI::QueueType to VkQueueFlags.
	/// </summary>
	/// <param name="queueType">QueueType to convert.</param>
	/// <returns>Converted VkQueueFlags.</returns>
	constexpr VkQueueFlags QueueTypeToVkQueueFlags(RendererAPI::QueueType queueType) noexcept;

	/// <summary>
	/// Convert the RendererAPI::SampleCount to VkSampleCountFlagBits.
	/// </summary>
	/// <param name="sampleCount">SampleCount to convert.</param>
	/// <returns>Converted VkSampleCountFlagBits.</returns>
	constexpr VkSampleCountFlagBits SampleCountToVkSampleCount(RendererAPI::SampleCount sampleCount) noexcept;
	/// <summary>
	/// Convert the ImageFormat to VkFormat.
	/// </summary>
	/// <param name="imageFormat">ImageFormat to convert.</param>
	/// <returns>Converted VkFormat.</returns>
	constexpr VkFormat ImageFormatToVkFormat(TRAP::Graphics::API::ImageFormat imageFormat) noexcept;
	/// <summary>
	/// Determine the aspect mask for an image.
	/// </summary>
	/// <param name="format">ImageFormat.</param>
	/// <param name="includeStencilBit">Whether to include the stencil bit in the aspect mask.</param>
	/// <returns>VkImageAspectFlags.</returns>
	constexpr VkImageAspectFlags DetermineAspectMask(VkFormat format, bool includeStencilBit) noexcept;
	/// <summary>
	/// Convert the RendererAPI::DescriptorType to VkImageUsageFlags.
	/// </summary>
	/// <param name="type">DescriptorType to convert.</param>
	/// <returns>Converted VkImageUsageFlags.</returns>
	constexpr VkImageUsageFlags DescriptorTypeToVkImageUsage(RendererAPI::DescriptorType type) noexcept;
	/// <summary>
	/// Convert the VkImageUsageFlags to VkFormatFeatureFlags.
	/// </summary>
	/// <param name="usage">VkImageUsageFlags to convert.</param>
	/// <returns>Converted VkFormatFeatureFlags.</returns>
	constexpr VkFormatFeatureFlags VkImageUsageToFormatFeatures(VkImageUsageFlags usage) noexcept;
	/// <summary>
	/// Convert the RendererAPI::DescriptorType to VkBufferUsage.
	/// </summary>
	/// <param name="usage">DescriptorType to convert.</param>
	/// <param name="typed">Whether the buffer is a typed buffer.</param>
	/// <returns>Converted VkBufferUsageFlags.</returns>
	constexpr VkBufferUsageFlags DescriptorTypeToVkBufferUsage(RendererAPI::DescriptorType usage, bool typed) noexcept;
	/// <summary>
	/// Convert the RendererAPI::FilterType to VkFilter.
	/// </summary>
	/// <param name="filter">FilterType to convert.</param>
	/// <returns>Converted VkFilter.</returns>
	constexpr VkFilter FilterTypeToVkFilter(RendererAPI::FilterType filter) noexcept;
	/// <summary>
	/// Convert the RendererAPI::MipMapMode to VkSamplerMipmapMode.
	/// </summary>
	/// <param name="mipMapMode">MipMapMode to convert.</param>
	/// <returns>Converted VkSamplerMipmapMode.</returns>
	constexpr VkSamplerMipmapMode MipMapModeToVkMipMapMode(RendererAPI::MipMapMode mipMapMode) noexcept;
	/// <summary>
	/// Convert the RendererAPI::AddressMode to VkSamplerAddressMode.
	/// </summary>
	/// <param name="addressMode">AddressMode to convert.</param>
	/// <returns>Converted VkSamplerAddressMode.</returns>
	constexpr VkSamplerAddressMode AddressModeToVkAddressMode(RendererAPI::AddressMode addressMode) noexcept;
	/// <summary>
	/// Convert the RendererAPI::DescriptorType to VkDescriptorType.
	/// </summary>
	/// <param name="type">DescriptorType to convert.</param>
	/// <returns>Converted VkDescriptorType.</returns>
	constexpr VkDescriptorType DescriptorTypeToVkDescriptorType(RendererAPI::DescriptorType type) noexcept;
	/// <summary>
	/// Convert the RendererAPI::ShaderStage to VkShaderStageFlags.
	/// </summary>
	/// <param name="stages">ShaderStage(s) to convert.</param>
	/// <returns>Converted VkShaderStageFlags.</returns>
	constexpr VkShaderStageFlags ShaderStageToVkShaderStageFlags(RendererAPI::ShaderStage stages) noexcept;
	/// <summary>
	/// Convert the RendererAPI::PipelineCacheFlags to VkPipelineCacheCreateFlags.
	/// </summary>
	/// <param name="flags">PipelineCacheFlag to convert.</param>
	/// <returns>Converted VkPipelineCacheCreateFlags.</returns>
	constexpr VkPipelineCacheCreateFlags PipelineCacheFlagsToVkPipelineCacheCreateFlags(RendererAPI::PipelineCacheFlags flags) noexcept;
	/// <summary>
	/// Convert the RendererAPI::ResourceState to VkAccessFlags.
	/// </summary>
	/// <param name="state">ResourceState to convert.</param>
	/// <returns>Converted VkAccessFlags.</returns>
	constexpr VkAccessFlags ResourceStateToVkAccessFlags(RendererAPI::ResourceState state) noexcept;
	/// <summary>
	/// Convert the RendererAPI::ResourceState to VkImageLayout.
	/// </summary>
	/// <param name="usage">ResourceState to convert.</param>
	/// <returns>Converted VkImageLayout.</returns>
	constexpr VkImageLayout ResourceStateToVkImageLayout(RendererAPI::ResourceState usage) noexcept;
	/// <summary>
	/// Determine the VkPipelineStageFlags from VkAccessFlags and RendererAPI::QueueType.
	/// </summary>
	/// <param name="accessFlags">VkAccessFlags.</param>
	/// <param name="queueType">QueueType.</param>
	/// <returns>Determined VkPipelineStageFlags.</returns>
	VkPipelineStageFlags DetermineVkPipelineStageFlags(VkAccessFlags accessFlags, RendererAPI::QueueType queueType) noexcept;
	/// <summary>
	/// Convert the RendererAPI::QueryType to VkQueryType.
	/// </summary>
	/// <param name="type">QueryType to convert.</param>
	/// <returns>Converted VkQueryType.</returns>
	constexpr VkQueryType QueryTypeToVkQueryType(RendererAPI::QueryType type) noexcept;

	/// <summary>
	/// Utility to create the VkPipelineColorBlendStateCreateInfo struct from
	/// a RendererAPI::BlendStateDesc and color blend attachments.
	/// </summary>
	/// <param name="desc">BlendStateDesc.</param>
	/// <param name="attachments">Color blend attachments.</param>
	/// <returns>Created VkPipelineColorBlendStateCreateInfo.</returns>
	VkPipelineColorBlendStateCreateInfo UtilToBlendDesc(const RendererAPI::BlendStateDesc& desc,
	                                                    std::vector<VkPipelineColorBlendAttachmentState>& attachments) ;
	/// <summary>
	/// Utility to create the VkPipelineDepthStencilStateCreateInfo struct from
	/// a RendererAPI::DepthStateDesc.
	/// </summary>
	/// <param name="desc">DepthStateDesc.</param>
	/// <returns>Created VkPipelineDepthStencilStateCreateInfo.</returns>
	VkPipelineDepthStencilStateCreateInfo UtilToDepthDesc(const RendererAPI::DepthStateDesc& desc);
	/// <summary>
	/// Utility to create the VkPipelineRasterizationStateCreateInfo struct from
	/// a RendererAPI::RasterizerStateDesc.
	/// </summary>
	/// <param name="desc">RasterizerStateDesc.</param>
	/// <returns>Created VkPipelineRasterizationStateCreateInfo.</returns>
	VkPipelineRasterizationStateCreateInfo UtilToRasterizerDesc(const RendererAPI::RasterizerStateDesc& desc);
	/// <summary>
	/// Utility to retrieve the planar image memory requirement for Vulkan.
	/// </summary>
	/// <param name="device">Vulkan device.</param>
	/// <param name="image">Vulkan image.</param>
	/// <param name="planesCount">Number of planes.</param>
	/// <param name="memReq">Output Vulkan memory requirement.</param>
	/// <param name="planesOffsets">Output plane offsets.</param>
	void UtilGetPlanarVkImageMemoryRequirement(VkDevice device, VkImage image, uint32_t planesCount,
	                                           VkMemoryRequirements& memReq, std::vector<uint64_t>& planesOffsets);
	/// <summary>
	/// Utility to create the VkFragmentShadingRateCombinerOpKHR from
	/// a RendererAPI::ShadingRateCombiner.
	/// </summary>
	/// <param name="combiner">ShadingRateCombiner.</param>
	/// <returns>Converted VkFragmentShadingRateCombinerOpKHR.</returns>
	constexpr VkFragmentShadingRateCombinerOpKHR ShadingRateCombinerToVkFragmentShadingRateCombinerOpKHR(const RendererAPI::ShadingRateCombiner& combiner);
	/// <summary>
	/// Utility to create the VkExtent2D (fragment size) from
	/// a RendererAPI::ShadingRate.
	/// </summary>
	/// <param name="rate">ShadingRate.</param>
	/// <returns>Converted VkExtent2D.</returns>
	constexpr VkExtent2D ShadingRateToVkExtent2D(const RendererAPI::ShadingRate& rate);

	/// <summary>
	/// Retrieve the recommended swapchain format for Vulkan.
	/// </summary>
	/// <param name="HDR">True if HDR is desired.</param>
	/// <param name="SRGB">True if SRGB is desired.</param>
	/// <returns>Recommended swapchain format.</returns>
	TRAP::Graphics::API::ImageFormat VulkanGetRecommendedSwapchainFormat(bool HDR, bool SRGB) noexcept;

	/// <summary>
	/// Convert the VkFormat to ImageFormat.
	/// </summary>
	/// <param name="format">VkFormat to convert.</param>
	/// <returns>Converted ImageFormat.</returns>
	constexpr TRAP::Graphics::API::ImageFormat ImageFormatFromVkFormat(VkFormat format) noexcept;

#ifdef ENABLE_DEBUG_UTILS_EXTENSION
	/// <summary>
	/// Set a name for a Vulkan object.
	/// </summary>
	/// <param name="device">Vulkan device.</param>
	/// <param name="handle">Vulkan object.</param>
	/// <param name="type">Vulkan object type.</param>
	/// <param name="name">Name to set.</param>
	void VkSetObjectName([[maybe_unused]] VkDevice device, [[maybe_unused]] uint64_t handle,
				         [[maybe_unused]] VkObjectType type, [[maybe_unused]] const std::string_view name);
#else
	/// <summary>
	/// Set a name for a Vulkan object.
	/// </summary>
	/// <param name="device">Vulkan device.</param>
	/// <param name="handle">Vulkan object.</param>
	/// <param name="type">Vulkan object type.</param>
	/// <param name="name">Name to set.</param>
	void VkSetObjectName([[maybe_unused]] VkDevice device, [[maybe_unused]] uint64_t handle,
	                     [[maybe_unused]] VkDebugReportObjectTypeEXT type, [[maybe_unused]] const std::string_view name);
#endif

	inline constexpr std::array<VkAttachmentLoadOp,
	                                   static_cast<uint32_t>(RendererAPI::LoadActionType::MAX_LOAD_ACTION_TYPE)> VkAttachmentLoadOpTranslator =
	{
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_LOAD_OP_CLEAR
	};

	inline constexpr std::array<VkAttachmentStoreOp,
	                                   static_cast<uint32_t>(RendererAPI::StoreActionType::MAX_STORE_ACTION_TYPE)> VkAttachmentStoreOpTranslator =
	{
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		//Don't case is treated as store op none in most drivers
		VK_ATTACHMENT_STORE_OP_DONT_CARE
	};

	inline constexpr std::array<VkCompareOp,
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

	inline constexpr std::array<VkPipelineBindPoint,
	                                   static_cast<uint32_t>(RendererAPI::PipelineType::PIPELINE_TYPE_COUNT)> VkPipelineBindPointTranslator =
	{
		VK_PIPELINE_BIND_POINT_MAX_ENUM,
		VK_PIPELINE_BIND_POINT_COMPUTE,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR
	};

	inline constexpr std::array<VkBlendFactor,
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

	inline constexpr std::array<VkBlendOp,
	                                   static_cast<uint32_t>(RendererAPI::BlendMode::MAX_BLEND_MODES)> VkBlendOpTranslator =
	{
		VK_BLEND_OP_ADD,
		VK_BLEND_OP_SUBTRACT,
		VK_BLEND_OP_REVERSE_SUBTRACT,
		VK_BLEND_OP_MIN,
		VK_BLEND_OP_MAX
	};

	inline constexpr std::array<VkStencilOp,
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

	inline constexpr std::array<VkCullModeFlagBits,
	                                   static_cast<uint32_t>(RendererAPI::CullMode::MAX_CULL_MODES)> VkCullModeTranslator =
	{
		VK_CULL_MODE_NONE,
		VK_CULL_MODE_BACK_BIT,
		VK_CULL_MODE_FRONT_BIT
	};

	inline constexpr std::array<VkPolygonMode,
	                                   static_cast<uint32_t>(RendererAPI::FillMode::MAX_FILL_MODES)> VkFillModeTranslator =
	{
		VK_POLYGON_MODE_FILL,
		VK_POLYGON_MODE_LINE
	};

	inline constexpr std::array<VkFrontFace, 2> VkFrontFaceTranslator =
	{
		VK_FRONT_FACE_COUNTER_CLOCKWISE,
		VK_FRONT_FACE_CLOCKWISE
	};
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_DEBUG
#if __cplusplus > 201703L
	//Utility to check VkResult for errors and log them.
	#define VkCall(x) std::source_location loc = std::source_location::current();\
	                  ::TRAP::Graphics::API::ErrorCheck(x, #x, loc.file_name, loc.line);
#else
	//Utility to check VkResult for errors and log them.
	#define VkCall(x) ::TRAP::Graphics::API::ErrorCheck(x, #x, __FILE__, __LINE__);
#ifdef NVIDIA_REFLEX_AVAILABLE
	#define VkReflexCall(x) ::TRAP::Graphics::API::ReflexErrorCheck(x, #x, __FILE__, __LINE__);
#endif /*NVIDIA_REFLEX_AVAILABLE*/
#endif
#else
	/// <summary>
	/// Utility to check VkResult for errors and log them.
	/// </summary>
	constexpr void VkCall(VkResult)
	{}
#ifdef NVIDIA_REFLEX_AVAILABLE
	constexpr void VkReflexCall(NvLL_VK_Status)
	{}
#endif /*NVIDIA_REFLEX_AVAILABLE*/
#endif

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool TRAP::Graphics::API::ErrorCheck(const VkResult result, const std::string_view function,
                                               const std::string_view file, const int32_t line)
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
		TP_ERROR(Log::RendererVulkanPrefix, "Unknown error: ", function, " @[", file, ':', line, ']');
		break;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef NVIDIA_REFLEX_AVAILABLE
constexpr bool TRAP::Graphics::API::ReflexErrorCheck(const NvLL_VK_Status result, const std::string_view function,
                                                     const std::string_view file, const int32_t line)
{
	if(result == NVLL_VK_OK)
		return true;

	switch (result)
	{
	case NVLL_VK_ERROR:
		TP_ERROR(Log::RendererVulkanReflexPrefix, "NVLL_VK_ERROR: ", function, " @[", file, ':', line, ']');
		break;
	case NVLL_VK_LIBRARY_NOT_FOUND:
		TP_ERROR(Log::RendererVulkanReflexPrefix, "NVLL_VK_LIBRARY_NOT_FOUND: ", function, " @[", file, ':', line,
				']');
		break;
	case NVLL_VK_NO_IMPLEMENTATION:
		TP_ERROR(Log::RendererVulkanReflexPrefix, "NVLL_VK_NO_IMPLEMENTATION: ", function, " @[", file, ':', line,
				']');
		break;
	case NVLL_VK_API_NOT_INITIALIZED:
		TP_ERROR(Log::RendererVulkanReflexPrefix, "NVLL_VK_API_NOT_INITIALIZED: ", function, " @[", file, ':', line, ']');
		break;
	case NVLL_VK_INVALID_ARGUMENT:
		TP_ERROR(Log::RendererVulkanReflexPrefix, "NVLL_VK_INVALID_ARGUMENT: ", function, " @[", file, ':', line, ']');
		break;
	case NVLL_VK_INVALID_HANDLE:
		TP_ERROR(Log::RendererVulkanReflexPrefix, "NVLL_VK_INVALID_HANDLE: ", function, " @[", file, ':', line, ']');
		break;
	case NVLL_VK_INCOMPATIBLE_STRUCT_VERSION:
		TP_ERROR(Log::RendererVulkanReflexPrefix, "NVLL_VK_INCOMPATIBLE_STRUCT_VERSION: ", function, " @[", file, ':', line,
				']');
		break;
	case NVLL_VK_INVALID_POINTER:
		TP_ERROR(Log::RendererVulkanReflexPrefix, "NVLL_VK_INVALID_POINTER: ", function, " @[", file, ':', line, ']');
		break;
	case NVLL_VK_OUT_OF_MEMORY:
		TP_ERROR(Log::RendererVulkanReflexPrefix, "NVLL_VK_OUT_OF_MEMORY: ", function, " @[", file, ':', line, ']');
		break;
	case NVLL_VK_API_IN_USE:
		TP_ERROR(Log::RendererVulkanReflexPrefix, "NVLL_VK_API_IN_USE: ", function, " @[", file, ':', line, ']');
		break;
	case NVLL_VK_NO_VULKAN:
		TP_ERROR(Log::RendererVulkanReflexPrefix, "NVLL_VK_NO_VULKAN: ", function, " @[", file, ':', line,
				']');
		break;

	default:
		TP_ERROR(Log::RendererVulkanReflexPrefix, "Unknown error: ", function, " @[", file, ':', line, ']');
		break;
	}

	return false;
}
#endif /*NVIDIA_REFLEX_AVAILABLE*/

//-------------------------------------------------------------------------------------------------------------------//

constexpr VkQueueFlags TRAP::Graphics::API::QueueTypeToVkQueueFlags(const RendererAPI::QueueType queueType) noexcept
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
		// TRAP_ASSERT(false, "QueueTypeToVkQueueFlags(): Invalid Queue Type");
		return VK_QUEUE_FLAG_BITS_MAX_ENUM;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr VkSampleCountFlagBits TRAP::Graphics::API::SampleCountToVkSampleCount(const RendererAPI::SampleCount sampleCount) noexcept
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

	default:
		return VK_SAMPLE_COUNT_1_BIT;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr VkFormat TRAP::Graphics::API::ImageFormatToVkFormat(const TRAP::Graphics::API::ImageFormat imageFormat) noexcept
{
	switch(imageFormat)
	{
	case TRAP::Graphics::API::ImageFormat::Undefined: return VK_FORMAT_UNDEFINED;
	case TRAP::Graphics::API::ImageFormat::G4R4_UNORM: return VK_FORMAT_R4G4_UNORM_PACK8;
	case TRAP::Graphics::API::ImageFormat::A4B4G4R4_UNORM: return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
	case TRAP::Graphics::API::ImageFormat::A4R4G4B4_UNORM: return VK_FORMAT_B4G4R4A4_UNORM_PACK16;
	case TRAP::Graphics::API::ImageFormat::R5G6B5_UNORM: return VK_FORMAT_B5G6R5_UNORM_PACK16;
	case TRAP::Graphics::API::ImageFormat::B5G6R5_UNORM: return VK_FORMAT_R5G6B5_UNORM_PACK16;
	case TRAP::Graphics::API::ImageFormat::A1B5G5R5_UNORM: return VK_FORMAT_R5G5B5A1_UNORM_PACK16;
	case TRAP::Graphics::API::ImageFormat::A1R5G5B5_UNORM: return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
	case TRAP::Graphics::API::ImageFormat::B5G5R5A1_UNORM: return VK_FORMAT_A1R5G5B5_UNORM_PACK16;
	case TRAP::Graphics::API::ImageFormat::A2B10G10R10_UNORM: return VK_FORMAT_A2B10G10R10_UNORM_PACK32;

	case TRAP::Graphics::API::ImageFormat::R8_UNORM: return VK_FORMAT_R8_UNORM;
	case TRAP::Graphics::API::ImageFormat::R8_SNORM: return VK_FORMAT_R8_SNORM;
	case TRAP::Graphics::API::ImageFormat::R8_UINT: return VK_FORMAT_R8_UINT;
	case TRAP::Graphics::API::ImageFormat::R8_SINT: return VK_FORMAT_R8_SINT;
	case TRAP::Graphics::API::ImageFormat::R8_SRGB: return VK_FORMAT_R8_SRGB;
	case TRAP::Graphics::API::ImageFormat::R8G8_UNORM: return VK_FORMAT_R8G8_UNORM;
	case TRAP::Graphics::API::ImageFormat::R8G8_SNORM: return VK_FORMAT_R8G8_SNORM;
	case TRAP::Graphics::API::ImageFormat::R8G8_UINT: return VK_FORMAT_R8G8_UINT;
	case TRAP::Graphics::API::ImageFormat::R8G8_SINT: return VK_FORMAT_R8G8_SINT;
	case TRAP::Graphics::API::ImageFormat::R8G8_SRGB: return VK_FORMAT_R8G8_SRGB;
	case TRAP::Graphics::API::ImageFormat::R8G8B8_UNORM: return VK_FORMAT_R8G8B8_UNORM;
	case TRAP::Graphics::API::ImageFormat::R8G8B8_SNORM: return VK_FORMAT_R8G8B8_SNORM;
	case TRAP::Graphics::API::ImageFormat::R8G8B8_UINT: return VK_FORMAT_R8G8B8_UINT;
	case TRAP::Graphics::API::ImageFormat::R8G8B8_SINT: return VK_FORMAT_R8G8B8_SINT;
	case TRAP::Graphics::API::ImageFormat::R8G8B8_SRGB: return VK_FORMAT_R8G8B8_SRGB;
	case TRAP::Graphics::API::ImageFormat::B8G8R8_UNORM: return VK_FORMAT_B8G8R8_UNORM;
	case TRAP::Graphics::API::ImageFormat::B8G8R8_SNORM: return VK_FORMAT_B8G8R8_SNORM;
	case TRAP::Graphics::API::ImageFormat::B8G8R8_UINT: return VK_FORMAT_B8G8R8_UINT;
	case TRAP::Graphics::API::ImageFormat::B8G8R8_SINT: return VK_FORMAT_B8G8R8_SINT;
	case TRAP::Graphics::API::ImageFormat::B8G8R8_SRGB: return VK_FORMAT_B8G8R8_SRGB;
	case TRAP::Graphics::API::ImageFormat::R8G8B8A8_UNORM: return VK_FORMAT_R8G8B8A8_UNORM;
	case TRAP::Graphics::API::ImageFormat::R8G8B8A8_SNORM: return VK_FORMAT_R8G8B8A8_SNORM;
	case TRAP::Graphics::API::ImageFormat::R8G8B8A8_UINT: return VK_FORMAT_R8G8B8A8_UINT;
	case TRAP::Graphics::API::ImageFormat::R8G8B8A8_SINT: return VK_FORMAT_R8G8B8A8_SINT;
	case TRAP::Graphics::API::ImageFormat::R8G8B8A8_SRGB: return VK_FORMAT_R8G8B8A8_SRGB;
	case TRAP::Graphics::API::ImageFormat::B8G8R8A8_UNORM: return VK_FORMAT_B8G8R8A8_UNORM;
	case TRAP::Graphics::API::ImageFormat::B8G8R8A8_SNORM: return VK_FORMAT_B8G8R8A8_SNORM;
	case TRAP::Graphics::API::ImageFormat::B8G8R8A8_UINT: return VK_FORMAT_B8G8R8A8_UINT;
	case TRAP::Graphics::API::ImageFormat::B8G8R8A8_SINT: return VK_FORMAT_B8G8R8A8_SINT;
	case TRAP::Graphics::API::ImageFormat::B8G8R8A8_SRGB: return VK_FORMAT_B8G8R8A8_SRGB;
	case TRAP::Graphics::API::ImageFormat::R16_UNORM: return VK_FORMAT_R16_UNORM;
	case TRAP::Graphics::API::ImageFormat::R16_SNORM: return VK_FORMAT_R16_SNORM;
	case TRAP::Graphics::API::ImageFormat::R16_UINT: return VK_FORMAT_R16_UINT;
	case TRAP::Graphics::API::ImageFormat::R16_SINT: return VK_FORMAT_R16_SINT;
	case TRAP::Graphics::API::ImageFormat::R16_SFLOAT: return VK_FORMAT_R16_SFLOAT;
	case TRAP::Graphics::API::ImageFormat::R16G16_UNORM: return VK_FORMAT_R16G16_UNORM;
	case TRAP::Graphics::API::ImageFormat::R16G16_SNORM: return VK_FORMAT_R16G16_SNORM;
	case TRAP::Graphics::API::ImageFormat::R16G16_UINT: return VK_FORMAT_R16G16_UINT;
	case TRAP::Graphics::API::ImageFormat::R16G16_SINT: return VK_FORMAT_R16G16_SINT;
	case TRAP::Graphics::API::ImageFormat::R16G16_SFLOAT: return VK_FORMAT_R16G16_SFLOAT;
	case TRAP::Graphics::API::ImageFormat::R16G16B16_UNORM: return VK_FORMAT_R16G16B16_UNORM;
	case TRAP::Graphics::API::ImageFormat::R16G16B16_SNORM: return VK_FORMAT_R16G16B16_SNORM;
	case TRAP::Graphics::API::ImageFormat::R16G16B16_UINT: return VK_FORMAT_R16G16B16_UINT;
	case TRAP::Graphics::API::ImageFormat::R16G16B16_SINT: return VK_FORMAT_R16G16B16_SINT;
	case TRAP::Graphics::API::ImageFormat::R16G16B16_SFLOAT: return VK_FORMAT_R16G16B16_SFLOAT;
	case TRAP::Graphics::API::ImageFormat::R16G16B16A16_UNORM: return VK_FORMAT_R16G16B16A16_UNORM;
	case TRAP::Graphics::API::ImageFormat::R16G16B16A16_SNORM: return VK_FORMAT_R16G16B16A16_SNORM;
	case TRAP::Graphics::API::ImageFormat::R16G16B16A16_UINT: return VK_FORMAT_R16G16B16A16_UINT;
	case TRAP::Graphics::API::ImageFormat::R16G16B16A16_SINT: return VK_FORMAT_R16G16B16A16_SINT;
	case TRAP::Graphics::API::ImageFormat::R16G16B16A16_SFLOAT: return VK_FORMAT_R16G16B16A16_SFLOAT;
	case TRAP::Graphics::API::ImageFormat::R32_UINT: return VK_FORMAT_R32_UINT;
	case TRAP::Graphics::API::ImageFormat::R32_SINT: return VK_FORMAT_R32_SINT;
	case TRAP::Graphics::API::ImageFormat::R32_SFLOAT: return VK_FORMAT_R32_SFLOAT;
	case TRAP::Graphics::API::ImageFormat::R32G32_UINT: return VK_FORMAT_R32G32_UINT;
	case TRAP::Graphics::API::ImageFormat::R32G32_SINT: return VK_FORMAT_R32G32_SINT;
	case TRAP::Graphics::API::ImageFormat::R32G32_SFLOAT: return VK_FORMAT_R32G32_SFLOAT;
	case TRAP::Graphics::API::ImageFormat::R32G32B32_UINT: return VK_FORMAT_R32G32B32_UINT;
	case TRAP::Graphics::API::ImageFormat::R32G32B32_SINT: return VK_FORMAT_R32G32B32_SINT;
	case TRAP::Graphics::API::ImageFormat::R32G32B32_SFLOAT: return VK_FORMAT_R32G32B32_SFLOAT;
	case TRAP::Graphics::API::ImageFormat::R32G32B32A32_UINT: return VK_FORMAT_R32G32B32A32_UINT;
	case TRAP::Graphics::API::ImageFormat::R32G32B32A32_SINT: return VK_FORMAT_R32G32B32A32_SINT;
	case TRAP::Graphics::API::ImageFormat::R32G32B32A32_SFLOAT: return VK_FORMAT_R32G32B32A32_SFLOAT;
	case TRAP::Graphics::API::ImageFormat::R64_UINT: return VK_FORMAT_R64_UINT;
	case TRAP::Graphics::API::ImageFormat::R64_SINT: return VK_FORMAT_R64_SINT;
	case TRAP::Graphics::API::ImageFormat::R64_SFLOAT: return VK_FORMAT_R64_SFLOAT;
	case TRAP::Graphics::API::ImageFormat::R64G64_UINT: return VK_FORMAT_R64G64_UINT;
	case TRAP::Graphics::API::ImageFormat::R64G64_SINT: return VK_FORMAT_R64G64_SINT;
	case TRAP::Graphics::API::ImageFormat::R64G64_SFLOAT: return VK_FORMAT_R64G64_SFLOAT;
	case TRAP::Graphics::API::ImageFormat::R64G64B64_UINT: return VK_FORMAT_R64G64B64_UINT;
	case TRAP::Graphics::API::ImageFormat::R64G64B64_SINT: return VK_FORMAT_R64G64B64_SINT;
	case TRAP::Graphics::API::ImageFormat::R64G64B64_SFLOAT: return VK_FORMAT_R64G64B64_SFLOAT;
	case TRAP::Graphics::API::ImageFormat::R64G64B64A64_UINT: return VK_FORMAT_R64G64B64A64_UINT;
	case TRAP::Graphics::API::ImageFormat::R64G64B64A64_SINT: return VK_FORMAT_R64G64B64A64_SINT;
	case TRAP::Graphics::API::ImageFormat::R64G64B64A64_SFLOAT: return VK_FORMAT_R64G64B64A64_SFLOAT;

	case TRAP::Graphics::API::ImageFormat::B10G10R10A2_UNORM: return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
	case TRAP::Graphics::API::ImageFormat::B10G10R10A2_UINT: return VK_FORMAT_A2R10G10B10_UINT_PACK32;
	case TRAP::Graphics::API::ImageFormat::R10G10B10A2_UNORM: return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
	case TRAP::Graphics::API::ImageFormat::R10G10B10A2_UINT: return VK_FORMAT_A2B10G10R10_UINT_PACK32;

	case TRAP::Graphics::API::ImageFormat::B10G11R11_UFLOAT: return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
	case TRAP::Graphics::API::ImageFormat::E5B9G9R9_UFLOAT: return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;

	case TRAP::Graphics::API::ImageFormat::D16_UNORM: return VK_FORMAT_D16_UNORM;
	case TRAP::Graphics::API::ImageFormat::X8_D24_UNORM: return VK_FORMAT_X8_D24_UNORM_PACK32;
	case TRAP::Graphics::API::ImageFormat::D32_SFLOAT: return VK_FORMAT_D32_SFLOAT;
	case TRAP::Graphics::API::ImageFormat::S8_UINT: return VK_FORMAT_S8_UINT;
	case TRAP::Graphics::API::ImageFormat::D16_UNORM_S8_UINT: return VK_FORMAT_D16_UNORM_S8_UINT;
	case TRAP::Graphics::API::ImageFormat::D24_UNORM_S8_UINT: return VK_FORMAT_D24_UNORM_S8_UINT;
	case TRAP::Graphics::API::ImageFormat::D32_SFLOAT_S8_UINT: return VK_FORMAT_D32_SFLOAT_S8_UINT;
	case TRAP::Graphics::API::ImageFormat::DXBC1_RGB_UNORM: return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::DXBC1_RGB_SRGB: return VK_FORMAT_BC1_RGB_SRGB_BLOCK;
	case TRAP::Graphics::API::ImageFormat::DXBC1_RGBA_UNORM: return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::DXBC1_RGBA_SRGB: return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
	case TRAP::Graphics::API::ImageFormat::DXBC2_UNORM: return VK_FORMAT_BC2_UNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::DXBC2_SRGB: return VK_FORMAT_BC2_SRGB_BLOCK;
	case TRAP::Graphics::API::ImageFormat::DXBC3_UNORM: return VK_FORMAT_BC3_UNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::DXBC3_SRGB: return VK_FORMAT_BC3_SRGB_BLOCK;
	case TRAP::Graphics::API::ImageFormat::DXBC4_UNORM: return VK_FORMAT_BC4_UNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::DXBC4_SNORM: return VK_FORMAT_BC4_SNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::DXBC5_UNORM: return VK_FORMAT_BC5_UNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::DXBC5_SNORM: return VK_FORMAT_BC5_SNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::DXBC6H_UFLOAT: return VK_FORMAT_BC6H_UFLOAT_BLOCK;
	case TRAP::Graphics::API::ImageFormat::DXBC6H_SFLOAT: return VK_FORMAT_BC6H_SFLOAT_BLOCK;
	case TRAP::Graphics::API::ImageFormat::DXBC7_UNORM: return VK_FORMAT_BC7_UNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::DXBC7_SRGB: return VK_FORMAT_BC7_SRGB_BLOCK;
	case TRAP::Graphics::API::ImageFormat::PVRTC1_2BPP_UNORM: return VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG;
	case TRAP::Graphics::API::ImageFormat::PVRTC1_4BPP_UNORM: return VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG;
	case TRAP::Graphics::API::ImageFormat::PVRTC1_2BPP_SRGB: return VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG;
	case TRAP::Graphics::API::ImageFormat::PVRTC1_4BPP_SRGB: return VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG;
	case TRAP::Graphics::API::ImageFormat::ETC2_R8G8B8_UNORM: return VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ETC2_R8G8B8A1_UNORM: return VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ETC2_R8G8B8A8_UNORM: return VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ETC2_R8G8B8_SRGB: return VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ETC2_R8G8B8A1_SRGB: return VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ETC2_R8G8B8A8_SRGB: return VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ETC2_EAC_R11_UNORM: return VK_FORMAT_EAC_R11_UNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ETC2_EAC_R11G11_UNORM: return VK_FORMAT_EAC_R11G11_UNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ETC2_EAC_R11_SNORM: return VK_FORMAT_EAC_R11_SNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ETC2_EAC_R11G11_SNORM: return VK_FORMAT_EAC_R11G11_SNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_4x4_UNORM: return VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_4x4_SRGB: return VK_FORMAT_ASTC_4x4_SRGB_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_5x4_UNORM: return VK_FORMAT_ASTC_5x4_UNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_5x4_SRGB: return VK_FORMAT_ASTC_5x4_SRGB_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_5x5_UNORM: return VK_FORMAT_ASTC_5x5_UNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_5x5_SRGB: return VK_FORMAT_ASTC_5x5_SRGB_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_6x5_UNORM: return VK_FORMAT_ASTC_6x5_UNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_6x5_SRGB: return VK_FORMAT_ASTC_6x5_SRGB_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_6x6_UNORM: return VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_6x6_SRGB: return VK_FORMAT_ASTC_6x6_SRGB_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_8x5_UNORM: return VK_FORMAT_ASTC_8x5_UNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_8x5_SRGB: return VK_FORMAT_ASTC_8x5_SRGB_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_8x6_UNORM: return VK_FORMAT_ASTC_8x6_UNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_8x6_SRGB: return VK_FORMAT_ASTC_8x6_SRGB_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_8x8_UNORM: return VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_8x8_SRGB: return VK_FORMAT_ASTC_8x8_SRGB_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_10x5_UNORM: return VK_FORMAT_ASTC_10x5_UNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_10x5_SRGB: return VK_FORMAT_ASTC_10x5_SRGB_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_10x6_UNORM: return VK_FORMAT_ASTC_10x6_UNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_10x6_SRGB: return VK_FORMAT_ASTC_10x6_SRGB_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_10x8_UNORM: return VK_FORMAT_ASTC_10x8_UNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_10x8_SRGB: return VK_FORMAT_ASTC_10x8_SRGB_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_10x10_UNORM: return VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_10x10_SRGB: return VK_FORMAT_ASTC_10x10_SRGB_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_12x10_UNORM: return VK_FORMAT_ASTC_12x10_UNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_12x10_SRGB: return VK_FORMAT_ASTC_12x10_SRGB_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_12x12_UNORM: return VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
	case TRAP::Graphics::API::ImageFormat::ASTC_12x12_SRGB: return VK_FORMAT_ASTC_12x12_SRGB_BLOCK;
	case TRAP::Graphics::API::ImageFormat::PVRTC2_2BPP_UNORM: return VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG;
	case TRAP::Graphics::API::ImageFormat::PVRTC2_4BPP_UNORM: return VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG;
	case TRAP::Graphics::API::ImageFormat::PVRTC2_2BPP_SRGB: return VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG;
	case TRAP::Graphics::API::ImageFormat::PVRTC2_4BPP_SRGB: return VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG;

	default:
		return VK_FORMAT_UNDEFINED;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr VkImageAspectFlags TRAP::Graphics::API::DetermineAspectMask(const VkFormat format, const bool includeStencilBit) noexcept
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

constexpr VkImageUsageFlags TRAP::Graphics::API::DescriptorTypeToVkImageUsage(const RendererAPI::DescriptorType type) noexcept
{
	VkImageUsageFlags result = 0;

	if ((static_cast<uint32_t>(type & RendererAPI::DescriptorType::Texture)))
		result |= VK_IMAGE_USAGE_SAMPLED_BIT;
	if ((static_cast<uint32_t>(type & RendererAPI::DescriptorType::RWTexture)))
		result |= VK_IMAGE_USAGE_STORAGE_BIT;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr VkFormatFeatureFlags TRAP::Graphics::API::VkImageUsageToFormatFeatures(const VkImageUsageFlags usage) noexcept
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

constexpr VkBufferUsageFlags TRAP::Graphics::API::DescriptorTypeToVkBufferUsage(const RendererAPI::DescriptorType usage,
                                                                                const bool typed) noexcept
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

	//RayTracing
	if (static_cast<uint32_t>(usage & RendererAPI::DescriptorType::AccelerationStructure))
		result |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;
	if (static_cast<uint32_t>(usage & RendererAPI::DescriptorType::AccelerationStructureBuildInput))
		result |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
	if (static_cast<uint32_t>(usage & RendererAPI::DescriptorType::ShaderDeviceAddress))
		result |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR;
	if (static_cast<uint32_t>(usage & RendererAPI::DescriptorType::ShaderBindingTable))
		result |= VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr VkFilter TRAP::Graphics::API::FilterTypeToVkFilter(const RendererAPI::FilterType filter) noexcept
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

constexpr VkSamplerMipmapMode TRAP::Graphics::API::MipMapModeToVkMipMapMode(const RendererAPI::MipMapMode mipMapMode) noexcept
{
	switch (mipMapMode)
	{
	case RendererAPI::MipMapMode::Nearest:
		return VK_SAMPLER_MIPMAP_MODE_NEAREST;

	case RendererAPI::MipMapMode::Linear:
		return VK_SAMPLER_MIPMAP_MODE_LINEAR;

	default:
		// TRAP_ASSERT(false, "MipMapModeToVkMipMapMode(): Invalid Mip Map Mode");
		return VK_SAMPLER_MIPMAP_MODE_MAX_ENUM;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr VkSamplerAddressMode TRAP::Graphics::API::AddressModeToVkAddressMode(const RendererAPI::AddressMode addressMode) noexcept
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

constexpr VkDescriptorType TRAP::Graphics::API::DescriptorTypeToVkDescriptorType(const RendererAPI::DescriptorType type) noexcept
{
	switch(type)
	{
	case RendererAPI::DescriptorType::Undefined:
		// TRAP_ASSERT(false, "DescriptorTypeToVkDescriptorType(): Invalid DescriptorInfo Type");
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
		// TRAP_ASSERT(false, "DescriptorTypeToVkDescriptorType(): Invalid DescriptorInfo Type");
		return VK_DESCRIPTOR_TYPE_MAX_ENUM;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr VkShaderStageFlags TRAP::Graphics::API::ShaderStageToVkShaderStageFlags(const RendererAPI::ShaderStage stages) noexcept
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
	if (static_cast<uint32_t>(stages & RendererAPI::ShaderStage::Fragment))
		res |= VK_SHADER_STAGE_FRAGMENT_BIT;
	if (static_cast<uint32_t>(stages & RendererAPI::ShaderStage::Compute))
		res |= VK_SHADER_STAGE_COMPUTE_BIT;
	//RayTracing
	if (static_cast<uint32_t>(stages & RendererAPI::ShaderStage::RayTracing))
		res |= (VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR |
			    VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR |
			    VK_SHADER_STAGE_INTERSECTION_BIT_KHR | VK_SHADER_STAGE_CALLABLE_BIT_KHR);

	// TRAP_ASSERT(res != 0, "ShaderStageToVkShaderStageFlags(): Invalid ShaderStage combination");
	return res;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr VkPipelineCacheCreateFlags TRAP::Graphics::API::PipelineCacheFlagsToVkPipelineCacheCreateFlags(const RendererAPI::PipelineCacheFlags flags) noexcept
{
	VkPipelineCacheCreateFlags ret = 0;

	if (static_cast<uint32_t>(flags & RendererAPI::PipelineCacheFlags::ExternallySynchronized))
		ret |= VK_PIPELINE_CACHE_CREATE_EXTERNALLY_SYNCHRONIZED_BIT_EXT;

	return ret;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr VkAccessFlags TRAP::Graphics::API::ResourceStateToVkAccessFlags(const RendererAPI::ResourceState state) noexcept
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

	//RayTracing
	if (static_cast<uint32_t>(state & RendererAPI::ResourceState::RayTracingAccelerationStructure))
		ret |= VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR | VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;

	return ret;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr VkImageLayout TRAP::Graphics::API::ResourceStateToVkImageLayout(const RendererAPI::ResourceState usage) noexcept
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

	if (static_cast<uint32_t>(usage & RendererAPI::ResourceState::ShadingRateSource))
		return VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;

	return VK_IMAGE_LAYOUT_UNDEFINED;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr VkQueryType TRAP::Graphics::API::QueryTypeToVkQueryType(const RendererAPI::QueryType type) noexcept
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
		// TRAP_ASSERT(false, "QueryTypeToVkQueryType(): Invalid query heap type");
		return VK_QUERY_TYPE_MAX_ENUM;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::API::ImageFormat TRAP::Graphics::API::ImageFormatFromVkFormat(const VkFormat format) noexcept
{
	switch(format)
	{
	case VK_FORMAT_UNDEFINED: return TRAP::Graphics::API::ImageFormat::Undefined;
	case VK_FORMAT_R4G4_UNORM_PACK8: return TRAP::Graphics::API::ImageFormat::G4R4_UNORM;
	case VK_FORMAT_R4G4B4A4_UNORM_PACK16: return TRAP::Graphics::API::ImageFormat::A4B4G4R4_UNORM;
	case VK_FORMAT_B4G4R4A4_UNORM_PACK16: return TRAP::Graphics::API::ImageFormat::A4R4G4B4_UNORM;
	case VK_FORMAT_R5G6B5_UNORM_PACK16: return TRAP::Graphics::API::ImageFormat::B5G6R5_UNORM;
	case VK_FORMAT_B5G6R5_UNORM_PACK16: return TRAP::Graphics::API::ImageFormat::R5G6B5_UNORM;
	case VK_FORMAT_R5G5B5A1_UNORM_PACK16: return TRAP::Graphics::API::ImageFormat::A1B5G5R5_UNORM;
	case VK_FORMAT_B5G5R5A1_UNORM_PACK16: return TRAP::Graphics::API::ImageFormat::A1R5G5B5_UNORM;
	case VK_FORMAT_A1R5G5B5_UNORM_PACK16: return TRAP::Graphics::API::ImageFormat::B5G5R5A1_UNORM;
	case VK_FORMAT_R8_UNORM: return TRAP::Graphics::API::ImageFormat::R8_UNORM;
	case VK_FORMAT_R8_SNORM: return TRAP::Graphics::API::ImageFormat::R8_SNORM;
	case VK_FORMAT_R8_UINT: return TRAP::Graphics::API::ImageFormat::R8_UINT;
	case VK_FORMAT_R8_SINT: return TRAP::Graphics::API::ImageFormat::R8_SINT;
	case VK_FORMAT_R8_SRGB: return TRAP::Graphics::API::ImageFormat::R8_SRGB;
	case VK_FORMAT_R8G8_UNORM: return TRAP::Graphics::API::ImageFormat::R8G8_UNORM;
	case VK_FORMAT_R8G8_SNORM: return TRAP::Graphics::API::ImageFormat::R8G8_SNORM;
	case VK_FORMAT_R8G8_UINT: return TRAP::Graphics::API::ImageFormat::R8G8_UINT;
	case VK_FORMAT_R8G8_SINT: return TRAP::Graphics::API::ImageFormat::R8G8_SINT;
	case VK_FORMAT_R8G8_SRGB: return TRAP::Graphics::API::ImageFormat::R8G8_SRGB;
	case VK_FORMAT_R8G8B8_UNORM: return TRAP::Graphics::API::ImageFormat::R8G8B8_UNORM;
	case VK_FORMAT_R8G8B8_SNORM: return TRAP::Graphics::API::ImageFormat::R8G8B8_SNORM;
	case VK_FORMAT_R8G8B8_UINT: return TRAP::Graphics::API::ImageFormat::R8G8B8_UINT;
	case VK_FORMAT_R8G8B8_SINT: return TRAP::Graphics::API::ImageFormat::R8G8B8_SINT;
	case VK_FORMAT_R8G8B8_SRGB: return TRAP::Graphics::API::ImageFormat::R8G8B8_SRGB;
	case VK_FORMAT_B8G8R8_UNORM: return TRAP::Graphics::API::ImageFormat::B8G8R8_UNORM;
	case VK_FORMAT_B8G8R8_SNORM: return TRAP::Graphics::API::ImageFormat::B8G8R8_SNORM;
	case VK_FORMAT_B8G8R8_UINT: return TRAP::Graphics::API::ImageFormat::B8G8R8_UINT;
	case VK_FORMAT_B8G8R8_SINT: return TRAP::Graphics::API::ImageFormat::B8G8R8_SINT;
	case VK_FORMAT_B8G8R8_SRGB: return TRAP::Graphics::API::ImageFormat::B8G8R8_SRGB;
	case VK_FORMAT_R8G8B8A8_UNORM: return TRAP::Graphics::API::ImageFormat::R8G8B8A8_UNORM;
	case VK_FORMAT_R8G8B8A8_SNORM: return TRAP::Graphics::API::ImageFormat::R8G8B8A8_SNORM;
	case VK_FORMAT_R8G8B8A8_UINT: return TRAP::Graphics::API::ImageFormat::R8G8B8A8_UINT;
	case VK_FORMAT_R8G8B8A8_SINT: return TRAP::Graphics::API::ImageFormat::R8G8B8A8_SINT;
	case VK_FORMAT_R8G8B8A8_SRGB: return TRAP::Graphics::API::ImageFormat::R8G8B8A8_SRGB;
	case VK_FORMAT_B8G8R8A8_UNORM: return TRAP::Graphics::API::ImageFormat::B8G8R8A8_UNORM;
	case VK_FORMAT_B8G8R8A8_SNORM: return TRAP::Graphics::API::ImageFormat::B8G8R8A8_SNORM;
	case VK_FORMAT_B8G8R8A8_UINT: return TRAP::Graphics::API::ImageFormat::B8G8R8A8_UINT;
	case VK_FORMAT_B8G8R8A8_SINT: return TRAP::Graphics::API::ImageFormat::B8G8R8A8_SINT;
	case VK_FORMAT_B8G8R8A8_SRGB: return TRAP::Graphics::API::ImageFormat::B8G8R8A8_SRGB;
	case VK_FORMAT_A2R10G10B10_UNORM_PACK32: return TRAP::Graphics::API::ImageFormat::A2R10G10B10_UNORM;
	case VK_FORMAT_A2R10G10B10_UINT_PACK32: return TRAP::Graphics::API::ImageFormat::A2R10G10B10_UINT;
	case VK_FORMAT_A2B10G10R10_UNORM_PACK32: return TRAP::Graphics::API::ImageFormat::A2B10G10R10_UNORM;
	case VK_FORMAT_A2B10G10R10_UINT_PACK32: return TRAP::Graphics::API::ImageFormat::A2B10G10R10_UINT;
	case VK_FORMAT_R16_UNORM: return TRAP::Graphics::API::ImageFormat::R16_UNORM;
	case VK_FORMAT_R16_SNORM: return TRAP::Graphics::API::ImageFormat::R16_SNORM;
	case VK_FORMAT_R16_UINT: return TRAP::Graphics::API::ImageFormat::R16_UINT;
	case VK_FORMAT_R16_SINT: return TRAP::Graphics::API::ImageFormat::R16_SINT;
	case VK_FORMAT_R16_SFLOAT: return TRAP::Graphics::API::ImageFormat::R16_SFLOAT;
	case VK_FORMAT_R16G16_UNORM: return TRAP::Graphics::API::ImageFormat::R16G16_UNORM;
	case VK_FORMAT_R16G16_SNORM: return TRAP::Graphics::API::ImageFormat::R16G16_SNORM;
	case VK_FORMAT_R16G16_UINT: return TRAP::Graphics::API::ImageFormat::R16G16_UINT;
	case VK_FORMAT_R16G16_SINT: return TRAP::Graphics::API::ImageFormat::R16G16_SINT;
	case VK_FORMAT_R16G16_SFLOAT: return TRAP::Graphics::API::ImageFormat::R16G16_SFLOAT;
	case VK_FORMAT_R16G16B16_UNORM: return TRAP::Graphics::API::ImageFormat::R16G16B16_UNORM;
	case VK_FORMAT_R16G16B16_SNORM: return TRAP::Graphics::API::ImageFormat::R16G16B16_SNORM;
	case VK_FORMAT_R16G16B16_UINT: return TRAP::Graphics::API::ImageFormat::R16G16B16_UINT;
	case VK_FORMAT_R16G16B16_SINT: return TRAP::Graphics::API::ImageFormat::R16G16B16_SINT;
	case VK_FORMAT_R16G16B16_SFLOAT: return TRAP::Graphics::API::ImageFormat::R16G16B16_SFLOAT;
	case VK_FORMAT_R16G16B16A16_UNORM: return TRAP::Graphics::API::ImageFormat::R16G16B16A16_UNORM;
	case VK_FORMAT_R16G16B16A16_SNORM: return TRAP::Graphics::API::ImageFormat::R16G16B16A16_SNORM;
	case VK_FORMAT_R16G16B16A16_UINT: return TRAP::Graphics::API::ImageFormat::R16G16B16A16_UINT;
	case VK_FORMAT_R16G16B16A16_SINT: return TRAP::Graphics::API::ImageFormat::R16G16B16A16_SINT;
	case VK_FORMAT_R16G16B16A16_SFLOAT: return TRAP::Graphics::API::ImageFormat::R16G16B16A16_SFLOAT;
	case VK_FORMAT_R32_UINT: return TRAP::Graphics::API::ImageFormat::R32_UINT;
	case VK_FORMAT_R32_SINT: return TRAP::Graphics::API::ImageFormat::R32_SINT;
	case VK_FORMAT_R32_SFLOAT: return TRAP::Graphics::API::ImageFormat::R32_SFLOAT;
	case VK_FORMAT_R32G32_UINT: return TRAP::Graphics::API::ImageFormat::R32G32_UINT;
	case VK_FORMAT_R32G32_SINT: return TRAP::Graphics::API::ImageFormat::R32G32_SINT;
	case VK_FORMAT_R32G32_SFLOAT: return TRAP::Graphics::API::ImageFormat::R32G32_SFLOAT;
	case VK_FORMAT_R32G32B32_UINT: return TRAP::Graphics::API::ImageFormat::R32G32B32_UINT;
	case VK_FORMAT_R32G32B32_SINT: return TRAP::Graphics::API::ImageFormat::R32G32B32_SINT;
	case VK_FORMAT_R32G32B32_SFLOAT: return TRAP::Graphics::API::ImageFormat::R32G32B32_SFLOAT;
	case VK_FORMAT_R32G32B32A32_UINT: return TRAP::Graphics::API::ImageFormat::R32G32B32A32_UINT;
	case VK_FORMAT_R32G32B32A32_SINT: return TRAP::Graphics::API::ImageFormat::R32G32B32A32_SINT;
	case VK_FORMAT_R32G32B32A32_SFLOAT: return TRAP::Graphics::API::ImageFormat::R32G32B32A32_SFLOAT;
	case VK_FORMAT_R64_UINT: return TRAP::Graphics::API::ImageFormat::R64_UINT;
	case VK_FORMAT_R64_SINT: return TRAP::Graphics::API::ImageFormat::R64_SINT;
	case VK_FORMAT_R64_SFLOAT: return TRAP::Graphics::API::ImageFormat::R64_SFLOAT;
	case VK_FORMAT_R64G64_UINT: return TRAP::Graphics::API::ImageFormat::R64G64_UINT;
	case VK_FORMAT_R64G64_SINT: return TRAP::Graphics::API::ImageFormat::R64G64_SINT;
	case VK_FORMAT_R64G64_SFLOAT: return TRAP::Graphics::API::ImageFormat::R64G64_SFLOAT;
	case VK_FORMAT_R64G64B64_UINT: return TRAP::Graphics::API::ImageFormat::R64G64B64_UINT;
	case VK_FORMAT_R64G64B64_SINT: return TRAP::Graphics::API::ImageFormat::R64G64B64_SINT;
	case VK_FORMAT_R64G64B64_SFLOAT: return TRAP::Graphics::API::ImageFormat::R64G64B64_SFLOAT;
	case VK_FORMAT_R64G64B64A64_UINT: return TRAP::Graphics::API::ImageFormat::R64G64B64A64_UINT;
	case VK_FORMAT_R64G64B64A64_SINT: return TRAP::Graphics::API::ImageFormat::R64G64B64A64_SINT;
	case VK_FORMAT_R64G64B64A64_SFLOAT: return TRAP::Graphics::API::ImageFormat::R64G64B64A64_SFLOAT;
	case VK_FORMAT_B10G11R11_UFLOAT_PACK32: return TRAP::Graphics::API::ImageFormat::B10G11R11_UFLOAT;
	case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32: return TRAP::Graphics::API::ImageFormat::E5B9G9R9_UFLOAT;
	case VK_FORMAT_D16_UNORM: return TRAP::Graphics::API::ImageFormat::D16_UNORM;
	case VK_FORMAT_X8_D24_UNORM_PACK32: return TRAP::Graphics::API::ImageFormat::X8_D24_UNORM;
	case VK_FORMAT_D32_SFLOAT: return TRAP::Graphics::API::ImageFormat::D32_SFLOAT;
	case VK_FORMAT_S8_UINT: return TRAP::Graphics::API::ImageFormat::S8_UINT;
	case VK_FORMAT_D16_UNORM_S8_UINT: return TRAP::Graphics::API::ImageFormat::D16_UNORM_S8_UINT;
	case VK_FORMAT_D24_UNORM_S8_UINT: return TRAP::Graphics::API::ImageFormat::D24_UNORM_S8_UINT;
	case VK_FORMAT_D32_SFLOAT_S8_UINT: return TRAP::Graphics::API::ImageFormat::D32_SFLOAT_S8_UINT;
	case VK_FORMAT_BC1_RGB_UNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::DXBC1_RGB_UNORM;
	case VK_FORMAT_BC1_RGB_SRGB_BLOCK: return TRAP::Graphics::API::ImageFormat::DXBC1_RGB_SRGB;
	case VK_FORMAT_BC1_RGBA_UNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::DXBC1_RGBA_UNORM;
	case VK_FORMAT_BC1_RGBA_SRGB_BLOCK: return TRAP::Graphics::API::ImageFormat::DXBC1_RGBA_SRGB;
	case VK_FORMAT_BC2_UNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::DXBC2_UNORM;
	case VK_FORMAT_BC2_SRGB_BLOCK: return TRAP::Graphics::API::ImageFormat::DXBC2_SRGB;
	case VK_FORMAT_BC3_UNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::DXBC3_UNORM;
	case VK_FORMAT_BC3_SRGB_BLOCK: return TRAP::Graphics::API::ImageFormat::DXBC3_SRGB;
	case VK_FORMAT_BC4_UNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::DXBC4_UNORM;
	case VK_FORMAT_BC4_SNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::DXBC4_SNORM;
	case VK_FORMAT_BC5_UNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::DXBC5_UNORM;
	case VK_FORMAT_BC5_SNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::DXBC5_SNORM;
	case VK_FORMAT_BC6H_UFLOAT_BLOCK: return TRAP::Graphics::API::ImageFormat::DXBC6H_UFLOAT;
	case VK_FORMAT_BC6H_SFLOAT_BLOCK: return TRAP::Graphics::API::ImageFormat::DXBC6H_SFLOAT;
	case VK_FORMAT_BC7_UNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::DXBC7_UNORM;
	case VK_FORMAT_BC7_SRGB_BLOCK: return TRAP::Graphics::API::ImageFormat::DXBC7_SRGB;
	case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::ETC2_R8G8B8_UNORM;
	case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK: return TRAP::Graphics::API::ImageFormat::ETC2_R8G8B8_SRGB;
	case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::ETC2_R8G8B8A1_UNORM;
	case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK: return TRAP::Graphics::API::ImageFormat::ETC2_R8G8B8A1_SRGB;
	case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::ETC2_R8G8B8A8_UNORM;
	case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK: return TRAP::Graphics::API::ImageFormat::ETC2_R8G8B8A8_SRGB;
	case VK_FORMAT_EAC_R11_UNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::ETC2_EAC_R11_UNORM;
	case VK_FORMAT_EAC_R11_SNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::ETC2_EAC_R11_SNORM;
	case VK_FORMAT_EAC_R11G11_UNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::ETC2_EAC_R11G11_UNORM;
	case VK_FORMAT_EAC_R11G11_SNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::ETC2_EAC_R11G11_SNORM;
	case VK_FORMAT_ASTC_4x4_UNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_4x4_UNORM;
	case VK_FORMAT_ASTC_4x4_SRGB_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_4x4_SRGB;
	case VK_FORMAT_ASTC_5x4_UNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_5x4_UNORM;
	case VK_FORMAT_ASTC_5x4_SRGB_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_5x4_SRGB;
	case VK_FORMAT_ASTC_5x5_UNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_5x5_UNORM;
	case VK_FORMAT_ASTC_5x5_SRGB_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_5x5_SRGB;
	case VK_FORMAT_ASTC_6x5_UNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_6x5_UNORM;
	case VK_FORMAT_ASTC_6x5_SRGB_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_6x5_SRGB;
	case VK_FORMAT_ASTC_6x6_UNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_6x6_UNORM;
	case VK_FORMAT_ASTC_6x6_SRGB_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_6x6_SRGB;
	case VK_FORMAT_ASTC_8x5_UNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_8x5_UNORM;
	case VK_FORMAT_ASTC_8x5_SRGB_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_8x5_SRGB;
	case VK_FORMAT_ASTC_8x6_UNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_8x6_UNORM;
	case VK_FORMAT_ASTC_8x6_SRGB_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_8x6_SRGB;
	case VK_FORMAT_ASTC_8x8_UNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_8x8_UNORM;
	case VK_FORMAT_ASTC_8x8_SRGB_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_8x8_SRGB;
	case VK_FORMAT_ASTC_10x5_UNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_10x5_UNORM;
	case VK_FORMAT_ASTC_10x5_SRGB_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_10x5_SRGB;
	case VK_FORMAT_ASTC_10x6_UNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_10x6_UNORM;
	case VK_FORMAT_ASTC_10x6_SRGB_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_10x6_SRGB;
	case VK_FORMAT_ASTC_10x8_UNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_10x8_UNORM;
	case VK_FORMAT_ASTC_10x8_SRGB_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_10x8_SRGB;
	case VK_FORMAT_ASTC_10x10_UNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_10x10_UNORM;
	case VK_FORMAT_ASTC_10x10_SRGB_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_10x10_SRGB;
	case VK_FORMAT_ASTC_12x10_UNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_12x10_UNORM;
	case VK_FORMAT_ASTC_12x10_SRGB_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_12x10_SRGB;
	case VK_FORMAT_ASTC_12x12_UNORM_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_12x12_UNORM;
	case VK_FORMAT_ASTC_12x12_SRGB_BLOCK: return TRAP::Graphics::API::ImageFormat::ASTC_12x12_SRGB;

	case VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG: return TRAP::Graphics::API::ImageFormat::PVRTC1_2BPP_UNORM;
	case VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG: return TRAP::Graphics::API::ImageFormat::PVRTC1_4BPP_UNORM;
	case VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG: return TRAP::Graphics::API::ImageFormat::PVRTC1_2BPP_SRGB;
	case VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG: return TRAP::Graphics::API::ImageFormat::PVRTC1_4BPP_SRGB;

	case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
		[[fallthrough]];
	case VK_FORMAT_R8_USCALED:
		[[fallthrough]];
	case VK_FORMAT_R8_SSCALED:
		[[fallthrough]];
	case VK_FORMAT_R8G8_USCALED:
		[[fallthrough]];
	case VK_FORMAT_R8G8_SSCALED:
		[[fallthrough]];
	case VK_FORMAT_R8G8B8_USCALED:
		[[fallthrough]];
	case VK_FORMAT_R8G8B8_SSCALED:
		[[fallthrough]];
	case VK_FORMAT_B8G8R8_USCALED:
		[[fallthrough]];
	case VK_FORMAT_B8G8R8_SSCALED:
		[[fallthrough]];
	case VK_FORMAT_R8G8B8A8_USCALED:
		[[fallthrough]];
	case VK_FORMAT_R8G8B8A8_SSCALED:
		[[fallthrough]];
	case VK_FORMAT_B8G8R8A8_USCALED:
		[[fallthrough]];
	case VK_FORMAT_B8G8R8A8_SSCALED:
		[[fallthrough]];
	case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
		[[fallthrough]];
	case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
		[[fallthrough]];
	case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
		[[fallthrough]];
	case VK_FORMAT_A8B8G8R8_UINT_PACK32:
		[[fallthrough]];
	case VK_FORMAT_A8B8G8R8_SINT_PACK32:
		[[fallthrough]];
	case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
		[[fallthrough]];
	case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
		[[fallthrough]];
	case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
		[[fallthrough]];
	case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
		[[fallthrough]];
	case VK_FORMAT_A2R10G10B10_SINT_PACK32:
		[[fallthrough]];
	case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
		[[fallthrough]];
	case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
		[[fallthrough]];
	case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
		[[fallthrough]];
	case VK_FORMAT_A2B10G10R10_SINT_PACK32:
		[[fallthrough]];
	case VK_FORMAT_R16_USCALED:
		[[fallthrough]];
	case VK_FORMAT_R16_SSCALED:
		[[fallthrough]];
	case VK_FORMAT_R16G16_USCALED:
		[[fallthrough]];
	case VK_FORMAT_R16G16_SSCALED:
		[[fallthrough]];
	case VK_FORMAT_R16G16B16_USCALED:
		[[fallthrough]];
	case VK_FORMAT_R16G16B16_SSCALED:
		[[fallthrough]];
	case VK_FORMAT_R16G16B16A16_USCALED:
		[[fallthrough]];
	case VK_FORMAT_R16G16B16A16_SSCALED:
		[[fallthrough]];
	case VK_FORMAT_R10X6_UNORM_PACK16:
		[[fallthrough]];
	case VK_FORMAT_R10X6G10X6_UNORM_2PACK16:
		[[fallthrough]];
	case VK_FORMAT_R12X4_UNORM_PACK16:
		[[fallthrough]];
	case VK_FORMAT_R12X4G12X4_UNORM_2PACK16:
		[[fallthrough]];
	case VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG:
		[[fallthrough]];
	case VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG:
		[[fallthrough]];
	case VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG:
		[[fallthrough]];
	case VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG:
		return TRAP::Graphics::API::ImageFormat::Undefined;

	default:
		return TRAP::Graphics::API::ImageFormat::Undefined;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr VkFragmentShadingRateCombinerOpKHR TRAP::Graphics::API::ShadingRateCombinerToVkFragmentShadingRateCombinerOpKHR(const RendererAPI::ShadingRateCombiner& combiner)
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

constexpr VkExtent2D TRAP::Graphics::API::ShadingRateToVkExtent2D(const RendererAPI::ShadingRate& rate)
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

#endif /*TRAP_VULKANCOMMON_H*/