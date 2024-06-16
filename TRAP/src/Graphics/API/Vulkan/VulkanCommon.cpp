#include "TRAPPCH.h"
#include "VulkanCommon.h"

#include "Core/Base.h"

#include "VulkanRenderer.h"
#include "Graphics/API/Vulkan/Objects/VulkanDevice.h"
#include "Graphics/API/Vulkan/Objects/VulkanInstance.h"
#include "Graphics/API/Vulkan/Objects/VulkanPhysicalDevice.h"
#include "Objects/VulkanInits.h"

#ifdef ENABLE_GRAPHICS_DEBUG
namespace
{
	[[maybe_unused]] constexpr TRAP::Utils::ConstexprMap<VkObjectType, VkDebugReportObjectTypeEXT, 51> VkObjectTypeToVkDebugReportObjectTypeEXT
	{
		{
			{
				{VK_OBJECT_TYPE_UNKNOWN, VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT},
				{VK_OBJECT_TYPE_INSTANCE, VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT},
				{VK_OBJECT_TYPE_PHYSICAL_DEVICE, VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT},
				{VK_OBJECT_TYPE_DEVICE, VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT},
				{VK_OBJECT_TYPE_QUEUE, VK_DEBUG_REPORT_OBJECT_TYPE_QUEUE_EXT},
				{VK_OBJECT_TYPE_SEMAPHORE, VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT},
				{VK_OBJECT_TYPE_COMMAND_BUFFER, VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT},
				{VK_OBJECT_TYPE_FENCE, VK_DEBUG_REPORT_OBJECT_TYPE_FENCE_EXT},
				{VK_OBJECT_TYPE_DEVICE_MEMORY, VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT},
				{VK_OBJECT_TYPE_BUFFER, VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT},
				{VK_OBJECT_TYPE_IMAGE, VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT},
				{VK_OBJECT_TYPE_EVENT, VK_DEBUG_REPORT_OBJECT_TYPE_EVENT_EXT},
				{VK_OBJECT_TYPE_QUERY_POOL, VK_DEBUG_REPORT_OBJECT_TYPE_QUERY_POOL_EXT},
				{VK_OBJECT_TYPE_BUFFER_VIEW, VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_VIEW_EXT},
				{VK_OBJECT_TYPE_IMAGE_VIEW, VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT},
				{VK_OBJECT_TYPE_SHADER_MODULE, VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT},
				{VK_OBJECT_TYPE_PIPELINE_CACHE, VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_CACHE_EXT},
				{VK_OBJECT_TYPE_PIPELINE_LAYOUT, VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_LAYOUT_EXT},
				{VK_OBJECT_TYPE_RENDER_PASS, VK_DEBUG_REPORT_OBJECT_TYPE_RENDER_PASS_EXT},
				{VK_OBJECT_TYPE_PIPELINE, VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT},
				{VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT},
				{VK_OBJECT_TYPE_SAMPLER, VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT},
				{VK_OBJECT_TYPE_DESCRIPTOR_POOL, VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_POOL_EXT},
				{VK_OBJECT_TYPE_DESCRIPTOR_SET, VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_EXT},
				{VK_OBJECT_TYPE_FRAMEBUFFER, VK_DEBUG_REPORT_OBJECT_TYPE_FRAMEBUFFER_EXT},
				{VK_OBJECT_TYPE_COMMAND_POOL, VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_POOL_EXT},
				{VK_OBJECT_TYPE_SURFACE_KHR, VK_DEBUG_REPORT_OBJECT_TYPE_SURFACE_KHR_EXT},
				{VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE, VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_EXT},
				{VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION, VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION_EXT},
				{VK_OBJECT_TYPE_PRIVATE_DATA_SLOT_EXT, VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT},
				{VK_OBJECT_TYPE_SWAPCHAIN_KHR, VK_DEBUG_REPORT_OBJECT_TYPE_SWAPCHAIN_KHR_EXT},
				{VK_OBJECT_TYPE_DISPLAY_KHR, VK_DEBUG_REPORT_OBJECT_TYPE_DISPLAY_KHR_EXT},
				{VK_OBJECT_TYPE_DISPLAY_MODE_KHR, VK_DEBUG_REPORT_OBJECT_TYPE_DISPLAY_MODE_KHR_EXT},
				{VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT, VK_DEBUG_REPORT_OBJECT_TYPE_DEBUG_REPORT_EXT},
				{VK_OBJECT_TYPE_VIDEO_SESSION_KHR, VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT},
				{VK_OBJECT_TYPE_VIDEO_SESSION_PARAMETERS_KHR, VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT},
				{VK_OBJECT_TYPE_CU_MODULE_NVX, VK_DEBUG_REPORT_OBJECT_TYPE_CU_MODULE_NVX_EXT},
				{VK_OBJECT_TYPE_CU_FUNCTION_NVX, VK_DEBUG_REPORT_OBJECT_TYPE_CU_FUNCTION_NVX_EXT},
				{VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT, VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT},
				{VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR, VK_DEBUG_REPORT_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR_EXT},
				{VK_OBJECT_TYPE_VALIDATION_CACHE_EXT, VK_DEBUG_REPORT_OBJECT_TYPE_VALIDATION_CACHE_EXT},
				{VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_NV, VK_DEBUG_REPORT_OBJECT_TYPE_ACCELERATION_STRUCTURE_NV_EXT},
				{VK_OBJECT_TYPE_PERFORMANCE_CONFIGURATION_INTEL, VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT},
				{VK_OBJECT_TYPE_DEFERRED_OPERATION_KHR, VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT},
				{VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NV, VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT},
				{VK_OBJECT_TYPE_CUDA_MODULE_NV, VK_DEBUG_REPORT_OBJECT_TYPE_CUDA_MODULE_NV_EXT},
				{VK_OBJECT_TYPE_CUDA_FUNCTION_NV, VK_DEBUG_REPORT_OBJECT_TYPE_CUDA_FUNCTION_NV_EXT},
				{VK_OBJECT_TYPE_BUFFER_COLLECTION_FUCHSIA, VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_COLLECTION_FUCHSIA_EXT},
				{VK_OBJECT_TYPE_MICROMAP_EXT, VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT},
				{VK_OBJECT_TYPE_OPTICAL_FLOW_SESSION_NV, VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT},
				{VK_OBJECT_TYPE_SHADER_EXT, VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT},
			}
		}
	};
}
#endif /*ENABLE_GRAPHICS_DEBUG*/

