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
#include "Utils/ConstexprMap.h"

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
	/// <param name="column">Column number of the error check call.</param>
	/// <returns>True for non error codes, otherwise false.</returns>
	constexpr bool ErrorCheck(VkResult result, std::string_view function, std::string_view file,
	                          std::uint_least32_t line, std::uint_least32_t column);
#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
	/// <summary>
	/// Check the if given NvLL_VK_Status contains an error.
	/// If the given NvLL_VK_Status contains an error, the function will log information about the error.
	/// </summary>
	/// <param name="result">NvLL_VK_Status to check.</param>
	/// <param name="function">Name of the function that called the error checker.</param>
	/// <param name="file">Name of the file where the function is in that called the error checker.</param>
	/// <param name="line">Line number of the error check call.</param>
	/// <param name="column">Column number of the error check call.</param>
	/// <returns>True for non error codes, otherwise false.</returns>
	constexpr bool ReflexErrorCheck(NvLL_VK_Status result, std::string_view function, std::string_view file,
	                                std::uint_least32_t line, std::uint_least32_t column);
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/
	/// <summary>
	/// Convert the RendererAPI::QueueType to VkQueueFlags.
	/// </summary>
	/// <param name="queueType">QueueType to convert.</param>
	/// <returns>Converted VkQueueFlags.</returns>
	[[nodiscard]] VkQueueFlags QueueTypeToVkQueueFlags(RendererAPI::QueueType queueType) noexcept;

	/// <summary>
	/// Convert the RendererAPI::SampleCount to VkSampleCountFlagBits.
	/// </summary>
	/// <param name="sampleCount">SampleCount to convert.</param>
	/// <returns>Converted VkSampleCountFlagBits.</returns>
	[[nodiscard]] constexpr VkSampleCountFlagBits SampleCountToVkSampleCount(RendererAPI::SampleCount sampleCount) noexcept;
	/// <summary>
	/// Convert the ImageFormat to VkFormat.
	/// </summary>
	/// <param name="imageFormat">ImageFormat to convert.</param>
	/// <returns>Converted VkFormat.</returns>
	[[nodiscard]] constexpr VkFormat ImageFormatToVkFormat(TRAP::Graphics::API::ImageFormat imageFormat) noexcept;
	/// <summary>
	/// Determine the aspect mask for an image.
	/// </summary>
	/// <param name="format">ImageFormat.</param>
	/// <param name="includeStencilBit">Whether to include the stencil bit in the aspect mask.</param>
	/// <returns>VkImageAspectFlags.</returns>
	[[nodiscard]] constexpr VkImageAspectFlags DetermineAspectMask(VkFormat format, bool includeStencilBit) noexcept;
	/// <summary>
	/// Convert the RendererAPI::DescriptorType to VkImageUsageFlags.
	/// </summary>
	/// <param name="type">DescriptorType to convert.</param>
	/// <returns>Converted VkImageUsageFlags.</returns>
	[[nodiscard]] constexpr VkImageUsageFlags DescriptorTypeToVkImageUsage(RendererAPI::DescriptorType type) noexcept;
	/// <summary>
	/// Convert the VkImageUsageFlags to VkFormatFeatureFlags.
	/// </summary>
	/// <param name="usage">VkImageUsageFlags to convert.</param>
	/// <returns>Converted VkFormatFeatureFlags.</returns>
	[[nodiscard]] constexpr VkFormatFeatureFlags VkImageUsageToFormatFeatures(VkImageUsageFlags usage) noexcept;
	/// <summary>
	/// Convert the RendererAPI::DescriptorType to VkBufferUsage.
	/// </summary>
	/// <param name="usage">DescriptorType to convert.</param>
	/// <param name="typed">Whether the buffer is a typed buffer.</param>
	/// <returns>Converted VkBufferUsageFlags.</returns>
	[[nodiscard]] constexpr VkBufferUsageFlags DescriptorTypeToVkBufferUsage(RendererAPI::DescriptorType usage, bool typed) noexcept;
	/// <summary>
	/// Convert the RendererAPI::FilterType to VkFilter.
	/// </summary>
	/// <param name="filter">FilterType to convert.</param>
	/// <returns>Converted VkFilter.</returns>
	[[nodiscard]] constexpr VkFilter FilterTypeToVkFilter(RendererAPI::FilterType filter) noexcept;
	/// <summary>
	/// Convert the RendererAPI::MipMapMode to VkSamplerMipmapMode.
	/// </summary>
	/// <param name="mipMapMode">MipMapMode to convert.</param>
	/// <returns>Converted VkSamplerMipmapMode.</returns>
	[[nodiscard]] VkSamplerMipmapMode MipMapModeToVkMipMapMode(RendererAPI::MipMapMode mipMapMode) noexcept;
	/// <summary>
	/// Convert the RendererAPI::AddressMode to VkSamplerAddressMode.
	/// </summary>
	/// <param name="addressMode">AddressMode to convert.</param>
	/// <returns>Converted VkSamplerAddressMode.</returns>
	[[nodiscard]] constexpr VkSamplerAddressMode AddressModeToVkAddressMode(RendererAPI::AddressMode addressMode) noexcept;
	/// <summary>
	/// Convert the RendererAPI::DescriptorType to VkDescriptorType.
	/// </summary>
	/// <param name="type">DescriptorType to convert.</param>
	/// <returns>Converted VkDescriptorType.</returns>
	[[nodiscard]] VkDescriptorType DescriptorTypeToVkDescriptorType(RendererAPI::DescriptorType type) noexcept;
	/// <summary>
	/// Convert the RendererAPI::ShaderStage to VkShaderStageFlags.
	/// </summary>
	/// <param name="stages">ShaderStage(s) to convert.</param>
	/// <returns>Converted VkShaderStageFlags.</returns>
	[[nodiscard]] VkShaderStageFlags ShaderStageToVkShaderStageFlags(RendererAPI::ShaderStage stages) noexcept;
	/// <summary>
	/// Convert the RendererAPI::PipelineCacheFlags to VkPipelineCacheCreateFlags.
	/// </summary>
	/// <param name="flags">PipelineCacheFlag to convert.</param>
	/// <returns>Converted VkPipelineCacheCreateFlags.</returns>
	[[nodiscard]] constexpr VkPipelineCacheCreateFlags PipelineCacheFlagsToVkPipelineCacheCreateFlags(RendererAPI::PipelineCacheFlags flags) noexcept;
	/// <summary>
	/// Convert the RendererAPI::ResourceState to VkAccessFlags.
	/// </summary>
	/// <param name="state">ResourceState to convert.</param>
	/// <returns>Converted VkAccessFlags.</returns>
	[[nodiscard]] constexpr VkAccessFlags ResourceStateToVkAccessFlags(RendererAPI::ResourceState state) noexcept;
	/// <summary>
	/// Convert the RendererAPI::ResourceState to VkImageLayout.
	/// </summary>
	/// <param name="usage">ResourceState to convert.</param>
	/// <returns>Converted VkImageLayout.</returns>
	[[nodiscard]] constexpr VkImageLayout ResourceStateToVkImageLayout(RendererAPI::ResourceState usage) noexcept;
	/// <summary>
	/// Determine the VkPipelineStageFlags from VkAccessFlags and RendererAPI::QueueType.
	/// </summary>
	/// <param name="accessFlags">VkAccessFlags.</param>
	/// <param name="queueType">QueueType.</param>
	/// <returns>Determined VkPipelineStageFlags.</returns>
	[[nodiscard]] VkPipelineStageFlags DetermineVkPipelineStageFlags(VkAccessFlags accessFlags, RendererAPI::QueueType queueType) noexcept;
	/// <summary>
	/// Convert the RendererAPI::QueryType to VkQueryType.
	/// </summary>
	/// <param name="type">QueryType to convert.</param>
	/// <returns>Converted VkQueryType.</returns>
	[[nodiscard]] VkQueryType QueryTypeToVkQueryType(RendererAPI::QueryType type) noexcept;

	/// <summary>
	/// Utility to create the VkPipelineColorBlendStateCreateInfo struct from
	/// a RendererAPI::BlendStateDesc and color blend attachments.
	/// </summary>
	/// <param name="desc">BlendStateDesc.</param>
	/// <param name="attachments">Color blend attachments.</param>
	/// <returns>Created VkPipelineColorBlendStateCreateInfo.</returns>
	[[nodiscard]] VkPipelineColorBlendStateCreateInfo UtilToBlendDesc(const RendererAPI::BlendStateDesc& desc,
	                                                                  std::vector<VkPipelineColorBlendAttachmentState>& attachments) ;
	/// <summary>
	/// Utility to create the VkPipelineDepthStencilStateCreateInfo struct from
	/// a RendererAPI::DepthStateDesc.
	/// </summary>
	/// <param name="desc">DepthStateDesc.</param>
	/// <returns>Created VkPipelineDepthStencilStateCreateInfo.</returns>
	[[nodiscard]] VkPipelineDepthStencilStateCreateInfo UtilToDepthDesc(const RendererAPI::DepthStateDesc& desc);
	/// <summary>
	/// Utility to create the VkPipelineRasterizationStateCreateInfo struct from
	/// a RendererAPI::RasterizerStateDesc.
	/// </summary>
	/// <param name="desc">RasterizerStateDesc.</param>
	/// <returns>Created VkPipelineRasterizationStateCreateInfo.</returns>
	[[nodiscard]] VkPipelineRasterizationStateCreateInfo UtilToRasterizerDesc(const RendererAPI::RasterizerStateDesc& desc);
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
	[[nodiscard]] VkFragmentShadingRateCombinerOpKHR ShadingRateCombinerToVkFragmentShadingRateCombinerOpKHR(const RendererAPI::ShadingRateCombiner& combiner);
	/// <summary>
	/// Utility to create the VkExtent2D (fragment size) from
	/// a RendererAPI::ShadingRate.
	/// </summary>
	/// <param name="rate">ShadingRate.</param>
	/// <returns>Converted VkExtent2D.</returns>
	[[nodiscard]] VkExtent2D ShadingRateToVkExtent2D(const RendererAPI::ShadingRate& rate);

	/// <summary>
	/// Retrieve the recommended swapchain format for Vulkan.
	/// </summary>
	/// <param name="HDR">True if HDR is desired.</param>
	/// <param name="SRGB">True if SRGB is desired.</param>
	/// <returns>Recommended swapchain format.</returns>
	[[nodiscard]] constexpr TRAP::Graphics::API::ImageFormat VulkanGetRecommendedSwapchainFormat(bool HDR, bool SRGB) noexcept;

	/// <summary>
	/// Convert the VkFormat to ImageFormat.
	/// </summary>
	/// <param name="format">VkFormat to convert.</param>
	/// <returns>Converted ImageFormat.</returns>
	[[nodiscard]] constexpr TRAP::Graphics::API::ImageFormat ImageFormatFromVkFormat(VkFormat format) noexcept;

#ifdef ENABLE_DEBUG_UTILS_EXTENSION
	/// <summary>
	/// Set a name for a Vulkan object.
	/// </summary>
	/// <param name="device">Vulkan device.</param>
	/// <param name="handle">Vulkan object.</param>
	/// <param name="type">Vulkan object type.</param>
	/// <param name="name">Name to set.</param>
	void VkSetObjectName([[maybe_unused]] VkDevice device, [[maybe_unused]] uint64_t handle,
				         [[maybe_unused]] VkObjectType type, [[maybe_unused]] std::string_view name);
#else
	/// <summary>
	/// Set a name for a Vulkan object.
	/// </summary>
	/// <param name="device">Vulkan device.</param>
	/// <param name="handle">Vulkan object.</param>
	/// <param name="type">Vulkan object type.</param>
	/// <param name="name">Name to set.</param>
	void VkSetObjectName([[maybe_unused]] VkDevice device, [[maybe_unused]] uint64_t handle,
	                     [[maybe_unused]] VkDebugReportObjectTypeEXT type, [[maybe_unused]] std::string_view name);
#endif

	inline constexpr std::array<VkAttachmentLoadOp,
	                            std::to_underlying(RendererAPI::LoadActionType::MAX_LOAD_ACTION_TYPE)> VkAttachmentLoadOpTranslator =
	{
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_LOAD_OP_CLEAR
	};

	inline constexpr std::array<VkAttachmentStoreOp,
	                            std::to_underlying(RendererAPI::StoreActionType::MAX_STORE_ACTION_TYPE)> VkAttachmentStoreOpTranslator =
	{
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		//Don't case is treated as store op none in most drivers
		VK_ATTACHMENT_STORE_OP_DONT_CARE
	};

	inline constexpr std::array<VkCompareOp,
	                            std::to_underlying(RendererAPI::CompareMode::MAX_COMPARE_MODES)> VkComparisonFuncTranslator =
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
	                            std::to_underlying(RendererAPI::PipelineType::PIPELINE_TYPE_COUNT)> VkPipelineBindPointTranslator =
	{
		VK_PIPELINE_BIND_POINT_MAX_ENUM,
		VK_PIPELINE_BIND_POINT_COMPUTE,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR
	};

	inline constexpr std::array<VkBlendFactor,
	                            std::to_underlying(RendererAPI::BlendConstant::MAX_BLEND_CONSTANTS)> VkBlendConstantTranslator =
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
	                            std::to_underlying(RendererAPI::BlendMode::MAX_BLEND_MODES)> VkBlendOpTranslator =
	{
		VK_BLEND_OP_ADD,
		VK_BLEND_OP_SUBTRACT,
		VK_BLEND_OP_REVERSE_SUBTRACT,
		VK_BLEND_OP_MIN,
		VK_BLEND_OP_MAX
	};

	inline constexpr std::array<VkStencilOp,
	                            std::to_underlying(RendererAPI::StencilOp::MAX_STENCIL_OPS)> VkStencilOpTranslator =
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
	                            std::to_underlying(RendererAPI::CullMode::MAX_CULL_MODES)> VkCullModeTranslator =
	{
		VK_CULL_MODE_NONE,
		VK_CULL_MODE_BACK_BIT,
		VK_CULL_MODE_FRONT_BIT
	};

	inline constexpr std::array<VkPolygonMode,
	                            std::to_underlying(RendererAPI::FillMode::MAX_FILL_MODES)> VkFillModeTranslator =
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
 	//Utility to check VkResult for errors and log them.
 	#define VkCall(x) { constexpr std::source_location loc = std::source_location::current(); \
 	                    ::TRAP::Graphics::API::ErrorCheck(x, loc.function_name(), loc.file_name(), loc.line(), loc.column()); }