#ifdef ENABLE_GRAPHICS_DEBUG
void TRAP::Graphics::API::VkSetObjectName(const VulkanDevice& device, const u64 handle, const VkObjectType type,
                                          const std::string_view name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	if (VulkanInstance::IsExtensionSupported(VulkanInstanceExtension::DebugUtils))
	{
		const VkDebugUtilsObjectNameInfoEXT nameInfo = VulkanInits::DebugUtilsObjectNameInfo(type, handle, name);
		VkCall(vkSetDebugUtilsObjectNameEXT(device.GetVkDevice(), &nameInfo));
	}
	else if (VulkanInstance::IsExtensionSupported(VulkanInstanceExtension::DebugReport) &&
	         device.GetPhysicalDevice().IsExtensionSupported(VulkanPhysicalDeviceExtension::DebugMarker))
	{
		const VkDebugMarkerObjectNameInfoEXT nameInfo = VulkanInits::DebugMarkerObjectNameInfo(VkObjectTypeToVkDebugReportObjectTypeEXT[type], handle, name);
		VkCall(vkDebugMarkerSetObjectNameEXT(device.GetVkDevice(), &nameInfo));
	}
}
#endif /*ENABLE_GRAPHICS_DEBUG*/

//-------------------------------------------------------------------------------------------------------------------//

//Determines pipeline stages involved for given accesses
[[nodiscard]] VkPipelineStageFlags TRAP::Graphics::API::DetermineVkPipelineStageFlags(const VulkanPhysicalDevice& physicalDevice,
                                                                                      const VkAccessFlags accessFlags,
                                                                                      const RendererAPI::QueueType queueType) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	VkPipelineStageFlags flags = 0;

	switch(queueType)
	{
	case RendererAPI::QueueType::Graphics:
	{
		if ((accessFlags & (VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT)) != 0)
			flags |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;

		if((accessFlags & (VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT)) != 0)
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
			//RayTracing
			if (physicalDevice.IsExtensionSupported(VulkanPhysicalDeviceExtension::RayTracing))
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

		if ((accessFlags & (VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT)) != 0)
			flags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

		break;
	}

	case RendererAPI::QueueType::Transfer:
		return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
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

void TRAP::Graphics::API::UtilGetPlanarVkImageMemoryRequirement(VkDevice device, VkImage image,
																const u32 planesCount,
                                                                VkMemoryRequirements& memReq,
                                                                std::vector<u64>& planesOffsets)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	memReq = {};

	VkImagePlaneMemoryRequirementsInfo imagePlaneMemReqInfo
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_PLANE_MEMORY_REQUIREMENTS_INFO,
		.pNext = nullptr,
		.planeAspect = VK_IMAGE_ASPECT_NONE
	};

	const VkImageMemoryRequirementsInfo2 imagePlaneMemReqInfo2
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2,
		.pNext = &imagePlaneMemReqInfo,
		.image = image
	};

	VkMemoryDedicatedRequirements memDedicatedReq
	{
		.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS,
		.pNext = nullptr,
		.prefersDedicatedAllocation = VK_FALSE,
		.requiresDedicatedAllocation = VK_FALSE
	};

	VkMemoryRequirements2 memReq2
	{
		.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2,
		.pNext = &memDedicatedReq,
		.memoryRequirements = {}
	};

	planesOffsets.resize(planesCount);
	for(u32 i = 0; i < planesCount; ++i)
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