#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
	#define VkReflexCall(x) { constexpr std::source_location loc = std::source_location::current(); \
 	                          ::TRAP::Graphics::API::ReflexErrorCheck(x, loc.function_name(), loc.file_name(), loc.line(), loc.column()); }
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/
#else
	/// <summary>
	/// Utility to check VkResult for errors and log them.
	/// </summary>
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
	inline constexpr Utils::ConstexprMap<VkResult, std::string_view, 35> VkResultToString
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
				{VK_ERROR_SURFACE_LOST_KHR, "VK_ERROR_SURFACE_LOST_KHR"},
				{VK_ERROR_NATIVE_WINDOW_IN_USE_KHR, "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR"},
				{VK_SUBOPTIMAL_KHR, "VK_SUBOPTIMAL_KHR"},
				{VK_ERROR_OUT_OF_DATE_KHR, "VK_ERROR_OUT_OF_DATE_KHR"},
				{VK_ERROR_INCOMPATIBLE_DISPLAY_KHR, "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR"},
				{VK_ERROR_VALIDATION_FAILED_EXT, "VK_ERROR_VALIDATION_FAILED_EXT"},
				{VK_ERROR_FRAGMENTED_POOL, "VK_ERROR_FRAGMENTED_POOL"},
				{VK_ERROR_UNKNOWN, "VK_ERROR_UNKNOWN"},
				{VK_ERROR_OUT_OF_POOL_MEMORY, "VK_ERROR_OUT_OF_POOL_MEMORY"},
				{VK_ERROR_INVALID_EXTERNAL_HANDLE, "VK_ERROR_INVALID_EXTERNAL_HANDLE"},
				{VK_ERROR_FRAGMENTATION, "VK_ERROR_FRAGMENTATION"},
				{VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS, "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS"},
				{VK_PIPELINE_COMPILE_REQUIRED, "VK_PIPELINE_COMPILE_REQUIRED"},
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
				{VK_ERROR_COMPRESSION_EXHAUSTED_EXT, "VK_ERROR_COMPRESSION_EXHAUSTED_EXT"}
			}
		}
	};

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
	inline constexpr Utils::ConstexprMap<NvLL_VK_Status, std::string_view, 11> NvLLVKStatusToString
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

constexpr bool TRAP::Graphics::API::ErrorCheck(const VkResult result, const std::string_view function,
                                               const std::string_view file, const std::uint_least32_t line,
											   const std::uint_least32_t column)
{
	if(result >= 0)
		return true;

	TP_ERROR(Log::RendererVulkanPrefix, INTERNAL::VkResultToString.at(result).value_or("Unknown error"),
	         ": ", function, " @[", file, ':', line, ':', column, ']');

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
constexpr bool TRAP::Graphics::API::ReflexErrorCheck(const NvLL_VK_Status result, const std::string_view function,
                                                     const std::string_view file, const std::uint_least32_t line,
													 const std::uint_least32_t column)
{
	if(result == NVLL_VK_OK)
		return true;

	TP_ERROR(Log::RendererVulkanReflexPrefix, INTERNAL::NvLLVKStatusToString.at(result).value_or("Unknown error"),
	         ": ", function, " @[", file, ':', line, ':', column, ']');

	return false;
}
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/

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

	default:
		return VK_SAMPLE_COUNT_1_BIT;
	}
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

	default:
		return VK_FILTER_LINEAR;
	}
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

	default:
		return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	}
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
	if ((state & RendererAPI::ResourceState::Present) != RendererAPI::ResourceState::Undefined)
		ret |= VK_ACCESS_MEMORY_READ_BIT;

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

	case VK_FORMAT_A8B8G8R8_UNORM_PACK32: [[fallthrough]];
	case VK_FORMAT_R8_USCALED: [[fallthrough]];
	case VK_FORMAT_R8_SSCALED: [[fallthrough]];
	case VK_FORMAT_R8G8_USCALED: [[fallthrough]];
	case VK_FORMAT_R8G8_SSCALED: [[fallthrough]];
	case VK_FORMAT_R8G8B8_USCALED: [[fallthrough]];
	case VK_FORMAT_R8G8B8_SSCALED: [[fallthrough]];
	case VK_FORMAT_B8G8R8_USCALED: [[fallthrough]];
	case VK_FORMAT_B8G8R8_SSCALED: [[fallthrough]];
	case VK_FORMAT_R8G8B8A8_USCALED: [[fallthrough]];
	case VK_FORMAT_R8G8B8A8_SSCALED: [[fallthrough]];
	case VK_FORMAT_B8G8R8A8_USCALED: [[fallthrough]];
	case VK_FORMAT_B8G8R8A8_SSCALED: [[fallthrough]];
	case VK_FORMAT_A8B8G8R8_SNORM_PACK32: [[fallthrough]];
	case VK_FORMAT_A8B8G8R8_USCALED_PACK32: [[fallthrough]];
	case VK_FORMAT_A8B8G8R8_SSCALED_PACK32: [[fallthrough]];
	case VK_FORMAT_A8B8G8R8_UINT_PACK32: [[fallthrough]];
	case VK_FORMAT_A8B8G8R8_SINT_PACK32: [[fallthrough]];
	case VK_FORMAT_A8B8G8R8_SRGB_PACK32: [[fallthrough]];
	case VK_FORMAT_A2R10G10B10_SNORM_PACK32: [[fallthrough]];
	case VK_FORMAT_A2R10G10B10_USCALED_PACK32: [[fallthrough]];
	case VK_FORMAT_A2R10G10B10_SSCALED_PACK32: [[fallthrough]];
	case VK_FORMAT_A2R10G10B10_SINT_PACK32: [[fallthrough]];
	case VK_FORMAT_A2B10G10R10_SNORM_PACK32: [[fallthrough]];
	case VK_FORMAT_A2B10G10R10_USCALED_PACK32: [[fallthrough]];
	case VK_FORMAT_A2B10G10R10_SSCALED_PACK32: [[fallthrough]];
	case VK_FORMAT_A2B10G10R10_SINT_PACK32: [[fallthrough]];
	case VK_FORMAT_R16_USCALED: [[fallthrough]];
	case VK_FORMAT_R16_SSCALED: [[fallthrough]];
	case VK_FORMAT_R16G16_USCALED: [[fallthrough]];
	case VK_FORMAT_R16G16_SSCALED: [[fallthrough]];
	case VK_FORMAT_R16G16B16_USCALED: [[fallthrough]];
	case VK_FORMAT_R16G16B16_SSCALED: [[fallthrough]];
	case VK_FORMAT_R16G16B16A16_USCALED: [[fallthrough]];
	case VK_FORMAT_R16G16B16A16_SSCALED: [[fallthrough]];
	case VK_FORMAT_R10X6_UNORM_PACK16: [[fallthrough]];
	case VK_FORMAT_R10X6G10X6_UNORM_2PACK16: [[fallthrough]];
	case VK_FORMAT_R12X4_UNORM_PACK16: [[fallthrough]];
	case VK_FORMAT_R12X4G12X4_UNORM_2PACK16: //[[fallthrough]];
	// case VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG: [[fallthrough]];
	// case VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG: [[fallthrough]];
	// case VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG: [[fallthrough]];
	// case VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG:
		return ImageFormat::Undefined;

	default:
		return TRAP::Graphics::API::ImageFormat::Undefined;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

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