/*
volk

Copyright (C) 2018-2023, by Arseny Kapoulkine (arseny.kapoulkine@gmail.com)
Report bugs and download new versions at https://github.com/zeux/volk

This library is distributed under the MIT License. See notice at the end of this file.

Modified by: Jan "GamesTrap" Schuerkamp
*/
#ifndef TRAP_VULKANLOADER_H
#define TRAP_VULKANLOADER_H

#include <optional>

#include "Core/Types.h"

#if defined(VULKAN_H_) && !defined(VK_NO_PROTOTYPES)
    #error To use VulkanLoader, you need to define VK_NO_PROTOTYPES before including vulkan.h
#endif /*VULKAN_H_ && !VK_NO_PROTOTYPES*/

/* VULKANLOADER_GENERATE_VERSION_DEFINE */
#define VULKANLOADER_HEADER_VERSION 277
/* VULKANLOADER_GENERATE_VERSION_DEFINE */

#ifndef VK_NO_PROTOTYPES
    #define VK_NO_PROTOTYPES
#endif /*VK_NO_PROTOTYPES*/

#ifndef VULKAN_H_
    #ifdef VK_USE_PLATFORM_WIN32_KHR
        #include <vulkan/vk_platform.h>
        #include <vulkan/vulkan_core.h>

        /*
		When VK_USE_PLATFORM_WIN32_KHR is defined, instead of including vulkan.h directly, we include individual parts of the SDK
		This is necessary to avoid including <windows.h> which is very heavy - it takes 200ms to parse without WIN32_LEAN_AND_MEAN
		and 100ms to parse with it. vulkan_win32.h only needs a few symbols that are easy to redefine ourselves.
		*/
		using DWORD = unsigned long;
		using LPCWSTR = const wchar_t*;
		using HANDLE = void*;
		using HINSTANCE = struct HINSTANCE__*;
		using HWND = struct HWND__*;
		using HMONITOR = struct HMONITOR__*;
		using SECURITY_ATTRIBUTES = struct _SECURITY_ATTRIBUTES;

        #include <vulkan/vulkan_win32.h>

        #ifdef VK_ENABLE_BETA_EXTENSIONS
            #include <vulkan/vulkan_beta.h>
        #endif /*VK_ENABLE_BETA_EXTENSIONS*/
    #else
        #include <vulkan/vulkan.h>
    #endif
#endif /*VULKAN_H_*/

//Disable VK_NVX_image_view_handle because SDK 140 introduced a change that can't be used with prior versions */
#if VK_HEADER_VERSION < 140
    #undef VK_NVX_image_view_handle
#endif /*VK_HEADER_VERSION < 140*/

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif /*_MSC_VER*/

struct VkDeviceTable;

/// @brief Initialize library by loading Vulkan loader; call this function before creating the Vulkan instance.
/// @return VK_SUCCESS on success and VK_ERROR_INITIALIZATION_FAILED otherwise.
[[nodiscard]] VkResult VkInitialize();

/// @brief Initialize library by providing a custom handler to load global symbols.
///        This function can be used instead of VkInitialize.
///        The handler function pointer will be asked to load global Vulkan symbols which require no instance
///        (such as vkCreateInstance, vkEnumerateInstance*and vkEnumerateInstanceVersion if available).
/// @param handler Function pointer for custom loading.
void VkInitializeCustom(PFN_vkGetInstanceProcAddr handler);

/// @brief Finalize library by unloading Vulkan loader and resetting global symbols to NULL.
void VkFinalize();

/// @brief Get Vulkan instance version supported by the Vulkan loader, or an empty optional if Vulkan isn't supported.
/// @return Empty optional if VkInitialize wasn't called or failed.
[[nodiscard]] std::optional<u32> VkGetInstanceVersion();

/// @brief Load global function pointers using application-created VkInstance; call this function after creating the Vulkan instance.
/// @param instance VkInstance to load functions with.
void VkLoadInstance(VkInstance instance);

/// @brief Load global function pointers using application-created VkInstance; call this function after creating the Vulkan instance.
///        Skips loading device-based function pointers, requires usage of VkLoadDevice afterwards.
/// @param instance VkInstance to load functions with.
void VkLoadInstanceOnly(VkInstance instance);

/// @brief Load global function pointers using application-created VkDevice; call this function after creating the Vulkan device.
/// @param device VkDevice to load functions with.
/// @note This is not suitable for applications that want to use multiple VkDevice objects concurrently.
void VkLoadDevice(VkDevice device);

/// @brief Retrieve the last VkInstance for which global function pointers have been loaded via VkLoadInstance().
/// @return VkInstance, or VK_NULL_HANDLE if VkLoadInstance() has not been called.
[[nodiscard]] VkInstance VkGetLoadedInstance() noexcept;

/// @brief Retrieve the last VkDevice for which global function pointers have been loaded via VkLoadDevice().
/// @return VkDevice, or VK_NULL_HANDLE if VkLoadDevice() has not been called.
[[nodiscard]] VkDevice VkGetLoadedDevice() noexcept;

/// @brief Load function pointers using application-created VkDevice into a table.
///        Application should use function pointers from that table instead of using global function pointers.
/// @param table Table where to store loaded functions into.
/// @param device Device from which function pointers should be loaded.
void VkLoadDeviceTable(VkDeviceTable& table, VkDevice device);

//Device-specific function pointer table
struct VkDeviceTable
{
	/* VULKANLOADER_GENERATE_DEVICE_TABLE */
#if defined(VK_VERSION_1_0)
	PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers;
	PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets;
	PFN_vkAllocateMemory vkAllocateMemory;
	PFN_vkBeginCommandBuffer vkBeginCommandBuffer;
	PFN_vkBindBufferMemory vkBindBufferMemory;
	PFN_vkBindImageMemory vkBindImageMemory;
	PFN_vkCmdBeginQuery vkCmdBeginQuery;
	PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass;
	PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets;
	PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer;
	PFN_vkCmdBindPipeline vkCmdBindPipeline;
	PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers;
	PFN_vkCmdBlitImage vkCmdBlitImage;
	PFN_vkCmdClearAttachments vkCmdClearAttachments;
	PFN_vkCmdClearColorImage vkCmdClearColorImage;
	PFN_vkCmdClearDepthStencilImage vkCmdClearDepthStencilImage;
	PFN_vkCmdCopyBuffer vkCmdCopyBuffer;
	PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage;
	PFN_vkCmdCopyImage vkCmdCopyImage;
	PFN_vkCmdCopyImageToBuffer vkCmdCopyImageToBuffer;
	PFN_vkCmdCopyQueryPoolResults vkCmdCopyQueryPoolResults;
	PFN_vkCmdDispatch vkCmdDispatch;
	PFN_vkCmdDispatchIndirect vkCmdDispatchIndirect;
	PFN_vkCmdDraw vkCmdDraw;
	PFN_vkCmdDrawIndexed vkCmdDrawIndexed;
	PFN_vkCmdDrawIndexedIndirect vkCmdDrawIndexedIndirect;
	PFN_vkCmdDrawIndirect vkCmdDrawIndirect;
	PFN_vkCmdEndQuery vkCmdEndQuery;
	PFN_vkCmdEndRenderPass vkCmdEndRenderPass;
	PFN_vkCmdExecuteCommands vkCmdExecuteCommands;
	PFN_vkCmdFillBuffer vkCmdFillBuffer;
	PFN_vkCmdNextSubpass vkCmdNextSubpass;
	PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier;
	PFN_vkCmdPushConstants vkCmdPushConstants;
	PFN_vkCmdResetEvent vkCmdResetEvent;
	PFN_vkCmdResetQueryPool vkCmdResetQueryPool;
	PFN_vkCmdResolveImage vkCmdResolveImage;
	PFN_vkCmdSetBlendConstants vkCmdSetBlendConstants;
	PFN_vkCmdSetDepthBias vkCmdSetDepthBias;
	PFN_vkCmdSetDepthBounds vkCmdSetDepthBounds;
	PFN_vkCmdSetEvent vkCmdSetEvent;
	PFN_vkCmdSetLineWidth vkCmdSetLineWidth;
	PFN_vkCmdSetScissor vkCmdSetScissor;
	PFN_vkCmdSetStencilCompareMask vkCmdSetStencilCompareMask;
	PFN_vkCmdSetStencilReference vkCmdSetStencilReference;
	PFN_vkCmdSetStencilWriteMask vkCmdSetStencilWriteMask;
	PFN_vkCmdSetViewport vkCmdSetViewport;
	PFN_vkCmdUpdateBuffer vkCmdUpdateBuffer;
	PFN_vkCmdWaitEvents vkCmdWaitEvents;
	PFN_vkCmdWriteTimestamp vkCmdWriteTimestamp;
	PFN_vkCreateBuffer vkCreateBuffer;
	PFN_vkCreateBufferView vkCreateBufferView;
	PFN_vkCreateCommandPool vkCreateCommandPool;
	PFN_vkCreateComputePipelines vkCreateComputePipelines;
	PFN_vkCreateDescriptorPool vkCreateDescriptorPool;
	PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout;
	PFN_vkCreateEvent vkCreateEvent;
	PFN_vkCreateFence vkCreateFence;
	PFN_vkCreateFramebuffer vkCreateFramebuffer;
	PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines;
	PFN_vkCreateImage vkCreateImage;
	PFN_vkCreateImageView vkCreateImageView;
	PFN_vkCreatePipelineCache vkCreatePipelineCache;
	PFN_vkCreatePipelineLayout vkCreatePipelineLayout;
	PFN_vkCreateQueryPool vkCreateQueryPool;
	PFN_vkCreateRenderPass vkCreateRenderPass;
	PFN_vkCreateSampler vkCreateSampler;
	PFN_vkCreateSemaphore vkCreateSemaphore;
	PFN_vkCreateShaderModule vkCreateShaderModule;
	PFN_vkDestroyBuffer vkDestroyBuffer;
	PFN_vkDestroyBufferView vkDestroyBufferView;
	PFN_vkDestroyCommandPool vkDestroyCommandPool;
	PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool;
	PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout;
	PFN_vkDestroyDevice vkDestroyDevice;
	PFN_vkDestroyEvent vkDestroyEvent;
	PFN_vkDestroyFence vkDestroyFence;
	PFN_vkDestroyFramebuffer vkDestroyFramebuffer;
	PFN_vkDestroyImage vkDestroyImage;
	PFN_vkDestroyImageView vkDestroyImageView;
	PFN_vkDestroyPipeline vkDestroyPipeline;
	PFN_vkDestroyPipelineCache vkDestroyPipelineCache;
	PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout;
	PFN_vkDestroyQueryPool vkDestroyQueryPool;
	PFN_vkDestroyRenderPass vkDestroyRenderPass;
	PFN_vkDestroySampler vkDestroySampler;
	PFN_vkDestroySemaphore vkDestroySemaphore;
	PFN_vkDestroyShaderModule vkDestroyShaderModule;
	PFN_vkDeviceWaitIdle vkDeviceWaitIdle;
	PFN_vkEndCommandBuffer vkEndCommandBuffer;
	PFN_vkFlushMappedMemoryRanges vkFlushMappedMemoryRanges;
	PFN_vkFreeCommandBuffers vkFreeCommandBuffers;
	PFN_vkFreeDescriptorSets vkFreeDescriptorSets;
	PFN_vkFreeMemory vkFreeMemory;
	PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
	PFN_vkGetDeviceMemoryCommitment vkGetDeviceMemoryCommitment;
	PFN_vkGetDeviceQueue vkGetDeviceQueue;
	PFN_vkGetEventStatus vkGetEventStatus;
	PFN_vkGetFenceStatus vkGetFenceStatus;
	PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements;
	PFN_vkGetImageSparseMemoryRequirements vkGetImageSparseMemoryRequirements;
	PFN_vkGetImageSubresourceLayout vkGetImageSubresourceLayout;
	PFN_vkGetPipelineCacheData vkGetPipelineCacheData;
	PFN_vkGetQueryPoolResults vkGetQueryPoolResults;
	PFN_vkGetRenderAreaGranularity vkGetRenderAreaGranularity;
	PFN_vkInvalidateMappedMemoryRanges vkInvalidateMappedMemoryRanges;
	PFN_vkMapMemory vkMapMemory;
	PFN_vkMergePipelineCaches vkMergePipelineCaches;
	PFN_vkQueueBindSparse vkQueueBindSparse;
	PFN_vkQueueSubmit vkQueueSubmit;
	PFN_vkQueueWaitIdle vkQueueWaitIdle;
	PFN_vkResetCommandBuffer vkResetCommandBuffer;
	PFN_vkResetCommandPool vkResetCommandPool;
	PFN_vkResetDescriptorPool vkResetDescriptorPool;
	PFN_vkResetEvent vkResetEvent;
	PFN_vkResetFences vkResetFences;
	PFN_vkSetEvent vkSetEvent;
	PFN_vkUnmapMemory vkUnmapMemory;
	PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets;
	PFN_vkWaitForFences vkWaitForFences;
#endif /* defined(VK_VERSION_1_0) */
#if defined(VK_VERSION_1_1)
	PFN_vkBindBufferMemory2 vkBindBufferMemory2;
	PFN_vkBindImageMemory2 vkBindImageMemory2;
	PFN_vkCmdDispatchBase vkCmdDispatchBase;
	PFN_vkCmdSetDeviceMask vkCmdSetDeviceMask;
	PFN_vkCreateDescriptorUpdateTemplate vkCreateDescriptorUpdateTemplate;
	PFN_vkCreateSamplerYcbcrConversion vkCreateSamplerYcbcrConversion;
	PFN_vkDestroyDescriptorUpdateTemplate vkDestroyDescriptorUpdateTemplate;
	PFN_vkDestroySamplerYcbcrConversion vkDestroySamplerYcbcrConversion;
	PFN_vkGetBufferMemoryRequirements2 vkGetBufferMemoryRequirements2;
	PFN_vkGetDescriptorSetLayoutSupport vkGetDescriptorSetLayoutSupport;
	PFN_vkGetDeviceGroupPeerMemoryFeatures vkGetDeviceGroupPeerMemoryFeatures;
	PFN_vkGetDeviceQueue2 vkGetDeviceQueue2;
	PFN_vkGetImageMemoryRequirements2 vkGetImageMemoryRequirements2;
	PFN_vkGetImageSparseMemoryRequirements2 vkGetImageSparseMemoryRequirements2;
	PFN_vkTrimCommandPool vkTrimCommandPool;
	PFN_vkUpdateDescriptorSetWithTemplate vkUpdateDescriptorSetWithTemplate;
#endif /* defined(VK_VERSION_1_1) */
#if defined(VK_VERSION_1_2)
	PFN_vkCmdBeginRenderPass2 vkCmdBeginRenderPass2;
	PFN_vkCmdDrawIndexedIndirectCount vkCmdDrawIndexedIndirectCount;
	PFN_vkCmdDrawIndirectCount vkCmdDrawIndirectCount;
	PFN_vkCmdEndRenderPass2 vkCmdEndRenderPass2;
	PFN_vkCmdNextSubpass2 vkCmdNextSubpass2;
	PFN_vkCreateRenderPass2 vkCreateRenderPass2;
	PFN_vkGetBufferDeviceAddress vkGetBufferDeviceAddress;
	PFN_vkGetBufferOpaqueCaptureAddress vkGetBufferOpaqueCaptureAddress;
	PFN_vkGetDeviceMemoryOpaqueCaptureAddress vkGetDeviceMemoryOpaqueCaptureAddress;
	PFN_vkGetSemaphoreCounterValue vkGetSemaphoreCounterValue;
	PFN_vkResetQueryPool vkResetQueryPool;
	PFN_vkSignalSemaphore vkSignalSemaphore;
	PFN_vkWaitSemaphores vkWaitSemaphores;
#endif /* defined(VK_VERSION_1_2) */
#if defined(VK_VERSION_1_3)
	PFN_vkCmdBeginRendering vkCmdBeginRendering;
	PFN_vkCmdBindVertexBuffers2 vkCmdBindVertexBuffers2;
	PFN_vkCmdBlitImage2 vkCmdBlitImage2;
	PFN_vkCmdCopyBuffer2 vkCmdCopyBuffer2;
	PFN_vkCmdCopyBufferToImage2 vkCmdCopyBufferToImage2;
	PFN_vkCmdCopyImage2 vkCmdCopyImage2;
	PFN_vkCmdCopyImageToBuffer2 vkCmdCopyImageToBuffer2;
	PFN_vkCmdEndRendering vkCmdEndRendering;
	PFN_vkCmdPipelineBarrier2 vkCmdPipelineBarrier2;
	PFN_vkCmdResetEvent2 vkCmdResetEvent2;
	PFN_vkCmdResolveImage2 vkCmdResolveImage2;
	PFN_vkCmdSetCullMode vkCmdSetCullMode;
	PFN_vkCmdSetDepthBiasEnable vkCmdSetDepthBiasEnable;
	PFN_vkCmdSetDepthBoundsTestEnable vkCmdSetDepthBoundsTestEnable;
	PFN_vkCmdSetDepthCompareOp vkCmdSetDepthCompareOp;
	PFN_vkCmdSetDepthTestEnable vkCmdSetDepthTestEnable;
	PFN_vkCmdSetDepthWriteEnable vkCmdSetDepthWriteEnable;
	PFN_vkCmdSetEvent2 vkCmdSetEvent2;
	PFN_vkCmdSetFrontFace vkCmdSetFrontFace;
	PFN_vkCmdSetPrimitiveRestartEnable vkCmdSetPrimitiveRestartEnable;
	PFN_vkCmdSetPrimitiveTopology vkCmdSetPrimitiveTopology;
	PFN_vkCmdSetRasterizerDiscardEnable vkCmdSetRasterizerDiscardEnable;
	PFN_vkCmdSetScissorWithCount vkCmdSetScissorWithCount;
	PFN_vkCmdSetStencilOp vkCmdSetStencilOp;
	PFN_vkCmdSetStencilTestEnable vkCmdSetStencilTestEnable;
	PFN_vkCmdSetViewportWithCount vkCmdSetViewportWithCount;
	PFN_vkCmdWaitEvents2 vkCmdWaitEvents2;
	PFN_vkCmdWriteTimestamp2 vkCmdWriteTimestamp2;
	PFN_vkCreatePrivateDataSlot vkCreatePrivateDataSlot;
	PFN_vkDestroyPrivateDataSlot vkDestroyPrivateDataSlot;
	PFN_vkGetDeviceBufferMemoryRequirements vkGetDeviceBufferMemoryRequirements;
	PFN_vkGetDeviceImageMemoryRequirements vkGetDeviceImageMemoryRequirements;
	PFN_vkGetDeviceImageSparseMemoryRequirements vkGetDeviceImageSparseMemoryRequirements;
	PFN_vkGetPrivateData vkGetPrivateData;
	PFN_vkQueueSubmit2 vkQueueSubmit2;
	PFN_vkSetPrivateData vkSetPrivateData;
#endif /* defined(VK_VERSION_1_3) */
#if defined(VK_AMDX_shader_enqueue)
	PFN_vkCmdDispatchGraphAMDX vkCmdDispatchGraphAMDX;
	PFN_vkCmdDispatchGraphIndirectAMDX vkCmdDispatchGraphIndirectAMDX;
	PFN_vkCmdDispatchGraphIndirectCountAMDX vkCmdDispatchGraphIndirectCountAMDX;
	PFN_vkCmdInitializeGraphScratchMemoryAMDX vkCmdInitializeGraphScratchMemoryAMDX;
	PFN_vkCreateExecutionGraphPipelinesAMDX vkCreateExecutionGraphPipelinesAMDX;
	PFN_vkGetExecutionGraphPipelineNodeIndexAMDX vkGetExecutionGraphPipelineNodeIndexAMDX;
	PFN_vkGetExecutionGraphPipelineScratchSizeAMDX vkGetExecutionGraphPipelineScratchSizeAMDX;
#endif /* defined(VK_AMDX_shader_enqueue) */
#if defined(VK_AMD_buffer_marker)
	PFN_vkCmdWriteBufferMarkerAMD vkCmdWriteBufferMarkerAMD;
#endif /* defined(VK_AMD_buffer_marker) */
#if defined(VK_AMD_display_native_hdr)
	PFN_vkSetLocalDimmingAMD vkSetLocalDimmingAMD;
#endif /* defined(VK_AMD_display_native_hdr) */
#if defined(VK_AMD_draw_indirect_count)
	PFN_vkCmdDrawIndexedIndirectCountAMD vkCmdDrawIndexedIndirectCountAMD;
	PFN_vkCmdDrawIndirectCountAMD vkCmdDrawIndirectCountAMD;
#endif /* defined(VK_AMD_draw_indirect_count) */
#if defined(VK_AMD_shader_info)
	PFN_vkGetShaderInfoAMD vkGetShaderInfoAMD;
#endif /* defined(VK_AMD_shader_info) */
#if defined(VK_ANDROID_external_memory_android_hardware_buffer)
	PFN_vkGetAndroidHardwareBufferPropertiesANDROID vkGetAndroidHardwareBufferPropertiesANDROID;
	PFN_vkGetMemoryAndroidHardwareBufferANDROID vkGetMemoryAndroidHardwareBufferANDROID;
#endif /* defined(VK_ANDROID_external_memory_android_hardware_buffer) */
#if defined(VK_EXT_attachment_feedback_loop_dynamic_state)
	PFN_vkCmdSetAttachmentFeedbackLoopEnableEXT vkCmdSetAttachmentFeedbackLoopEnableEXT;
#endif /* defined(VK_EXT_attachment_feedback_loop_dynamic_state) */
#if defined(VK_EXT_buffer_device_address)
	PFN_vkGetBufferDeviceAddressEXT vkGetBufferDeviceAddressEXT;
#endif /* defined(VK_EXT_buffer_device_address) */
#if defined(VK_EXT_calibrated_timestamps)
	PFN_vkGetCalibratedTimestampsEXT vkGetCalibratedTimestampsEXT;
#endif /* defined(VK_EXT_calibrated_timestamps) */
#if defined(VK_EXT_color_write_enable)
	PFN_vkCmdSetColorWriteEnableEXT vkCmdSetColorWriteEnableEXT;
#endif /* defined(VK_EXT_color_write_enable) */
#if defined(VK_EXT_conditional_rendering)
	PFN_vkCmdBeginConditionalRenderingEXT vkCmdBeginConditionalRenderingEXT;
	PFN_vkCmdEndConditionalRenderingEXT vkCmdEndConditionalRenderingEXT;
#endif /* defined(VK_EXT_conditional_rendering) */
#if defined(VK_EXT_debug_marker)
	PFN_vkCmdDebugMarkerBeginEXT vkCmdDebugMarkerBeginEXT;
	PFN_vkCmdDebugMarkerEndEXT vkCmdDebugMarkerEndEXT;
	PFN_vkCmdDebugMarkerInsertEXT vkCmdDebugMarkerInsertEXT;
	PFN_vkDebugMarkerSetObjectNameEXT vkDebugMarkerSetObjectNameEXT;
	PFN_vkDebugMarkerSetObjectTagEXT vkDebugMarkerSetObjectTagEXT;
#endif /* defined(VK_EXT_debug_marker) */
#if defined(VK_EXT_depth_bias_control)
	PFN_vkCmdSetDepthBias2EXT vkCmdSetDepthBias2EXT;
#endif /* defined(VK_EXT_depth_bias_control) */
#if defined(VK_EXT_descriptor_buffer)
	PFN_vkCmdBindDescriptorBufferEmbeddedSamplersEXT vkCmdBindDescriptorBufferEmbeddedSamplersEXT;
	PFN_vkCmdBindDescriptorBuffersEXT vkCmdBindDescriptorBuffersEXT;
	PFN_vkCmdSetDescriptorBufferOffsetsEXT vkCmdSetDescriptorBufferOffsetsEXT;
	PFN_vkGetBufferOpaqueCaptureDescriptorDataEXT vkGetBufferOpaqueCaptureDescriptorDataEXT;
	PFN_vkGetDescriptorEXT vkGetDescriptorEXT;
	PFN_vkGetDescriptorSetLayoutBindingOffsetEXT vkGetDescriptorSetLayoutBindingOffsetEXT;
	PFN_vkGetDescriptorSetLayoutSizeEXT vkGetDescriptorSetLayoutSizeEXT;
	PFN_vkGetImageOpaqueCaptureDescriptorDataEXT vkGetImageOpaqueCaptureDescriptorDataEXT;
	PFN_vkGetImageViewOpaqueCaptureDescriptorDataEXT vkGetImageViewOpaqueCaptureDescriptorDataEXT;
	PFN_vkGetSamplerOpaqueCaptureDescriptorDataEXT vkGetSamplerOpaqueCaptureDescriptorDataEXT;
#endif /* defined(VK_EXT_descriptor_buffer) */
#if defined(VK_EXT_descriptor_buffer) && (defined(VK_KHR_acceleration_structure) || defined(VK_NV_ray_tracing))
	PFN_vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT;
#endif /* defined(VK_EXT_descriptor_buffer) && (defined(VK_KHR_acceleration_structure) || defined(VK_NV_ray_tracing)) */
#if defined(VK_EXT_device_fault)
	PFN_vkGetDeviceFaultInfoEXT vkGetDeviceFaultInfoEXT;
#endif /* defined(VK_EXT_device_fault) */
#if defined(VK_EXT_discard_rectangles)
	PFN_vkCmdSetDiscardRectangleEXT vkCmdSetDiscardRectangleEXT;
#endif /* defined(VK_EXT_discard_rectangles) */
#if defined(VK_EXT_discard_rectangles) && VK_EXT_DISCARD_RECTANGLES_SPEC_VERSION >= 2
	PFN_vkCmdSetDiscardRectangleEnableEXT vkCmdSetDiscardRectangleEnableEXT;
	PFN_vkCmdSetDiscardRectangleModeEXT vkCmdSetDiscardRectangleModeEXT;
#endif /* defined(VK_EXT_discard_rectangles) && VK_EXT_DISCARD_RECTANGLES_SPEC_VERSION >= 2 */
#if defined(VK_EXT_display_control)
	PFN_vkDisplayPowerControlEXT vkDisplayPowerControlEXT;
	PFN_vkGetSwapchainCounterEXT vkGetSwapchainCounterEXT;
	PFN_vkRegisterDeviceEventEXT vkRegisterDeviceEventEXT;
	PFN_vkRegisterDisplayEventEXT vkRegisterDisplayEventEXT;
#endif /* defined(VK_EXT_display_control) */
#if defined(VK_EXT_external_memory_host)
	PFN_vkGetMemoryHostPointerPropertiesEXT vkGetMemoryHostPointerPropertiesEXT;
#endif /* defined(VK_EXT_external_memory_host) */
#if defined(VK_EXT_full_screen_exclusive)
	PFN_vkAcquireFullScreenExclusiveModeEXT vkAcquireFullScreenExclusiveModeEXT;
	PFN_vkReleaseFullScreenExclusiveModeEXT vkReleaseFullScreenExclusiveModeEXT;
#endif /* defined(VK_EXT_full_screen_exclusive) */
#if defined(VK_EXT_hdr_metadata)
	PFN_vkSetHdrMetadataEXT vkSetHdrMetadataEXT;
#endif /* defined(VK_EXT_hdr_metadata) */
#if defined(VK_EXT_host_image_copy)
	PFN_vkCopyImageToImageEXT vkCopyImageToImageEXT;
	PFN_vkCopyImageToMemoryEXT vkCopyImageToMemoryEXT;
	PFN_vkCopyMemoryToImageEXT vkCopyMemoryToImageEXT;
	PFN_vkTransitionImageLayoutEXT vkTransitionImageLayoutEXT;
#endif /* defined(VK_EXT_host_image_copy) */
#if defined(VK_EXT_host_query_reset)
	PFN_vkResetQueryPoolEXT vkResetQueryPoolEXT;
#endif /* defined(VK_EXT_host_query_reset) */
#if defined(VK_EXT_image_drm_format_modifier)
	PFN_vkGetImageDrmFormatModifierPropertiesEXT vkGetImageDrmFormatModifierPropertiesEXT;
#endif /* defined(VK_EXT_image_drm_format_modifier) */
#if defined(VK_EXT_line_rasterization)
	PFN_vkCmdSetLineStippleEXT vkCmdSetLineStippleEXT;
#endif /* defined(VK_EXT_line_rasterization) */
#if defined(VK_EXT_mesh_shader)
	PFN_vkCmdDrawMeshTasksEXT vkCmdDrawMeshTasksEXT;
	PFN_vkCmdDrawMeshTasksIndirectCountEXT vkCmdDrawMeshTasksIndirectCountEXT;
	PFN_vkCmdDrawMeshTasksIndirectEXT vkCmdDrawMeshTasksIndirectEXT;
#endif /* defined(VK_EXT_mesh_shader) */
#if defined(VK_EXT_metal_objects)
	PFN_vkExportMetalObjectsEXT vkExportMetalObjectsEXT;
#endif /* defined(VK_EXT_metal_objects) */
#if defined(VK_EXT_multi_draw)
	PFN_vkCmdDrawMultiEXT vkCmdDrawMultiEXT;
	PFN_vkCmdDrawMultiIndexedEXT vkCmdDrawMultiIndexedEXT;
#endif /* defined(VK_EXT_multi_draw) */
#if defined(VK_EXT_opacity_micromap)
	PFN_vkBuildMicromapsEXT vkBuildMicromapsEXT;
	PFN_vkCmdBuildMicromapsEXT vkCmdBuildMicromapsEXT;
	PFN_vkCmdCopyMemoryToMicromapEXT vkCmdCopyMemoryToMicromapEXT;
	PFN_vkCmdCopyMicromapEXT vkCmdCopyMicromapEXT;
	PFN_vkCmdCopyMicromapToMemoryEXT vkCmdCopyMicromapToMemoryEXT;
	PFN_vkCmdWriteMicromapsPropertiesEXT vkCmdWriteMicromapsPropertiesEXT;
	PFN_vkCopyMemoryToMicromapEXT vkCopyMemoryToMicromapEXT;
	PFN_vkCopyMicromapEXT vkCopyMicromapEXT;
	PFN_vkCopyMicromapToMemoryEXT vkCopyMicromapToMemoryEXT;
	PFN_vkCreateMicromapEXT vkCreateMicromapEXT;
	PFN_vkDestroyMicromapEXT vkDestroyMicromapEXT;
	PFN_vkGetDeviceMicromapCompatibilityEXT vkGetDeviceMicromapCompatibilityEXT;
	PFN_vkGetMicromapBuildSizesEXT vkGetMicromapBuildSizesEXT;
	PFN_vkWriteMicromapsPropertiesEXT vkWriteMicromapsPropertiesEXT;
#endif /* defined(VK_EXT_opacity_micromap) */
#if defined(VK_EXT_pageable_device_local_memory)
	PFN_vkSetDeviceMemoryPriorityEXT vkSetDeviceMemoryPriorityEXT;
#endif /* defined(VK_EXT_pageable_device_local_memory) */
#if defined(VK_EXT_pipeline_properties)
	PFN_vkGetPipelinePropertiesEXT vkGetPipelinePropertiesEXT;
#endif /* defined(VK_EXT_pipeline_properties) */
#if defined(VK_EXT_private_data)
	PFN_vkCreatePrivateDataSlotEXT vkCreatePrivateDataSlotEXT;
	PFN_vkDestroyPrivateDataSlotEXT vkDestroyPrivateDataSlotEXT;
	PFN_vkGetPrivateDataEXT vkGetPrivateDataEXT;
	PFN_vkSetPrivateDataEXT vkSetPrivateDataEXT;
#endif /* defined(VK_EXT_private_data) */
#if defined(VK_EXT_sample_locations)
	PFN_vkCmdSetSampleLocationsEXT vkCmdSetSampleLocationsEXT;
#endif /* defined(VK_EXT_sample_locations) */
#if defined(VK_EXT_shader_module_identifier)
	PFN_vkGetShaderModuleCreateInfoIdentifierEXT vkGetShaderModuleCreateInfoIdentifierEXT;
	PFN_vkGetShaderModuleIdentifierEXT vkGetShaderModuleIdentifierEXT;
#endif /* defined(VK_EXT_shader_module_identifier) */
#if defined(VK_EXT_shader_object)
	PFN_vkCmdBindShadersEXT vkCmdBindShadersEXT;
	PFN_vkCreateShadersEXT vkCreateShadersEXT;
	PFN_vkDestroyShaderEXT vkDestroyShaderEXT;
	PFN_vkGetShaderBinaryDataEXT vkGetShaderBinaryDataEXT;
#endif /* defined(VK_EXT_shader_object) */
#if defined(VK_EXT_swapchain_maintenance1)
	PFN_vkReleaseSwapchainImagesEXT vkReleaseSwapchainImagesEXT;
#endif /* defined(VK_EXT_swapchain_maintenance1) */
#if defined(VK_EXT_transform_feedback)
	PFN_vkCmdBeginQueryIndexedEXT vkCmdBeginQueryIndexedEXT;
	PFN_vkCmdBeginTransformFeedbackEXT vkCmdBeginTransformFeedbackEXT;
	PFN_vkCmdBindTransformFeedbackBuffersEXT vkCmdBindTransformFeedbackBuffersEXT;
	PFN_vkCmdDrawIndirectByteCountEXT vkCmdDrawIndirectByteCountEXT;
	PFN_vkCmdEndQueryIndexedEXT vkCmdEndQueryIndexedEXT;
	PFN_vkCmdEndTransformFeedbackEXT vkCmdEndTransformFeedbackEXT;
#endif /* defined(VK_EXT_transform_feedback) */
#if defined(VK_EXT_validation_cache)
	PFN_vkCreateValidationCacheEXT vkCreateValidationCacheEXT;
	PFN_vkDestroyValidationCacheEXT vkDestroyValidationCacheEXT;
	PFN_vkGetValidationCacheDataEXT vkGetValidationCacheDataEXT;
	PFN_vkMergeValidationCachesEXT vkMergeValidationCachesEXT;
#endif /* defined(VK_EXT_validation_cache) */
#if defined(VK_FUCHSIA_buffer_collection)
	PFN_vkCreateBufferCollectionFUCHSIA vkCreateBufferCollectionFUCHSIA;
	PFN_vkDestroyBufferCollectionFUCHSIA vkDestroyBufferCollectionFUCHSIA;
	PFN_vkGetBufferCollectionPropertiesFUCHSIA vkGetBufferCollectionPropertiesFUCHSIA;
	PFN_vkSetBufferCollectionBufferConstraintsFUCHSIA vkSetBufferCollectionBufferConstraintsFUCHSIA;
	PFN_vkSetBufferCollectionImageConstraintsFUCHSIA vkSetBufferCollectionImageConstraintsFUCHSIA;
#endif /* defined(VK_FUCHSIA_buffer_collection) */
#if defined(VK_FUCHSIA_external_memory)
	PFN_vkGetMemoryZirconHandleFUCHSIA vkGetMemoryZirconHandleFUCHSIA;
	PFN_vkGetMemoryZirconHandlePropertiesFUCHSIA vkGetMemoryZirconHandlePropertiesFUCHSIA;
#endif /* defined(VK_FUCHSIA_external_memory) */
#if defined(VK_FUCHSIA_external_semaphore)
	PFN_vkGetSemaphoreZirconHandleFUCHSIA vkGetSemaphoreZirconHandleFUCHSIA;
	PFN_vkImportSemaphoreZirconHandleFUCHSIA vkImportSemaphoreZirconHandleFUCHSIA;
#endif /* defined(VK_FUCHSIA_external_semaphore) */
#if defined(VK_GOOGLE_display_timing)
	PFN_vkGetPastPresentationTimingGOOGLE vkGetPastPresentationTimingGOOGLE;
	PFN_vkGetRefreshCycleDurationGOOGLE vkGetRefreshCycleDurationGOOGLE;
#endif /* defined(VK_GOOGLE_display_timing) */
#if defined(VK_HUAWEI_cluster_culling_shader)
	PFN_vkCmdDrawClusterHUAWEI vkCmdDrawClusterHUAWEI;
	PFN_vkCmdDrawClusterIndirectHUAWEI vkCmdDrawClusterIndirectHUAWEI;
#endif /* defined(VK_HUAWEI_cluster_culling_shader) */
#if defined(VK_HUAWEI_invocation_mask)
	PFN_vkCmdBindInvocationMaskHUAWEI vkCmdBindInvocationMaskHUAWEI;
#endif /* defined(VK_HUAWEI_invocation_mask) */
#if defined(VK_HUAWEI_subpass_shading)
	PFN_vkCmdSubpassShadingHUAWEI vkCmdSubpassShadingHUAWEI;
	PFN_vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI;
#endif /* defined(VK_HUAWEI_subpass_shading) */
#if defined(VK_INTEL_performance_query)
	PFN_vkAcquirePerformanceConfigurationINTEL vkAcquirePerformanceConfigurationINTEL;
	PFN_vkCmdSetPerformanceMarkerINTEL vkCmdSetPerformanceMarkerINTEL;
	PFN_vkCmdSetPerformanceOverrideINTEL vkCmdSetPerformanceOverrideINTEL;
	PFN_vkCmdSetPerformanceStreamMarkerINTEL vkCmdSetPerformanceStreamMarkerINTEL;
	PFN_vkGetPerformanceParameterINTEL vkGetPerformanceParameterINTEL;
	PFN_vkInitializePerformanceApiINTEL vkInitializePerformanceApiINTEL;
	PFN_vkQueueSetPerformanceConfigurationINTEL vkQueueSetPerformanceConfigurationINTEL;
	PFN_vkReleasePerformanceConfigurationINTEL vkReleasePerformanceConfigurationINTEL;
	PFN_vkUninitializePerformanceApiINTEL vkUninitializePerformanceApiINTEL;
#endif /* defined(VK_INTEL_performance_query) */
#if defined(VK_KHR_acceleration_structure)
	PFN_vkBuildAccelerationStructuresKHR vkBuildAccelerationStructuresKHR;
	PFN_vkCmdBuildAccelerationStructuresIndirectKHR vkCmdBuildAccelerationStructuresIndirectKHR;
	PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR;
	PFN_vkCmdCopyAccelerationStructureKHR vkCmdCopyAccelerationStructureKHR;
	PFN_vkCmdCopyAccelerationStructureToMemoryKHR vkCmdCopyAccelerationStructureToMemoryKHR;
	PFN_vkCmdCopyMemoryToAccelerationStructureKHR vkCmdCopyMemoryToAccelerationStructureKHR;
	PFN_vkCmdWriteAccelerationStructuresPropertiesKHR vkCmdWriteAccelerationStructuresPropertiesKHR;
	PFN_vkCopyAccelerationStructureKHR vkCopyAccelerationStructureKHR;
	PFN_vkCopyAccelerationStructureToMemoryKHR vkCopyAccelerationStructureToMemoryKHR;
	PFN_vkCopyMemoryToAccelerationStructureKHR vkCopyMemoryToAccelerationStructureKHR;
	PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR;
	PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR;
	PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR;
	PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR;
	PFN_vkGetDeviceAccelerationStructureCompatibilityKHR vkGetDeviceAccelerationStructureCompatibilityKHR;
	PFN_vkWriteAccelerationStructuresPropertiesKHR vkWriteAccelerationStructuresPropertiesKHR;
#endif /* defined(VK_KHR_acceleration_structure) */
#if defined(VK_KHR_bind_memory2)
	PFN_vkBindBufferMemory2KHR vkBindBufferMemory2KHR;
	PFN_vkBindImageMemory2KHR vkBindImageMemory2KHR;
#endif /* defined(VK_KHR_bind_memory2) */
#if defined(VK_KHR_buffer_device_address)
	PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddressKHR;
	PFN_vkGetBufferOpaqueCaptureAddressKHR vkGetBufferOpaqueCaptureAddressKHR;
	PFN_vkGetDeviceMemoryOpaqueCaptureAddressKHR vkGetDeviceMemoryOpaqueCaptureAddressKHR;
#endif /* defined(VK_KHR_buffer_device_address) */
#if defined(VK_KHR_calibrated_timestamps)
	PFN_vkGetCalibratedTimestampsKHR vkGetCalibratedTimestampsKHR;
#endif /* defined(VK_KHR_calibrated_timestamps) */
#if defined(VK_KHR_copy_commands2)
	PFN_vkCmdBlitImage2KHR vkCmdBlitImage2KHR;
	PFN_vkCmdCopyBuffer2KHR vkCmdCopyBuffer2KHR;
	PFN_vkCmdCopyBufferToImage2KHR vkCmdCopyBufferToImage2KHR;
	PFN_vkCmdCopyImage2KHR vkCmdCopyImage2KHR;
	PFN_vkCmdCopyImageToBuffer2KHR vkCmdCopyImageToBuffer2KHR;
	PFN_vkCmdResolveImage2KHR vkCmdResolveImage2KHR;
#endif /* defined(VK_KHR_copy_commands2) */
#if defined(VK_KHR_create_renderpass2)
	PFN_vkCmdBeginRenderPass2KHR vkCmdBeginRenderPass2KHR;
	PFN_vkCmdEndRenderPass2KHR vkCmdEndRenderPass2KHR;
	PFN_vkCmdNextSubpass2KHR vkCmdNextSubpass2KHR;
	PFN_vkCreateRenderPass2KHR vkCreateRenderPass2KHR;
#endif /* defined(VK_KHR_create_renderpass2) */
#if defined(VK_KHR_deferred_host_operations)
	PFN_vkCreateDeferredOperationKHR vkCreateDeferredOperationKHR;
	PFN_vkDeferredOperationJoinKHR vkDeferredOperationJoinKHR;
	PFN_vkDestroyDeferredOperationKHR vkDestroyDeferredOperationKHR;
	PFN_vkGetDeferredOperationMaxConcurrencyKHR vkGetDeferredOperationMaxConcurrencyKHR;
	PFN_vkGetDeferredOperationResultKHR vkGetDeferredOperationResultKHR;
#endif /* defined(VK_KHR_deferred_host_operations) */
#if defined(VK_KHR_descriptor_update_template)
	PFN_vkCreateDescriptorUpdateTemplateKHR vkCreateDescriptorUpdateTemplateKHR;
	PFN_vkDestroyDescriptorUpdateTemplateKHR vkDestroyDescriptorUpdateTemplateKHR;
	PFN_vkUpdateDescriptorSetWithTemplateKHR vkUpdateDescriptorSetWithTemplateKHR;
#endif /* defined(VK_KHR_descriptor_update_template) */
#if defined(VK_KHR_device_group)
	PFN_vkCmdDispatchBaseKHR vkCmdDispatchBaseKHR;
	PFN_vkCmdSetDeviceMaskKHR vkCmdSetDeviceMaskKHR;
	PFN_vkGetDeviceGroupPeerMemoryFeaturesKHR vkGetDeviceGroupPeerMemoryFeaturesKHR;
#endif /* defined(VK_KHR_device_group) */
#if defined(VK_KHR_display_swapchain)
	PFN_vkCreateSharedSwapchainsKHR vkCreateSharedSwapchainsKHR;
#endif /* defined(VK_KHR_display_swapchain) */
#if defined(VK_KHR_draw_indirect_count)
	PFN_vkCmdDrawIndexedIndirectCountKHR vkCmdDrawIndexedIndirectCountKHR;
	PFN_vkCmdDrawIndirectCountKHR vkCmdDrawIndirectCountKHR;
#endif /* defined(VK_KHR_draw_indirect_count) */
#if defined(VK_KHR_dynamic_rendering)
	PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR;
	PFN_vkCmdEndRenderingKHR vkCmdEndRenderingKHR;
#endif /* defined(VK_KHR_dynamic_rendering) */
#if defined(VK_KHR_dynamic_rendering_local_read)
	PFN_vkCmdSetRenderingAttachmentLocationsKHR vkCmdSetRenderingAttachmentLocationsKHR;
	PFN_vkCmdSetRenderingInputAttachmentIndicesKHR vkCmdSetRenderingInputAttachmentIndicesKHR;
#endif /* defined(VK_KHR_dynamic_rendering_local_read) */
#if defined(VK_KHR_external_fence_fd)
	PFN_vkGetFenceFdKHR vkGetFenceFdKHR;
	PFN_vkImportFenceFdKHR vkImportFenceFdKHR;
#endif /* defined(VK_KHR_external_fence_fd) */
#if defined(VK_KHR_external_fence_win32)
	PFN_vkGetFenceWin32HandleKHR vkGetFenceWin32HandleKHR;
	PFN_vkImportFenceWin32HandleKHR vkImportFenceWin32HandleKHR;
#endif /* defined(VK_KHR_external_fence_win32) */
#if defined(VK_KHR_external_memory_fd)
	PFN_vkGetMemoryFdKHR vkGetMemoryFdKHR;
	PFN_vkGetMemoryFdPropertiesKHR vkGetMemoryFdPropertiesKHR;
#endif /* defined(VK_KHR_external_memory_fd) */
#if defined(VK_KHR_external_memory_win32)
	PFN_vkGetMemoryWin32HandleKHR vkGetMemoryWin32HandleKHR;
	PFN_vkGetMemoryWin32HandlePropertiesKHR vkGetMemoryWin32HandlePropertiesKHR;
#endif /* defined(VK_KHR_external_memory_win32) */
#if defined(VK_KHR_external_semaphore_fd)
	PFN_vkGetSemaphoreFdKHR vkGetSemaphoreFdKHR;
	PFN_vkImportSemaphoreFdKHR vkImportSemaphoreFdKHR;
#endif /* defined(VK_KHR_external_semaphore_fd) */
#if defined(VK_KHR_external_semaphore_win32)
	PFN_vkGetSemaphoreWin32HandleKHR vkGetSemaphoreWin32HandleKHR;
	PFN_vkImportSemaphoreWin32HandleKHR vkImportSemaphoreWin32HandleKHR;
#endif /* defined(VK_KHR_external_semaphore_win32) */
#if defined(VK_KHR_fragment_shading_rate)
	PFN_vkCmdSetFragmentShadingRateKHR vkCmdSetFragmentShadingRateKHR;
#endif /* defined(VK_KHR_fragment_shading_rate) */
#if defined(VK_KHR_get_memory_requirements2)
	PFN_vkGetBufferMemoryRequirements2KHR vkGetBufferMemoryRequirements2KHR;
	PFN_vkGetImageMemoryRequirements2KHR vkGetImageMemoryRequirements2KHR;
	PFN_vkGetImageSparseMemoryRequirements2KHR vkGetImageSparseMemoryRequirements2KHR;
#endif /* defined(VK_KHR_get_memory_requirements2) */
#if defined(VK_KHR_line_rasterization)
	PFN_vkCmdSetLineStippleKHR vkCmdSetLineStippleKHR;
#endif /* defined(VK_KHR_line_rasterization) */
#if defined(VK_KHR_maintenance1)
	PFN_vkTrimCommandPoolKHR vkTrimCommandPoolKHR;
#endif /* defined(VK_KHR_maintenance1) */
#if defined(VK_KHR_maintenance3)
	PFN_vkGetDescriptorSetLayoutSupportKHR vkGetDescriptorSetLayoutSupportKHR;
#endif /* defined(VK_KHR_maintenance3) */
#if defined(VK_KHR_maintenance4)
	PFN_vkGetDeviceBufferMemoryRequirementsKHR vkGetDeviceBufferMemoryRequirementsKHR;
	PFN_vkGetDeviceImageMemoryRequirementsKHR vkGetDeviceImageMemoryRequirementsKHR;
	PFN_vkGetDeviceImageSparseMemoryRequirementsKHR vkGetDeviceImageSparseMemoryRequirementsKHR;
#endif /* defined(VK_KHR_maintenance4) */
#if defined(VK_KHR_maintenance5)
	PFN_vkCmdBindIndexBuffer2KHR vkCmdBindIndexBuffer2KHR;
	PFN_vkGetDeviceImageSubresourceLayoutKHR vkGetDeviceImageSubresourceLayoutKHR;
	PFN_vkGetImageSubresourceLayout2KHR vkGetImageSubresourceLayout2KHR;
	PFN_vkGetRenderingAreaGranularityKHR vkGetRenderingAreaGranularityKHR;
#endif /* defined(VK_KHR_maintenance5) */
#if defined(VK_KHR_maintenance6)
	PFN_vkCmdBindDescriptorSets2KHR vkCmdBindDescriptorSets2KHR;
	PFN_vkCmdPushConstants2KHR vkCmdPushConstants2KHR;
#endif /* defined(VK_KHR_maintenance6) */
#if defined(VK_KHR_maintenance6) && defined(VK_KHR_push_descriptor)
	PFN_vkCmdPushDescriptorSet2KHR vkCmdPushDescriptorSet2KHR;
	PFN_vkCmdPushDescriptorSetWithTemplate2KHR vkCmdPushDescriptorSetWithTemplate2KHR;
#endif /* defined(VK_KHR_maintenance6) && defined(VK_KHR_push_descriptor) */
#if defined(VK_KHR_maintenance6) && defined(VK_EXT_descriptor_buffer)
	PFN_vkCmdBindDescriptorBufferEmbeddedSamplers2EXT vkCmdBindDescriptorBufferEmbeddedSamplers2EXT;
	PFN_vkCmdSetDescriptorBufferOffsets2EXT vkCmdSetDescriptorBufferOffsets2EXT;
#endif /* defined(VK_KHR_maintenance6) && defined(VK_EXT_descriptor_buffer) */
#if defined(VK_KHR_map_memory2)
	PFN_vkMapMemory2KHR vkMapMemory2KHR;
	PFN_vkUnmapMemory2KHR vkUnmapMemory2KHR;
#endif /* defined(VK_KHR_map_memory2) */
#if defined(VK_KHR_performance_query)
	PFN_vkAcquireProfilingLockKHR vkAcquireProfilingLockKHR;
	PFN_vkReleaseProfilingLockKHR vkReleaseProfilingLockKHR;
#endif /* defined(VK_KHR_performance_query) */
#if defined(VK_KHR_pipeline_executable_properties)
	PFN_vkGetPipelineExecutableInternalRepresentationsKHR vkGetPipelineExecutableInternalRepresentationsKHR;
	PFN_vkGetPipelineExecutablePropertiesKHR vkGetPipelineExecutablePropertiesKHR;
	PFN_vkGetPipelineExecutableStatisticsKHR vkGetPipelineExecutableStatisticsKHR;
#endif /* defined(VK_KHR_pipeline_executable_properties) */
#if defined(VK_KHR_present_wait)
	PFN_vkWaitForPresentKHR vkWaitForPresentKHR;
#endif /* defined(VK_KHR_present_wait) */
#if defined(VK_KHR_push_descriptor)
	PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR;
#endif /* defined(VK_KHR_push_descriptor) */
#if defined(VK_KHR_ray_tracing_maintenance1) && defined(VK_KHR_ray_tracing_pipeline)
	PFN_vkCmdTraceRaysIndirect2KHR vkCmdTraceRaysIndirect2KHR;
#endif /* defined(VK_KHR_ray_tracing_maintenance1) && defined(VK_KHR_ray_tracing_pipeline) */
#if defined(VK_KHR_ray_tracing_pipeline)
	PFN_vkCmdSetRayTracingPipelineStackSizeKHR vkCmdSetRayTracingPipelineStackSizeKHR;
	PFN_vkCmdTraceRaysIndirectKHR vkCmdTraceRaysIndirectKHR;
	PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR;
	PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR;
	PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR vkGetRayTracingCaptureReplayShaderGroupHandlesKHR;
	PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR;
	PFN_vkGetRayTracingShaderGroupStackSizeKHR vkGetRayTracingShaderGroupStackSizeKHR;
#endif /* defined(VK_KHR_ray_tracing_pipeline) */
#if defined(VK_KHR_sampler_ycbcr_conversion)
	PFN_vkCreateSamplerYcbcrConversionKHR vkCreateSamplerYcbcrConversionKHR;
	PFN_vkDestroySamplerYcbcrConversionKHR vkDestroySamplerYcbcrConversionKHR;
#endif /* defined(VK_KHR_sampler_ycbcr_conversion) */
#if defined(VK_KHR_shared_presentable_image)
	PFN_vkGetSwapchainStatusKHR vkGetSwapchainStatusKHR;
#endif /* defined(VK_KHR_shared_presentable_image) */
#if defined(VK_KHR_swapchain)
	PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
	PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
	PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;
	PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
	PFN_vkQueuePresentKHR vkQueuePresentKHR;
#endif /* defined(VK_KHR_swapchain) */
#if defined(VK_KHR_synchronization2)
	PFN_vkCmdPipelineBarrier2KHR vkCmdPipelineBarrier2KHR;
	PFN_vkCmdResetEvent2KHR vkCmdResetEvent2KHR;
	PFN_vkCmdSetEvent2KHR vkCmdSetEvent2KHR;
	PFN_vkCmdWaitEvents2KHR vkCmdWaitEvents2KHR;
	PFN_vkCmdWriteTimestamp2KHR vkCmdWriteTimestamp2KHR;
	PFN_vkQueueSubmit2KHR vkQueueSubmit2KHR;
#endif /* defined(VK_KHR_synchronization2) */
#if defined(VK_KHR_synchronization2) && defined(VK_AMD_buffer_marker)
	PFN_vkCmdWriteBufferMarker2AMD vkCmdWriteBufferMarker2AMD;
#endif /* defined(VK_KHR_synchronization2) && defined(VK_AMD_buffer_marker) */
#if defined(VK_KHR_synchronization2) && defined(VK_NV_device_diagnostic_checkpoints)
	PFN_vkGetQueueCheckpointData2NV vkGetQueueCheckpointData2NV;
#endif /* defined(VK_KHR_synchronization2) && defined(VK_NV_device_diagnostic_checkpoints) */
#if defined(VK_KHR_timeline_semaphore)
	PFN_vkGetSemaphoreCounterValueKHR vkGetSemaphoreCounterValueKHR;
	PFN_vkSignalSemaphoreKHR vkSignalSemaphoreKHR;
	PFN_vkWaitSemaphoresKHR vkWaitSemaphoresKHR;
#endif /* defined(VK_KHR_timeline_semaphore) */
#if defined(VK_KHR_video_decode_queue)
	PFN_vkCmdDecodeVideoKHR vkCmdDecodeVideoKHR;
#endif /* defined(VK_KHR_video_decode_queue) */
#if defined(VK_KHR_video_encode_queue)
	PFN_vkCmdEncodeVideoKHR vkCmdEncodeVideoKHR;
	PFN_vkGetEncodedVideoSessionParametersKHR vkGetEncodedVideoSessionParametersKHR;
#endif /* defined(VK_KHR_video_encode_queue) */
#if defined(VK_KHR_video_queue)
	PFN_vkBindVideoSessionMemoryKHR vkBindVideoSessionMemoryKHR;
	PFN_vkCmdBeginVideoCodingKHR vkCmdBeginVideoCodingKHR;
	PFN_vkCmdControlVideoCodingKHR vkCmdControlVideoCodingKHR;
	PFN_vkCmdEndVideoCodingKHR vkCmdEndVideoCodingKHR;
	PFN_vkCreateVideoSessionKHR vkCreateVideoSessionKHR;
	PFN_vkCreateVideoSessionParametersKHR vkCreateVideoSessionParametersKHR;
	PFN_vkDestroyVideoSessionKHR vkDestroyVideoSessionKHR;
	PFN_vkDestroyVideoSessionParametersKHR vkDestroyVideoSessionParametersKHR;
	PFN_vkGetVideoSessionMemoryRequirementsKHR vkGetVideoSessionMemoryRequirementsKHR;
	PFN_vkUpdateVideoSessionParametersKHR vkUpdateVideoSessionParametersKHR;
#endif /* defined(VK_KHR_video_queue) */
#if defined(VK_NVX_binary_import)
	PFN_vkCmdCuLaunchKernelNVX vkCmdCuLaunchKernelNVX;
	PFN_vkCreateCuFunctionNVX vkCreateCuFunctionNVX;
	PFN_vkCreateCuModuleNVX vkCreateCuModuleNVX;
	PFN_vkDestroyCuFunctionNVX vkDestroyCuFunctionNVX;
	PFN_vkDestroyCuModuleNVX vkDestroyCuModuleNVX;
#endif /* defined(VK_NVX_binary_import) */
#if defined(VK_NVX_image_view_handle)
	PFN_vkGetImageViewAddressNVX vkGetImageViewAddressNVX;
	PFN_vkGetImageViewHandleNVX vkGetImageViewHandleNVX;
#endif /* defined(VK_NVX_image_view_handle) */
#if defined(VK_NV_clip_space_w_scaling)
	PFN_vkCmdSetViewportWScalingNV vkCmdSetViewportWScalingNV;
#endif /* defined(VK_NV_clip_space_w_scaling) */
#if defined(VK_NV_copy_memory_indirect)
	PFN_vkCmdCopyMemoryIndirectNV vkCmdCopyMemoryIndirectNV;
	PFN_vkCmdCopyMemoryToImageIndirectNV vkCmdCopyMemoryToImageIndirectNV;
#endif /* defined(VK_NV_copy_memory_indirect) */
#if defined(VK_NV_cuda_kernel_launch)
	PFN_vkCmdCudaLaunchKernelNV vkCmdCudaLaunchKernelNV;
	PFN_vkCreateCudaFunctionNV vkCreateCudaFunctionNV;
	PFN_vkCreateCudaModuleNV vkCreateCudaModuleNV;
	PFN_vkDestroyCudaFunctionNV vkDestroyCudaFunctionNV;
	PFN_vkDestroyCudaModuleNV vkDestroyCudaModuleNV;
	PFN_vkGetCudaModuleCacheNV vkGetCudaModuleCacheNV;
#endif /* defined(VK_NV_cuda_kernel_launch) */
#if defined(VK_NV_device_diagnostic_checkpoints)
	PFN_vkCmdSetCheckpointNV vkCmdSetCheckpointNV;
	PFN_vkGetQueueCheckpointDataNV vkGetQueueCheckpointDataNV;
#endif /* defined(VK_NV_device_diagnostic_checkpoints) */
#if defined(VK_NV_device_generated_commands)
	PFN_vkCmdBindPipelineShaderGroupNV vkCmdBindPipelineShaderGroupNV;
	PFN_vkCmdExecuteGeneratedCommandsNV vkCmdExecuteGeneratedCommandsNV;
	PFN_vkCmdPreprocessGeneratedCommandsNV vkCmdPreprocessGeneratedCommandsNV;
	PFN_vkCreateIndirectCommandsLayoutNV vkCreateIndirectCommandsLayoutNV;
	PFN_vkDestroyIndirectCommandsLayoutNV vkDestroyIndirectCommandsLayoutNV;
	PFN_vkGetGeneratedCommandsMemoryRequirementsNV vkGetGeneratedCommandsMemoryRequirementsNV;
#endif /* defined(VK_NV_device_generated_commands) */
#if defined(VK_NV_device_generated_commands_compute)
	PFN_vkCmdUpdatePipelineIndirectBufferNV vkCmdUpdatePipelineIndirectBufferNV;
	PFN_vkGetPipelineIndirectDeviceAddressNV vkGetPipelineIndirectDeviceAddressNV;
	PFN_vkGetPipelineIndirectMemoryRequirementsNV vkGetPipelineIndirectMemoryRequirementsNV;
#endif /* defined(VK_NV_device_generated_commands_compute) */
#if defined(VK_NV_external_memory_rdma)
	PFN_vkGetMemoryRemoteAddressNV vkGetMemoryRemoteAddressNV;
#endif /* defined(VK_NV_external_memory_rdma) */
#if defined(VK_NV_external_memory_win32)
	PFN_vkGetMemoryWin32HandleNV vkGetMemoryWin32HandleNV;
#endif /* defined(VK_NV_external_memory_win32) */
#if defined(VK_NV_fragment_shading_rate_enums)
	PFN_vkCmdSetFragmentShadingRateEnumNV vkCmdSetFragmentShadingRateEnumNV;
#endif /* defined(VK_NV_fragment_shading_rate_enums) */
#if defined(VK_NV_low_latency2)
	PFN_vkGetLatencyTimingsNV vkGetLatencyTimingsNV;
	PFN_vkLatencySleepNV vkLatencySleepNV;
	PFN_vkQueueNotifyOutOfBandNV vkQueueNotifyOutOfBandNV;
	PFN_vkSetLatencyMarkerNV vkSetLatencyMarkerNV;
	PFN_vkSetLatencySleepModeNV vkSetLatencySleepModeNV;
#endif /* defined(VK_NV_low_latency2) */
#if defined(VK_NV_memory_decompression)
	PFN_vkCmdDecompressMemoryIndirectCountNV vkCmdDecompressMemoryIndirectCountNV;
	PFN_vkCmdDecompressMemoryNV vkCmdDecompressMemoryNV;
#endif /* defined(VK_NV_memory_decompression) */
#if defined(VK_NV_mesh_shader)
	PFN_vkCmdDrawMeshTasksIndirectCountNV vkCmdDrawMeshTasksIndirectCountNV;
	PFN_vkCmdDrawMeshTasksIndirectNV vkCmdDrawMeshTasksIndirectNV;
	PFN_vkCmdDrawMeshTasksNV vkCmdDrawMeshTasksNV;
#endif /* defined(VK_NV_mesh_shader) */
#if defined(VK_NV_optical_flow)
	PFN_vkBindOpticalFlowSessionImageNV vkBindOpticalFlowSessionImageNV;
	PFN_vkCmdOpticalFlowExecuteNV vkCmdOpticalFlowExecuteNV;
	PFN_vkCreateOpticalFlowSessionNV vkCreateOpticalFlowSessionNV;
	PFN_vkDestroyOpticalFlowSessionNV vkDestroyOpticalFlowSessionNV;
#endif /* defined(VK_NV_optical_flow) */
#if defined(VK_NV_ray_tracing)
	PFN_vkBindAccelerationStructureMemoryNV vkBindAccelerationStructureMemoryNV;
	PFN_vkCmdBuildAccelerationStructureNV vkCmdBuildAccelerationStructureNV;
	PFN_vkCmdCopyAccelerationStructureNV vkCmdCopyAccelerationStructureNV;
	PFN_vkCmdTraceRaysNV vkCmdTraceRaysNV;
	PFN_vkCmdWriteAccelerationStructuresPropertiesNV vkCmdWriteAccelerationStructuresPropertiesNV;
	PFN_vkCompileDeferredNV vkCompileDeferredNV;
	PFN_vkCreateAccelerationStructureNV vkCreateAccelerationStructureNV;
	PFN_vkCreateRayTracingPipelinesNV vkCreateRayTracingPipelinesNV;
	PFN_vkDestroyAccelerationStructureNV vkDestroyAccelerationStructureNV;
	PFN_vkGetAccelerationStructureHandleNV vkGetAccelerationStructureHandleNV;
	PFN_vkGetAccelerationStructureMemoryRequirementsNV vkGetAccelerationStructureMemoryRequirementsNV;
	PFN_vkGetRayTracingShaderGroupHandlesNV vkGetRayTracingShaderGroupHandlesNV;
#endif /* defined(VK_NV_ray_tracing) */
#if defined(VK_NV_scissor_exclusive) && VK_NV_SCISSOR_EXCLUSIVE_SPEC_VERSION >= 2
	PFN_vkCmdSetExclusiveScissorEnableNV vkCmdSetExclusiveScissorEnableNV;
#endif /* defined(VK_NV_scissor_exclusive) && VK_NV_SCISSOR_EXCLUSIVE_SPEC_VERSION >= 2 */
#if defined(VK_NV_scissor_exclusive)
	PFN_vkCmdSetExclusiveScissorNV vkCmdSetExclusiveScissorNV;
#endif /* defined(VK_NV_scissor_exclusive) */
#if defined(VK_NV_shading_rate_image)
	PFN_vkCmdBindShadingRateImageNV vkCmdBindShadingRateImageNV;
	PFN_vkCmdSetCoarseSampleOrderNV vkCmdSetCoarseSampleOrderNV;
	PFN_vkCmdSetViewportShadingRatePaletteNV vkCmdSetViewportShadingRatePaletteNV;
#endif /* defined(VK_NV_shading_rate_image) */
#if defined(VK_QCOM_tile_properties)
	PFN_vkGetDynamicRenderingTilePropertiesQCOM vkGetDynamicRenderingTilePropertiesQCOM;
	PFN_vkGetFramebufferTilePropertiesQCOM vkGetFramebufferTilePropertiesQCOM;
#endif /* defined(VK_QCOM_tile_properties) */
#if defined(VK_QNX_external_memory_screen_buffer)
	PFN_vkGetScreenBufferPropertiesQNX vkGetScreenBufferPropertiesQNX;
#endif /* defined(VK_QNX_external_memory_screen_buffer) */
#if defined(VK_VALVE_descriptor_set_host_mapping)
	PFN_vkGetDescriptorSetHostMappingVALVE vkGetDescriptorSetHostMappingVALVE;
	PFN_vkGetDescriptorSetLayoutHostMappingInfoVALVE vkGetDescriptorSetLayoutHostMappingInfoVALVE;
#endif /* defined(VK_VALVE_descriptor_set_host_mapping) */
#if (defined(VK_EXT_extended_dynamic_state)) || (defined(VK_EXT_shader_object))
	PFN_vkCmdBindVertexBuffers2EXT vkCmdBindVertexBuffers2EXT;
	PFN_vkCmdSetCullModeEXT vkCmdSetCullModeEXT;
	PFN_vkCmdSetDepthBoundsTestEnableEXT vkCmdSetDepthBoundsTestEnableEXT;
	PFN_vkCmdSetDepthCompareOpEXT vkCmdSetDepthCompareOpEXT;
	PFN_vkCmdSetDepthTestEnableEXT vkCmdSetDepthTestEnableEXT;
	PFN_vkCmdSetDepthWriteEnableEXT vkCmdSetDepthWriteEnableEXT;
	PFN_vkCmdSetFrontFaceEXT vkCmdSetFrontFaceEXT;
	PFN_vkCmdSetPrimitiveTopologyEXT vkCmdSetPrimitiveTopologyEXT;
	PFN_vkCmdSetScissorWithCountEXT vkCmdSetScissorWithCountEXT;
	PFN_vkCmdSetStencilOpEXT vkCmdSetStencilOpEXT;
	PFN_vkCmdSetStencilTestEnableEXT vkCmdSetStencilTestEnableEXT;
	PFN_vkCmdSetViewportWithCountEXT vkCmdSetViewportWithCountEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state)) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state2)) || (defined(VK_EXT_shader_object))
	PFN_vkCmdSetDepthBiasEnableEXT vkCmdSetDepthBiasEnableEXT;
	PFN_vkCmdSetLogicOpEXT vkCmdSetLogicOpEXT;
	PFN_vkCmdSetPatchControlPointsEXT vkCmdSetPatchControlPointsEXT;
	PFN_vkCmdSetPrimitiveRestartEnableEXT vkCmdSetPrimitiveRestartEnableEXT;
	PFN_vkCmdSetRasterizerDiscardEnableEXT vkCmdSetRasterizerDiscardEnableEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state2)) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state3)) || (defined(VK_EXT_shader_object))
	PFN_vkCmdSetAlphaToCoverageEnableEXT vkCmdSetAlphaToCoverageEnableEXT;
	PFN_vkCmdSetAlphaToOneEnableEXT vkCmdSetAlphaToOneEnableEXT;
	PFN_vkCmdSetColorBlendEnableEXT vkCmdSetColorBlendEnableEXT;
	PFN_vkCmdSetColorBlendEquationEXT vkCmdSetColorBlendEquationEXT;
	PFN_vkCmdSetColorWriteMaskEXT vkCmdSetColorWriteMaskEXT;
	PFN_vkCmdSetDepthClampEnableEXT vkCmdSetDepthClampEnableEXT;
	PFN_vkCmdSetLogicOpEnableEXT vkCmdSetLogicOpEnableEXT;
	PFN_vkCmdSetPolygonModeEXT vkCmdSetPolygonModeEXT;
	PFN_vkCmdSetRasterizationSamplesEXT vkCmdSetRasterizationSamplesEXT;
	PFN_vkCmdSetSampleMaskEXT vkCmdSetSampleMaskEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3)) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state3) && (defined(VK_KHR_maintenance2) || defined(VK_VERSION_1_1))) || (defined(VK_EXT_shader_object))
	PFN_vkCmdSetTessellationDomainOriginEXT vkCmdSetTessellationDomainOriginEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && (defined(VK_KHR_maintenance2) || defined(VK_VERSION_1_1))) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_transform_feedback)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_transform_feedback))
	PFN_vkCmdSetRasterizationStreamEXT vkCmdSetRasterizationStreamEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_transform_feedback)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_transform_feedback)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_conservative_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_conservative_rasterization))
	PFN_vkCmdSetConservativeRasterizationModeEXT vkCmdSetConservativeRasterizationModeEXT;
	PFN_vkCmdSetExtraPrimitiveOverestimationSizeEXT vkCmdSetExtraPrimitiveOverestimationSizeEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_conservative_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_conservative_rasterization)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_enable)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_enable))
	PFN_vkCmdSetDepthClipEnableEXT vkCmdSetDepthClipEnableEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_enable)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_enable)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_sample_locations)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_sample_locations))
	PFN_vkCmdSetSampleLocationsEnableEXT vkCmdSetSampleLocationsEnableEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_sample_locations)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_sample_locations)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_blend_operation_advanced)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_blend_operation_advanced))
	PFN_vkCmdSetColorBlendAdvancedEXT vkCmdSetColorBlendAdvancedEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_blend_operation_advanced)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_blend_operation_advanced)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_provoking_vertex)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_provoking_vertex))
	PFN_vkCmdSetProvokingVertexModeEXT vkCmdSetProvokingVertexModeEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_provoking_vertex)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_provoking_vertex)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_line_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_line_rasterization))
	PFN_vkCmdSetLineRasterizationModeEXT vkCmdSetLineRasterizationModeEXT;
	PFN_vkCmdSetLineStippleEnableEXT vkCmdSetLineStippleEnableEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_line_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_line_rasterization)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_control)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_control))
	PFN_vkCmdSetDepthClipNegativeOneToOneEXT vkCmdSetDepthClipNegativeOneToOneEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_control)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_control)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_clip_space_w_scaling)) || (defined(VK_EXT_shader_object) && defined(VK_NV_clip_space_w_scaling))
	PFN_vkCmdSetViewportWScalingEnableNV vkCmdSetViewportWScalingEnableNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_clip_space_w_scaling)) || (defined(VK_EXT_shader_object) && defined(VK_NV_clip_space_w_scaling)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_viewport_swizzle)) || (defined(VK_EXT_shader_object) && defined(VK_NV_viewport_swizzle))
	PFN_vkCmdSetViewportSwizzleNV vkCmdSetViewportSwizzleNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_viewport_swizzle)) || (defined(VK_EXT_shader_object) && defined(VK_NV_viewport_swizzle)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_fragment_coverage_to_color)) || (defined(VK_EXT_shader_object) && defined(VK_NV_fragment_coverage_to_color))
	PFN_vkCmdSetCoverageToColorEnableNV vkCmdSetCoverageToColorEnableNV;
	PFN_vkCmdSetCoverageToColorLocationNV vkCmdSetCoverageToColorLocationNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_fragment_coverage_to_color)) || (defined(VK_EXT_shader_object) && defined(VK_NV_fragment_coverage_to_color)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_framebuffer_mixed_samples)) || (defined(VK_EXT_shader_object) && defined(VK_NV_framebuffer_mixed_samples))
	PFN_vkCmdSetCoverageModulationModeNV vkCmdSetCoverageModulationModeNV;
	PFN_vkCmdSetCoverageModulationTableEnableNV vkCmdSetCoverageModulationTableEnableNV;
	PFN_vkCmdSetCoverageModulationTableNV vkCmdSetCoverageModulationTableNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_framebuffer_mixed_samples)) || (defined(VK_EXT_shader_object) && defined(VK_NV_framebuffer_mixed_samples)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_shading_rate_image)) || (defined(VK_EXT_shader_object) && defined(VK_NV_shading_rate_image))
	PFN_vkCmdSetShadingRateImageEnableNV vkCmdSetShadingRateImageEnableNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_shading_rate_image)) || (defined(VK_EXT_shader_object) && defined(VK_NV_shading_rate_image)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_representative_fragment_test)) || (defined(VK_EXT_shader_object) && defined(VK_NV_representative_fragment_test))
	PFN_vkCmdSetRepresentativeFragmentTestEnableNV vkCmdSetRepresentativeFragmentTestEnableNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_representative_fragment_test)) || (defined(VK_EXT_shader_object) && defined(VK_NV_representative_fragment_test)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_coverage_reduction_mode)) || (defined(VK_EXT_shader_object) && defined(VK_NV_coverage_reduction_mode))
	PFN_vkCmdSetCoverageReductionModeNV vkCmdSetCoverageReductionModeNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_coverage_reduction_mode)) || (defined(VK_EXT_shader_object) && defined(VK_NV_coverage_reduction_mode)) */
#if (defined(VK_EXT_full_screen_exclusive) && defined(VK_KHR_device_group)) || (defined(VK_EXT_full_screen_exclusive) && defined(VK_VERSION_1_1))
	PFN_vkGetDeviceGroupSurfacePresentModes2EXT vkGetDeviceGroupSurfacePresentModes2EXT;
#endif /* (defined(VK_EXT_full_screen_exclusive) && defined(VK_KHR_device_group)) || (defined(VK_EXT_full_screen_exclusive) && defined(VK_VERSION_1_1)) */
#if (defined(VK_EXT_host_image_copy)) || (defined(VK_EXT_image_compression_control))
	PFN_vkGetImageSubresourceLayout2EXT vkGetImageSubresourceLayout2EXT;
#endif /* (defined(VK_EXT_host_image_copy)) || (defined(VK_EXT_image_compression_control)) */
#if (defined(VK_EXT_shader_object)) || (defined(VK_EXT_vertex_input_dynamic_state))
	PFN_vkCmdSetVertexInputEXT vkCmdSetVertexInputEXT;
#endif /* (defined(VK_EXT_shader_object)) || (defined(VK_EXT_vertex_input_dynamic_state)) */
#if (defined(VK_KHR_descriptor_update_template) && defined(VK_KHR_push_descriptor)) || (defined(VK_KHR_push_descriptor) && defined(VK_VERSION_1_1)) || (defined(VK_KHR_push_descriptor) && defined(VK_KHR_descriptor_update_template))
	PFN_vkCmdPushDescriptorSetWithTemplateKHR vkCmdPushDescriptorSetWithTemplateKHR;
#endif /* (defined(VK_KHR_descriptor_update_template) && defined(VK_KHR_push_descriptor)) || (defined(VK_KHR_push_descriptor) && defined(VK_VERSION_1_1)) || (defined(VK_KHR_push_descriptor) && defined(VK_KHR_descriptor_update_template)) */
#if (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
	PFN_vkGetDeviceGroupPresentCapabilitiesKHR vkGetDeviceGroupPresentCapabilitiesKHR;
	PFN_vkGetDeviceGroupSurfacePresentModesKHR vkGetDeviceGroupSurfacePresentModesKHR;
#endif /* (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1)) */
#if (defined(VK_KHR_device_group) && defined(VK_KHR_swapchain)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
	PFN_vkAcquireNextImage2KHR vkAcquireNextImage2KHR;
#endif /* (defined(VK_KHR_device_group) && defined(VK_KHR_swapchain)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1)) */
	/* VULKANLOADER_GENERATE_DEVICE_TABLE */
};

/* VULKANLOADER_GENERATE_PROTOTYPES_H */
#if defined(VK_VERSION_1_0)
extern PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers;
extern PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets;
extern PFN_vkAllocateMemory vkAllocateMemory;
extern PFN_vkBeginCommandBuffer vkBeginCommandBuffer;
extern PFN_vkBindBufferMemory vkBindBufferMemory;
extern PFN_vkBindImageMemory vkBindImageMemory;
extern PFN_vkCmdBeginQuery vkCmdBeginQuery;
extern PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass;
extern PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets;
extern PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer;
extern PFN_vkCmdBindPipeline vkCmdBindPipeline;
extern PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers;
extern PFN_vkCmdBlitImage vkCmdBlitImage;
extern PFN_vkCmdClearAttachments vkCmdClearAttachments;
extern PFN_vkCmdClearColorImage vkCmdClearColorImage;
extern PFN_vkCmdClearDepthStencilImage vkCmdClearDepthStencilImage;
extern PFN_vkCmdCopyBuffer vkCmdCopyBuffer;
extern PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage;
extern PFN_vkCmdCopyImage vkCmdCopyImage;
extern PFN_vkCmdCopyImageToBuffer vkCmdCopyImageToBuffer;
extern PFN_vkCmdCopyQueryPoolResults vkCmdCopyQueryPoolResults;
extern PFN_vkCmdDispatch vkCmdDispatch;
extern PFN_vkCmdDispatchIndirect vkCmdDispatchIndirect;
extern PFN_vkCmdDraw vkCmdDraw;
extern PFN_vkCmdDrawIndexed vkCmdDrawIndexed;
extern PFN_vkCmdDrawIndexedIndirect vkCmdDrawIndexedIndirect;
extern PFN_vkCmdDrawIndirect vkCmdDrawIndirect;
extern PFN_vkCmdEndQuery vkCmdEndQuery;
extern PFN_vkCmdEndRenderPass vkCmdEndRenderPass;
extern PFN_vkCmdExecuteCommands vkCmdExecuteCommands;
extern PFN_vkCmdFillBuffer vkCmdFillBuffer;
extern PFN_vkCmdNextSubpass vkCmdNextSubpass;
extern PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier;
extern PFN_vkCmdPushConstants vkCmdPushConstants;
extern PFN_vkCmdResetEvent vkCmdResetEvent;
extern PFN_vkCmdResetQueryPool vkCmdResetQueryPool;
extern PFN_vkCmdResolveImage vkCmdResolveImage;
extern PFN_vkCmdSetBlendConstants vkCmdSetBlendConstants;
extern PFN_vkCmdSetDepthBias vkCmdSetDepthBias;
extern PFN_vkCmdSetDepthBounds vkCmdSetDepthBounds;
extern PFN_vkCmdSetEvent vkCmdSetEvent;
extern PFN_vkCmdSetLineWidth vkCmdSetLineWidth;
extern PFN_vkCmdSetScissor vkCmdSetScissor;
extern PFN_vkCmdSetStencilCompareMask vkCmdSetStencilCompareMask;
extern PFN_vkCmdSetStencilReference vkCmdSetStencilReference;
extern PFN_vkCmdSetStencilWriteMask vkCmdSetStencilWriteMask;
extern PFN_vkCmdSetViewport vkCmdSetViewport;
extern PFN_vkCmdUpdateBuffer vkCmdUpdateBuffer;
extern PFN_vkCmdWaitEvents vkCmdWaitEvents;
extern PFN_vkCmdWriteTimestamp vkCmdWriteTimestamp;
extern PFN_vkCreateBuffer vkCreateBuffer;
extern PFN_vkCreateBufferView vkCreateBufferView;
extern PFN_vkCreateCommandPool vkCreateCommandPool;
extern PFN_vkCreateComputePipelines vkCreateComputePipelines;
extern PFN_vkCreateDescriptorPool vkCreateDescriptorPool;
extern PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout;
extern PFN_vkCreateDevice vkCreateDevice;
extern PFN_vkCreateEvent vkCreateEvent;
extern PFN_vkCreateFence vkCreateFence;
extern PFN_vkCreateFramebuffer vkCreateFramebuffer;
extern PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines;
extern PFN_vkCreateImage vkCreateImage;
extern PFN_vkCreateImageView vkCreateImageView;
extern PFN_vkCreateInstance vkCreateInstance;
extern PFN_vkCreatePipelineCache vkCreatePipelineCache;
extern PFN_vkCreatePipelineLayout vkCreatePipelineLayout;
extern PFN_vkCreateQueryPool vkCreateQueryPool;
extern PFN_vkCreateRenderPass vkCreateRenderPass;
extern PFN_vkCreateSampler vkCreateSampler;
extern PFN_vkCreateSemaphore vkCreateSemaphore;
extern PFN_vkCreateShaderModule vkCreateShaderModule;
extern PFN_vkDestroyBuffer vkDestroyBuffer;
extern PFN_vkDestroyBufferView vkDestroyBufferView;
extern PFN_vkDestroyCommandPool vkDestroyCommandPool;
extern PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool;
extern PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout;
extern PFN_vkDestroyDevice vkDestroyDevice;
extern PFN_vkDestroyEvent vkDestroyEvent;
extern PFN_vkDestroyFence vkDestroyFence;
extern PFN_vkDestroyFramebuffer vkDestroyFramebuffer;
extern PFN_vkDestroyImage vkDestroyImage;
extern PFN_vkDestroyImageView vkDestroyImageView;
extern PFN_vkDestroyInstance vkDestroyInstance;
extern PFN_vkDestroyPipeline vkDestroyPipeline;
extern PFN_vkDestroyPipelineCache vkDestroyPipelineCache;
extern PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout;
extern PFN_vkDestroyQueryPool vkDestroyQueryPool;
extern PFN_vkDestroyRenderPass vkDestroyRenderPass;
extern PFN_vkDestroySampler vkDestroySampler;
extern PFN_vkDestroySemaphore vkDestroySemaphore;
extern PFN_vkDestroyShaderModule vkDestroyShaderModule;
extern PFN_vkDeviceWaitIdle vkDeviceWaitIdle;
extern PFN_vkEndCommandBuffer vkEndCommandBuffer;
extern PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties;
extern PFN_vkEnumerateDeviceLayerProperties vkEnumerateDeviceLayerProperties;
extern PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
extern PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
extern PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
extern PFN_vkFlushMappedMemoryRanges vkFlushMappedMemoryRanges;
extern PFN_vkFreeCommandBuffers vkFreeCommandBuffers;
extern PFN_vkFreeDescriptorSets vkFreeDescriptorSets;
extern PFN_vkFreeMemory vkFreeMemory;
extern PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
extern PFN_vkGetDeviceMemoryCommitment vkGetDeviceMemoryCommitment;
extern PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;
extern PFN_vkGetDeviceQueue vkGetDeviceQueue;
extern PFN_vkGetEventStatus vkGetEventStatus;
extern PFN_vkGetFenceStatus vkGetFenceStatus;
extern PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements;
extern PFN_vkGetImageSparseMemoryRequirements vkGetImageSparseMemoryRequirements;
extern PFN_vkGetImageSubresourceLayout vkGetImageSubresourceLayout;
extern PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
extern PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures;
extern PFN_vkGetPhysicalDeviceFormatProperties vkGetPhysicalDeviceFormatProperties;
extern PFN_vkGetPhysicalDeviceImageFormatProperties vkGetPhysicalDeviceImageFormatProperties;
extern PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties;
extern PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
extern PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
extern PFN_vkGetPhysicalDeviceSparseImageFormatProperties vkGetPhysicalDeviceSparseImageFormatProperties;
extern PFN_vkGetPipelineCacheData vkGetPipelineCacheData;
extern PFN_vkGetQueryPoolResults vkGetQueryPoolResults;
extern PFN_vkGetRenderAreaGranularity vkGetRenderAreaGranularity;
extern PFN_vkInvalidateMappedMemoryRanges vkInvalidateMappedMemoryRanges;
extern PFN_vkMapMemory vkMapMemory;
extern PFN_vkMergePipelineCaches vkMergePipelineCaches;
extern PFN_vkQueueBindSparse vkQueueBindSparse;
extern PFN_vkQueueSubmit vkQueueSubmit;
extern PFN_vkQueueWaitIdle vkQueueWaitIdle;
extern PFN_vkResetCommandBuffer vkResetCommandBuffer;
extern PFN_vkResetCommandPool vkResetCommandPool;
extern PFN_vkResetDescriptorPool vkResetDescriptorPool;
extern PFN_vkResetEvent vkResetEvent;
extern PFN_vkResetFences vkResetFences;
extern PFN_vkSetEvent vkSetEvent;
extern PFN_vkUnmapMemory vkUnmapMemory;
extern PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets;
extern PFN_vkWaitForFences vkWaitForFences;
#endif /* defined(VK_VERSION_1_0) */
#if defined(VK_VERSION_1_1)
extern PFN_vkBindBufferMemory2 vkBindBufferMemory2;
extern PFN_vkBindImageMemory2 vkBindImageMemory2;
extern PFN_vkCmdDispatchBase vkCmdDispatchBase;
extern PFN_vkCmdSetDeviceMask vkCmdSetDeviceMask;
extern PFN_vkCreateDescriptorUpdateTemplate vkCreateDescriptorUpdateTemplate;
extern PFN_vkCreateSamplerYcbcrConversion vkCreateSamplerYcbcrConversion;
extern PFN_vkDestroyDescriptorUpdateTemplate vkDestroyDescriptorUpdateTemplate;
extern PFN_vkDestroySamplerYcbcrConversion vkDestroySamplerYcbcrConversion;
extern PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion;
extern PFN_vkEnumeratePhysicalDeviceGroups vkEnumeratePhysicalDeviceGroups;
extern PFN_vkGetBufferMemoryRequirements2 vkGetBufferMemoryRequirements2;
extern PFN_vkGetDescriptorSetLayoutSupport vkGetDescriptorSetLayoutSupport;
extern PFN_vkGetDeviceGroupPeerMemoryFeatures vkGetDeviceGroupPeerMemoryFeatures;
extern PFN_vkGetDeviceQueue2 vkGetDeviceQueue2;
extern PFN_vkGetImageMemoryRequirements2 vkGetImageMemoryRequirements2;
extern PFN_vkGetImageSparseMemoryRequirements2 vkGetImageSparseMemoryRequirements2;
extern PFN_vkGetPhysicalDeviceExternalBufferProperties vkGetPhysicalDeviceExternalBufferProperties;
extern PFN_vkGetPhysicalDeviceExternalFenceProperties vkGetPhysicalDeviceExternalFenceProperties;
extern PFN_vkGetPhysicalDeviceExternalSemaphoreProperties vkGetPhysicalDeviceExternalSemaphoreProperties;
extern PFN_vkGetPhysicalDeviceFeatures2 vkGetPhysicalDeviceFeatures2;
extern PFN_vkGetPhysicalDeviceFormatProperties2 vkGetPhysicalDeviceFormatProperties2;
extern PFN_vkGetPhysicalDeviceImageFormatProperties2 vkGetPhysicalDeviceImageFormatProperties2;
extern PFN_vkGetPhysicalDeviceMemoryProperties2 vkGetPhysicalDeviceMemoryProperties2;
extern PFN_vkGetPhysicalDeviceProperties2 vkGetPhysicalDeviceProperties2;
extern PFN_vkGetPhysicalDeviceQueueFamilyProperties2 vkGetPhysicalDeviceQueueFamilyProperties2;
extern PFN_vkGetPhysicalDeviceSparseImageFormatProperties2 vkGetPhysicalDeviceSparseImageFormatProperties2;
extern PFN_vkTrimCommandPool vkTrimCommandPool;
extern PFN_vkUpdateDescriptorSetWithTemplate vkUpdateDescriptorSetWithTemplate;
#endif /* defined(VK_VERSION_1_1) */
#if defined(VK_VERSION_1_2)
extern PFN_vkCmdBeginRenderPass2 vkCmdBeginRenderPass2;
extern PFN_vkCmdDrawIndexedIndirectCount vkCmdDrawIndexedIndirectCount;
extern PFN_vkCmdDrawIndirectCount vkCmdDrawIndirectCount;
extern PFN_vkCmdEndRenderPass2 vkCmdEndRenderPass2;
extern PFN_vkCmdNextSubpass2 vkCmdNextSubpass2;
extern PFN_vkCreateRenderPass2 vkCreateRenderPass2;
extern PFN_vkGetBufferDeviceAddress vkGetBufferDeviceAddress;
extern PFN_vkGetBufferOpaqueCaptureAddress vkGetBufferOpaqueCaptureAddress;
extern PFN_vkGetDeviceMemoryOpaqueCaptureAddress vkGetDeviceMemoryOpaqueCaptureAddress;
extern PFN_vkGetSemaphoreCounterValue vkGetSemaphoreCounterValue;
extern PFN_vkResetQueryPool vkResetQueryPool;
extern PFN_vkSignalSemaphore vkSignalSemaphore;
extern PFN_vkWaitSemaphores vkWaitSemaphores;
#endif /* defined(VK_VERSION_1_2) */
#if defined(VK_VERSION_1_3)
extern PFN_vkCmdBeginRendering vkCmdBeginRendering;
extern PFN_vkCmdBindVertexBuffers2 vkCmdBindVertexBuffers2;
extern PFN_vkCmdBlitImage2 vkCmdBlitImage2;
extern PFN_vkCmdCopyBuffer2 vkCmdCopyBuffer2;
extern PFN_vkCmdCopyBufferToImage2 vkCmdCopyBufferToImage2;
extern PFN_vkCmdCopyImage2 vkCmdCopyImage2;
extern PFN_vkCmdCopyImageToBuffer2 vkCmdCopyImageToBuffer2;
extern PFN_vkCmdEndRendering vkCmdEndRendering;
extern PFN_vkCmdPipelineBarrier2 vkCmdPipelineBarrier2;
extern PFN_vkCmdResetEvent2 vkCmdResetEvent2;
extern PFN_vkCmdResolveImage2 vkCmdResolveImage2;
extern PFN_vkCmdSetCullMode vkCmdSetCullMode;
extern PFN_vkCmdSetDepthBiasEnable vkCmdSetDepthBiasEnable;
extern PFN_vkCmdSetDepthBoundsTestEnable vkCmdSetDepthBoundsTestEnable;
extern PFN_vkCmdSetDepthCompareOp vkCmdSetDepthCompareOp;
extern PFN_vkCmdSetDepthTestEnable vkCmdSetDepthTestEnable;
extern PFN_vkCmdSetDepthWriteEnable vkCmdSetDepthWriteEnable;
extern PFN_vkCmdSetEvent2 vkCmdSetEvent2;
extern PFN_vkCmdSetFrontFace vkCmdSetFrontFace;
extern PFN_vkCmdSetPrimitiveRestartEnable vkCmdSetPrimitiveRestartEnable;
extern PFN_vkCmdSetPrimitiveTopology vkCmdSetPrimitiveTopology;
extern PFN_vkCmdSetRasterizerDiscardEnable vkCmdSetRasterizerDiscardEnable;
extern PFN_vkCmdSetScissorWithCount vkCmdSetScissorWithCount;
extern PFN_vkCmdSetStencilOp vkCmdSetStencilOp;
extern PFN_vkCmdSetStencilTestEnable vkCmdSetStencilTestEnable;
extern PFN_vkCmdSetViewportWithCount vkCmdSetViewportWithCount;
extern PFN_vkCmdWaitEvents2 vkCmdWaitEvents2;
extern PFN_vkCmdWriteTimestamp2 vkCmdWriteTimestamp2;
extern PFN_vkCreatePrivateDataSlot vkCreatePrivateDataSlot;
extern PFN_vkDestroyPrivateDataSlot vkDestroyPrivateDataSlot;
extern PFN_vkGetDeviceBufferMemoryRequirements vkGetDeviceBufferMemoryRequirements;
extern PFN_vkGetDeviceImageMemoryRequirements vkGetDeviceImageMemoryRequirements;
extern PFN_vkGetDeviceImageSparseMemoryRequirements vkGetDeviceImageSparseMemoryRequirements;
extern PFN_vkGetPhysicalDeviceToolProperties vkGetPhysicalDeviceToolProperties;
extern PFN_vkGetPrivateData vkGetPrivateData;
extern PFN_vkQueueSubmit2 vkQueueSubmit2;
extern PFN_vkSetPrivateData vkSetPrivateData;
#endif /* defined(VK_VERSION_1_3) */
#if defined(VK_AMDX_shader_enqueue)
extern PFN_vkCmdDispatchGraphAMDX vkCmdDispatchGraphAMDX;
extern PFN_vkCmdDispatchGraphIndirectAMDX vkCmdDispatchGraphIndirectAMDX;
extern PFN_vkCmdDispatchGraphIndirectCountAMDX vkCmdDispatchGraphIndirectCountAMDX;
extern PFN_vkCmdInitializeGraphScratchMemoryAMDX vkCmdInitializeGraphScratchMemoryAMDX;
extern PFN_vkCreateExecutionGraphPipelinesAMDX vkCreateExecutionGraphPipelinesAMDX;
extern PFN_vkGetExecutionGraphPipelineNodeIndexAMDX vkGetExecutionGraphPipelineNodeIndexAMDX;
extern PFN_vkGetExecutionGraphPipelineScratchSizeAMDX vkGetExecutionGraphPipelineScratchSizeAMDX;
#endif /* defined(VK_AMDX_shader_enqueue) */
#if defined(VK_AMD_buffer_marker)
extern PFN_vkCmdWriteBufferMarkerAMD vkCmdWriteBufferMarkerAMD;
#endif /* defined(VK_AMD_buffer_marker) */
#if defined(VK_AMD_display_native_hdr)
extern PFN_vkSetLocalDimmingAMD vkSetLocalDimmingAMD;
#endif /* defined(VK_AMD_display_native_hdr) */
#if defined(VK_AMD_draw_indirect_count)
extern PFN_vkCmdDrawIndexedIndirectCountAMD vkCmdDrawIndexedIndirectCountAMD;
extern PFN_vkCmdDrawIndirectCountAMD vkCmdDrawIndirectCountAMD;
#endif /* defined(VK_AMD_draw_indirect_count) */
#if defined(VK_AMD_shader_info)
extern PFN_vkGetShaderInfoAMD vkGetShaderInfoAMD;
#endif /* defined(VK_AMD_shader_info) */
#if defined(VK_ANDROID_external_memory_android_hardware_buffer)
extern PFN_vkGetAndroidHardwareBufferPropertiesANDROID vkGetAndroidHardwareBufferPropertiesANDROID;
extern PFN_vkGetMemoryAndroidHardwareBufferANDROID vkGetMemoryAndroidHardwareBufferANDROID;
#endif /* defined(VK_ANDROID_external_memory_android_hardware_buffer) */
#if defined(VK_EXT_acquire_drm_display)
extern PFN_vkAcquireDrmDisplayEXT vkAcquireDrmDisplayEXT;
extern PFN_vkGetDrmDisplayEXT vkGetDrmDisplayEXT;
#endif /* defined(VK_EXT_acquire_drm_display) */
#if defined(VK_EXT_acquire_xlib_display)
extern PFN_vkAcquireXlibDisplayEXT vkAcquireXlibDisplayEXT;
extern PFN_vkGetRandROutputDisplayEXT vkGetRandROutputDisplayEXT;
#endif /* defined(VK_EXT_acquire_xlib_display) */
#if defined(VK_EXT_attachment_feedback_loop_dynamic_state)
extern PFN_vkCmdSetAttachmentFeedbackLoopEnableEXT vkCmdSetAttachmentFeedbackLoopEnableEXT;
#endif /* defined(VK_EXT_attachment_feedback_loop_dynamic_state) */
#if defined(VK_EXT_buffer_device_address)
extern PFN_vkGetBufferDeviceAddressEXT vkGetBufferDeviceAddressEXT;
#endif /* defined(VK_EXT_buffer_device_address) */
#if defined(VK_EXT_calibrated_timestamps)
extern PFN_vkGetCalibratedTimestampsEXT vkGetCalibratedTimestampsEXT;
extern PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT vkGetPhysicalDeviceCalibrateableTimeDomainsEXT;
#endif /* defined(VK_EXT_calibrated_timestamps) */
#if defined(VK_EXT_color_write_enable)
extern PFN_vkCmdSetColorWriteEnableEXT vkCmdSetColorWriteEnableEXT;
#endif /* defined(VK_EXT_color_write_enable) */
#if defined(VK_EXT_conditional_rendering)
extern PFN_vkCmdBeginConditionalRenderingEXT vkCmdBeginConditionalRenderingEXT;
extern PFN_vkCmdEndConditionalRenderingEXT vkCmdEndConditionalRenderingEXT;
#endif /* defined(VK_EXT_conditional_rendering) */
#if defined(VK_EXT_debug_marker)
extern PFN_vkCmdDebugMarkerBeginEXT vkCmdDebugMarkerBeginEXT;
extern PFN_vkCmdDebugMarkerEndEXT vkCmdDebugMarkerEndEXT;
extern PFN_vkCmdDebugMarkerInsertEXT vkCmdDebugMarkerInsertEXT;
extern PFN_vkDebugMarkerSetObjectNameEXT vkDebugMarkerSetObjectNameEXT;
extern PFN_vkDebugMarkerSetObjectTagEXT vkDebugMarkerSetObjectTagEXT;
#endif /* defined(VK_EXT_debug_marker) */
#if defined(VK_EXT_debug_report)
extern PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT;
extern PFN_vkDebugReportMessageEXT vkDebugReportMessageEXT;
extern PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT;
#endif /* defined(VK_EXT_debug_report) */
#if defined(VK_EXT_debug_utils)
extern PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT;
extern PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT;
extern PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT;
extern PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
extern PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
extern PFN_vkQueueBeginDebugUtilsLabelEXT vkQueueBeginDebugUtilsLabelEXT;
extern PFN_vkQueueEndDebugUtilsLabelEXT vkQueueEndDebugUtilsLabelEXT;
extern PFN_vkQueueInsertDebugUtilsLabelEXT vkQueueInsertDebugUtilsLabelEXT;
extern PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT;
extern PFN_vkSetDebugUtilsObjectTagEXT vkSetDebugUtilsObjectTagEXT;
extern PFN_vkSubmitDebugUtilsMessageEXT vkSubmitDebugUtilsMessageEXT;
#endif /* defined(VK_EXT_debug_utils) */
#if defined(VK_EXT_depth_bias_control)
extern PFN_vkCmdSetDepthBias2EXT vkCmdSetDepthBias2EXT;
#endif /* defined(VK_EXT_depth_bias_control) */
#if defined(VK_EXT_descriptor_buffer)
extern PFN_vkCmdBindDescriptorBufferEmbeddedSamplersEXT vkCmdBindDescriptorBufferEmbeddedSamplersEXT;
extern PFN_vkCmdBindDescriptorBuffersEXT vkCmdBindDescriptorBuffersEXT;
extern PFN_vkCmdSetDescriptorBufferOffsetsEXT vkCmdSetDescriptorBufferOffsetsEXT;
extern PFN_vkGetBufferOpaqueCaptureDescriptorDataEXT vkGetBufferOpaqueCaptureDescriptorDataEXT;
extern PFN_vkGetDescriptorEXT vkGetDescriptorEXT;
extern PFN_vkGetDescriptorSetLayoutBindingOffsetEXT vkGetDescriptorSetLayoutBindingOffsetEXT;
extern PFN_vkGetDescriptorSetLayoutSizeEXT vkGetDescriptorSetLayoutSizeEXT;
extern PFN_vkGetImageOpaqueCaptureDescriptorDataEXT vkGetImageOpaqueCaptureDescriptorDataEXT;
extern PFN_vkGetImageViewOpaqueCaptureDescriptorDataEXT vkGetImageViewOpaqueCaptureDescriptorDataEXT;
extern PFN_vkGetSamplerOpaqueCaptureDescriptorDataEXT vkGetSamplerOpaqueCaptureDescriptorDataEXT;
#endif /* defined(VK_EXT_descriptor_buffer) */
#if defined(VK_EXT_descriptor_buffer) && (defined(VK_KHR_acceleration_structure) || defined(VK_NV_ray_tracing))
extern PFN_vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT;
#endif /* defined(VK_EXT_descriptor_buffer) && (defined(VK_KHR_acceleration_structure) || defined(VK_NV_ray_tracing)) */
#if defined(VK_EXT_device_fault)
extern PFN_vkGetDeviceFaultInfoEXT vkGetDeviceFaultInfoEXT;
#endif /* defined(VK_EXT_device_fault) */
#if defined(VK_EXT_direct_mode_display)
extern PFN_vkReleaseDisplayEXT vkReleaseDisplayEXT;
#endif /* defined(VK_EXT_direct_mode_display) */
#if defined(VK_EXT_directfb_surface)
extern PFN_vkCreateDirectFBSurfaceEXT vkCreateDirectFBSurfaceEXT;
extern PFN_vkGetPhysicalDeviceDirectFBPresentationSupportEXT vkGetPhysicalDeviceDirectFBPresentationSupportEXT;
#endif /* defined(VK_EXT_directfb_surface) */
#if defined(VK_EXT_discard_rectangles)
extern PFN_vkCmdSetDiscardRectangleEXT vkCmdSetDiscardRectangleEXT;
#endif /* defined(VK_EXT_discard_rectangles) */
#if defined(VK_EXT_discard_rectangles) && VK_EXT_DISCARD_RECTANGLES_SPEC_VERSION >= 2
extern PFN_vkCmdSetDiscardRectangleEnableEXT vkCmdSetDiscardRectangleEnableEXT;
extern PFN_vkCmdSetDiscardRectangleModeEXT vkCmdSetDiscardRectangleModeEXT;
#endif /* defined(VK_EXT_discard_rectangles) && VK_EXT_DISCARD_RECTANGLES_SPEC_VERSION >= 2 */
#if defined(VK_EXT_display_control)
extern PFN_vkDisplayPowerControlEXT vkDisplayPowerControlEXT;
extern PFN_vkGetSwapchainCounterEXT vkGetSwapchainCounterEXT;
extern PFN_vkRegisterDeviceEventEXT vkRegisterDeviceEventEXT;
extern PFN_vkRegisterDisplayEventEXT vkRegisterDisplayEventEXT;
#endif /* defined(VK_EXT_display_control) */
#if defined(VK_EXT_display_surface_counter)
extern PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT vkGetPhysicalDeviceSurfaceCapabilities2EXT;
#endif /* defined(VK_EXT_display_surface_counter) */
#if defined(VK_EXT_external_memory_host)
extern PFN_vkGetMemoryHostPointerPropertiesEXT vkGetMemoryHostPointerPropertiesEXT;
#endif /* defined(VK_EXT_external_memory_host) */
#if defined(VK_EXT_full_screen_exclusive)
extern PFN_vkAcquireFullScreenExclusiveModeEXT vkAcquireFullScreenExclusiveModeEXT;
extern PFN_vkGetPhysicalDeviceSurfacePresentModes2EXT vkGetPhysicalDeviceSurfacePresentModes2EXT;
extern PFN_vkReleaseFullScreenExclusiveModeEXT vkReleaseFullScreenExclusiveModeEXT;
#endif /* defined(VK_EXT_full_screen_exclusive) */
#if defined(VK_EXT_hdr_metadata)
extern PFN_vkSetHdrMetadataEXT vkSetHdrMetadataEXT;
#endif /* defined(VK_EXT_hdr_metadata) */
#if defined(VK_EXT_headless_surface)
extern PFN_vkCreateHeadlessSurfaceEXT vkCreateHeadlessSurfaceEXT;
#endif /* defined(VK_EXT_headless_surface) */
#if defined(VK_EXT_host_image_copy)
extern PFN_vkCopyImageToImageEXT vkCopyImageToImageEXT;
extern PFN_vkCopyImageToMemoryEXT vkCopyImageToMemoryEXT;
extern PFN_vkCopyMemoryToImageEXT vkCopyMemoryToImageEXT;
extern PFN_vkTransitionImageLayoutEXT vkTransitionImageLayoutEXT;
#endif /* defined(VK_EXT_host_image_copy) */
#if defined(VK_EXT_host_query_reset)
extern PFN_vkResetQueryPoolEXT vkResetQueryPoolEXT;
#endif /* defined(VK_EXT_host_query_reset) */
#if defined(VK_EXT_image_drm_format_modifier)
extern PFN_vkGetImageDrmFormatModifierPropertiesEXT vkGetImageDrmFormatModifierPropertiesEXT;
#endif /* defined(VK_EXT_image_drm_format_modifier) */
#if defined(VK_EXT_line_rasterization)
extern PFN_vkCmdSetLineStippleEXT vkCmdSetLineStippleEXT;
#endif /* defined(VK_EXT_line_rasterization) */
#if defined(VK_EXT_mesh_shader)
extern PFN_vkCmdDrawMeshTasksEXT vkCmdDrawMeshTasksEXT;
extern PFN_vkCmdDrawMeshTasksIndirectCountEXT vkCmdDrawMeshTasksIndirectCountEXT;
extern PFN_vkCmdDrawMeshTasksIndirectEXT vkCmdDrawMeshTasksIndirectEXT;
#endif /* defined(VK_EXT_mesh_shader) */
#if defined(VK_EXT_metal_objects)
extern PFN_vkExportMetalObjectsEXT vkExportMetalObjectsEXT;
#endif /* defined(VK_EXT_metal_objects) */
#if defined(VK_EXT_metal_surface)
extern PFN_vkCreateMetalSurfaceEXT vkCreateMetalSurfaceEXT;
#endif /* defined(VK_EXT_metal_surface) */
#if defined(VK_EXT_multi_draw)
extern PFN_vkCmdDrawMultiEXT vkCmdDrawMultiEXT;
extern PFN_vkCmdDrawMultiIndexedEXT vkCmdDrawMultiIndexedEXT;
#endif /* defined(VK_EXT_multi_draw) */
#if defined(VK_EXT_opacity_micromap)
extern PFN_vkBuildMicromapsEXT vkBuildMicromapsEXT;
extern PFN_vkCmdBuildMicromapsEXT vkCmdBuildMicromapsEXT;
extern PFN_vkCmdCopyMemoryToMicromapEXT vkCmdCopyMemoryToMicromapEXT;
extern PFN_vkCmdCopyMicromapEXT vkCmdCopyMicromapEXT;
extern PFN_vkCmdCopyMicromapToMemoryEXT vkCmdCopyMicromapToMemoryEXT;
extern PFN_vkCmdWriteMicromapsPropertiesEXT vkCmdWriteMicromapsPropertiesEXT;
extern PFN_vkCopyMemoryToMicromapEXT vkCopyMemoryToMicromapEXT;
extern PFN_vkCopyMicromapEXT vkCopyMicromapEXT;
extern PFN_vkCopyMicromapToMemoryEXT vkCopyMicromapToMemoryEXT;
extern PFN_vkCreateMicromapEXT vkCreateMicromapEXT;
extern PFN_vkDestroyMicromapEXT vkDestroyMicromapEXT;
extern PFN_vkGetDeviceMicromapCompatibilityEXT vkGetDeviceMicromapCompatibilityEXT;
extern PFN_vkGetMicromapBuildSizesEXT vkGetMicromapBuildSizesEXT;
extern PFN_vkWriteMicromapsPropertiesEXT vkWriteMicromapsPropertiesEXT;
#endif /* defined(VK_EXT_opacity_micromap) */
#if defined(VK_EXT_pageable_device_local_memory)
extern PFN_vkSetDeviceMemoryPriorityEXT vkSetDeviceMemoryPriorityEXT;
#endif /* defined(VK_EXT_pageable_device_local_memory) */
#if defined(VK_EXT_pipeline_properties)
extern PFN_vkGetPipelinePropertiesEXT vkGetPipelinePropertiesEXT;
#endif /* defined(VK_EXT_pipeline_properties) */
#if defined(VK_EXT_private_data)
extern PFN_vkCreatePrivateDataSlotEXT vkCreatePrivateDataSlotEXT;
extern PFN_vkDestroyPrivateDataSlotEXT vkDestroyPrivateDataSlotEXT;
extern PFN_vkGetPrivateDataEXT vkGetPrivateDataEXT;
extern PFN_vkSetPrivateDataEXT vkSetPrivateDataEXT;
#endif /* defined(VK_EXT_private_data) */
#if defined(VK_EXT_sample_locations)
extern PFN_vkCmdSetSampleLocationsEXT vkCmdSetSampleLocationsEXT;
extern PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT vkGetPhysicalDeviceMultisamplePropertiesEXT;
#endif /* defined(VK_EXT_sample_locations) */
#if defined(VK_EXT_shader_module_identifier)
extern PFN_vkGetShaderModuleCreateInfoIdentifierEXT vkGetShaderModuleCreateInfoIdentifierEXT;
extern PFN_vkGetShaderModuleIdentifierEXT vkGetShaderModuleIdentifierEXT;
#endif /* defined(VK_EXT_shader_module_identifier) */
#if defined(VK_EXT_shader_object)
extern PFN_vkCmdBindShadersEXT vkCmdBindShadersEXT;
extern PFN_vkCreateShadersEXT vkCreateShadersEXT;
extern PFN_vkDestroyShaderEXT vkDestroyShaderEXT;
extern PFN_vkGetShaderBinaryDataEXT vkGetShaderBinaryDataEXT;
#endif /* defined(VK_EXT_shader_object) */
#if defined(VK_EXT_swapchain_maintenance1)
extern PFN_vkReleaseSwapchainImagesEXT vkReleaseSwapchainImagesEXT;
#endif /* defined(VK_EXT_swapchain_maintenance1) */
#if defined(VK_EXT_tooling_info)
extern PFN_vkGetPhysicalDeviceToolPropertiesEXT vkGetPhysicalDeviceToolPropertiesEXT;
#endif /* defined(VK_EXT_tooling_info) */
#if defined(VK_EXT_transform_feedback)
extern PFN_vkCmdBeginQueryIndexedEXT vkCmdBeginQueryIndexedEXT;
extern PFN_vkCmdBeginTransformFeedbackEXT vkCmdBeginTransformFeedbackEXT;
extern PFN_vkCmdBindTransformFeedbackBuffersEXT vkCmdBindTransformFeedbackBuffersEXT;
extern PFN_vkCmdDrawIndirectByteCountEXT vkCmdDrawIndirectByteCountEXT;
extern PFN_vkCmdEndQueryIndexedEXT vkCmdEndQueryIndexedEXT;
extern PFN_vkCmdEndTransformFeedbackEXT vkCmdEndTransformFeedbackEXT;
#endif /* defined(VK_EXT_transform_feedback) */
#if defined(VK_EXT_validation_cache)
extern PFN_vkCreateValidationCacheEXT vkCreateValidationCacheEXT;
extern PFN_vkDestroyValidationCacheEXT vkDestroyValidationCacheEXT;
extern PFN_vkGetValidationCacheDataEXT vkGetValidationCacheDataEXT;
extern PFN_vkMergeValidationCachesEXT vkMergeValidationCachesEXT;
#endif /* defined(VK_EXT_validation_cache) */
#if defined(VK_FUCHSIA_buffer_collection)
extern PFN_vkCreateBufferCollectionFUCHSIA vkCreateBufferCollectionFUCHSIA;
extern PFN_vkDestroyBufferCollectionFUCHSIA vkDestroyBufferCollectionFUCHSIA;
extern PFN_vkGetBufferCollectionPropertiesFUCHSIA vkGetBufferCollectionPropertiesFUCHSIA;
extern PFN_vkSetBufferCollectionBufferConstraintsFUCHSIA vkSetBufferCollectionBufferConstraintsFUCHSIA;
extern PFN_vkSetBufferCollectionImageConstraintsFUCHSIA vkSetBufferCollectionImageConstraintsFUCHSIA;
#endif /* defined(VK_FUCHSIA_buffer_collection) */
#if defined(VK_FUCHSIA_external_memory)
extern PFN_vkGetMemoryZirconHandleFUCHSIA vkGetMemoryZirconHandleFUCHSIA;
extern PFN_vkGetMemoryZirconHandlePropertiesFUCHSIA vkGetMemoryZirconHandlePropertiesFUCHSIA;
#endif /* defined(VK_FUCHSIA_external_memory) */
#if defined(VK_FUCHSIA_external_semaphore)
extern PFN_vkGetSemaphoreZirconHandleFUCHSIA vkGetSemaphoreZirconHandleFUCHSIA;
extern PFN_vkImportSemaphoreZirconHandleFUCHSIA vkImportSemaphoreZirconHandleFUCHSIA;
#endif /* defined(VK_FUCHSIA_external_semaphore) */
#if defined(VK_FUCHSIA_imagepipe_surface)
extern PFN_vkCreateImagePipeSurfaceFUCHSIA vkCreateImagePipeSurfaceFUCHSIA;
#endif /* defined(VK_FUCHSIA_imagepipe_surface) */
#if defined(VK_GGP_stream_descriptor_surface)
extern PFN_vkCreateStreamDescriptorSurfaceGGP vkCreateStreamDescriptorSurfaceGGP;
#endif /* defined(VK_GGP_stream_descriptor_surface) */
#if defined(VK_GOOGLE_display_timing)
extern PFN_vkGetPastPresentationTimingGOOGLE vkGetPastPresentationTimingGOOGLE;
extern PFN_vkGetRefreshCycleDurationGOOGLE vkGetRefreshCycleDurationGOOGLE;
#endif /* defined(VK_GOOGLE_display_timing) */
#if defined(VK_HUAWEI_cluster_culling_shader)
extern PFN_vkCmdDrawClusterHUAWEI vkCmdDrawClusterHUAWEI;
extern PFN_vkCmdDrawClusterIndirectHUAWEI vkCmdDrawClusterIndirectHUAWEI;
#endif /* defined(VK_HUAWEI_cluster_culling_shader) */
#if defined(VK_HUAWEI_invocation_mask)
extern PFN_vkCmdBindInvocationMaskHUAWEI vkCmdBindInvocationMaskHUAWEI;
#endif /* defined(VK_HUAWEI_invocation_mask) */
#if defined(VK_HUAWEI_subpass_shading)
extern PFN_vkCmdSubpassShadingHUAWEI vkCmdSubpassShadingHUAWEI;
extern PFN_vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI;
#endif /* defined(VK_HUAWEI_subpass_shading) */
#if defined(VK_INTEL_performance_query)
extern PFN_vkAcquirePerformanceConfigurationINTEL vkAcquirePerformanceConfigurationINTEL;
extern PFN_vkCmdSetPerformanceMarkerINTEL vkCmdSetPerformanceMarkerINTEL;
extern PFN_vkCmdSetPerformanceOverrideINTEL vkCmdSetPerformanceOverrideINTEL;
extern PFN_vkCmdSetPerformanceStreamMarkerINTEL vkCmdSetPerformanceStreamMarkerINTEL;
extern PFN_vkGetPerformanceParameterINTEL vkGetPerformanceParameterINTEL;
extern PFN_vkInitializePerformanceApiINTEL vkInitializePerformanceApiINTEL;
extern PFN_vkQueueSetPerformanceConfigurationINTEL vkQueueSetPerformanceConfigurationINTEL;
extern PFN_vkReleasePerformanceConfigurationINTEL vkReleasePerformanceConfigurationINTEL;
extern PFN_vkUninitializePerformanceApiINTEL vkUninitializePerformanceApiINTEL;
#endif /* defined(VK_INTEL_performance_query) */
#if defined(VK_KHR_acceleration_structure)
extern PFN_vkBuildAccelerationStructuresKHR vkBuildAccelerationStructuresKHR;
extern PFN_vkCmdBuildAccelerationStructuresIndirectKHR vkCmdBuildAccelerationStructuresIndirectKHR;
extern PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR;
extern PFN_vkCmdCopyAccelerationStructureKHR vkCmdCopyAccelerationStructureKHR;
extern PFN_vkCmdCopyAccelerationStructureToMemoryKHR vkCmdCopyAccelerationStructureToMemoryKHR;
extern PFN_vkCmdCopyMemoryToAccelerationStructureKHR vkCmdCopyMemoryToAccelerationStructureKHR;
extern PFN_vkCmdWriteAccelerationStructuresPropertiesKHR vkCmdWriteAccelerationStructuresPropertiesKHR;
extern PFN_vkCopyAccelerationStructureKHR vkCopyAccelerationStructureKHR;
extern PFN_vkCopyAccelerationStructureToMemoryKHR vkCopyAccelerationStructureToMemoryKHR;
extern PFN_vkCopyMemoryToAccelerationStructureKHR vkCopyMemoryToAccelerationStructureKHR;
extern PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR;
extern PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR;
extern PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR;
extern PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR;
extern PFN_vkGetDeviceAccelerationStructureCompatibilityKHR vkGetDeviceAccelerationStructureCompatibilityKHR;
extern PFN_vkWriteAccelerationStructuresPropertiesKHR vkWriteAccelerationStructuresPropertiesKHR;
#endif /* defined(VK_KHR_acceleration_structure) */
#if defined(VK_KHR_android_surface)
extern PFN_vkCreateAndroidSurfaceKHR vkCreateAndroidSurfaceKHR;
#endif /* defined(VK_KHR_android_surface) */
#if defined(VK_KHR_bind_memory2)
extern PFN_vkBindBufferMemory2KHR vkBindBufferMemory2KHR;
extern PFN_vkBindImageMemory2KHR vkBindImageMemory2KHR;
#endif /* defined(VK_KHR_bind_memory2) */
#if defined(VK_KHR_buffer_device_address)
extern PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddressKHR;
extern PFN_vkGetBufferOpaqueCaptureAddressKHR vkGetBufferOpaqueCaptureAddressKHR;
extern PFN_vkGetDeviceMemoryOpaqueCaptureAddressKHR vkGetDeviceMemoryOpaqueCaptureAddressKHR;
#endif /* defined(VK_KHR_buffer_device_address) */
#if defined(VK_KHR_calibrated_timestamps)
extern PFN_vkGetCalibratedTimestampsKHR vkGetCalibratedTimestampsKHR;
extern PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsKHR vkGetPhysicalDeviceCalibrateableTimeDomainsKHR;
#endif /* defined(VK_KHR_calibrated_timestamps) */
#if defined(VK_KHR_cooperative_matrix)
extern PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR;
#endif /* defined(VK_KHR_cooperative_matrix) */
#if defined(VK_KHR_copy_commands2)
extern PFN_vkCmdBlitImage2KHR vkCmdBlitImage2KHR;
extern PFN_vkCmdCopyBuffer2KHR vkCmdCopyBuffer2KHR;
extern PFN_vkCmdCopyBufferToImage2KHR vkCmdCopyBufferToImage2KHR;
extern PFN_vkCmdCopyImage2KHR vkCmdCopyImage2KHR;
extern PFN_vkCmdCopyImageToBuffer2KHR vkCmdCopyImageToBuffer2KHR;
extern PFN_vkCmdResolveImage2KHR vkCmdResolveImage2KHR;
#endif /* defined(VK_KHR_copy_commands2) */
#if defined(VK_KHR_create_renderpass2)
extern PFN_vkCmdBeginRenderPass2KHR vkCmdBeginRenderPass2KHR;
extern PFN_vkCmdEndRenderPass2KHR vkCmdEndRenderPass2KHR;
extern PFN_vkCmdNextSubpass2KHR vkCmdNextSubpass2KHR;
extern PFN_vkCreateRenderPass2KHR vkCreateRenderPass2KHR;
#endif /* defined(VK_KHR_create_renderpass2) */
#if defined(VK_KHR_deferred_host_operations)
extern PFN_vkCreateDeferredOperationKHR vkCreateDeferredOperationKHR;
extern PFN_vkDeferredOperationJoinKHR vkDeferredOperationJoinKHR;
extern PFN_vkDestroyDeferredOperationKHR vkDestroyDeferredOperationKHR;
extern PFN_vkGetDeferredOperationMaxConcurrencyKHR vkGetDeferredOperationMaxConcurrencyKHR;
extern PFN_vkGetDeferredOperationResultKHR vkGetDeferredOperationResultKHR;
#endif /* defined(VK_KHR_deferred_host_operations) */
#if defined(VK_KHR_descriptor_update_template)
extern PFN_vkCreateDescriptorUpdateTemplateKHR vkCreateDescriptorUpdateTemplateKHR;
extern PFN_vkDestroyDescriptorUpdateTemplateKHR vkDestroyDescriptorUpdateTemplateKHR;
extern PFN_vkUpdateDescriptorSetWithTemplateKHR vkUpdateDescriptorSetWithTemplateKHR;
#endif /* defined(VK_KHR_descriptor_update_template) */
#if defined(VK_KHR_device_group)
extern PFN_vkCmdDispatchBaseKHR vkCmdDispatchBaseKHR;
extern PFN_vkCmdSetDeviceMaskKHR vkCmdSetDeviceMaskKHR;
extern PFN_vkGetDeviceGroupPeerMemoryFeaturesKHR vkGetDeviceGroupPeerMemoryFeaturesKHR;
#endif /* defined(VK_KHR_device_group) */
#if defined(VK_KHR_device_group_creation)
extern PFN_vkEnumeratePhysicalDeviceGroupsKHR vkEnumeratePhysicalDeviceGroupsKHR;
#endif /* defined(VK_KHR_device_group_creation) */
#if defined(VK_KHR_display)
extern PFN_vkCreateDisplayModeKHR vkCreateDisplayModeKHR;
extern PFN_vkCreateDisplayPlaneSurfaceKHR vkCreateDisplayPlaneSurfaceKHR;
extern PFN_vkGetDisplayModePropertiesKHR vkGetDisplayModePropertiesKHR;
extern PFN_vkGetDisplayPlaneCapabilitiesKHR vkGetDisplayPlaneCapabilitiesKHR;
extern PFN_vkGetDisplayPlaneSupportedDisplaysKHR vkGetDisplayPlaneSupportedDisplaysKHR;
extern PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR vkGetPhysicalDeviceDisplayPlanePropertiesKHR;
extern PFN_vkGetPhysicalDeviceDisplayPropertiesKHR vkGetPhysicalDeviceDisplayPropertiesKHR;
#endif /* defined(VK_KHR_display) */
#if defined(VK_KHR_display_swapchain)
extern PFN_vkCreateSharedSwapchainsKHR vkCreateSharedSwapchainsKHR;
#endif /* defined(VK_KHR_display_swapchain) */
#if defined(VK_KHR_draw_indirect_count)
extern PFN_vkCmdDrawIndexedIndirectCountKHR vkCmdDrawIndexedIndirectCountKHR;
extern PFN_vkCmdDrawIndirectCountKHR vkCmdDrawIndirectCountKHR;
#endif /* defined(VK_KHR_draw_indirect_count) */
#if defined(VK_KHR_dynamic_rendering)
extern PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR;
extern PFN_vkCmdEndRenderingKHR vkCmdEndRenderingKHR;
#endif /* defined(VK_KHR_dynamic_rendering) */
#if defined(VK_KHR_dynamic_rendering_local_read)
extern PFN_vkCmdSetRenderingAttachmentLocationsKHR vkCmdSetRenderingAttachmentLocationsKHR;
extern PFN_vkCmdSetRenderingInputAttachmentIndicesKHR vkCmdSetRenderingInputAttachmentIndicesKHR;
#endif /* defined(VK_KHR_dynamic_rendering_local_read) */
#if defined(VK_KHR_external_fence_capabilities)
extern PFN_vkGetPhysicalDeviceExternalFencePropertiesKHR vkGetPhysicalDeviceExternalFencePropertiesKHR;
#endif /* defined(VK_KHR_external_fence_capabilities) */
#if defined(VK_KHR_external_fence_fd)
extern PFN_vkGetFenceFdKHR vkGetFenceFdKHR;
extern PFN_vkImportFenceFdKHR vkImportFenceFdKHR;
#endif /* defined(VK_KHR_external_fence_fd) */
#if defined(VK_KHR_external_fence_win32)
extern PFN_vkGetFenceWin32HandleKHR vkGetFenceWin32HandleKHR;
extern PFN_vkImportFenceWin32HandleKHR vkImportFenceWin32HandleKHR;
#endif /* defined(VK_KHR_external_fence_win32) */
#if defined(VK_KHR_external_memory_capabilities)
extern PFN_vkGetPhysicalDeviceExternalBufferPropertiesKHR vkGetPhysicalDeviceExternalBufferPropertiesKHR;
#endif /* defined(VK_KHR_external_memory_capabilities) */
#if defined(VK_KHR_external_memory_fd)
extern PFN_vkGetMemoryFdKHR vkGetMemoryFdKHR;
extern PFN_vkGetMemoryFdPropertiesKHR vkGetMemoryFdPropertiesKHR;
#endif /* defined(VK_KHR_external_memory_fd) */
#if defined(VK_KHR_external_memory_win32)
extern PFN_vkGetMemoryWin32HandleKHR vkGetMemoryWin32HandleKHR;
extern PFN_vkGetMemoryWin32HandlePropertiesKHR vkGetMemoryWin32HandlePropertiesKHR;
#endif /* defined(VK_KHR_external_memory_win32) */
#if defined(VK_KHR_external_semaphore_capabilities)
extern PFN_vkGetPhysicalDeviceExternalSemaphorePropertiesKHR vkGetPhysicalDeviceExternalSemaphorePropertiesKHR;
#endif /* defined(VK_KHR_external_semaphore_capabilities) */
#if defined(VK_KHR_external_semaphore_fd)
extern PFN_vkGetSemaphoreFdKHR vkGetSemaphoreFdKHR;
extern PFN_vkImportSemaphoreFdKHR vkImportSemaphoreFdKHR;
#endif /* defined(VK_KHR_external_semaphore_fd) */
#if defined(VK_KHR_external_semaphore_win32)
extern PFN_vkGetSemaphoreWin32HandleKHR vkGetSemaphoreWin32HandleKHR;
extern PFN_vkImportSemaphoreWin32HandleKHR vkImportSemaphoreWin32HandleKHR;
#endif /* defined(VK_KHR_external_semaphore_win32) */
#if defined(VK_KHR_fragment_shading_rate)
extern PFN_vkCmdSetFragmentShadingRateKHR vkCmdSetFragmentShadingRateKHR;
extern PFN_vkGetPhysicalDeviceFragmentShadingRatesKHR vkGetPhysicalDeviceFragmentShadingRatesKHR;
#endif /* defined(VK_KHR_fragment_shading_rate) */
#if defined(VK_KHR_get_display_properties2)
extern PFN_vkGetDisplayModeProperties2KHR vkGetDisplayModeProperties2KHR;
extern PFN_vkGetDisplayPlaneCapabilities2KHR vkGetDisplayPlaneCapabilities2KHR;
extern PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR vkGetPhysicalDeviceDisplayPlaneProperties2KHR;
extern PFN_vkGetPhysicalDeviceDisplayProperties2KHR vkGetPhysicalDeviceDisplayProperties2KHR;
#endif /* defined(VK_KHR_get_display_properties2) */
#if defined(VK_KHR_get_memory_requirements2)
extern PFN_vkGetBufferMemoryRequirements2KHR vkGetBufferMemoryRequirements2KHR;
extern PFN_vkGetImageMemoryRequirements2KHR vkGetImageMemoryRequirements2KHR;
extern PFN_vkGetImageSparseMemoryRequirements2KHR vkGetImageSparseMemoryRequirements2KHR;
#endif /* defined(VK_KHR_get_memory_requirements2) */
#if defined(VK_KHR_get_physical_device_properties2)
extern PFN_vkGetPhysicalDeviceFeatures2KHR vkGetPhysicalDeviceFeatures2KHR;
extern PFN_vkGetPhysicalDeviceFormatProperties2KHR vkGetPhysicalDeviceFormatProperties2KHR;
extern PFN_vkGetPhysicalDeviceImageFormatProperties2KHR vkGetPhysicalDeviceImageFormatProperties2KHR;
extern PFN_vkGetPhysicalDeviceMemoryProperties2KHR vkGetPhysicalDeviceMemoryProperties2KHR;
extern PFN_vkGetPhysicalDeviceProperties2KHR vkGetPhysicalDeviceProperties2KHR;
extern PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR vkGetPhysicalDeviceQueueFamilyProperties2KHR;
extern PFN_vkGetPhysicalDeviceSparseImageFormatProperties2KHR vkGetPhysicalDeviceSparseImageFormatProperties2KHR;
#endif /* defined(VK_KHR_get_physical_device_properties2) */
#if defined(VK_KHR_get_surface_capabilities2)
extern PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR vkGetPhysicalDeviceSurfaceCapabilities2KHR;
extern PFN_vkGetPhysicalDeviceSurfaceFormats2KHR vkGetPhysicalDeviceSurfaceFormats2KHR;
#endif /* defined(VK_KHR_get_surface_capabilities2) */
#if defined(VK_KHR_line_rasterization)
extern PFN_vkCmdSetLineStippleKHR vkCmdSetLineStippleKHR;
#endif /* defined(VK_KHR_line_rasterization) */
#if defined(VK_KHR_maintenance1)
extern PFN_vkTrimCommandPoolKHR vkTrimCommandPoolKHR;
#endif /* defined(VK_KHR_maintenance1) */
#if defined(VK_KHR_maintenance3)
extern PFN_vkGetDescriptorSetLayoutSupportKHR vkGetDescriptorSetLayoutSupportKHR;
#endif /* defined(VK_KHR_maintenance3) */
#if defined(VK_KHR_maintenance4)
extern PFN_vkGetDeviceBufferMemoryRequirementsKHR vkGetDeviceBufferMemoryRequirementsKHR;
extern PFN_vkGetDeviceImageMemoryRequirementsKHR vkGetDeviceImageMemoryRequirementsKHR;
extern PFN_vkGetDeviceImageSparseMemoryRequirementsKHR vkGetDeviceImageSparseMemoryRequirementsKHR;
#endif /* defined(VK_KHR_maintenance4) */
#if defined(VK_KHR_maintenance5)
extern PFN_vkCmdBindIndexBuffer2KHR vkCmdBindIndexBuffer2KHR;
extern PFN_vkGetDeviceImageSubresourceLayoutKHR vkGetDeviceImageSubresourceLayoutKHR;
extern PFN_vkGetImageSubresourceLayout2KHR vkGetImageSubresourceLayout2KHR;
extern PFN_vkGetRenderingAreaGranularityKHR vkGetRenderingAreaGranularityKHR;
#endif /* defined(VK_KHR_maintenance5) */
#if defined(VK_KHR_maintenance6)
extern PFN_vkCmdBindDescriptorSets2KHR vkCmdBindDescriptorSets2KHR;
extern PFN_vkCmdPushConstants2KHR vkCmdPushConstants2KHR;
#endif /* defined(VK_KHR_maintenance6) */
#if defined(VK_KHR_maintenance6) && defined(VK_KHR_push_descriptor)
extern PFN_vkCmdPushDescriptorSet2KHR vkCmdPushDescriptorSet2KHR;
extern PFN_vkCmdPushDescriptorSetWithTemplate2KHR vkCmdPushDescriptorSetWithTemplate2KHR;
#endif /* defined(VK_KHR_maintenance6) && defined(VK_KHR_push_descriptor) */
#if defined(VK_KHR_maintenance6) && defined(VK_EXT_descriptor_buffer)
extern PFN_vkCmdBindDescriptorBufferEmbeddedSamplers2EXT vkCmdBindDescriptorBufferEmbeddedSamplers2EXT;
extern PFN_vkCmdSetDescriptorBufferOffsets2EXT vkCmdSetDescriptorBufferOffsets2EXT;
#endif /* defined(VK_KHR_maintenance6) && defined(VK_EXT_descriptor_buffer) */
#if defined(VK_KHR_map_memory2)
extern PFN_vkMapMemory2KHR vkMapMemory2KHR;
extern PFN_vkUnmapMemory2KHR vkUnmapMemory2KHR;
#endif /* defined(VK_KHR_map_memory2) */
#if defined(VK_KHR_performance_query)
extern PFN_vkAcquireProfilingLockKHR vkAcquireProfilingLockKHR;
extern PFN_vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR;
extern PFN_vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR;
extern PFN_vkReleaseProfilingLockKHR vkReleaseProfilingLockKHR;
#endif /* defined(VK_KHR_performance_query) */
#if defined(VK_KHR_pipeline_executable_properties)
extern PFN_vkGetPipelineExecutableInternalRepresentationsKHR vkGetPipelineExecutableInternalRepresentationsKHR;
extern PFN_vkGetPipelineExecutablePropertiesKHR vkGetPipelineExecutablePropertiesKHR;
extern PFN_vkGetPipelineExecutableStatisticsKHR vkGetPipelineExecutableStatisticsKHR;
#endif /* defined(VK_KHR_pipeline_executable_properties) */
#if defined(VK_KHR_present_wait)
extern PFN_vkWaitForPresentKHR vkWaitForPresentKHR;
#endif /* defined(VK_KHR_present_wait) */
#if defined(VK_KHR_push_descriptor)
extern PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR;
#endif /* defined(VK_KHR_push_descriptor) */
#if defined(VK_KHR_ray_tracing_maintenance1) && defined(VK_KHR_ray_tracing_pipeline)
extern PFN_vkCmdTraceRaysIndirect2KHR vkCmdTraceRaysIndirect2KHR;
#endif /* defined(VK_KHR_ray_tracing_maintenance1) && defined(VK_KHR_ray_tracing_pipeline) */
#if defined(VK_KHR_ray_tracing_pipeline)
extern PFN_vkCmdSetRayTracingPipelineStackSizeKHR vkCmdSetRayTracingPipelineStackSizeKHR;
extern PFN_vkCmdTraceRaysIndirectKHR vkCmdTraceRaysIndirectKHR;
extern PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR;
extern PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR;
extern PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR vkGetRayTracingCaptureReplayShaderGroupHandlesKHR;
extern PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR;
extern PFN_vkGetRayTracingShaderGroupStackSizeKHR vkGetRayTracingShaderGroupStackSizeKHR;
#endif /* defined(VK_KHR_ray_tracing_pipeline) */
#if defined(VK_KHR_sampler_ycbcr_conversion)
extern PFN_vkCreateSamplerYcbcrConversionKHR vkCreateSamplerYcbcrConversionKHR;
extern PFN_vkDestroySamplerYcbcrConversionKHR vkDestroySamplerYcbcrConversionKHR;
#endif /* defined(VK_KHR_sampler_ycbcr_conversion) */
#if defined(VK_KHR_shared_presentable_image)
extern PFN_vkGetSwapchainStatusKHR vkGetSwapchainStatusKHR;
#endif /* defined(VK_KHR_shared_presentable_image) */
#if defined(VK_KHR_surface)
extern PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;
extern PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
extern PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR;
extern PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;
extern PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;
#endif /* defined(VK_KHR_surface) */
#if defined(VK_KHR_swapchain)
extern PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
extern PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
extern PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;
extern PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
extern PFN_vkQueuePresentKHR vkQueuePresentKHR;
#endif /* defined(VK_KHR_swapchain) */
#if defined(VK_KHR_synchronization2)
extern PFN_vkCmdPipelineBarrier2KHR vkCmdPipelineBarrier2KHR;
extern PFN_vkCmdResetEvent2KHR vkCmdResetEvent2KHR;
extern PFN_vkCmdSetEvent2KHR vkCmdSetEvent2KHR;
extern PFN_vkCmdWaitEvents2KHR vkCmdWaitEvents2KHR;
extern PFN_vkCmdWriteTimestamp2KHR vkCmdWriteTimestamp2KHR;
extern PFN_vkQueueSubmit2KHR vkQueueSubmit2KHR;
#endif /* defined(VK_KHR_synchronization2) */
#if defined(VK_KHR_synchronization2) && defined(VK_AMD_buffer_marker)
extern PFN_vkCmdWriteBufferMarker2AMD vkCmdWriteBufferMarker2AMD;
#endif /* defined(VK_KHR_synchronization2) && defined(VK_AMD_buffer_marker) */
#if defined(VK_KHR_synchronization2) && defined(VK_NV_device_diagnostic_checkpoints)
extern PFN_vkGetQueueCheckpointData2NV vkGetQueueCheckpointData2NV;
#endif /* defined(VK_KHR_synchronization2) && defined(VK_NV_device_diagnostic_checkpoints) */
#if defined(VK_KHR_timeline_semaphore)
extern PFN_vkGetSemaphoreCounterValueKHR vkGetSemaphoreCounterValueKHR;
extern PFN_vkSignalSemaphoreKHR vkSignalSemaphoreKHR;
extern PFN_vkWaitSemaphoresKHR vkWaitSemaphoresKHR;
#endif /* defined(VK_KHR_timeline_semaphore) */
#if defined(VK_KHR_video_decode_queue)
extern PFN_vkCmdDecodeVideoKHR vkCmdDecodeVideoKHR;
#endif /* defined(VK_KHR_video_decode_queue) */
#if defined(VK_KHR_video_encode_queue)
extern PFN_vkCmdEncodeVideoKHR vkCmdEncodeVideoKHR;
extern PFN_vkGetEncodedVideoSessionParametersKHR vkGetEncodedVideoSessionParametersKHR;
extern PFN_vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR;
#endif /* defined(VK_KHR_video_encode_queue) */
#if defined(VK_KHR_video_queue)
extern PFN_vkBindVideoSessionMemoryKHR vkBindVideoSessionMemoryKHR;
extern PFN_vkCmdBeginVideoCodingKHR vkCmdBeginVideoCodingKHR;
extern PFN_vkCmdControlVideoCodingKHR vkCmdControlVideoCodingKHR;
extern PFN_vkCmdEndVideoCodingKHR vkCmdEndVideoCodingKHR;
extern PFN_vkCreateVideoSessionKHR vkCreateVideoSessionKHR;
extern PFN_vkCreateVideoSessionParametersKHR vkCreateVideoSessionParametersKHR;
extern PFN_vkDestroyVideoSessionKHR vkDestroyVideoSessionKHR;
extern PFN_vkDestroyVideoSessionParametersKHR vkDestroyVideoSessionParametersKHR;
extern PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR vkGetPhysicalDeviceVideoCapabilitiesKHR;
extern PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR vkGetPhysicalDeviceVideoFormatPropertiesKHR;
extern PFN_vkGetVideoSessionMemoryRequirementsKHR vkGetVideoSessionMemoryRequirementsKHR;
extern PFN_vkUpdateVideoSessionParametersKHR vkUpdateVideoSessionParametersKHR;
#endif /* defined(VK_KHR_video_queue) */
#if defined(VK_KHR_wayland_surface)
extern PFN_vkCreateWaylandSurfaceKHR vkCreateWaylandSurfaceKHR;
extern PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR vkGetPhysicalDeviceWaylandPresentationSupportKHR;
#endif /* defined(VK_KHR_wayland_surface) */
#if defined(VK_KHR_win32_surface)
extern PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR;
extern PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR vkGetPhysicalDeviceWin32PresentationSupportKHR;
#endif /* defined(VK_KHR_win32_surface) */
#if defined(VK_KHR_xcb_surface)
extern PFN_vkCreateXcbSurfaceKHR vkCreateXcbSurfaceKHR;
extern PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR vkGetPhysicalDeviceXcbPresentationSupportKHR;
#endif /* defined(VK_KHR_xcb_surface) */
#if defined(VK_KHR_xlib_surface)
extern PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR;
extern PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR vkGetPhysicalDeviceXlibPresentationSupportKHR;
#endif /* defined(VK_KHR_xlib_surface) */
#if defined(VK_MVK_ios_surface)
extern PFN_vkCreateIOSSurfaceMVK vkCreateIOSSurfaceMVK;
#endif /* defined(VK_MVK_ios_surface) */
#if defined(VK_MVK_macos_surface)
extern PFN_vkCreateMacOSSurfaceMVK vkCreateMacOSSurfaceMVK;
#endif /* defined(VK_MVK_macos_surface) */
#if defined(VK_NN_vi_surface)
extern PFN_vkCreateViSurfaceNN vkCreateViSurfaceNN;
#endif /* defined(VK_NN_vi_surface) */
#if defined(VK_NVX_binary_import)
extern PFN_vkCmdCuLaunchKernelNVX vkCmdCuLaunchKernelNVX;
extern PFN_vkCreateCuFunctionNVX vkCreateCuFunctionNVX;
extern PFN_vkCreateCuModuleNVX vkCreateCuModuleNVX;
extern PFN_vkDestroyCuFunctionNVX vkDestroyCuFunctionNVX;
extern PFN_vkDestroyCuModuleNVX vkDestroyCuModuleNVX;
#endif /* defined(VK_NVX_binary_import) */
#if defined(VK_NVX_image_view_handle)
extern PFN_vkGetImageViewAddressNVX vkGetImageViewAddressNVX;
extern PFN_vkGetImageViewHandleNVX vkGetImageViewHandleNVX;
#endif /* defined(VK_NVX_image_view_handle) */
#if defined(VK_NV_acquire_winrt_display)
extern PFN_vkAcquireWinrtDisplayNV vkAcquireWinrtDisplayNV;
extern PFN_vkGetWinrtDisplayNV vkGetWinrtDisplayNV;
#endif /* defined(VK_NV_acquire_winrt_display) */
#if defined(VK_NV_clip_space_w_scaling)
extern PFN_vkCmdSetViewportWScalingNV vkCmdSetViewportWScalingNV;
#endif /* defined(VK_NV_clip_space_w_scaling) */
#if defined(VK_NV_cooperative_matrix)
extern PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV vkGetPhysicalDeviceCooperativeMatrixPropertiesNV;
#endif /* defined(VK_NV_cooperative_matrix) */
#if defined(VK_NV_copy_memory_indirect)
extern PFN_vkCmdCopyMemoryIndirectNV vkCmdCopyMemoryIndirectNV;
extern PFN_vkCmdCopyMemoryToImageIndirectNV vkCmdCopyMemoryToImageIndirectNV;
#endif /* defined(VK_NV_copy_memory_indirect) */
#if defined(VK_NV_coverage_reduction_mode)
extern PFN_vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV;
#endif /* defined(VK_NV_coverage_reduction_mode) */
#if defined(VK_NV_cuda_kernel_launch)
extern PFN_vkCmdCudaLaunchKernelNV vkCmdCudaLaunchKernelNV;
extern PFN_vkCreateCudaFunctionNV vkCreateCudaFunctionNV;
extern PFN_vkCreateCudaModuleNV vkCreateCudaModuleNV;
extern PFN_vkDestroyCudaFunctionNV vkDestroyCudaFunctionNV;
extern PFN_vkDestroyCudaModuleNV vkDestroyCudaModuleNV;
extern PFN_vkGetCudaModuleCacheNV vkGetCudaModuleCacheNV;
#endif /* defined(VK_NV_cuda_kernel_launch) */
#if defined(VK_NV_device_diagnostic_checkpoints)
extern PFN_vkCmdSetCheckpointNV vkCmdSetCheckpointNV;
extern PFN_vkGetQueueCheckpointDataNV vkGetQueueCheckpointDataNV;
#endif /* defined(VK_NV_device_diagnostic_checkpoints) */
#if defined(VK_NV_device_generated_commands)
extern PFN_vkCmdBindPipelineShaderGroupNV vkCmdBindPipelineShaderGroupNV;
extern PFN_vkCmdExecuteGeneratedCommandsNV vkCmdExecuteGeneratedCommandsNV;
extern PFN_vkCmdPreprocessGeneratedCommandsNV vkCmdPreprocessGeneratedCommandsNV;
extern PFN_vkCreateIndirectCommandsLayoutNV vkCreateIndirectCommandsLayoutNV;
extern PFN_vkDestroyIndirectCommandsLayoutNV vkDestroyIndirectCommandsLayoutNV;
extern PFN_vkGetGeneratedCommandsMemoryRequirementsNV vkGetGeneratedCommandsMemoryRequirementsNV;
#endif /* defined(VK_NV_device_generated_commands) */
#if defined(VK_NV_device_generated_commands_compute)
extern PFN_vkCmdUpdatePipelineIndirectBufferNV vkCmdUpdatePipelineIndirectBufferNV;
extern PFN_vkGetPipelineIndirectDeviceAddressNV vkGetPipelineIndirectDeviceAddressNV;
extern PFN_vkGetPipelineIndirectMemoryRequirementsNV vkGetPipelineIndirectMemoryRequirementsNV;
#endif /* defined(VK_NV_device_generated_commands_compute) */
#if defined(VK_NV_external_memory_capabilities)
extern PFN_vkGetPhysicalDeviceExternalImageFormatPropertiesNV vkGetPhysicalDeviceExternalImageFormatPropertiesNV;
#endif /* defined(VK_NV_external_memory_capabilities) */
#if defined(VK_NV_external_memory_rdma)
extern PFN_vkGetMemoryRemoteAddressNV vkGetMemoryRemoteAddressNV;
#endif /* defined(VK_NV_external_memory_rdma) */
#if defined(VK_NV_external_memory_win32)
extern PFN_vkGetMemoryWin32HandleNV vkGetMemoryWin32HandleNV;
#endif /* defined(VK_NV_external_memory_win32) */
#if defined(VK_NV_fragment_shading_rate_enums)
extern PFN_vkCmdSetFragmentShadingRateEnumNV vkCmdSetFragmentShadingRateEnumNV;
#endif /* defined(VK_NV_fragment_shading_rate_enums) */
#if defined(VK_NV_low_latency2)
extern PFN_vkGetLatencyTimingsNV vkGetLatencyTimingsNV;
extern PFN_vkLatencySleepNV vkLatencySleepNV;
extern PFN_vkQueueNotifyOutOfBandNV vkQueueNotifyOutOfBandNV;
extern PFN_vkSetLatencyMarkerNV vkSetLatencyMarkerNV;
extern PFN_vkSetLatencySleepModeNV vkSetLatencySleepModeNV;
#endif /* defined(VK_NV_low_latency2) */
#if defined(VK_NV_memory_decompression)
extern PFN_vkCmdDecompressMemoryIndirectCountNV vkCmdDecompressMemoryIndirectCountNV;
extern PFN_vkCmdDecompressMemoryNV vkCmdDecompressMemoryNV;
#endif /* defined(VK_NV_memory_decompression) */
#if defined(VK_NV_mesh_shader)
extern PFN_vkCmdDrawMeshTasksIndirectCountNV vkCmdDrawMeshTasksIndirectCountNV;
extern PFN_vkCmdDrawMeshTasksIndirectNV vkCmdDrawMeshTasksIndirectNV;
extern PFN_vkCmdDrawMeshTasksNV vkCmdDrawMeshTasksNV;
#endif /* defined(VK_NV_mesh_shader) */
#if defined(VK_NV_optical_flow)
extern PFN_vkBindOpticalFlowSessionImageNV vkBindOpticalFlowSessionImageNV;
extern PFN_vkCmdOpticalFlowExecuteNV vkCmdOpticalFlowExecuteNV;
extern PFN_vkCreateOpticalFlowSessionNV vkCreateOpticalFlowSessionNV;
extern PFN_vkDestroyOpticalFlowSessionNV vkDestroyOpticalFlowSessionNV;
extern PFN_vkGetPhysicalDeviceOpticalFlowImageFormatsNV vkGetPhysicalDeviceOpticalFlowImageFormatsNV;
#endif /* defined(VK_NV_optical_flow) */
#if defined(VK_NV_ray_tracing)
extern PFN_vkBindAccelerationStructureMemoryNV vkBindAccelerationStructureMemoryNV;
extern PFN_vkCmdBuildAccelerationStructureNV vkCmdBuildAccelerationStructureNV;
extern PFN_vkCmdCopyAccelerationStructureNV vkCmdCopyAccelerationStructureNV;
extern PFN_vkCmdTraceRaysNV vkCmdTraceRaysNV;
extern PFN_vkCmdWriteAccelerationStructuresPropertiesNV vkCmdWriteAccelerationStructuresPropertiesNV;
extern PFN_vkCompileDeferredNV vkCompileDeferredNV;
extern PFN_vkCreateAccelerationStructureNV vkCreateAccelerationStructureNV;
extern PFN_vkCreateRayTracingPipelinesNV vkCreateRayTracingPipelinesNV;
extern PFN_vkDestroyAccelerationStructureNV vkDestroyAccelerationStructureNV;
extern PFN_vkGetAccelerationStructureHandleNV vkGetAccelerationStructureHandleNV;
extern PFN_vkGetAccelerationStructureMemoryRequirementsNV vkGetAccelerationStructureMemoryRequirementsNV;
extern PFN_vkGetRayTracingShaderGroupHandlesNV vkGetRayTracingShaderGroupHandlesNV;
#endif /* defined(VK_NV_ray_tracing) */
#if defined(VK_NV_scissor_exclusive) && VK_NV_SCISSOR_EXCLUSIVE_SPEC_VERSION >= 2
extern PFN_vkCmdSetExclusiveScissorEnableNV vkCmdSetExclusiveScissorEnableNV;
#endif /* defined(VK_NV_scissor_exclusive) && VK_NV_SCISSOR_EXCLUSIVE_SPEC_VERSION >= 2 */
#if defined(VK_NV_scissor_exclusive)
extern PFN_vkCmdSetExclusiveScissorNV vkCmdSetExclusiveScissorNV;
#endif /* defined(VK_NV_scissor_exclusive) */
#if defined(VK_NV_shading_rate_image)
extern PFN_vkCmdBindShadingRateImageNV vkCmdBindShadingRateImageNV;
extern PFN_vkCmdSetCoarseSampleOrderNV vkCmdSetCoarseSampleOrderNV;
extern PFN_vkCmdSetViewportShadingRatePaletteNV vkCmdSetViewportShadingRatePaletteNV;
#endif /* defined(VK_NV_shading_rate_image) */
#if defined(VK_QCOM_tile_properties)
extern PFN_vkGetDynamicRenderingTilePropertiesQCOM vkGetDynamicRenderingTilePropertiesQCOM;
extern PFN_vkGetFramebufferTilePropertiesQCOM vkGetFramebufferTilePropertiesQCOM;
#endif /* defined(VK_QCOM_tile_properties) */
#if defined(VK_QNX_external_memory_screen_buffer)
extern PFN_vkGetScreenBufferPropertiesQNX vkGetScreenBufferPropertiesQNX;
#endif /* defined(VK_QNX_external_memory_screen_buffer) */
#if defined(VK_QNX_screen_surface)
extern PFN_vkCreateScreenSurfaceQNX vkCreateScreenSurfaceQNX;
extern PFN_vkGetPhysicalDeviceScreenPresentationSupportQNX vkGetPhysicalDeviceScreenPresentationSupportQNX;
#endif /* defined(VK_QNX_screen_surface) */
#if defined(VK_VALVE_descriptor_set_host_mapping)
extern PFN_vkGetDescriptorSetHostMappingVALVE vkGetDescriptorSetHostMappingVALVE;
extern PFN_vkGetDescriptorSetLayoutHostMappingInfoVALVE vkGetDescriptorSetLayoutHostMappingInfoVALVE;
#endif /* defined(VK_VALVE_descriptor_set_host_mapping) */
#if (defined(VK_EXT_extended_dynamic_state)) || (defined(VK_EXT_shader_object))
extern PFN_vkCmdBindVertexBuffers2EXT vkCmdBindVertexBuffers2EXT;
extern PFN_vkCmdSetCullModeEXT vkCmdSetCullModeEXT;
extern PFN_vkCmdSetDepthBoundsTestEnableEXT vkCmdSetDepthBoundsTestEnableEXT;
extern PFN_vkCmdSetDepthCompareOpEXT vkCmdSetDepthCompareOpEXT;
extern PFN_vkCmdSetDepthTestEnableEXT vkCmdSetDepthTestEnableEXT;
extern PFN_vkCmdSetDepthWriteEnableEXT vkCmdSetDepthWriteEnableEXT;
extern PFN_vkCmdSetFrontFaceEXT vkCmdSetFrontFaceEXT;
extern PFN_vkCmdSetPrimitiveTopologyEXT vkCmdSetPrimitiveTopologyEXT;
extern PFN_vkCmdSetScissorWithCountEXT vkCmdSetScissorWithCountEXT;
extern PFN_vkCmdSetStencilOpEXT vkCmdSetStencilOpEXT;
extern PFN_vkCmdSetStencilTestEnableEXT vkCmdSetStencilTestEnableEXT;
extern PFN_vkCmdSetViewportWithCountEXT vkCmdSetViewportWithCountEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state)) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state2)) || (defined(VK_EXT_shader_object))
extern PFN_vkCmdSetDepthBiasEnableEXT vkCmdSetDepthBiasEnableEXT;
extern PFN_vkCmdSetLogicOpEXT vkCmdSetLogicOpEXT;
extern PFN_vkCmdSetPatchControlPointsEXT vkCmdSetPatchControlPointsEXT;
extern PFN_vkCmdSetPrimitiveRestartEnableEXT vkCmdSetPrimitiveRestartEnableEXT;
extern PFN_vkCmdSetRasterizerDiscardEnableEXT vkCmdSetRasterizerDiscardEnableEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state2)) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state3)) || (defined(VK_EXT_shader_object))
extern PFN_vkCmdSetAlphaToCoverageEnableEXT vkCmdSetAlphaToCoverageEnableEXT;
extern PFN_vkCmdSetAlphaToOneEnableEXT vkCmdSetAlphaToOneEnableEXT;
extern PFN_vkCmdSetColorBlendEnableEXT vkCmdSetColorBlendEnableEXT;
extern PFN_vkCmdSetColorBlendEquationEXT vkCmdSetColorBlendEquationEXT;
extern PFN_vkCmdSetColorWriteMaskEXT vkCmdSetColorWriteMaskEXT;
extern PFN_vkCmdSetDepthClampEnableEXT vkCmdSetDepthClampEnableEXT;
extern PFN_vkCmdSetLogicOpEnableEXT vkCmdSetLogicOpEnableEXT;
extern PFN_vkCmdSetPolygonModeEXT vkCmdSetPolygonModeEXT;
extern PFN_vkCmdSetRasterizationSamplesEXT vkCmdSetRasterizationSamplesEXT;
extern PFN_vkCmdSetSampleMaskEXT vkCmdSetSampleMaskEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3)) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state3) && (defined(VK_KHR_maintenance2) || defined(VK_VERSION_1_1))) || (defined(VK_EXT_shader_object))
extern PFN_vkCmdSetTessellationDomainOriginEXT vkCmdSetTessellationDomainOriginEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && (defined(VK_KHR_maintenance2) || defined(VK_VERSION_1_1))) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_transform_feedback)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_transform_feedback))
extern PFN_vkCmdSetRasterizationStreamEXT vkCmdSetRasterizationStreamEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_transform_feedback)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_transform_feedback)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_conservative_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_conservative_rasterization))
extern PFN_vkCmdSetConservativeRasterizationModeEXT vkCmdSetConservativeRasterizationModeEXT;
extern PFN_vkCmdSetExtraPrimitiveOverestimationSizeEXT vkCmdSetExtraPrimitiveOverestimationSizeEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_conservative_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_conservative_rasterization)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_enable)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_enable))
extern PFN_vkCmdSetDepthClipEnableEXT vkCmdSetDepthClipEnableEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_enable)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_enable)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_sample_locations)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_sample_locations))
extern PFN_vkCmdSetSampleLocationsEnableEXT vkCmdSetSampleLocationsEnableEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_sample_locations)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_sample_locations)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_blend_operation_advanced)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_blend_operation_advanced))
extern PFN_vkCmdSetColorBlendAdvancedEXT vkCmdSetColorBlendAdvancedEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_blend_operation_advanced)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_blend_operation_advanced)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_provoking_vertex)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_provoking_vertex))
extern PFN_vkCmdSetProvokingVertexModeEXT vkCmdSetProvokingVertexModeEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_provoking_vertex)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_provoking_vertex)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_line_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_line_rasterization))
extern PFN_vkCmdSetLineRasterizationModeEXT vkCmdSetLineRasterizationModeEXT;
extern PFN_vkCmdSetLineStippleEnableEXT vkCmdSetLineStippleEnableEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_line_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_line_rasterization)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_control)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_control))
extern PFN_vkCmdSetDepthClipNegativeOneToOneEXT vkCmdSetDepthClipNegativeOneToOneEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_control)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_control)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_clip_space_w_scaling)) || (defined(VK_EXT_shader_object) && defined(VK_NV_clip_space_w_scaling))
extern PFN_vkCmdSetViewportWScalingEnableNV vkCmdSetViewportWScalingEnableNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_clip_space_w_scaling)) || (defined(VK_EXT_shader_object) && defined(VK_NV_clip_space_w_scaling)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_viewport_swizzle)) || (defined(VK_EXT_shader_object) && defined(VK_NV_viewport_swizzle))
extern PFN_vkCmdSetViewportSwizzleNV vkCmdSetViewportSwizzleNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_viewport_swizzle)) || (defined(VK_EXT_shader_object) && defined(VK_NV_viewport_swizzle)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_fragment_coverage_to_color)) || (defined(VK_EXT_shader_object) && defined(VK_NV_fragment_coverage_to_color))
extern PFN_vkCmdSetCoverageToColorEnableNV vkCmdSetCoverageToColorEnableNV;
extern PFN_vkCmdSetCoverageToColorLocationNV vkCmdSetCoverageToColorLocationNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_fragment_coverage_to_color)) || (defined(VK_EXT_shader_object) && defined(VK_NV_fragment_coverage_to_color)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_framebuffer_mixed_samples)) || (defined(VK_EXT_shader_object) && defined(VK_NV_framebuffer_mixed_samples))
extern PFN_vkCmdSetCoverageModulationModeNV vkCmdSetCoverageModulationModeNV;
extern PFN_vkCmdSetCoverageModulationTableEnableNV vkCmdSetCoverageModulationTableEnableNV;
extern PFN_vkCmdSetCoverageModulationTableNV vkCmdSetCoverageModulationTableNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_framebuffer_mixed_samples)) || (defined(VK_EXT_shader_object) && defined(VK_NV_framebuffer_mixed_samples)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_shading_rate_image)) || (defined(VK_EXT_shader_object) && defined(VK_NV_shading_rate_image))
extern PFN_vkCmdSetShadingRateImageEnableNV vkCmdSetShadingRateImageEnableNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_shading_rate_image)) || (defined(VK_EXT_shader_object) && defined(VK_NV_shading_rate_image)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_representative_fragment_test)) || (defined(VK_EXT_shader_object) && defined(VK_NV_representative_fragment_test))
extern PFN_vkCmdSetRepresentativeFragmentTestEnableNV vkCmdSetRepresentativeFragmentTestEnableNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_representative_fragment_test)) || (defined(VK_EXT_shader_object) && defined(VK_NV_representative_fragment_test)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_coverage_reduction_mode)) || (defined(VK_EXT_shader_object) && defined(VK_NV_coverage_reduction_mode))
extern PFN_vkCmdSetCoverageReductionModeNV vkCmdSetCoverageReductionModeNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_coverage_reduction_mode)) || (defined(VK_EXT_shader_object) && defined(VK_NV_coverage_reduction_mode)) */
#if (defined(VK_EXT_full_screen_exclusive) && defined(VK_KHR_device_group)) || (defined(VK_EXT_full_screen_exclusive) && defined(VK_VERSION_1_1))
extern PFN_vkGetDeviceGroupSurfacePresentModes2EXT vkGetDeviceGroupSurfacePresentModes2EXT;
#endif /* (defined(VK_EXT_full_screen_exclusive) && defined(VK_KHR_device_group)) || (defined(VK_EXT_full_screen_exclusive) && defined(VK_VERSION_1_1)) */
#if (defined(VK_EXT_host_image_copy)) || (defined(VK_EXT_image_compression_control))
extern PFN_vkGetImageSubresourceLayout2EXT vkGetImageSubresourceLayout2EXT;
#endif /* (defined(VK_EXT_host_image_copy)) || (defined(VK_EXT_image_compression_control)) */
#if (defined(VK_EXT_shader_object)) || (defined(VK_EXT_vertex_input_dynamic_state))
extern PFN_vkCmdSetVertexInputEXT vkCmdSetVertexInputEXT;
#endif /* (defined(VK_EXT_shader_object)) || (defined(VK_EXT_vertex_input_dynamic_state)) */
#if (defined(VK_KHR_descriptor_update_template) && defined(VK_KHR_push_descriptor)) || (defined(VK_KHR_push_descriptor) && defined(VK_VERSION_1_1)) || (defined(VK_KHR_push_descriptor) && defined(VK_KHR_descriptor_update_template))
extern PFN_vkCmdPushDescriptorSetWithTemplateKHR vkCmdPushDescriptorSetWithTemplateKHR;
#endif /* (defined(VK_KHR_descriptor_update_template) && defined(VK_KHR_push_descriptor)) || (defined(VK_KHR_push_descriptor) && defined(VK_VERSION_1_1)) || (defined(VK_KHR_push_descriptor) && defined(VK_KHR_descriptor_update_template)) */
#if (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
extern PFN_vkGetDeviceGroupPresentCapabilitiesKHR vkGetDeviceGroupPresentCapabilitiesKHR;
extern PFN_vkGetDeviceGroupSurfacePresentModesKHR vkGetDeviceGroupSurfacePresentModesKHR;
extern PFN_vkGetPhysicalDevicePresentRectanglesKHR vkGetPhysicalDevicePresentRectanglesKHR;
#endif /* (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1)) */
#if (defined(VK_KHR_device_group) && defined(VK_KHR_swapchain)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
extern PFN_vkAcquireNextImage2KHR vkAcquireNextImage2KHR;
#endif /* (defined(VK_KHR_device_group) && defined(VK_KHR_swapchain)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1)) */
/* VULKANLOADER_GENERATE_PROTOTYPES_H */

#endif /*TRAP_VULKANLOADER_H*/

// For Visual Studio IntelliSense.
#if defined(__cplusplus) && defined(__INTELLISENSE__)
#define VULKANLOADER_IMPLEMENTATION
#endif /*__cplusplus && __INTELLISENSE__*/

#ifdef VULKANLOADER_IMPLEMENTATION
#undef VULKANLOADER_IMPLEMENTATION

#ifdef _WIN32
	using LPCSTR = const char*;
	using HINSTANCE = struct HINSTANCE__*;
	using HMODULE = HINSTANCE;
	#if defined(_MINWINDEF_)
		/*minwindef.h defined FARPROC, and attempting to redefine it may conflict with -Wstrict-prototypes*/
	#elif defined(_WIN64)
		typedef __int64 (__stdcall* FARPROC)(void);
	#else
		typedef i32 (__stdcall* FARPROC)(void);
	#endif
#else
    #include <dlfcn.h>
#endif

#include "Utils/DynamicLoading/DynamicLoading.h"

static void* loadedModule = NULL;
static VkInstance loadedInstance = VK_NULL_HANDLE;
static VkDevice loadedDevice = VK_NULL_HANDLE;

using VkGenLoaderFunction = PFN_vkVoidFunction (*)(void*, std::string_view);

static void VkGenLoadLoader(void* context, VkGenLoaderFunction load);
static void VkGenLoadInstance(VkInstance instance, VkGenLoaderFunction load);
static void VkGenLoadDevice(void* context, VkGenLoaderFunction load);
static void VkGenLoadDeviceTable(VkDeviceTable& table, VkDevice device, VkGenLoaderFunction load);

static PFN_vkVoidFunction vkGetInstanceProcAddrStub(void* context, std::string_view name)
{
	return vkGetInstanceProcAddr(static_cast<VkInstance>(context), name.data());
}

static PFN_vkVoidFunction vkGetDeviceProcAddrStub(void* context, std::string_view name)
{
	return vkGetDeviceProcAddr(static_cast<VkDevice>(context), name.data());
}

static PFN_vkVoidFunction nullProcAddrStub([[maybe_unused]] void* context, [[maybe_unused]] std::string_view name) noexcept
{
	return nullptr;
}

[[nodiscard]] VkResult VkInitialize()
{
#if defined(_WIN32)
	HMODULE module = static_cast<HMODULE>(TRAP::Utils::DynamicLoading::LoadLibrary("vulkan-1.dll"));
#else
	void* module = TRAP::Utils::DynamicLoading::LoadLibrary("libvulkan.so.1");
	if (!module)
		module = TRAP::Utils::DynamicLoading::LoadLibrary("libvulkan.so");
#endif

	if (!module)
		return VK_ERROR_INITIALIZATION_FAILED;

	vkGetInstanceProcAddr = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_vkGetInstanceProcAddr>(static_cast<void*>(module), "vkGetInstanceProcAddr");

	loadedModule = module;
	VkGenLoadLoader(nullptr, vkGetInstanceProcAddrStub);

	return VK_SUCCESS;
}

void VkInitializeCustom(PFN_vkGetInstanceProcAddr handler)
{
	vkGetInstanceProcAddr = handler;

	loadedModule = nullptr;
	VkGenLoadLoader(nullptr, vkGetInstanceProcAddrStub);
}

void VkFinalize()
{
	if(loadedModule)
		TRAP::Utils::DynamicLoading::FreeLibrary(loadedModule);

	vkGetInstanceProcAddr = nullptr;
	VkGenLoadLoader(nullptr, nullProcAddrStub);
	VkGenLoadInstance(VK_NULL_HANDLE, nullProcAddrStub);
	VkGenLoadDevice(VK_NULL_HANDLE, nullProcAddrStub);

	loadedModule = nullptr;
	loadedInstance = VK_NULL_HANDLE;
	loadedDevice = VK_NULL_HANDLE;
}

[[nodiscard]] std::optional<u32> VkGetInstanceVersion()
{
#if defined(VK_VERSION_1_1)
	u32 apiVersion = 0;
	if (vkEnumerateInstanceVersion && vkEnumerateInstanceVersion(&apiVersion) == VK_SUCCESS)
		return apiVersion;
#endif /*VK_VERSION_1_1*/

	if (vkCreateInstance)
		return VK_API_VERSION_1_0;

	return std::nullopt;
}

void VkLoadInstance(VkInstance instance)
{
	loadedInstance = instance;
	VkGenLoadInstance(instance, vkGetInstanceProcAddrStub);
	VkGenLoadDevice(instance, vkGetInstanceProcAddrStub);
}

void VkLoadInstanceOnly(VkInstance instance)
{
    loadedInstance = instance;
    VkGenLoadInstance(instance, vkGetInstanceProcAddrStub);
}

[[nodiscard]] VkInstance VkGetLoadedInstance() noexcept
{
	return loadedInstance;
}

void VkLoadDevice(VkDevice device)
{
	loadedDevice = device;
	VkGenLoadDevice(device, vkGetDeviceProcAddrStub);
}

[[nodiscard]] VkDevice VkGetLoadedDevice() noexcept
{
	return loadedDevice;
}

void VkLoadDeviceTable(VkDeviceTable& table, VkDevice device)
{
	VkGenLoadDeviceTable(table, device, vkGetDeviceProcAddrStub);
}

static void VkGenLoadLoader(void* context, VkGenLoaderFunction load)
{
	/* VULKANLOADER_GENERATE_LOAD_LOADER */
#if defined(VK_VERSION_1_0)
	vkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(load(context, "vkCreateInstance"));
	vkEnumerateInstanceExtensionProperties = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(load(context, "vkEnumerateInstanceExtensionProperties"));
	vkEnumerateInstanceLayerProperties = reinterpret_cast<PFN_vkEnumerateInstanceLayerProperties>(load(context, "vkEnumerateInstanceLayerProperties"));
#endif /* defined(VK_VERSION_1_0) */
#if defined(VK_VERSION_1_1)
	vkEnumerateInstanceVersion = reinterpret_cast<PFN_vkEnumerateInstanceVersion>(load(context, "vkEnumerateInstanceVersion"));
#endif /* defined(VK_VERSION_1_1) */
	/* VULKANLOADER_GENERATE_LOAD_LOADER */
}

static void VkGenLoadInstance(VkInstance instance, VkGenLoaderFunction load)
{
	/* VULKANLOADER_GENERATE_LOAD_INSTANCE */
#if defined(VK_VERSION_1_0)
	vkCreateDevice = reinterpret_cast<PFN_vkCreateDevice>(load(instance, "vkCreateDevice"));
	vkDestroyInstance = reinterpret_cast<PFN_vkDestroyInstance>(load(instance, "vkDestroyInstance"));
	vkEnumerateDeviceExtensionProperties = reinterpret_cast<PFN_vkEnumerateDeviceExtensionProperties>(load(instance, "vkEnumerateDeviceExtensionProperties"));
	vkEnumerateDeviceLayerProperties = reinterpret_cast<PFN_vkEnumerateDeviceLayerProperties>(load(instance, "vkEnumerateDeviceLayerProperties"));
	vkEnumeratePhysicalDevices = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(load(instance, "vkEnumeratePhysicalDevices"));
	vkGetDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(load(instance, "vkGetDeviceProcAddr"));
	vkGetPhysicalDeviceFeatures = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures>(load(instance, "vkGetPhysicalDeviceFeatures"));
	vkGetPhysicalDeviceFormatProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceFormatProperties>(load(instance, "vkGetPhysicalDeviceFormatProperties"));
	vkGetPhysicalDeviceImageFormatProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceImageFormatProperties>(load(instance, "vkGetPhysicalDeviceImageFormatProperties"));
	vkGetPhysicalDeviceMemoryProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties>(load(instance, "vkGetPhysicalDeviceMemoryProperties"));
	vkGetPhysicalDeviceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(load(instance, "vkGetPhysicalDeviceProperties"));
	vkGetPhysicalDeviceQueueFamilyProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties>(load(instance, "vkGetPhysicalDeviceQueueFamilyProperties"));
	vkGetPhysicalDeviceSparseImageFormatProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceSparseImageFormatProperties>(load(instance, "vkGetPhysicalDeviceSparseImageFormatProperties"));
#endif /* defined(VK_VERSION_1_0) */
#if defined(VK_VERSION_1_1)
	vkEnumeratePhysicalDeviceGroups = reinterpret_cast<PFN_vkEnumeratePhysicalDeviceGroups>(load(instance, "vkEnumeratePhysicalDeviceGroups"));
	vkGetPhysicalDeviceExternalBufferProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalBufferProperties>(load(instance, "vkGetPhysicalDeviceExternalBufferProperties"));
	vkGetPhysicalDeviceExternalFenceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalFenceProperties>(load(instance, "vkGetPhysicalDeviceExternalFenceProperties"));
	vkGetPhysicalDeviceExternalSemaphoreProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalSemaphoreProperties>(load(instance, "vkGetPhysicalDeviceExternalSemaphoreProperties"));
	vkGetPhysicalDeviceFeatures2 = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures2>(load(instance, "vkGetPhysicalDeviceFeatures2"));
	vkGetPhysicalDeviceFormatProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceFormatProperties2>(load(instance, "vkGetPhysicalDeviceFormatProperties2"));
	vkGetPhysicalDeviceImageFormatProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceImageFormatProperties2>(load(instance, "vkGetPhysicalDeviceImageFormatProperties2"));
	vkGetPhysicalDeviceMemoryProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties2>(load(instance, "vkGetPhysicalDeviceMemoryProperties2"));
	vkGetPhysicalDeviceProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2>(load(instance, "vkGetPhysicalDeviceProperties2"));
	vkGetPhysicalDeviceQueueFamilyProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties2>(load(instance, "vkGetPhysicalDeviceQueueFamilyProperties2"));
	vkGetPhysicalDeviceSparseImageFormatProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceSparseImageFormatProperties2>(load(instance, "vkGetPhysicalDeviceSparseImageFormatProperties2"));
#endif /* defined(VK_VERSION_1_1) */
#if defined(VK_VERSION_1_3)
	vkGetPhysicalDeviceToolProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceToolProperties>(load(instance, "vkGetPhysicalDeviceToolProperties"));
#endif /* defined(VK_VERSION_1_3) */
#if defined(VK_EXT_acquire_drm_display)
	vkAcquireDrmDisplayEXT = reinterpret_cast<PFN_vkAcquireDrmDisplayEXT>(load(instance, "vkAcquireDrmDisplayEXT"));
	vkGetDrmDisplayEXT = reinterpret_cast<PFN_vkGetDrmDisplayEXT>(load(instance, "vkGetDrmDisplayEXT"));
#endif /* defined(VK_EXT_acquire_drm_display) */
#if defined(VK_EXT_acquire_xlib_display)
	vkAcquireXlibDisplayEXT = reinterpret_cast<PFN_vkAcquireXlibDisplayEXT>(load(instance, "vkAcquireXlibDisplayEXT"));
	vkGetRandROutputDisplayEXT = reinterpret_cast<PFN_vkGetRandROutputDisplayEXT>(load(instance, "vkGetRandROutputDisplayEXT"));
#endif /* defined(VK_EXT_acquire_xlib_display) */
#if defined(VK_EXT_calibrated_timestamps)
	vkGetPhysicalDeviceCalibrateableTimeDomainsEXT = reinterpret_cast<PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT>(load(instance, "vkGetPhysicalDeviceCalibrateableTimeDomainsEXT"));
#endif /* defined(VK_EXT_calibrated_timestamps) */
#if defined(VK_EXT_debug_report)
	vkCreateDebugReportCallbackEXT = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(load(instance, "vkCreateDebugReportCallbackEXT"));
	vkDebugReportMessageEXT = reinterpret_cast<PFN_vkDebugReportMessageEXT>(load(instance, "vkDebugReportMessageEXT"));
	vkDestroyDebugReportCallbackEXT = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(load(instance, "vkDestroyDebugReportCallbackEXT"));
#endif /* defined(VK_EXT_debug_report) */
#if defined(VK_EXT_debug_utils)
	vkCmdBeginDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdBeginDebugUtilsLabelEXT>(load(instance, "vkCmdBeginDebugUtilsLabelEXT"));
	vkCmdEndDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(load(instance, "vkCmdEndDebugUtilsLabelEXT"));
	vkCmdInsertDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdInsertDebugUtilsLabelEXT>(load(instance, "vkCmdInsertDebugUtilsLabelEXT"));
	vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(load(instance, "vkCreateDebugUtilsMessengerEXT"));
	vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(load(instance, "vkDestroyDebugUtilsMessengerEXT"));
	vkQueueBeginDebugUtilsLabelEXT = reinterpret_cast<PFN_vkQueueBeginDebugUtilsLabelEXT>(load(instance, "vkQueueBeginDebugUtilsLabelEXT"));
	vkQueueEndDebugUtilsLabelEXT = reinterpret_cast<PFN_vkQueueEndDebugUtilsLabelEXT>(load(instance, "vkQueueEndDebugUtilsLabelEXT"));
	vkQueueInsertDebugUtilsLabelEXT = reinterpret_cast<PFN_vkQueueInsertDebugUtilsLabelEXT>(load(instance, "vkQueueInsertDebugUtilsLabelEXT"));
	vkSetDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(load(instance, "vkSetDebugUtilsObjectNameEXT"));
	vkSetDebugUtilsObjectTagEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectTagEXT>(load(instance, "vkSetDebugUtilsObjectTagEXT"));
	vkSubmitDebugUtilsMessageEXT = reinterpret_cast<PFN_vkSubmitDebugUtilsMessageEXT>(load(instance, "vkSubmitDebugUtilsMessageEXT"));
#endif /* defined(VK_EXT_debug_utils) */
#if defined(VK_EXT_direct_mode_display)
	vkReleaseDisplayEXT = reinterpret_cast<PFN_vkReleaseDisplayEXT>(load(instance, "vkReleaseDisplayEXT"));
#endif /* defined(VK_EXT_direct_mode_display) */
#if defined(VK_EXT_directfb_surface)
	vkCreateDirectFBSurfaceEXT = reinterpret_cast<PFN_vkCreateDirectFBSurfaceEXT>(load(instance, "vkCreateDirectFBSurfaceEXT"));
	vkGetPhysicalDeviceDirectFBPresentationSupportEXT = reinterpret_cast<PFN_vkGetPhysicalDeviceDirectFBPresentationSupportEXT>(load(instance, "vkGetPhysicalDeviceDirectFBPresentationSupportEXT"));
#endif /* defined(VK_EXT_directfb_surface) */
#if defined(VK_EXT_display_surface_counter)
	vkGetPhysicalDeviceSurfaceCapabilities2EXT = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT>(load(instance, "vkGetPhysicalDeviceSurfaceCapabilities2EXT"));
#endif /* defined(VK_EXT_display_surface_counter) */
#if defined(VK_EXT_full_screen_exclusive)
	vkGetPhysicalDeviceSurfacePresentModes2EXT = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfacePresentModes2EXT>(load(instance, "vkGetPhysicalDeviceSurfacePresentModes2EXT"));
#endif /* defined(VK_EXT_full_screen_exclusive) */
#if defined(VK_EXT_headless_surface)
	vkCreateHeadlessSurfaceEXT = reinterpret_cast<PFN_vkCreateHeadlessSurfaceEXT>(load(instance, "vkCreateHeadlessSurfaceEXT"));
#endif /* defined(VK_EXT_headless_surface) */
#if defined(VK_EXT_metal_surface)
	vkCreateMetalSurfaceEXT = reinterpret_cast<PFN_vkCreateMetalSurfaceEXT>(load(instance, "vkCreateMetalSurfaceEXT"));
#endif /* defined(VK_EXT_metal_surface) */
#if defined(VK_EXT_sample_locations)
	vkGetPhysicalDeviceMultisamplePropertiesEXT = reinterpret_cast<PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT>(load(instance, "vkGetPhysicalDeviceMultisamplePropertiesEXT"));
#endif /* defined(VK_EXT_sample_locations) */
#if defined(VK_EXT_tooling_info)
	vkGetPhysicalDeviceToolPropertiesEXT = reinterpret_cast<PFN_vkGetPhysicalDeviceToolPropertiesEXT>(load(instance, "vkGetPhysicalDeviceToolPropertiesEXT"));
#endif /* defined(VK_EXT_tooling_info) */
#if defined(VK_FUCHSIA_imagepipe_surface)
	vkCreateImagePipeSurfaceFUCHSIA = reinterpret_cast<PFN_vkCreateImagePipeSurfaceFUCHSIA>(load(instance, "vkCreateImagePipeSurfaceFUCHSIA"));
#endif /* defined(VK_FUCHSIA_imagepipe_surface) */
#if defined(VK_GGP_stream_descriptor_surface)
	vkCreateStreamDescriptorSurfaceGGP = reinterpret_cast<PFN_vkCreateStreamDescriptorSurfaceGGP>(load(instance, "vkCreateStreamDescriptorSurfaceGGP"));
#endif /* defined(VK_GGP_stream_descriptor_surface) */
#if defined(VK_KHR_android_surface)
	vkCreateAndroidSurfaceKHR = reinterpret_cast<PFN_vkCreateAndroidSurfaceKHR>(load(instance, "vkCreateAndroidSurfaceKHR"));
#endif /* defined(VK_KHR_android_surface) */
#if defined(VK_KHR_calibrated_timestamps)
	vkGetPhysicalDeviceCalibrateableTimeDomainsKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsKHR>(load(instance, "vkGetPhysicalDeviceCalibrateableTimeDomainsKHR"));
#endif /* defined(VK_KHR_calibrated_timestamps) */
#if defined(VK_KHR_cooperative_matrix)
	vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR>(load(instance, "vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR"));
#endif /* defined(VK_KHR_cooperative_matrix) */
#if defined(VK_KHR_device_group_creation)
	vkEnumeratePhysicalDeviceGroupsKHR = reinterpret_cast<PFN_vkEnumeratePhysicalDeviceGroupsKHR>(load(instance, "vkEnumeratePhysicalDeviceGroupsKHR"));
#endif /* defined(VK_KHR_device_group_creation) */
#if defined(VK_KHR_display)
	vkCreateDisplayModeKHR = reinterpret_cast<PFN_vkCreateDisplayModeKHR>(load(instance, "vkCreateDisplayModeKHR"));
	vkCreateDisplayPlaneSurfaceKHR = reinterpret_cast<PFN_vkCreateDisplayPlaneSurfaceKHR>(load(instance, "vkCreateDisplayPlaneSurfaceKHR"));
	vkGetDisplayModePropertiesKHR = reinterpret_cast<PFN_vkGetDisplayModePropertiesKHR>(load(instance, "vkGetDisplayModePropertiesKHR"));
	vkGetDisplayPlaneCapabilitiesKHR = reinterpret_cast<PFN_vkGetDisplayPlaneCapabilitiesKHR>(load(instance, "vkGetDisplayPlaneCapabilitiesKHR"));
	vkGetDisplayPlaneSupportedDisplaysKHR = reinterpret_cast<PFN_vkGetDisplayPlaneSupportedDisplaysKHR>(load(instance, "vkGetDisplayPlaneSupportedDisplaysKHR"));
	vkGetPhysicalDeviceDisplayPlanePropertiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR>(load(instance, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR"));
	vkGetPhysicalDeviceDisplayPropertiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayPropertiesKHR>(load(instance, "vkGetPhysicalDeviceDisplayPropertiesKHR"));
#endif /* defined(VK_KHR_display) */
#if defined(VK_KHR_external_fence_capabilities)
	vkGetPhysicalDeviceExternalFencePropertiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalFencePropertiesKHR>(load(instance, "vkGetPhysicalDeviceExternalFencePropertiesKHR"));
#endif /* defined(VK_KHR_external_fence_capabilities) */
#if defined(VK_KHR_external_memory_capabilities)
	vkGetPhysicalDeviceExternalBufferPropertiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalBufferPropertiesKHR>(load(instance, "vkGetPhysicalDeviceExternalBufferPropertiesKHR"));
#endif /* defined(VK_KHR_external_memory_capabilities) */
#if defined(VK_KHR_external_semaphore_capabilities)
	vkGetPhysicalDeviceExternalSemaphorePropertiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalSemaphorePropertiesKHR>(load(instance, "vkGetPhysicalDeviceExternalSemaphorePropertiesKHR"));
#endif /* defined(VK_KHR_external_semaphore_capabilities) */
#if defined(VK_KHR_fragment_shading_rate)
	vkGetPhysicalDeviceFragmentShadingRatesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceFragmentShadingRatesKHR>(load(instance, "vkGetPhysicalDeviceFragmentShadingRatesKHR"));
#endif /* defined(VK_KHR_fragment_shading_rate) */
#if defined(VK_KHR_get_display_properties2)
	vkGetDisplayModeProperties2KHR = reinterpret_cast<PFN_vkGetDisplayModeProperties2KHR>(load(instance, "vkGetDisplayModeProperties2KHR"));
	vkGetDisplayPlaneCapabilities2KHR = reinterpret_cast<PFN_vkGetDisplayPlaneCapabilities2KHR>(load(instance, "vkGetDisplayPlaneCapabilities2KHR"));
	vkGetPhysicalDeviceDisplayPlaneProperties2KHR = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR>(load(instance, "vkGetPhysicalDeviceDisplayPlaneProperties2KHR"));
	vkGetPhysicalDeviceDisplayProperties2KHR = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayProperties2KHR>(load(instance, "vkGetPhysicalDeviceDisplayProperties2KHR"));
#endif /* defined(VK_KHR_get_display_properties2) */
#if defined(VK_KHR_get_physical_device_properties2)
	vkGetPhysicalDeviceFeatures2KHR = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures2KHR>(load(instance, "vkGetPhysicalDeviceFeatures2KHR"));
	vkGetPhysicalDeviceFormatProperties2KHR = reinterpret_cast<PFN_vkGetPhysicalDeviceFormatProperties2KHR>(load(instance, "vkGetPhysicalDeviceFormatProperties2KHR"));
	vkGetPhysicalDeviceImageFormatProperties2KHR = reinterpret_cast<PFN_vkGetPhysicalDeviceImageFormatProperties2KHR>(load(instance, "vkGetPhysicalDeviceImageFormatProperties2KHR"));
	vkGetPhysicalDeviceMemoryProperties2KHR = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties2KHR>(load(instance, "vkGetPhysicalDeviceMemoryProperties2KHR"));
	vkGetPhysicalDeviceProperties2KHR = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2KHR>(load(instance, "vkGetPhysicalDeviceProperties2KHR"));
	vkGetPhysicalDeviceQueueFamilyProperties2KHR = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR>(load(instance, "vkGetPhysicalDeviceQueueFamilyProperties2KHR"));
	vkGetPhysicalDeviceSparseImageFormatProperties2KHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSparseImageFormatProperties2KHR>(load(instance, "vkGetPhysicalDeviceSparseImageFormatProperties2KHR"));
#endif /* defined(VK_KHR_get_physical_device_properties2) */
#if defined(VK_KHR_get_surface_capabilities2)
	vkGetPhysicalDeviceSurfaceCapabilities2KHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR>(load(instance, "vkGetPhysicalDeviceSurfaceCapabilities2KHR"));
	vkGetPhysicalDeviceSurfaceFormats2KHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormats2KHR>(load(instance, "vkGetPhysicalDeviceSurfaceFormats2KHR"));
#endif /* defined(VK_KHR_get_surface_capabilities2) */
#if defined(VK_KHR_performance_query)
	vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR = reinterpret_cast<PFN_vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR>(load(instance, "vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR"));
	vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR>(load(instance, "vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR"));
#endif /* defined(VK_KHR_performance_query) */
#if defined(VK_KHR_surface)
	vkDestroySurfaceKHR = reinterpret_cast<PFN_vkDestroySurfaceKHR>(load(instance, "vkDestroySurfaceKHR"));
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(load(instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
	vkGetPhysicalDeviceSurfaceFormatsKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(load(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR"));
	vkGetPhysicalDeviceSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfacePresentModesKHR>(load(instance, "vkGetPhysicalDeviceSurfacePresentModesKHR"));
	vkGetPhysicalDeviceSurfaceSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceSupportKHR>(load(instance, "vkGetPhysicalDeviceSurfaceSupportKHR"));
#endif /* defined(VK_KHR_surface) */
#if defined(VK_KHR_video_encode_queue)
	vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR>(load(instance, "vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR"));
#endif /* defined(VK_KHR_video_encode_queue) */
#if defined(VK_KHR_video_queue)
	vkGetPhysicalDeviceVideoCapabilitiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR>(load(instance, "vkGetPhysicalDeviceVideoCapabilitiesKHR"));
	vkGetPhysicalDeviceVideoFormatPropertiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR>(load(instance, "vkGetPhysicalDeviceVideoFormatPropertiesKHR"));
#endif /* defined(VK_KHR_video_queue) */
#if defined(VK_KHR_wayland_surface)
	vkCreateWaylandSurfaceKHR = reinterpret_cast<PFN_vkCreateWaylandSurfaceKHR>(load(instance, "vkCreateWaylandSurfaceKHR"));
	vkGetPhysicalDeviceWaylandPresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR>(load(instance, "vkGetPhysicalDeviceWaylandPresentationSupportKHR"));
#endif /* defined(VK_KHR_wayland_surface) */
#if defined(VK_KHR_win32_surface)
	vkCreateWin32SurfaceKHR = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(load(instance, "vkCreateWin32SurfaceKHR"));
	vkGetPhysicalDeviceWin32PresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR>(load(instance, "vkGetPhysicalDeviceWin32PresentationSupportKHR"));
#endif /* defined(VK_KHR_win32_surface) */
#if defined(VK_KHR_xcb_surface)
	vkCreateXcbSurfaceKHR = reinterpret_cast<PFN_vkCreateXcbSurfaceKHR>(load(instance, "vkCreateXcbSurfaceKHR"));
	vkGetPhysicalDeviceXcbPresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR>(load(instance, "vkGetPhysicalDeviceXcbPresentationSupportKHR"));
#endif /* defined(VK_KHR_xcb_surface) */
#if defined(VK_KHR_xlib_surface)
	vkCreateXlibSurfaceKHR = reinterpret_cast<PFN_vkCreateXlibSurfaceKHR>(load(instance, "vkCreateXlibSurfaceKHR"));
	vkGetPhysicalDeviceXlibPresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR>(load(instance, "vkGetPhysicalDeviceXlibPresentationSupportKHR"));
#endif /* defined(VK_KHR_xlib_surface) */
#if defined(VK_MVK_ios_surface)
	vkCreateIOSSurfaceMVK = reinterpret_cast<PFN_vkCreateIOSSurfaceMVK>(load(instance, "vkCreateIOSSurfaceMVK"));
#endif /* defined(VK_MVK_ios_surface) */
#if defined(VK_MVK_macos_surface)
	vkCreateMacOSSurfaceMVK = reinterpret_cast<PFN_vkCreateMacOSSurfaceMVK>(load(instance, "vkCreateMacOSSurfaceMVK"));
#endif /* defined(VK_MVK_macos_surface) */
#if defined(VK_NN_vi_surface)
	vkCreateViSurfaceNN = reinterpret_cast<PFN_vkCreateViSurfaceNN>(load(instance, "vkCreateViSurfaceNN"));
#endif /* defined(VK_NN_vi_surface) */
#if defined(VK_NV_acquire_winrt_display)
	vkAcquireWinrtDisplayNV = reinterpret_cast<PFN_vkAcquireWinrtDisplayNV>(load(instance, "vkAcquireWinrtDisplayNV"));
	vkGetWinrtDisplayNV = reinterpret_cast<PFN_vkGetWinrtDisplayNV>(load(instance, "vkGetWinrtDisplayNV"));
#endif /* defined(VK_NV_acquire_winrt_display) */
#if defined(VK_NV_cooperative_matrix)
	vkGetPhysicalDeviceCooperativeMatrixPropertiesNV = reinterpret_cast<PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV>(load(instance, "vkGetPhysicalDeviceCooperativeMatrixPropertiesNV"));
#endif /* defined(VK_NV_cooperative_matrix) */
#if defined(VK_NV_coverage_reduction_mode)
	vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV = reinterpret_cast<PFN_vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV>(load(instance, "vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV"));
#endif /* defined(VK_NV_coverage_reduction_mode) */
#if defined(VK_NV_external_memory_capabilities)
	vkGetPhysicalDeviceExternalImageFormatPropertiesNV = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalImageFormatPropertiesNV>(load(instance, "vkGetPhysicalDeviceExternalImageFormatPropertiesNV"));
#endif /* defined(VK_NV_external_memory_capabilities) */
#if defined(VK_NV_optical_flow)
	vkGetPhysicalDeviceOpticalFlowImageFormatsNV = reinterpret_cast<PFN_vkGetPhysicalDeviceOpticalFlowImageFormatsNV>(load(instance, "vkGetPhysicalDeviceOpticalFlowImageFormatsNV"));
#endif /* defined(VK_NV_optical_flow) */
#if defined(VK_QNX_screen_surface)
	vkCreateScreenSurfaceQNX = reinterpret_cast<PFN_vkCreateScreenSurfaceQNX>(load(instance, "vkCreateScreenSurfaceQNX"));
	vkGetPhysicalDeviceScreenPresentationSupportQNX = reinterpret_cast<PFN_vkGetPhysicalDeviceScreenPresentationSupportQNX>(load(instance, "vkGetPhysicalDeviceScreenPresentationSupportQNX"));
#endif /* defined(VK_QNX_screen_surface) */
#if (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
	vkGetPhysicalDevicePresentRectanglesKHR = reinterpret_cast<PFN_vkGetPhysicalDevicePresentRectanglesKHR>(load(instance, "vkGetPhysicalDevicePresentRectanglesKHR"));
#endif /* (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1)) */
	/* VULKANLOADER_GENERATE_LOAD_INSTANCE */
}

static void VkGenLoadDevice(void* context, VkGenLoaderFunction load)
{
	/* VULKANLOADER_GENERATE_LOAD_DEVICE */
#if defined(VK_VERSION_1_0)
	vkAllocateCommandBuffers = reinterpret_cast<PFN_vkAllocateCommandBuffers>(load(context, "vkAllocateCommandBuffers"));
	vkAllocateDescriptorSets = reinterpret_cast<PFN_vkAllocateDescriptorSets>(load(context, "vkAllocateDescriptorSets"));
	vkAllocateMemory = reinterpret_cast<PFN_vkAllocateMemory>(load(context, "vkAllocateMemory"));
	vkBeginCommandBuffer = reinterpret_cast<PFN_vkBeginCommandBuffer>(load(context, "vkBeginCommandBuffer"));
	vkBindBufferMemory = reinterpret_cast<PFN_vkBindBufferMemory>(load(context, "vkBindBufferMemory"));
	vkBindImageMemory = reinterpret_cast<PFN_vkBindImageMemory>(load(context, "vkBindImageMemory"));
	vkCmdBeginQuery = reinterpret_cast<PFN_vkCmdBeginQuery>(load(context, "vkCmdBeginQuery"));
	vkCmdBeginRenderPass = reinterpret_cast<PFN_vkCmdBeginRenderPass>(load(context, "vkCmdBeginRenderPass"));
	vkCmdBindDescriptorSets = reinterpret_cast<PFN_vkCmdBindDescriptorSets>(load(context, "vkCmdBindDescriptorSets"));
	vkCmdBindIndexBuffer = reinterpret_cast<PFN_vkCmdBindIndexBuffer>(load(context, "vkCmdBindIndexBuffer"));
	vkCmdBindPipeline = reinterpret_cast<PFN_vkCmdBindPipeline>(load(context, "vkCmdBindPipeline"));
	vkCmdBindVertexBuffers = reinterpret_cast<PFN_vkCmdBindVertexBuffers>(load(context, "vkCmdBindVertexBuffers"));
	vkCmdBlitImage = reinterpret_cast<PFN_vkCmdBlitImage>(load(context, "vkCmdBlitImage"));
	vkCmdClearAttachments = reinterpret_cast<PFN_vkCmdClearAttachments>(load(context, "vkCmdClearAttachments"));
	vkCmdClearColorImage = reinterpret_cast<PFN_vkCmdClearColorImage>(load(context, "vkCmdClearColorImage"));
	vkCmdClearDepthStencilImage = reinterpret_cast<PFN_vkCmdClearDepthStencilImage>(load(context, "vkCmdClearDepthStencilImage"));
	vkCmdCopyBuffer = reinterpret_cast<PFN_vkCmdCopyBuffer>(load(context, "vkCmdCopyBuffer"));
	vkCmdCopyBufferToImage = reinterpret_cast<PFN_vkCmdCopyBufferToImage>(load(context, "vkCmdCopyBufferToImage"));
	vkCmdCopyImage = reinterpret_cast<PFN_vkCmdCopyImage>(load(context, "vkCmdCopyImage"));
	vkCmdCopyImageToBuffer = reinterpret_cast<PFN_vkCmdCopyImageToBuffer>(load(context, "vkCmdCopyImageToBuffer"));
	vkCmdCopyQueryPoolResults = reinterpret_cast<PFN_vkCmdCopyQueryPoolResults>(load(context, "vkCmdCopyQueryPoolResults"));
	vkCmdDispatch = reinterpret_cast<PFN_vkCmdDispatch>(load(context, "vkCmdDispatch"));
	vkCmdDispatchIndirect = reinterpret_cast<PFN_vkCmdDispatchIndirect>(load(context, "vkCmdDispatchIndirect"));
	vkCmdDraw = reinterpret_cast<PFN_vkCmdDraw>(load(context, "vkCmdDraw"));
	vkCmdDrawIndexed = reinterpret_cast<PFN_vkCmdDrawIndexed>(load(context, "vkCmdDrawIndexed"));
	vkCmdDrawIndexedIndirect = reinterpret_cast<PFN_vkCmdDrawIndexedIndirect>(load(context, "vkCmdDrawIndexedIndirect"));
	vkCmdDrawIndirect = reinterpret_cast<PFN_vkCmdDrawIndirect>(load(context, "vkCmdDrawIndirect"));
	vkCmdEndQuery = reinterpret_cast<PFN_vkCmdEndQuery>(load(context, "vkCmdEndQuery"));
	vkCmdEndRenderPass = reinterpret_cast<PFN_vkCmdEndRenderPass>(load(context, "vkCmdEndRenderPass"));
	vkCmdExecuteCommands = reinterpret_cast<PFN_vkCmdExecuteCommands>(load(context, "vkCmdExecuteCommands"));
	vkCmdFillBuffer = reinterpret_cast<PFN_vkCmdFillBuffer>(load(context, "vkCmdFillBuffer"));
	vkCmdNextSubpass = reinterpret_cast<PFN_vkCmdNextSubpass>(load(context, "vkCmdNextSubpass"));
	vkCmdPipelineBarrier = reinterpret_cast<PFN_vkCmdPipelineBarrier>(load(context, "vkCmdPipelineBarrier"));
	vkCmdPushConstants = reinterpret_cast<PFN_vkCmdPushConstants>(load(context, "vkCmdPushConstants"));
	vkCmdResetEvent = reinterpret_cast<PFN_vkCmdResetEvent>(load(context, "vkCmdResetEvent"));
	vkCmdResetQueryPool = reinterpret_cast<PFN_vkCmdResetQueryPool>(load(context, "vkCmdResetQueryPool"));
	vkCmdResolveImage = reinterpret_cast<PFN_vkCmdResolveImage>(load(context, "vkCmdResolveImage"));
	vkCmdSetBlendConstants = reinterpret_cast<PFN_vkCmdSetBlendConstants>(load(context, "vkCmdSetBlendConstants"));
	vkCmdSetDepthBias = reinterpret_cast<PFN_vkCmdSetDepthBias>(load(context, "vkCmdSetDepthBias"));
	vkCmdSetDepthBounds = reinterpret_cast<PFN_vkCmdSetDepthBounds>(load(context, "vkCmdSetDepthBounds"));
	vkCmdSetEvent = reinterpret_cast<PFN_vkCmdSetEvent>(load(context, "vkCmdSetEvent"));
	vkCmdSetLineWidth = reinterpret_cast<PFN_vkCmdSetLineWidth>(load(context, "vkCmdSetLineWidth"));
	vkCmdSetScissor = reinterpret_cast<PFN_vkCmdSetScissor>(load(context, "vkCmdSetScissor"));
	vkCmdSetStencilCompareMask = reinterpret_cast<PFN_vkCmdSetStencilCompareMask>(load(context, "vkCmdSetStencilCompareMask"));
	vkCmdSetStencilReference = reinterpret_cast<PFN_vkCmdSetStencilReference>(load(context, "vkCmdSetStencilReference"));
	vkCmdSetStencilWriteMask = reinterpret_cast<PFN_vkCmdSetStencilWriteMask>(load(context, "vkCmdSetStencilWriteMask"));
	vkCmdSetViewport = reinterpret_cast<PFN_vkCmdSetViewport>(load(context, "vkCmdSetViewport"));
	vkCmdUpdateBuffer = reinterpret_cast<PFN_vkCmdUpdateBuffer>(load(context, "vkCmdUpdateBuffer"));
	vkCmdWaitEvents = reinterpret_cast<PFN_vkCmdWaitEvents>(load(context, "vkCmdWaitEvents"));
	vkCmdWriteTimestamp = reinterpret_cast<PFN_vkCmdWriteTimestamp>(load(context, "vkCmdWriteTimestamp"));
	vkCreateBuffer = reinterpret_cast<PFN_vkCreateBuffer>(load(context, "vkCreateBuffer"));
	vkCreateBufferView = reinterpret_cast<PFN_vkCreateBufferView>(load(context, "vkCreateBufferView"));
	vkCreateCommandPool = reinterpret_cast<PFN_vkCreateCommandPool>(load(context, "vkCreateCommandPool"));
	vkCreateComputePipelines = reinterpret_cast<PFN_vkCreateComputePipelines>(load(context, "vkCreateComputePipelines"));
	vkCreateDescriptorPool = reinterpret_cast<PFN_vkCreateDescriptorPool>(load(context, "vkCreateDescriptorPool"));
	vkCreateDescriptorSetLayout = reinterpret_cast<PFN_vkCreateDescriptorSetLayout>(load(context, "vkCreateDescriptorSetLayout"));
	vkCreateEvent = reinterpret_cast<PFN_vkCreateEvent>(load(context, "vkCreateEvent"));
	vkCreateFence = reinterpret_cast<PFN_vkCreateFence>(load(context, "vkCreateFence"));
	vkCreateFramebuffer = reinterpret_cast<PFN_vkCreateFramebuffer>(load(context, "vkCreateFramebuffer"));
	vkCreateGraphicsPipelines = reinterpret_cast<PFN_vkCreateGraphicsPipelines>(load(context, "vkCreateGraphicsPipelines"));
	vkCreateImage = reinterpret_cast<PFN_vkCreateImage>(load(context, "vkCreateImage"));
	vkCreateImageView = reinterpret_cast<PFN_vkCreateImageView>(load(context, "vkCreateImageView"));
	vkCreatePipelineCache = reinterpret_cast<PFN_vkCreatePipelineCache>(load(context, "vkCreatePipelineCache"));
	vkCreatePipelineLayout = reinterpret_cast<PFN_vkCreatePipelineLayout>(load(context, "vkCreatePipelineLayout"));
	vkCreateQueryPool = reinterpret_cast<PFN_vkCreateQueryPool>(load(context, "vkCreateQueryPool"));
	vkCreateRenderPass = reinterpret_cast<PFN_vkCreateRenderPass>(load(context, "vkCreateRenderPass"));
	vkCreateSampler = reinterpret_cast<PFN_vkCreateSampler>(load(context, "vkCreateSampler"));
	vkCreateSemaphore = reinterpret_cast<PFN_vkCreateSemaphore>(load(context, "vkCreateSemaphore"));
	vkCreateShaderModule = reinterpret_cast<PFN_vkCreateShaderModule>(load(context, "vkCreateShaderModule"));
	vkDestroyBuffer = reinterpret_cast<PFN_vkDestroyBuffer>(load(context, "vkDestroyBuffer"));
	vkDestroyBufferView = reinterpret_cast<PFN_vkDestroyBufferView>(load(context, "vkDestroyBufferView"));
	vkDestroyCommandPool = reinterpret_cast<PFN_vkDestroyCommandPool>(load(context, "vkDestroyCommandPool"));
	vkDestroyDescriptorPool = reinterpret_cast<PFN_vkDestroyDescriptorPool>(load(context, "vkDestroyDescriptorPool"));
	vkDestroyDescriptorSetLayout = reinterpret_cast<PFN_vkDestroyDescriptorSetLayout>(load(context, "vkDestroyDescriptorSetLayout"));
	vkDestroyDevice = reinterpret_cast<PFN_vkDestroyDevice>(load(context, "vkDestroyDevice"));
	vkDestroyEvent = reinterpret_cast<PFN_vkDestroyEvent>(load(context, "vkDestroyEvent"));
	vkDestroyFence = reinterpret_cast<PFN_vkDestroyFence>(load(context, "vkDestroyFence"));
	vkDestroyFramebuffer = reinterpret_cast<PFN_vkDestroyFramebuffer>(load(context, "vkDestroyFramebuffer"));
	vkDestroyImage = reinterpret_cast<PFN_vkDestroyImage>(load(context, "vkDestroyImage"));
	vkDestroyImageView = reinterpret_cast<PFN_vkDestroyImageView>(load(context, "vkDestroyImageView"));
	vkDestroyPipeline = reinterpret_cast<PFN_vkDestroyPipeline>(load(context, "vkDestroyPipeline"));
	vkDestroyPipelineCache = reinterpret_cast<PFN_vkDestroyPipelineCache>(load(context, "vkDestroyPipelineCache"));
	vkDestroyPipelineLayout = reinterpret_cast<PFN_vkDestroyPipelineLayout>(load(context, "vkDestroyPipelineLayout"));
	vkDestroyQueryPool = reinterpret_cast<PFN_vkDestroyQueryPool>(load(context, "vkDestroyQueryPool"));
	vkDestroyRenderPass = reinterpret_cast<PFN_vkDestroyRenderPass>(load(context, "vkDestroyRenderPass"));
	vkDestroySampler = reinterpret_cast<PFN_vkDestroySampler>(load(context, "vkDestroySampler"));
	vkDestroySemaphore = reinterpret_cast<PFN_vkDestroySemaphore>(load(context, "vkDestroySemaphore"));
	vkDestroyShaderModule = reinterpret_cast<PFN_vkDestroyShaderModule>(load(context, "vkDestroyShaderModule"));
	vkDeviceWaitIdle = reinterpret_cast<PFN_vkDeviceWaitIdle>(load(context, "vkDeviceWaitIdle"));
	vkEndCommandBuffer = reinterpret_cast<PFN_vkEndCommandBuffer>(load(context, "vkEndCommandBuffer"));
	vkFlushMappedMemoryRanges = reinterpret_cast<PFN_vkFlushMappedMemoryRanges>(load(context, "vkFlushMappedMemoryRanges"));
	vkFreeCommandBuffers = reinterpret_cast<PFN_vkFreeCommandBuffers>(load(context, "vkFreeCommandBuffers"));
	vkFreeDescriptorSets = reinterpret_cast<PFN_vkFreeDescriptorSets>(load(context, "vkFreeDescriptorSets"));
	vkFreeMemory = reinterpret_cast<PFN_vkFreeMemory>(load(context, "vkFreeMemory"));
	vkGetBufferMemoryRequirements = reinterpret_cast<PFN_vkGetBufferMemoryRequirements>(load(context, "vkGetBufferMemoryRequirements"));
	vkGetDeviceMemoryCommitment = reinterpret_cast<PFN_vkGetDeviceMemoryCommitment>(load(context, "vkGetDeviceMemoryCommitment"));
	vkGetDeviceQueue = reinterpret_cast<PFN_vkGetDeviceQueue>(load(context, "vkGetDeviceQueue"));
	vkGetEventStatus = reinterpret_cast<PFN_vkGetEventStatus>(load(context, "vkGetEventStatus"));
	vkGetFenceStatus = reinterpret_cast<PFN_vkGetFenceStatus>(load(context, "vkGetFenceStatus"));
	vkGetImageMemoryRequirements = reinterpret_cast<PFN_vkGetImageMemoryRequirements>(load(context, "vkGetImageMemoryRequirements"));
	vkGetImageSparseMemoryRequirements = reinterpret_cast<PFN_vkGetImageSparseMemoryRequirements>(load(context, "vkGetImageSparseMemoryRequirements"));
	vkGetImageSubresourceLayout = reinterpret_cast<PFN_vkGetImageSubresourceLayout>(load(context, "vkGetImageSubresourceLayout"));
	vkGetPipelineCacheData = reinterpret_cast<PFN_vkGetPipelineCacheData>(load(context, "vkGetPipelineCacheData"));
	vkGetQueryPoolResults = reinterpret_cast<PFN_vkGetQueryPoolResults>(load(context, "vkGetQueryPoolResults"));
	vkGetRenderAreaGranularity = reinterpret_cast<PFN_vkGetRenderAreaGranularity>(load(context, "vkGetRenderAreaGranularity"));
	vkInvalidateMappedMemoryRanges = reinterpret_cast<PFN_vkInvalidateMappedMemoryRanges>(load(context, "vkInvalidateMappedMemoryRanges"));
	vkMapMemory = reinterpret_cast<PFN_vkMapMemory>(load(context, "vkMapMemory"));
	vkMergePipelineCaches = reinterpret_cast<PFN_vkMergePipelineCaches>(load(context, "vkMergePipelineCaches"));
	vkQueueBindSparse = reinterpret_cast<PFN_vkQueueBindSparse>(load(context, "vkQueueBindSparse"));
	vkQueueSubmit = reinterpret_cast<PFN_vkQueueSubmit>(load(context, "vkQueueSubmit"));
	vkQueueWaitIdle = reinterpret_cast<PFN_vkQueueWaitIdle>(load(context, "vkQueueWaitIdle"));
	vkResetCommandBuffer = reinterpret_cast<PFN_vkResetCommandBuffer>(load(context, "vkResetCommandBuffer"));
	vkResetCommandPool = reinterpret_cast<PFN_vkResetCommandPool>(load(context, "vkResetCommandPool"));
	vkResetDescriptorPool = reinterpret_cast<PFN_vkResetDescriptorPool>(load(context, "vkResetDescriptorPool"));
	vkResetEvent = reinterpret_cast<PFN_vkResetEvent>(load(context, "vkResetEvent"));
	vkResetFences = reinterpret_cast<PFN_vkResetFences>(load(context, "vkResetFences"));
	vkSetEvent = reinterpret_cast<PFN_vkSetEvent>(load(context, "vkSetEvent"));
	vkUnmapMemory = reinterpret_cast<PFN_vkUnmapMemory>(load(context, "vkUnmapMemory"));
	vkUpdateDescriptorSets = reinterpret_cast<PFN_vkUpdateDescriptorSets>(load(context, "vkUpdateDescriptorSets"));
	vkWaitForFences = reinterpret_cast<PFN_vkWaitForFences>(load(context, "vkWaitForFences"));
#endif /* defined(VK_VERSION_1_0) */
#if defined(VK_VERSION_1_1)
	vkBindBufferMemory2 = reinterpret_cast<PFN_vkBindBufferMemory2>(load(context, "vkBindBufferMemory2"));
	vkBindImageMemory2 = reinterpret_cast<PFN_vkBindImageMemory2>(load(context, "vkBindImageMemory2"));
	vkCmdDispatchBase = reinterpret_cast<PFN_vkCmdDispatchBase>(load(context, "vkCmdDispatchBase"));
	vkCmdSetDeviceMask = reinterpret_cast<PFN_vkCmdSetDeviceMask>(load(context, "vkCmdSetDeviceMask"));
	vkCreateDescriptorUpdateTemplate = reinterpret_cast<PFN_vkCreateDescriptorUpdateTemplate>(load(context, "vkCreateDescriptorUpdateTemplate"));
	vkCreateSamplerYcbcrConversion = reinterpret_cast<PFN_vkCreateSamplerYcbcrConversion>(load(context, "vkCreateSamplerYcbcrConversion"));
	vkDestroyDescriptorUpdateTemplate = reinterpret_cast<PFN_vkDestroyDescriptorUpdateTemplate>(load(context, "vkDestroyDescriptorUpdateTemplate"));
	vkDestroySamplerYcbcrConversion = reinterpret_cast<PFN_vkDestroySamplerYcbcrConversion>(load(context, "vkDestroySamplerYcbcrConversion"));
	vkGetBufferMemoryRequirements2 = reinterpret_cast<PFN_vkGetBufferMemoryRequirements2>(load(context, "vkGetBufferMemoryRequirements2"));
	vkGetDescriptorSetLayoutSupport = reinterpret_cast<PFN_vkGetDescriptorSetLayoutSupport>(load(context, "vkGetDescriptorSetLayoutSupport"));
	vkGetDeviceGroupPeerMemoryFeatures = reinterpret_cast<PFN_vkGetDeviceGroupPeerMemoryFeatures>(load(context, "vkGetDeviceGroupPeerMemoryFeatures"));
	vkGetDeviceQueue2 = reinterpret_cast<PFN_vkGetDeviceQueue2>(load(context, "vkGetDeviceQueue2"));
	vkGetImageMemoryRequirements2 = reinterpret_cast<PFN_vkGetImageMemoryRequirements2>(load(context, "vkGetImageMemoryRequirements2"));
	vkGetImageSparseMemoryRequirements2 = reinterpret_cast<PFN_vkGetImageSparseMemoryRequirements2>(load(context, "vkGetImageSparseMemoryRequirements2"));
	vkTrimCommandPool = reinterpret_cast<PFN_vkTrimCommandPool>(load(context, "vkTrimCommandPool"));
	vkUpdateDescriptorSetWithTemplate = reinterpret_cast<PFN_vkUpdateDescriptorSetWithTemplate>(load(context, "vkUpdateDescriptorSetWithTemplate"));
#endif /* defined(VK_VERSION_1_1) */
#if defined(VK_VERSION_1_2)
	vkCmdBeginRenderPass2 = reinterpret_cast<PFN_vkCmdBeginRenderPass2>(load(context, "vkCmdBeginRenderPass2"));
	vkCmdDrawIndexedIndirectCount = reinterpret_cast<PFN_vkCmdDrawIndexedIndirectCount>(load(context, "vkCmdDrawIndexedIndirectCount"));
	vkCmdDrawIndirectCount = reinterpret_cast<PFN_vkCmdDrawIndirectCount>(load(context, "vkCmdDrawIndirectCount"));
	vkCmdEndRenderPass2 = reinterpret_cast<PFN_vkCmdEndRenderPass2>(load(context, "vkCmdEndRenderPass2"));
	vkCmdNextSubpass2 = reinterpret_cast<PFN_vkCmdNextSubpass2>(load(context, "vkCmdNextSubpass2"));
	vkCreateRenderPass2 = reinterpret_cast<PFN_vkCreateRenderPass2>(load(context, "vkCreateRenderPass2"));
	vkGetBufferDeviceAddress = reinterpret_cast<PFN_vkGetBufferDeviceAddress>(load(context, "vkGetBufferDeviceAddress"));
	vkGetBufferOpaqueCaptureAddress = reinterpret_cast<PFN_vkGetBufferOpaqueCaptureAddress>(load(context, "vkGetBufferOpaqueCaptureAddress"));
	vkGetDeviceMemoryOpaqueCaptureAddress = reinterpret_cast<PFN_vkGetDeviceMemoryOpaqueCaptureAddress>(load(context, "vkGetDeviceMemoryOpaqueCaptureAddress"));
	vkGetSemaphoreCounterValue = reinterpret_cast<PFN_vkGetSemaphoreCounterValue>(load(context, "vkGetSemaphoreCounterValue"));
	vkResetQueryPool = reinterpret_cast<PFN_vkResetQueryPool>(load(context, "vkResetQueryPool"));
	vkSignalSemaphore = reinterpret_cast<PFN_vkSignalSemaphore>(load(context, "vkSignalSemaphore"));
	vkWaitSemaphores = reinterpret_cast<PFN_vkWaitSemaphores>(load(context, "vkWaitSemaphores"));
#endif /* defined(VK_VERSION_1_2) */
#if defined(VK_VERSION_1_3)
	vkCmdBeginRendering = reinterpret_cast<PFN_vkCmdBeginRendering>(load(context, "vkCmdBeginRendering"));
	vkCmdBindVertexBuffers2 = reinterpret_cast<PFN_vkCmdBindVertexBuffers2>(load(context, "vkCmdBindVertexBuffers2"));
	vkCmdBlitImage2 = reinterpret_cast<PFN_vkCmdBlitImage2>(load(context, "vkCmdBlitImage2"));
	vkCmdCopyBuffer2 = reinterpret_cast<PFN_vkCmdCopyBuffer2>(load(context, "vkCmdCopyBuffer2"));
	vkCmdCopyBufferToImage2 = reinterpret_cast<PFN_vkCmdCopyBufferToImage2>(load(context, "vkCmdCopyBufferToImage2"));
	vkCmdCopyImage2 = reinterpret_cast<PFN_vkCmdCopyImage2>(load(context, "vkCmdCopyImage2"));
	vkCmdCopyImageToBuffer2 = reinterpret_cast<PFN_vkCmdCopyImageToBuffer2>(load(context, "vkCmdCopyImageToBuffer2"));
	vkCmdEndRendering = reinterpret_cast<PFN_vkCmdEndRendering>(load(context, "vkCmdEndRendering"));
	vkCmdPipelineBarrier2 = reinterpret_cast<PFN_vkCmdPipelineBarrier2>(load(context, "vkCmdPipelineBarrier2"));
	vkCmdResetEvent2 = reinterpret_cast<PFN_vkCmdResetEvent2>(load(context, "vkCmdResetEvent2"));
	vkCmdResolveImage2 = reinterpret_cast<PFN_vkCmdResolveImage2>(load(context, "vkCmdResolveImage2"));
	vkCmdSetCullMode = reinterpret_cast<PFN_vkCmdSetCullMode>(load(context, "vkCmdSetCullMode"));
	vkCmdSetDepthBiasEnable = reinterpret_cast<PFN_vkCmdSetDepthBiasEnable>(load(context, "vkCmdSetDepthBiasEnable"));
	vkCmdSetDepthBoundsTestEnable = reinterpret_cast<PFN_vkCmdSetDepthBoundsTestEnable>(load(context, "vkCmdSetDepthBoundsTestEnable"));
	vkCmdSetDepthCompareOp = reinterpret_cast<PFN_vkCmdSetDepthCompareOp>(load(context, "vkCmdSetDepthCompareOp"));
	vkCmdSetDepthTestEnable = reinterpret_cast<PFN_vkCmdSetDepthTestEnable>(load(context, "vkCmdSetDepthTestEnable"));
	vkCmdSetDepthWriteEnable = reinterpret_cast<PFN_vkCmdSetDepthWriteEnable>(load(context, "vkCmdSetDepthWriteEnable"));
	vkCmdSetEvent2 = reinterpret_cast<PFN_vkCmdSetEvent2>(load(context, "vkCmdSetEvent2"));
	vkCmdSetFrontFace = reinterpret_cast<PFN_vkCmdSetFrontFace>(load(context, "vkCmdSetFrontFace"));
	vkCmdSetPrimitiveRestartEnable = reinterpret_cast<PFN_vkCmdSetPrimitiveRestartEnable>(load(context, "vkCmdSetPrimitiveRestartEnable"));
	vkCmdSetPrimitiveTopology = reinterpret_cast<PFN_vkCmdSetPrimitiveTopology>(load(context, "vkCmdSetPrimitiveTopology"));
	vkCmdSetRasterizerDiscardEnable = reinterpret_cast<PFN_vkCmdSetRasterizerDiscardEnable>(load(context, "vkCmdSetRasterizerDiscardEnable"));
	vkCmdSetScissorWithCount = reinterpret_cast<PFN_vkCmdSetScissorWithCount>(load(context, "vkCmdSetScissorWithCount"));
	vkCmdSetStencilOp = reinterpret_cast<PFN_vkCmdSetStencilOp>(load(context, "vkCmdSetStencilOp"));
	vkCmdSetStencilTestEnable = reinterpret_cast<PFN_vkCmdSetStencilTestEnable>(load(context, "vkCmdSetStencilTestEnable"));
	vkCmdSetViewportWithCount = reinterpret_cast<PFN_vkCmdSetViewportWithCount>(load(context, "vkCmdSetViewportWithCount"));
	vkCmdWaitEvents2 = reinterpret_cast<PFN_vkCmdWaitEvents2>(load(context, "vkCmdWaitEvents2"));
	vkCmdWriteTimestamp2 = reinterpret_cast<PFN_vkCmdWriteTimestamp2>(load(context, "vkCmdWriteTimestamp2"));
	vkCreatePrivateDataSlot = reinterpret_cast<PFN_vkCreatePrivateDataSlot>(load(context, "vkCreatePrivateDataSlot"));
	vkDestroyPrivateDataSlot = reinterpret_cast<PFN_vkDestroyPrivateDataSlot>(load(context, "vkDestroyPrivateDataSlot"));
	vkGetDeviceBufferMemoryRequirements = reinterpret_cast<PFN_vkGetDeviceBufferMemoryRequirements>(load(context, "vkGetDeviceBufferMemoryRequirements"));
	vkGetDeviceImageMemoryRequirements = reinterpret_cast<PFN_vkGetDeviceImageMemoryRequirements>(load(context, "vkGetDeviceImageMemoryRequirements"));
	vkGetDeviceImageSparseMemoryRequirements = reinterpret_cast<PFN_vkGetDeviceImageSparseMemoryRequirements>(load(context, "vkGetDeviceImageSparseMemoryRequirements"));
	vkGetPrivateData = reinterpret_cast<PFN_vkGetPrivateData>(load(context, "vkGetPrivateData"));
	vkQueueSubmit2 = reinterpret_cast<PFN_vkQueueSubmit2>(load(context, "vkQueueSubmit2"));
	vkSetPrivateData = reinterpret_cast<PFN_vkSetPrivateData>(load(context, "vkSetPrivateData"));
#endif /* defined(VK_VERSION_1_3) */
#if defined(VK_AMDX_shader_enqueue)
	vkCmdDispatchGraphAMDX = reinterpret_cast<PFN_vkCmdDispatchGraphAMDX>(load(context, "vkCmdDispatchGraphAMDX"));
	vkCmdDispatchGraphIndirectAMDX = reinterpret_cast<PFN_vkCmdDispatchGraphIndirectAMDX>(load(context, "vkCmdDispatchGraphIndirectAMDX"));
	vkCmdDispatchGraphIndirectCountAMDX = reinterpret_cast<PFN_vkCmdDispatchGraphIndirectCountAMDX>(load(context, "vkCmdDispatchGraphIndirectCountAMDX"));
	vkCmdInitializeGraphScratchMemoryAMDX = reinterpret_cast<PFN_vkCmdInitializeGraphScratchMemoryAMDX>(load(context, "vkCmdInitializeGraphScratchMemoryAMDX"));
	vkCreateExecutionGraphPipelinesAMDX = reinterpret_cast<PFN_vkCreateExecutionGraphPipelinesAMDX>(load(context, "vkCreateExecutionGraphPipelinesAMDX"));
	vkGetExecutionGraphPipelineNodeIndexAMDX = reinterpret_cast<PFN_vkGetExecutionGraphPipelineNodeIndexAMDX>(load(context, "vkGetExecutionGraphPipelineNodeIndexAMDX"));
	vkGetExecutionGraphPipelineScratchSizeAMDX = reinterpret_cast<PFN_vkGetExecutionGraphPipelineScratchSizeAMDX>(load(context, "vkGetExecutionGraphPipelineScratchSizeAMDX"));
#endif /* defined(VK_AMDX_shader_enqueue) */
#if defined(VK_AMD_buffer_marker)
	vkCmdWriteBufferMarkerAMD = reinterpret_cast<PFN_vkCmdWriteBufferMarkerAMD>(load(context, "vkCmdWriteBufferMarkerAMD"));
#endif /* defined(VK_AMD_buffer_marker) */
#if defined(VK_AMD_display_native_hdr)
	vkSetLocalDimmingAMD = reinterpret_cast<PFN_vkSetLocalDimmingAMD>(load(context, "vkSetLocalDimmingAMD"));
#endif /* defined(VK_AMD_display_native_hdr) */
#if defined(VK_AMD_draw_indirect_count)
	vkCmdDrawIndexedIndirectCountAMD = reinterpret_cast<PFN_vkCmdDrawIndexedIndirectCountAMD>(load(context, "vkCmdDrawIndexedIndirectCountAMD"));
	vkCmdDrawIndirectCountAMD = reinterpret_cast<PFN_vkCmdDrawIndirectCountAMD>(load(context, "vkCmdDrawIndirectCountAMD"));
#endif /* defined(VK_AMD_draw_indirect_count) */
#if defined(VK_AMD_shader_info)
	vkGetShaderInfoAMD = reinterpret_cast<PFN_vkGetShaderInfoAMD>(load(context, "vkGetShaderInfoAMD"));
#endif /* defined(VK_AMD_shader_info) */
#if defined(VK_ANDROID_external_memory_android_hardware_buffer)
	vkGetAndroidHardwareBufferPropertiesANDROID = reinterpret_cast<PFN_vkGetAndroidHardwareBufferPropertiesANDROID>(load(context, "vkGetAndroidHardwareBufferPropertiesANDROID"));
	vkGetMemoryAndroidHardwareBufferANDROID = reinterpret_cast<PFN_vkGetMemoryAndroidHardwareBufferANDROID>(load(context, "vkGetMemoryAndroidHardwareBufferANDROID"));
#endif /* defined(VK_ANDROID_external_memory_android_hardware_buffer) */
#if defined(VK_EXT_attachment_feedback_loop_dynamic_state)
	vkCmdSetAttachmentFeedbackLoopEnableEXT = reinterpret_cast<PFN_vkCmdSetAttachmentFeedbackLoopEnableEXT>(load(context, "vkCmdSetAttachmentFeedbackLoopEnableEXT"));
#endif /* defined(VK_EXT_attachment_feedback_loop_dynamic_state) */
#if defined(VK_EXT_buffer_device_address)
	vkGetBufferDeviceAddressEXT = reinterpret_cast<PFN_vkGetBufferDeviceAddressEXT>(load(context, "vkGetBufferDeviceAddressEXT"));
#endif /* defined(VK_EXT_buffer_device_address) */
#if defined(VK_EXT_calibrated_timestamps)
	vkGetCalibratedTimestampsEXT = reinterpret_cast<PFN_vkGetCalibratedTimestampsEXT>(load(context, "vkGetCalibratedTimestampsEXT"));
#endif /* defined(VK_EXT_calibrated_timestamps) */
#if defined(VK_EXT_color_write_enable)
	vkCmdSetColorWriteEnableEXT = reinterpret_cast<PFN_vkCmdSetColorWriteEnableEXT>(load(context, "vkCmdSetColorWriteEnableEXT"));
#endif /* defined(VK_EXT_color_write_enable) */
#if defined(VK_EXT_conditional_rendering)
	vkCmdBeginConditionalRenderingEXT = reinterpret_cast<PFN_vkCmdBeginConditionalRenderingEXT>(load(context, "vkCmdBeginConditionalRenderingEXT"));
	vkCmdEndConditionalRenderingEXT = reinterpret_cast<PFN_vkCmdEndConditionalRenderingEXT>(load(context, "vkCmdEndConditionalRenderingEXT"));
#endif /* defined(VK_EXT_conditional_rendering) */
#if defined(VK_EXT_debug_marker)
	vkCmdDebugMarkerBeginEXT = reinterpret_cast<PFN_vkCmdDebugMarkerBeginEXT>(load(context, "vkCmdDebugMarkerBeginEXT"));
	vkCmdDebugMarkerEndEXT = reinterpret_cast<PFN_vkCmdDebugMarkerEndEXT>(load(context, "vkCmdDebugMarkerEndEXT"));
	vkCmdDebugMarkerInsertEXT = reinterpret_cast<PFN_vkCmdDebugMarkerInsertEXT>(load(context, "vkCmdDebugMarkerInsertEXT"));
	vkDebugMarkerSetObjectNameEXT = reinterpret_cast<PFN_vkDebugMarkerSetObjectNameEXT>(load(context, "vkDebugMarkerSetObjectNameEXT"));
	vkDebugMarkerSetObjectTagEXT = reinterpret_cast<PFN_vkDebugMarkerSetObjectTagEXT>(load(context, "vkDebugMarkerSetObjectTagEXT"));
#endif /* defined(VK_EXT_debug_marker) */
#if defined(VK_EXT_depth_bias_control)
	vkCmdSetDepthBias2EXT = reinterpret_cast<PFN_vkCmdSetDepthBias2EXT>(load(context, "vkCmdSetDepthBias2EXT"));
#endif /* defined(VK_EXT_depth_bias_control) */
#if defined(VK_EXT_descriptor_buffer)
	vkCmdBindDescriptorBufferEmbeddedSamplersEXT = reinterpret_cast<PFN_vkCmdBindDescriptorBufferEmbeddedSamplersEXT>(load(context, "vkCmdBindDescriptorBufferEmbeddedSamplersEXT"));
	vkCmdBindDescriptorBuffersEXT = reinterpret_cast<PFN_vkCmdBindDescriptorBuffersEXT>(load(context, "vkCmdBindDescriptorBuffersEXT"));
	vkCmdSetDescriptorBufferOffsetsEXT = reinterpret_cast<PFN_vkCmdSetDescriptorBufferOffsetsEXT>(load(context, "vkCmdSetDescriptorBufferOffsetsEXT"));
	vkGetBufferOpaqueCaptureDescriptorDataEXT = reinterpret_cast<PFN_vkGetBufferOpaqueCaptureDescriptorDataEXT>(load(context, "vkGetBufferOpaqueCaptureDescriptorDataEXT"));
	vkGetDescriptorEXT = reinterpret_cast<PFN_vkGetDescriptorEXT>(load(context, "vkGetDescriptorEXT"));
	vkGetDescriptorSetLayoutBindingOffsetEXT = reinterpret_cast<PFN_vkGetDescriptorSetLayoutBindingOffsetEXT>(load(context, "vkGetDescriptorSetLayoutBindingOffsetEXT"));
	vkGetDescriptorSetLayoutSizeEXT = reinterpret_cast<PFN_vkGetDescriptorSetLayoutSizeEXT>(load(context, "vkGetDescriptorSetLayoutSizeEXT"));
	vkGetImageOpaqueCaptureDescriptorDataEXT = reinterpret_cast<PFN_vkGetImageOpaqueCaptureDescriptorDataEXT>(load(context, "vkGetImageOpaqueCaptureDescriptorDataEXT"));
	vkGetImageViewOpaqueCaptureDescriptorDataEXT = reinterpret_cast<PFN_vkGetImageViewOpaqueCaptureDescriptorDataEXT>(load(context, "vkGetImageViewOpaqueCaptureDescriptorDataEXT"));
	vkGetSamplerOpaqueCaptureDescriptorDataEXT = reinterpret_cast<PFN_vkGetSamplerOpaqueCaptureDescriptorDataEXT>(load(context, "vkGetSamplerOpaqueCaptureDescriptorDataEXT"));
#endif /* defined(VK_EXT_descriptor_buffer) */
#if defined(VK_EXT_descriptor_buffer) && (defined(VK_KHR_acceleration_structure) || defined(VK_NV_ray_tracing))
	vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT = reinterpret_cast<PFN_vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT>(load(context, "vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT"));
#endif /* defined(VK_EXT_descriptor_buffer) && (defined(VK_KHR_acceleration_structure) || defined(VK_NV_ray_tracing)) */
#if defined(VK_EXT_device_fault)
	vkGetDeviceFaultInfoEXT = reinterpret_cast<PFN_vkGetDeviceFaultInfoEXT>(load(context, "vkGetDeviceFaultInfoEXT"));
#endif /* defined(VK_EXT_device_fault) */
#if defined(VK_EXT_discard_rectangles)
	vkCmdSetDiscardRectangleEXT = reinterpret_cast<PFN_vkCmdSetDiscardRectangleEXT>(load(context, "vkCmdSetDiscardRectangleEXT"));
#endif /* defined(VK_EXT_discard_rectangles) */
#if defined(VK_EXT_discard_rectangles) && VK_EXT_DISCARD_RECTANGLES_SPEC_VERSION >= 2
	vkCmdSetDiscardRectangleEnableEXT = reinterpret_cast<PFN_vkCmdSetDiscardRectangleEnableEXT>(load(context, "vkCmdSetDiscardRectangleEnableEXT"));
	vkCmdSetDiscardRectangleModeEXT = reinterpret_cast<PFN_vkCmdSetDiscardRectangleModeEXT>(load(context, "vkCmdSetDiscardRectangleModeEXT"));
#endif /* defined(VK_EXT_discard_rectangles) && VK_EXT_DISCARD_RECTANGLES_SPEC_VERSION >= 2 */
#if defined(VK_EXT_display_control)
	vkDisplayPowerControlEXT = reinterpret_cast<PFN_vkDisplayPowerControlEXT>(load(context, "vkDisplayPowerControlEXT"));
	vkGetSwapchainCounterEXT = reinterpret_cast<PFN_vkGetSwapchainCounterEXT>(load(context, "vkGetSwapchainCounterEXT"));
	vkRegisterDeviceEventEXT = reinterpret_cast<PFN_vkRegisterDeviceEventEXT>(load(context, "vkRegisterDeviceEventEXT"));
	vkRegisterDisplayEventEXT = reinterpret_cast<PFN_vkRegisterDisplayEventEXT>(load(context, "vkRegisterDisplayEventEXT"));
#endif /* defined(VK_EXT_display_control) */
#if defined(VK_EXT_external_memory_host)
	vkGetMemoryHostPointerPropertiesEXT = reinterpret_cast<PFN_vkGetMemoryHostPointerPropertiesEXT>(load(context, "vkGetMemoryHostPointerPropertiesEXT"));
#endif /* defined(VK_EXT_external_memory_host) */
#if defined(VK_EXT_full_screen_exclusive)
	vkAcquireFullScreenExclusiveModeEXT = reinterpret_cast<PFN_vkAcquireFullScreenExclusiveModeEXT>(load(context, "vkAcquireFullScreenExclusiveModeEXT"));
	vkReleaseFullScreenExclusiveModeEXT = reinterpret_cast<PFN_vkReleaseFullScreenExclusiveModeEXT>(load(context, "vkReleaseFullScreenExclusiveModeEXT"));
#endif /* defined(VK_EXT_full_screen_exclusive) */
#if defined(VK_EXT_hdr_metadata)
	vkSetHdrMetadataEXT = reinterpret_cast<PFN_vkSetHdrMetadataEXT>(load(context, "vkSetHdrMetadataEXT"));
#endif /* defined(VK_EXT_hdr_metadata) */
#if defined(VK_EXT_host_image_copy)
	vkCopyImageToImageEXT = reinterpret_cast<PFN_vkCopyImageToImageEXT>(load(context, "vkCopyImageToImageEXT"));
	vkCopyImageToMemoryEXT = reinterpret_cast<PFN_vkCopyImageToMemoryEXT>(load(context, "vkCopyImageToMemoryEXT"));
	vkCopyMemoryToImageEXT = reinterpret_cast<PFN_vkCopyMemoryToImageEXT>(load(context, "vkCopyMemoryToImageEXT"));
	vkTransitionImageLayoutEXT = reinterpret_cast<PFN_vkTransitionImageLayoutEXT>(load(context, "vkTransitionImageLayoutEXT"));
#endif /* defined(VK_EXT_host_image_copy) */
#if defined(VK_EXT_host_query_reset)
	vkResetQueryPoolEXT = reinterpret_cast<PFN_vkResetQueryPoolEXT>(load(context, "vkResetQueryPoolEXT"));
#endif /* defined(VK_EXT_host_query_reset) */
#if defined(VK_EXT_image_drm_format_modifier)
	vkGetImageDrmFormatModifierPropertiesEXT = reinterpret_cast<PFN_vkGetImageDrmFormatModifierPropertiesEXT>(load(context, "vkGetImageDrmFormatModifierPropertiesEXT"));
#endif /* defined(VK_EXT_image_drm_format_modifier) */
#if defined(VK_EXT_line_rasterization)
	vkCmdSetLineStippleEXT = reinterpret_cast<PFN_vkCmdSetLineStippleEXT>(load(context, "vkCmdSetLineStippleEXT"));
#endif /* defined(VK_EXT_line_rasterization) */
#if defined(VK_EXT_mesh_shader)
	vkCmdDrawMeshTasksEXT = reinterpret_cast<PFN_vkCmdDrawMeshTasksEXT>(load(context, "vkCmdDrawMeshTasksEXT"));
	vkCmdDrawMeshTasksIndirectCountEXT = reinterpret_cast<PFN_vkCmdDrawMeshTasksIndirectCountEXT>(load(context, "vkCmdDrawMeshTasksIndirectCountEXT"));
	vkCmdDrawMeshTasksIndirectEXT = reinterpret_cast<PFN_vkCmdDrawMeshTasksIndirectEXT>(load(context, "vkCmdDrawMeshTasksIndirectEXT"));
#endif /* defined(VK_EXT_mesh_shader) */
#if defined(VK_EXT_metal_objects)
	vkExportMetalObjectsEXT = reinterpret_cast<PFN_vkExportMetalObjectsEXT>(load(context, "vkExportMetalObjectsEXT"));
#endif /* defined(VK_EXT_metal_objects) */
#if defined(VK_EXT_multi_draw)
	vkCmdDrawMultiEXT = reinterpret_cast<PFN_vkCmdDrawMultiEXT>(load(context, "vkCmdDrawMultiEXT"));
	vkCmdDrawMultiIndexedEXT = reinterpret_cast<PFN_vkCmdDrawMultiIndexedEXT>(load(context, "vkCmdDrawMultiIndexedEXT"));
#endif /* defined(VK_EXT_multi_draw) */
#if defined(VK_EXT_opacity_micromap)
	vkBuildMicromapsEXT = reinterpret_cast<PFN_vkBuildMicromapsEXT>(load(context, "vkBuildMicromapsEXT"));
	vkCmdBuildMicromapsEXT = reinterpret_cast<PFN_vkCmdBuildMicromapsEXT>(load(context, "vkCmdBuildMicromapsEXT"));
	vkCmdCopyMemoryToMicromapEXT = reinterpret_cast<PFN_vkCmdCopyMemoryToMicromapEXT>(load(context, "vkCmdCopyMemoryToMicromapEXT"));
	vkCmdCopyMicromapEXT = reinterpret_cast<PFN_vkCmdCopyMicromapEXT>(load(context, "vkCmdCopyMicromapEXT"));
	vkCmdCopyMicromapToMemoryEXT = reinterpret_cast<PFN_vkCmdCopyMicromapToMemoryEXT>(load(context, "vkCmdCopyMicromapToMemoryEXT"));
	vkCmdWriteMicromapsPropertiesEXT = reinterpret_cast<PFN_vkCmdWriteMicromapsPropertiesEXT>(load(context, "vkCmdWriteMicromapsPropertiesEXT"));
	vkCopyMemoryToMicromapEXT = reinterpret_cast<PFN_vkCopyMemoryToMicromapEXT>(load(context, "vkCopyMemoryToMicromapEXT"));
	vkCopyMicromapEXT = reinterpret_cast<PFN_vkCopyMicromapEXT>(load(context, "vkCopyMicromapEXT"));
	vkCopyMicromapToMemoryEXT = reinterpret_cast<PFN_vkCopyMicromapToMemoryEXT>(load(context, "vkCopyMicromapToMemoryEXT"));
	vkCreateMicromapEXT = reinterpret_cast<PFN_vkCreateMicromapEXT>(load(context, "vkCreateMicromapEXT"));
	vkDestroyMicromapEXT = reinterpret_cast<PFN_vkDestroyMicromapEXT>(load(context, "vkDestroyMicromapEXT"));
	vkGetDeviceMicromapCompatibilityEXT = reinterpret_cast<PFN_vkGetDeviceMicromapCompatibilityEXT>(load(context, "vkGetDeviceMicromapCompatibilityEXT"));
	vkGetMicromapBuildSizesEXT = reinterpret_cast<PFN_vkGetMicromapBuildSizesEXT>(load(context, "vkGetMicromapBuildSizesEXT"));
	vkWriteMicromapsPropertiesEXT = reinterpret_cast<PFN_vkWriteMicromapsPropertiesEXT>(load(context, "vkWriteMicromapsPropertiesEXT"));
#endif /* defined(VK_EXT_opacity_micromap) */
#if defined(VK_EXT_pageable_device_local_memory)
	vkSetDeviceMemoryPriorityEXT = reinterpret_cast<PFN_vkSetDeviceMemoryPriorityEXT>(load(context, "vkSetDeviceMemoryPriorityEXT"));
#endif /* defined(VK_EXT_pageable_device_local_memory) */
#if defined(VK_EXT_pipeline_properties)
	vkGetPipelinePropertiesEXT = reinterpret_cast<PFN_vkGetPipelinePropertiesEXT>(load(context, "vkGetPipelinePropertiesEXT"));
#endif /* defined(VK_EXT_pipeline_properties) */
#if defined(VK_EXT_private_data)
	vkCreatePrivateDataSlotEXT = reinterpret_cast<PFN_vkCreatePrivateDataSlotEXT>(load(context, "vkCreatePrivateDataSlotEXT"));
	vkDestroyPrivateDataSlotEXT = reinterpret_cast<PFN_vkDestroyPrivateDataSlotEXT>(load(context, "vkDestroyPrivateDataSlotEXT"));
	vkGetPrivateDataEXT = reinterpret_cast<PFN_vkGetPrivateDataEXT>(load(context, "vkGetPrivateDataEXT"));
	vkSetPrivateDataEXT = reinterpret_cast<PFN_vkSetPrivateDataEXT>(load(context, "vkSetPrivateDataEXT"));
#endif /* defined(VK_EXT_private_data) */
#if defined(VK_EXT_sample_locations)
	vkCmdSetSampleLocationsEXT = reinterpret_cast<PFN_vkCmdSetSampleLocationsEXT>(load(context, "vkCmdSetSampleLocationsEXT"));
#endif /* defined(VK_EXT_sample_locations) */
#if defined(VK_EXT_shader_module_identifier)
	vkGetShaderModuleCreateInfoIdentifierEXT = reinterpret_cast<PFN_vkGetShaderModuleCreateInfoIdentifierEXT>(load(context, "vkGetShaderModuleCreateInfoIdentifierEXT"));
	vkGetShaderModuleIdentifierEXT = reinterpret_cast<PFN_vkGetShaderModuleIdentifierEXT>(load(context, "vkGetShaderModuleIdentifierEXT"));
#endif /* defined(VK_EXT_shader_module_identifier) */
#if defined(VK_EXT_shader_object)
	vkCmdBindShadersEXT = reinterpret_cast<PFN_vkCmdBindShadersEXT>(load(context, "vkCmdBindShadersEXT"));
	vkCreateShadersEXT = reinterpret_cast<PFN_vkCreateShadersEXT>(load(context, "vkCreateShadersEXT"));
	vkDestroyShaderEXT = reinterpret_cast<PFN_vkDestroyShaderEXT>(load(context, "vkDestroyShaderEXT"));
	vkGetShaderBinaryDataEXT = reinterpret_cast<PFN_vkGetShaderBinaryDataEXT>(load(context, "vkGetShaderBinaryDataEXT"));
#endif /* defined(VK_EXT_shader_object) */
#if defined(VK_EXT_swapchain_maintenance1)
	vkReleaseSwapchainImagesEXT = reinterpret_cast<PFN_vkReleaseSwapchainImagesEXT>(load(context, "vkReleaseSwapchainImagesEXT"));
#endif /* defined(VK_EXT_swapchain_maintenance1) */
#if defined(VK_EXT_transform_feedback)
	vkCmdBeginQueryIndexedEXT = reinterpret_cast<PFN_vkCmdBeginQueryIndexedEXT>(load(context, "vkCmdBeginQueryIndexedEXT"));
	vkCmdBeginTransformFeedbackEXT = reinterpret_cast<PFN_vkCmdBeginTransformFeedbackEXT>(load(context, "vkCmdBeginTransformFeedbackEXT"));
	vkCmdBindTransformFeedbackBuffersEXT = reinterpret_cast<PFN_vkCmdBindTransformFeedbackBuffersEXT>(load(context, "vkCmdBindTransformFeedbackBuffersEXT"));
	vkCmdDrawIndirectByteCountEXT = reinterpret_cast<PFN_vkCmdDrawIndirectByteCountEXT>(load(context, "vkCmdDrawIndirectByteCountEXT"));
	vkCmdEndQueryIndexedEXT = reinterpret_cast<PFN_vkCmdEndQueryIndexedEXT>(load(context, "vkCmdEndQueryIndexedEXT"));
	vkCmdEndTransformFeedbackEXT = reinterpret_cast<PFN_vkCmdEndTransformFeedbackEXT>(load(context, "vkCmdEndTransformFeedbackEXT"));
#endif /* defined(VK_EXT_transform_feedback) */
#if defined(VK_EXT_validation_cache)
	vkCreateValidationCacheEXT = reinterpret_cast<PFN_vkCreateValidationCacheEXT>(load(context, "vkCreateValidationCacheEXT"));
	vkDestroyValidationCacheEXT = reinterpret_cast<PFN_vkDestroyValidationCacheEXT>(load(context, "vkDestroyValidationCacheEXT"));
	vkGetValidationCacheDataEXT = reinterpret_cast<PFN_vkGetValidationCacheDataEXT>(load(context, "vkGetValidationCacheDataEXT"));
	vkMergeValidationCachesEXT = reinterpret_cast<PFN_vkMergeValidationCachesEXT>(load(context, "vkMergeValidationCachesEXT"));
#endif /* defined(VK_EXT_validation_cache) */
#if defined(VK_FUCHSIA_buffer_collection)
	vkCreateBufferCollectionFUCHSIA = reinterpret_cast<PFN_vkCreateBufferCollectionFUCHSIA>(load(context, "vkCreateBufferCollectionFUCHSIA"));
	vkDestroyBufferCollectionFUCHSIA = reinterpret_cast<PFN_vkDestroyBufferCollectionFUCHSIA>(load(context, "vkDestroyBufferCollectionFUCHSIA"));
	vkGetBufferCollectionPropertiesFUCHSIA = reinterpret_cast<PFN_vkGetBufferCollectionPropertiesFUCHSIA>(load(context, "vkGetBufferCollectionPropertiesFUCHSIA"));
	vkSetBufferCollectionBufferConstraintsFUCHSIA = reinterpret_cast<PFN_vkSetBufferCollectionBufferConstraintsFUCHSIA>(load(context, "vkSetBufferCollectionBufferConstraintsFUCHSIA"));
	vkSetBufferCollectionImageConstraintsFUCHSIA = reinterpret_cast<PFN_vkSetBufferCollectionImageConstraintsFUCHSIA>(load(context, "vkSetBufferCollectionImageConstraintsFUCHSIA"));
#endif /* defined(VK_FUCHSIA_buffer_collection) */
#if defined(VK_FUCHSIA_external_memory)
	vkGetMemoryZirconHandleFUCHSIA = reinterpret_cast<PFN_vkGetMemoryZirconHandleFUCHSIA>(load(context, "vkGetMemoryZirconHandleFUCHSIA"));
	vkGetMemoryZirconHandlePropertiesFUCHSIA = reinterpret_cast<PFN_vkGetMemoryZirconHandlePropertiesFUCHSIA>(load(context, "vkGetMemoryZirconHandlePropertiesFUCHSIA"));
#endif /* defined(VK_FUCHSIA_external_memory) */
#if defined(VK_FUCHSIA_external_semaphore)
	vkGetSemaphoreZirconHandleFUCHSIA = reinterpret_cast<PFN_vkGetSemaphoreZirconHandleFUCHSIA>(load(context, "vkGetSemaphoreZirconHandleFUCHSIA"));
	vkImportSemaphoreZirconHandleFUCHSIA = reinterpret_cast<PFN_vkImportSemaphoreZirconHandleFUCHSIA>(load(context, "vkImportSemaphoreZirconHandleFUCHSIA"));
#endif /* defined(VK_FUCHSIA_external_semaphore) */
#if defined(VK_GOOGLE_display_timing)
	vkGetPastPresentationTimingGOOGLE = reinterpret_cast<PFN_vkGetPastPresentationTimingGOOGLE>(load(context, "vkGetPastPresentationTimingGOOGLE"));
	vkGetRefreshCycleDurationGOOGLE = reinterpret_cast<PFN_vkGetRefreshCycleDurationGOOGLE>(load(context, "vkGetRefreshCycleDurationGOOGLE"));
#endif /* defined(VK_GOOGLE_display_timing) */
#if defined(VK_HUAWEI_cluster_culling_shader)
	vkCmdDrawClusterHUAWEI = reinterpret_cast<PFN_vkCmdDrawClusterHUAWEI>(load(context, "vkCmdDrawClusterHUAWEI"));
	vkCmdDrawClusterIndirectHUAWEI = reinterpret_cast<PFN_vkCmdDrawClusterIndirectHUAWEI>(load(context, "vkCmdDrawClusterIndirectHUAWEI"));
#endif /* defined(VK_HUAWEI_cluster_culling_shader) */
#if defined(VK_HUAWEI_invocation_mask)
	vkCmdBindInvocationMaskHUAWEI = reinterpret_cast<PFN_vkCmdBindInvocationMaskHUAWEI>(load(context, "vkCmdBindInvocationMaskHUAWEI"));
#endif /* defined(VK_HUAWEI_invocation_mask) */
#if defined(VK_HUAWEI_subpass_shading)
	vkCmdSubpassShadingHUAWEI = reinterpret_cast<PFN_vkCmdSubpassShadingHUAWEI>(load(context, "vkCmdSubpassShadingHUAWEI"));
	vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI = reinterpret_cast<PFN_vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI>(load(context, "vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI"));
#endif /* defined(VK_HUAWEI_subpass_shading) */
#if defined(VK_INTEL_performance_query)
	vkAcquirePerformanceConfigurationINTEL = reinterpret_cast<PFN_vkAcquirePerformanceConfigurationINTEL>(load(context, "vkAcquirePerformanceConfigurationINTEL"));
	vkCmdSetPerformanceMarkerINTEL = reinterpret_cast<PFN_vkCmdSetPerformanceMarkerINTEL>(load(context, "vkCmdSetPerformanceMarkerINTEL"));
	vkCmdSetPerformanceOverrideINTEL = reinterpret_cast<PFN_vkCmdSetPerformanceOverrideINTEL>(load(context, "vkCmdSetPerformanceOverrideINTEL"));
	vkCmdSetPerformanceStreamMarkerINTEL = reinterpret_cast<PFN_vkCmdSetPerformanceStreamMarkerINTEL>(load(context, "vkCmdSetPerformanceStreamMarkerINTEL"));
	vkGetPerformanceParameterINTEL = reinterpret_cast<PFN_vkGetPerformanceParameterINTEL>(load(context, "vkGetPerformanceParameterINTEL"));
	vkInitializePerformanceApiINTEL = reinterpret_cast<PFN_vkInitializePerformanceApiINTEL>(load(context, "vkInitializePerformanceApiINTEL"));
	vkQueueSetPerformanceConfigurationINTEL = reinterpret_cast<PFN_vkQueueSetPerformanceConfigurationINTEL>(load(context, "vkQueueSetPerformanceConfigurationINTEL"));
	vkReleasePerformanceConfigurationINTEL = reinterpret_cast<PFN_vkReleasePerformanceConfigurationINTEL>(load(context, "vkReleasePerformanceConfigurationINTEL"));
	vkUninitializePerformanceApiINTEL = reinterpret_cast<PFN_vkUninitializePerformanceApiINTEL>(load(context, "vkUninitializePerformanceApiINTEL"));
#endif /* defined(VK_INTEL_performance_query) */
#if defined(VK_KHR_acceleration_structure)
	vkBuildAccelerationStructuresKHR = reinterpret_cast<PFN_vkBuildAccelerationStructuresKHR>(load(context, "vkBuildAccelerationStructuresKHR"));
	vkCmdBuildAccelerationStructuresIndirectKHR = reinterpret_cast<PFN_vkCmdBuildAccelerationStructuresIndirectKHR>(load(context, "vkCmdBuildAccelerationStructuresIndirectKHR"));
	vkCmdBuildAccelerationStructuresKHR = reinterpret_cast<PFN_vkCmdBuildAccelerationStructuresKHR>(load(context, "vkCmdBuildAccelerationStructuresKHR"));
	vkCmdCopyAccelerationStructureKHR = reinterpret_cast<PFN_vkCmdCopyAccelerationStructureKHR>(load(context, "vkCmdCopyAccelerationStructureKHR"));
	vkCmdCopyAccelerationStructureToMemoryKHR = reinterpret_cast<PFN_vkCmdCopyAccelerationStructureToMemoryKHR>(load(context, "vkCmdCopyAccelerationStructureToMemoryKHR"));
	vkCmdCopyMemoryToAccelerationStructureKHR = reinterpret_cast<PFN_vkCmdCopyMemoryToAccelerationStructureKHR>(load(context, "vkCmdCopyMemoryToAccelerationStructureKHR"));
	vkCmdWriteAccelerationStructuresPropertiesKHR = reinterpret_cast<PFN_vkCmdWriteAccelerationStructuresPropertiesKHR>(load(context, "vkCmdWriteAccelerationStructuresPropertiesKHR"));
	vkCopyAccelerationStructureKHR = reinterpret_cast<PFN_vkCopyAccelerationStructureKHR>(load(context, "vkCopyAccelerationStructureKHR"));
	vkCopyAccelerationStructureToMemoryKHR = reinterpret_cast<PFN_vkCopyAccelerationStructureToMemoryKHR>(load(context, "vkCopyAccelerationStructureToMemoryKHR"));
	vkCopyMemoryToAccelerationStructureKHR = reinterpret_cast<PFN_vkCopyMemoryToAccelerationStructureKHR>(load(context, "vkCopyMemoryToAccelerationStructureKHR"));
	vkCreateAccelerationStructureKHR = reinterpret_cast<PFN_vkCreateAccelerationStructureKHR>(load(context, "vkCreateAccelerationStructureKHR"));
	vkDestroyAccelerationStructureKHR = reinterpret_cast<PFN_vkDestroyAccelerationStructureKHR>(load(context, "vkDestroyAccelerationStructureKHR"));
	vkGetAccelerationStructureBuildSizesKHR = reinterpret_cast<PFN_vkGetAccelerationStructureBuildSizesKHR>(load(context, "vkGetAccelerationStructureBuildSizesKHR"));
	vkGetAccelerationStructureDeviceAddressKHR = reinterpret_cast<PFN_vkGetAccelerationStructureDeviceAddressKHR>(load(context, "vkGetAccelerationStructureDeviceAddressKHR"));
	vkGetDeviceAccelerationStructureCompatibilityKHR = reinterpret_cast<PFN_vkGetDeviceAccelerationStructureCompatibilityKHR>(load(context, "vkGetDeviceAccelerationStructureCompatibilityKHR"));
	vkWriteAccelerationStructuresPropertiesKHR = reinterpret_cast<PFN_vkWriteAccelerationStructuresPropertiesKHR>(load(context, "vkWriteAccelerationStructuresPropertiesKHR"));
#endif /* defined(VK_KHR_acceleration_structure) */
#if defined(VK_KHR_bind_memory2)
	vkBindBufferMemory2KHR = reinterpret_cast<PFN_vkBindBufferMemory2KHR>(load(context, "vkBindBufferMemory2KHR"));
	vkBindImageMemory2KHR = reinterpret_cast<PFN_vkBindImageMemory2KHR>(load(context, "vkBindImageMemory2KHR"));
#endif /* defined(VK_KHR_bind_memory2) */
#if defined(VK_KHR_buffer_device_address)
	vkGetBufferDeviceAddressKHR = reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(load(context, "vkGetBufferDeviceAddressKHR"));
	vkGetBufferOpaqueCaptureAddressKHR = reinterpret_cast<PFN_vkGetBufferOpaqueCaptureAddressKHR>(load(context, "vkGetBufferOpaqueCaptureAddressKHR"));
	vkGetDeviceMemoryOpaqueCaptureAddressKHR = reinterpret_cast<PFN_vkGetDeviceMemoryOpaqueCaptureAddressKHR>(load(context, "vkGetDeviceMemoryOpaqueCaptureAddressKHR"));
#endif /* defined(VK_KHR_buffer_device_address) */
#if defined(VK_KHR_calibrated_timestamps)
	vkGetCalibratedTimestampsKHR = reinterpret_cast<PFN_vkGetCalibratedTimestampsKHR>(load(context, "vkGetCalibratedTimestampsKHR"));
#endif /* defined(VK_KHR_calibrated_timestamps) */
#if defined(VK_KHR_copy_commands2)
	vkCmdBlitImage2KHR = reinterpret_cast<PFN_vkCmdBlitImage2KHR>(load(context, "vkCmdBlitImage2KHR"));
	vkCmdCopyBuffer2KHR = reinterpret_cast<PFN_vkCmdCopyBuffer2KHR>(load(context, "vkCmdCopyBuffer2KHR"));
	vkCmdCopyBufferToImage2KHR = reinterpret_cast<PFN_vkCmdCopyBufferToImage2KHR>(load(context, "vkCmdCopyBufferToImage2KHR"));
	vkCmdCopyImage2KHR = reinterpret_cast<PFN_vkCmdCopyImage2KHR>(load(context, "vkCmdCopyImage2KHR"));
	vkCmdCopyImageToBuffer2KHR = reinterpret_cast<PFN_vkCmdCopyImageToBuffer2KHR>(load(context, "vkCmdCopyImageToBuffer2KHR"));
	vkCmdResolveImage2KHR = reinterpret_cast<PFN_vkCmdResolveImage2KHR>(load(context, "vkCmdResolveImage2KHR"));
#endif /* defined(VK_KHR_copy_commands2) */
#if defined(VK_KHR_create_renderpass2)
	vkCmdBeginRenderPass2KHR = reinterpret_cast<PFN_vkCmdBeginRenderPass2KHR>(load(context, "vkCmdBeginRenderPass2KHR"));
	vkCmdEndRenderPass2KHR = reinterpret_cast<PFN_vkCmdEndRenderPass2KHR>(load(context, "vkCmdEndRenderPass2KHR"));
	vkCmdNextSubpass2KHR = reinterpret_cast<PFN_vkCmdNextSubpass2KHR>(load(context, "vkCmdNextSubpass2KHR"));
	vkCreateRenderPass2KHR = reinterpret_cast<PFN_vkCreateRenderPass2KHR>(load(context, "vkCreateRenderPass2KHR"));
#endif /* defined(VK_KHR_create_renderpass2) */
#if defined(VK_KHR_deferred_host_operations)
	vkCreateDeferredOperationKHR = reinterpret_cast<PFN_vkCreateDeferredOperationKHR>(load(context, "vkCreateDeferredOperationKHR"));
	vkDeferredOperationJoinKHR = reinterpret_cast<PFN_vkDeferredOperationJoinKHR>(load(context, "vkDeferredOperationJoinKHR"));
	vkDestroyDeferredOperationKHR = reinterpret_cast<PFN_vkDestroyDeferredOperationKHR>(load(context, "vkDestroyDeferredOperationKHR"));
	vkGetDeferredOperationMaxConcurrencyKHR = reinterpret_cast<PFN_vkGetDeferredOperationMaxConcurrencyKHR>(load(context, "vkGetDeferredOperationMaxConcurrencyKHR"));
	vkGetDeferredOperationResultKHR = reinterpret_cast<PFN_vkGetDeferredOperationResultKHR>(load(context, "vkGetDeferredOperationResultKHR"));
#endif /* defined(VK_KHR_deferred_host_operations) */
#if defined(VK_KHR_descriptor_update_template)
	vkCreateDescriptorUpdateTemplateKHR = reinterpret_cast<PFN_vkCreateDescriptorUpdateTemplateKHR>(load(context, "vkCreateDescriptorUpdateTemplateKHR"));
	vkDestroyDescriptorUpdateTemplateKHR = reinterpret_cast<PFN_vkDestroyDescriptorUpdateTemplateKHR>(load(context, "vkDestroyDescriptorUpdateTemplateKHR"));
	vkUpdateDescriptorSetWithTemplateKHR = reinterpret_cast<PFN_vkUpdateDescriptorSetWithTemplateKHR>(load(context, "vkUpdateDescriptorSetWithTemplateKHR"));
#endif /* defined(VK_KHR_descriptor_update_template) */
#if defined(VK_KHR_device_group)
	vkCmdDispatchBaseKHR = reinterpret_cast<PFN_vkCmdDispatchBaseKHR>(load(context, "vkCmdDispatchBaseKHR"));
	vkCmdSetDeviceMaskKHR = reinterpret_cast<PFN_vkCmdSetDeviceMaskKHR>(load(context, "vkCmdSetDeviceMaskKHR"));
	vkGetDeviceGroupPeerMemoryFeaturesKHR = reinterpret_cast<PFN_vkGetDeviceGroupPeerMemoryFeaturesKHR>(load(context, "vkGetDeviceGroupPeerMemoryFeaturesKHR"));
#endif /* defined(VK_KHR_device_group) */
#if defined(VK_KHR_display_swapchain)
	vkCreateSharedSwapchainsKHR = reinterpret_cast<PFN_vkCreateSharedSwapchainsKHR>(load(context, "vkCreateSharedSwapchainsKHR"));
#endif /* defined(VK_KHR_display_swapchain) */
#if defined(VK_KHR_draw_indirect_count)
	vkCmdDrawIndexedIndirectCountKHR = reinterpret_cast<PFN_vkCmdDrawIndexedIndirectCountKHR>(load(context, "vkCmdDrawIndexedIndirectCountKHR"));
	vkCmdDrawIndirectCountKHR = reinterpret_cast<PFN_vkCmdDrawIndirectCountKHR>(load(context, "vkCmdDrawIndirectCountKHR"));
#endif /* defined(VK_KHR_draw_indirect_count) */
#if defined(VK_KHR_dynamic_rendering)
	vkCmdBeginRenderingKHR = reinterpret_cast<PFN_vkCmdBeginRenderingKHR>(load(context, "vkCmdBeginRenderingKHR"));
	vkCmdEndRenderingKHR = reinterpret_cast<PFN_vkCmdEndRenderingKHR>(load(context, "vkCmdEndRenderingKHR"));
#endif /* defined(VK_KHR_dynamic_rendering) */
#if defined(VK_KHR_dynamic_rendering_local_read)
	vkCmdSetRenderingAttachmentLocationsKHR = reinterpret_cast<PFN_vkCmdSetRenderingAttachmentLocationsKHR>(load(context, "vkCmdSetRenderingAttachmentLocationsKHR"));
	vkCmdSetRenderingInputAttachmentIndicesKHR = reinterpret_cast<PFN_vkCmdSetRenderingInputAttachmentIndicesKHR>(load(context, "vkCmdSetRenderingInputAttachmentIndicesKHR"));
#endif /* defined(VK_KHR_dynamic_rendering_local_read) */
#if defined(VK_KHR_external_fence_fd)
	vkGetFenceFdKHR = reinterpret_cast<PFN_vkGetFenceFdKHR>(load(context, "vkGetFenceFdKHR"));
	vkImportFenceFdKHR = reinterpret_cast<PFN_vkImportFenceFdKHR>(load(context, "vkImportFenceFdKHR"));
#endif /* defined(VK_KHR_external_fence_fd) */
#if defined(VK_KHR_external_fence_win32)
	vkGetFenceWin32HandleKHR = reinterpret_cast<PFN_vkGetFenceWin32HandleKHR>(load(context, "vkGetFenceWin32HandleKHR"));
	vkImportFenceWin32HandleKHR = reinterpret_cast<PFN_vkImportFenceWin32HandleKHR>(load(context, "vkImportFenceWin32HandleKHR"));
#endif /* defined(VK_KHR_external_fence_win32) */
#if defined(VK_KHR_external_memory_fd)
	vkGetMemoryFdKHR = reinterpret_cast<PFN_vkGetMemoryFdKHR>(load(context, "vkGetMemoryFdKHR"));
	vkGetMemoryFdPropertiesKHR = reinterpret_cast<PFN_vkGetMemoryFdPropertiesKHR>(load(context, "vkGetMemoryFdPropertiesKHR"));
#endif /* defined(VK_KHR_external_memory_fd) */
#if defined(VK_KHR_external_memory_win32)
	vkGetMemoryWin32HandleKHR = reinterpret_cast<PFN_vkGetMemoryWin32HandleKHR>(load(context, "vkGetMemoryWin32HandleKHR"));
	vkGetMemoryWin32HandlePropertiesKHR = reinterpret_cast<PFN_vkGetMemoryWin32HandlePropertiesKHR>(load(context, "vkGetMemoryWin32HandlePropertiesKHR"));
#endif /* defined(VK_KHR_external_memory_win32) */
#if defined(VK_KHR_external_semaphore_fd)
	vkGetSemaphoreFdKHR = reinterpret_cast<PFN_vkGetSemaphoreFdKHR>(load(context, "vkGetSemaphoreFdKHR"));
	vkImportSemaphoreFdKHR = reinterpret_cast<PFN_vkImportSemaphoreFdKHR>(load(context, "vkImportSemaphoreFdKHR"));
#endif /* defined(VK_KHR_external_semaphore_fd) */
#if defined(VK_KHR_external_semaphore_win32)
	vkGetSemaphoreWin32HandleKHR = reinterpret_cast<PFN_vkGetSemaphoreWin32HandleKHR>(load(context, "vkGetSemaphoreWin32HandleKHR"));
	vkImportSemaphoreWin32HandleKHR = reinterpret_cast<PFN_vkImportSemaphoreWin32HandleKHR>(load(context, "vkImportSemaphoreWin32HandleKHR"));
#endif /* defined(VK_KHR_external_semaphore_win32) */
#if defined(VK_KHR_fragment_shading_rate)
	vkCmdSetFragmentShadingRateKHR = reinterpret_cast<PFN_vkCmdSetFragmentShadingRateKHR>(load(context, "vkCmdSetFragmentShadingRateKHR"));
#endif /* defined(VK_KHR_fragment_shading_rate) */
#if defined(VK_KHR_get_memory_requirements2)
	vkGetBufferMemoryRequirements2KHR = reinterpret_cast<PFN_vkGetBufferMemoryRequirements2KHR>(load(context, "vkGetBufferMemoryRequirements2KHR"));
	vkGetImageMemoryRequirements2KHR = reinterpret_cast<PFN_vkGetImageMemoryRequirements2KHR>(load(context, "vkGetImageMemoryRequirements2KHR"));
	vkGetImageSparseMemoryRequirements2KHR = reinterpret_cast<PFN_vkGetImageSparseMemoryRequirements2KHR>(load(context, "vkGetImageSparseMemoryRequirements2KHR"));
#endif /* defined(VK_KHR_get_memory_requirements2) */
#if defined(VK_KHR_line_rasterization)
	vkCmdSetLineStippleKHR = reinterpret_cast<PFN_vkCmdSetLineStippleKHR>(load(context, "vkCmdSetLineStippleKHR"));
#endif /* defined(VK_KHR_line_rasterization) */
#if defined(VK_KHR_maintenance1)
	vkTrimCommandPoolKHR = reinterpret_cast<PFN_vkTrimCommandPoolKHR>(load(context, "vkTrimCommandPoolKHR"));
#endif /* defined(VK_KHR_maintenance1) */
#if defined(VK_KHR_maintenance3)
	vkGetDescriptorSetLayoutSupportKHR = reinterpret_cast<PFN_vkGetDescriptorSetLayoutSupportKHR>(load(context, "vkGetDescriptorSetLayoutSupportKHR"));
#endif /* defined(VK_KHR_maintenance3) */
#if defined(VK_KHR_maintenance4)
	vkGetDeviceBufferMemoryRequirementsKHR = reinterpret_cast<PFN_vkGetDeviceBufferMemoryRequirementsKHR>(load(context, "vkGetDeviceBufferMemoryRequirementsKHR"));
	vkGetDeviceImageMemoryRequirementsKHR = reinterpret_cast<PFN_vkGetDeviceImageMemoryRequirementsKHR>(load(context, "vkGetDeviceImageMemoryRequirementsKHR"));
	vkGetDeviceImageSparseMemoryRequirementsKHR = reinterpret_cast<PFN_vkGetDeviceImageSparseMemoryRequirementsKHR>(load(context, "vkGetDeviceImageSparseMemoryRequirementsKHR"));
#endif /* defined(VK_KHR_maintenance4) */
#if defined(VK_KHR_maintenance5)
	vkCmdBindIndexBuffer2KHR = reinterpret_cast<PFN_vkCmdBindIndexBuffer2KHR>(load(context, "vkCmdBindIndexBuffer2KHR"));
	vkGetDeviceImageSubresourceLayoutKHR = reinterpret_cast<PFN_vkGetDeviceImageSubresourceLayoutKHR>(load(context, "vkGetDeviceImageSubresourceLayoutKHR"));
	vkGetImageSubresourceLayout2KHR = reinterpret_cast<PFN_vkGetImageSubresourceLayout2KHR>(load(context, "vkGetImageSubresourceLayout2KHR"));
	vkGetRenderingAreaGranularityKHR = reinterpret_cast<PFN_vkGetRenderingAreaGranularityKHR>(load(context, "vkGetRenderingAreaGranularityKHR"));
#endif /* defined(VK_KHR_maintenance5) */
#if defined(VK_KHR_maintenance6)
	vkCmdBindDescriptorSets2KHR = reinterpret_cast<PFN_vkCmdBindDescriptorSets2KHR>(load(context, "vkCmdBindDescriptorSets2KHR"));
	vkCmdPushConstants2KHR = reinterpret_cast<PFN_vkCmdPushConstants2KHR>(load(context, "vkCmdPushConstants2KHR"));
#endif /* defined(VK_KHR_maintenance6) */
#if defined(VK_KHR_maintenance6) && defined(VK_KHR_push_descriptor)
	vkCmdPushDescriptorSet2KHR = reinterpret_cast<PFN_vkCmdPushDescriptorSet2KHR>(load(context, "vkCmdPushDescriptorSet2KHR"));
	vkCmdPushDescriptorSetWithTemplate2KHR = reinterpret_cast<PFN_vkCmdPushDescriptorSetWithTemplate2KHR>(load(context, "vkCmdPushDescriptorSetWithTemplate2KHR"));
#endif /* defined(VK_KHR_maintenance6) && defined(VK_KHR_push_descriptor) */
#if defined(VK_KHR_maintenance6) && defined(VK_EXT_descriptor_buffer)
	vkCmdBindDescriptorBufferEmbeddedSamplers2EXT = reinterpret_cast<PFN_vkCmdBindDescriptorBufferEmbeddedSamplers2EXT>(load(context, "vkCmdBindDescriptorBufferEmbeddedSamplers2EXT"));
	vkCmdSetDescriptorBufferOffsets2EXT = reinterpret_cast<PFN_vkCmdSetDescriptorBufferOffsets2EXT>(load(context, "vkCmdSetDescriptorBufferOffsets2EXT"));
#endif /* defined(VK_KHR_maintenance6) && defined(VK_EXT_descriptor_buffer) */
#if defined(VK_KHR_map_memory2)
	vkMapMemory2KHR = reinterpret_cast<PFN_vkMapMemory2KHR>(load(context, "vkMapMemory2KHR"));
	vkUnmapMemory2KHR = reinterpret_cast<PFN_vkUnmapMemory2KHR>(load(context, "vkUnmapMemory2KHR"));
#endif /* defined(VK_KHR_map_memory2) */
#if defined(VK_KHR_performance_query)
	vkAcquireProfilingLockKHR = reinterpret_cast<PFN_vkAcquireProfilingLockKHR>(load(context, "vkAcquireProfilingLockKHR"));
	vkReleaseProfilingLockKHR = reinterpret_cast<PFN_vkReleaseProfilingLockKHR>(load(context, "vkReleaseProfilingLockKHR"));
#endif /* defined(VK_KHR_performance_query) */
#if defined(VK_KHR_pipeline_executable_properties)
	vkGetPipelineExecutableInternalRepresentationsKHR = reinterpret_cast<PFN_vkGetPipelineExecutableInternalRepresentationsKHR>(load(context, "vkGetPipelineExecutableInternalRepresentationsKHR"));
	vkGetPipelineExecutablePropertiesKHR = reinterpret_cast<PFN_vkGetPipelineExecutablePropertiesKHR>(load(context, "vkGetPipelineExecutablePropertiesKHR"));
	vkGetPipelineExecutableStatisticsKHR = reinterpret_cast<PFN_vkGetPipelineExecutableStatisticsKHR>(load(context, "vkGetPipelineExecutableStatisticsKHR"));
#endif /* defined(VK_KHR_pipeline_executable_properties) */
#if defined(VK_KHR_present_wait)
	vkWaitForPresentKHR = reinterpret_cast<PFN_vkWaitForPresentKHR>(load(context, "vkWaitForPresentKHR"));
#endif /* defined(VK_KHR_present_wait) */
#if defined(VK_KHR_push_descriptor)
	vkCmdPushDescriptorSetKHR = reinterpret_cast<PFN_vkCmdPushDescriptorSetKHR>(load(context, "vkCmdPushDescriptorSetKHR"));
#endif /* defined(VK_KHR_push_descriptor) */
#if defined(VK_KHR_ray_tracing_maintenance1) && defined(VK_KHR_ray_tracing_pipeline)
	vkCmdTraceRaysIndirect2KHR = reinterpret_cast<PFN_vkCmdTraceRaysIndirect2KHR>(load(context, "vkCmdTraceRaysIndirect2KHR"));
#endif /* defined(VK_KHR_ray_tracing_maintenance1) && defined(VK_KHR_ray_tracing_pipeline) */
#if defined(VK_KHR_ray_tracing_pipeline)
	vkCmdSetRayTracingPipelineStackSizeKHR = reinterpret_cast<PFN_vkCmdSetRayTracingPipelineStackSizeKHR>(load(context, "vkCmdSetRayTracingPipelineStackSizeKHR"));
	vkCmdTraceRaysIndirectKHR = reinterpret_cast<PFN_vkCmdTraceRaysIndirectKHR>(load(context, "vkCmdTraceRaysIndirectKHR"));
	vkCmdTraceRaysKHR = reinterpret_cast<PFN_vkCmdTraceRaysKHR>(load(context, "vkCmdTraceRaysKHR"));
	vkCreateRayTracingPipelinesKHR = reinterpret_cast<PFN_vkCreateRayTracingPipelinesKHR>(load(context, "vkCreateRayTracingPipelinesKHR"));
	vkGetRayTracingCaptureReplayShaderGroupHandlesKHR = reinterpret_cast<PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR>(load(context, "vkGetRayTracingCaptureReplayShaderGroupHandlesKHR"));
	vkGetRayTracingShaderGroupHandlesKHR = reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesKHR>(load(context, "vkGetRayTracingShaderGroupHandlesKHR"));
	vkGetRayTracingShaderGroupStackSizeKHR = reinterpret_cast<PFN_vkGetRayTracingShaderGroupStackSizeKHR>(load(context, "vkGetRayTracingShaderGroupStackSizeKHR"));
#endif /* defined(VK_KHR_ray_tracing_pipeline) */
#if defined(VK_KHR_sampler_ycbcr_conversion)
	vkCreateSamplerYcbcrConversionKHR = reinterpret_cast<PFN_vkCreateSamplerYcbcrConversionKHR>(load(context, "vkCreateSamplerYcbcrConversionKHR"));
	vkDestroySamplerYcbcrConversionKHR = reinterpret_cast<PFN_vkDestroySamplerYcbcrConversionKHR>(load(context, "vkDestroySamplerYcbcrConversionKHR"));
#endif /* defined(VK_KHR_sampler_ycbcr_conversion) */
#if defined(VK_KHR_shared_presentable_image)
	vkGetSwapchainStatusKHR = reinterpret_cast<PFN_vkGetSwapchainStatusKHR>(load(context, "vkGetSwapchainStatusKHR"));
#endif /* defined(VK_KHR_shared_presentable_image) */
#if defined(VK_KHR_swapchain)
	vkAcquireNextImageKHR = reinterpret_cast<PFN_vkAcquireNextImageKHR>(load(context, "vkAcquireNextImageKHR"));
	vkCreateSwapchainKHR = reinterpret_cast<PFN_vkCreateSwapchainKHR>(load(context, "vkCreateSwapchainKHR"));
	vkDestroySwapchainKHR = reinterpret_cast<PFN_vkDestroySwapchainKHR>(load(context, "vkDestroySwapchainKHR"));
	vkGetSwapchainImagesKHR = reinterpret_cast<PFN_vkGetSwapchainImagesKHR>(load(context, "vkGetSwapchainImagesKHR"));
	vkQueuePresentKHR = reinterpret_cast<PFN_vkQueuePresentKHR>(load(context, "vkQueuePresentKHR"));
#endif /* defined(VK_KHR_swapchain) */
#if defined(VK_KHR_synchronization2)
	vkCmdPipelineBarrier2KHR = reinterpret_cast<PFN_vkCmdPipelineBarrier2KHR>(load(context, "vkCmdPipelineBarrier2KHR"));
	vkCmdResetEvent2KHR = reinterpret_cast<PFN_vkCmdResetEvent2KHR>(load(context, "vkCmdResetEvent2KHR"));
	vkCmdSetEvent2KHR = reinterpret_cast<PFN_vkCmdSetEvent2KHR>(load(context, "vkCmdSetEvent2KHR"));
	vkCmdWaitEvents2KHR = reinterpret_cast<PFN_vkCmdWaitEvents2KHR>(load(context, "vkCmdWaitEvents2KHR"));
	vkCmdWriteTimestamp2KHR = reinterpret_cast<PFN_vkCmdWriteTimestamp2KHR>(load(context, "vkCmdWriteTimestamp2KHR"));
	vkQueueSubmit2KHR = reinterpret_cast<PFN_vkQueueSubmit2KHR>(load(context, "vkQueueSubmit2KHR"));
#endif /* defined(VK_KHR_synchronization2) */
#if defined(VK_KHR_synchronization2) && defined(VK_AMD_buffer_marker)
	vkCmdWriteBufferMarker2AMD = reinterpret_cast<PFN_vkCmdWriteBufferMarker2AMD>(load(context, "vkCmdWriteBufferMarker2AMD"));
#endif /* defined(VK_KHR_synchronization2) && defined(VK_AMD_buffer_marker) */
#if defined(VK_KHR_synchronization2) && defined(VK_NV_device_diagnostic_checkpoints)
	vkGetQueueCheckpointData2NV = reinterpret_cast<PFN_vkGetQueueCheckpointData2NV>(load(context, "vkGetQueueCheckpointData2NV"));
#endif /* defined(VK_KHR_synchronization2) && defined(VK_NV_device_diagnostic_checkpoints) */
#if defined(VK_KHR_timeline_semaphore)
	vkGetSemaphoreCounterValueKHR = reinterpret_cast<PFN_vkGetSemaphoreCounterValueKHR>(load(context, "vkGetSemaphoreCounterValueKHR"));
	vkSignalSemaphoreKHR = reinterpret_cast<PFN_vkSignalSemaphoreKHR>(load(context, "vkSignalSemaphoreKHR"));
	vkWaitSemaphoresKHR = reinterpret_cast<PFN_vkWaitSemaphoresKHR>(load(context, "vkWaitSemaphoresKHR"));
#endif /* defined(VK_KHR_timeline_semaphore) */
#if defined(VK_KHR_video_decode_queue)
	vkCmdDecodeVideoKHR = reinterpret_cast<PFN_vkCmdDecodeVideoKHR>(load(context, "vkCmdDecodeVideoKHR"));
#endif /* defined(VK_KHR_video_decode_queue) */
#if defined(VK_KHR_video_encode_queue)
	vkCmdEncodeVideoKHR = reinterpret_cast<PFN_vkCmdEncodeVideoKHR>(load(context, "vkCmdEncodeVideoKHR"));
	vkGetEncodedVideoSessionParametersKHR = reinterpret_cast<PFN_vkGetEncodedVideoSessionParametersKHR>(load(context, "vkGetEncodedVideoSessionParametersKHR"));
#endif /* defined(VK_KHR_video_encode_queue) */
#if defined(VK_KHR_video_queue)
	vkBindVideoSessionMemoryKHR = reinterpret_cast<PFN_vkBindVideoSessionMemoryKHR>(load(context, "vkBindVideoSessionMemoryKHR"));
	vkCmdBeginVideoCodingKHR = reinterpret_cast<PFN_vkCmdBeginVideoCodingKHR>(load(context, "vkCmdBeginVideoCodingKHR"));
	vkCmdControlVideoCodingKHR = reinterpret_cast<PFN_vkCmdControlVideoCodingKHR>(load(context, "vkCmdControlVideoCodingKHR"));
	vkCmdEndVideoCodingKHR = reinterpret_cast<PFN_vkCmdEndVideoCodingKHR>(load(context, "vkCmdEndVideoCodingKHR"));
	vkCreateVideoSessionKHR = reinterpret_cast<PFN_vkCreateVideoSessionKHR>(load(context, "vkCreateVideoSessionKHR"));
	vkCreateVideoSessionParametersKHR = reinterpret_cast<PFN_vkCreateVideoSessionParametersKHR>(load(context, "vkCreateVideoSessionParametersKHR"));
	vkDestroyVideoSessionKHR = reinterpret_cast<PFN_vkDestroyVideoSessionKHR>(load(context, "vkDestroyVideoSessionKHR"));
	vkDestroyVideoSessionParametersKHR = reinterpret_cast<PFN_vkDestroyVideoSessionParametersKHR>(load(context, "vkDestroyVideoSessionParametersKHR"));
	vkGetVideoSessionMemoryRequirementsKHR = reinterpret_cast<PFN_vkGetVideoSessionMemoryRequirementsKHR>(load(context, "vkGetVideoSessionMemoryRequirementsKHR"));
	vkUpdateVideoSessionParametersKHR = reinterpret_cast<PFN_vkUpdateVideoSessionParametersKHR>(load(context, "vkUpdateVideoSessionParametersKHR"));
#endif /* defined(VK_KHR_video_queue) */
#if defined(VK_NVX_binary_import)
	vkCmdCuLaunchKernelNVX = reinterpret_cast<PFN_vkCmdCuLaunchKernelNVX>(load(context, "vkCmdCuLaunchKernelNVX"));
	vkCreateCuFunctionNVX = reinterpret_cast<PFN_vkCreateCuFunctionNVX>(load(context, "vkCreateCuFunctionNVX"));
	vkCreateCuModuleNVX = reinterpret_cast<PFN_vkCreateCuModuleNVX>(load(context, "vkCreateCuModuleNVX"));
	vkDestroyCuFunctionNVX = reinterpret_cast<PFN_vkDestroyCuFunctionNVX>(load(context, "vkDestroyCuFunctionNVX"));
	vkDestroyCuModuleNVX = reinterpret_cast<PFN_vkDestroyCuModuleNVX>(load(context, "vkDestroyCuModuleNVX"));
#endif /* defined(VK_NVX_binary_import) */
#if defined(VK_NVX_image_view_handle)
	vkGetImageViewAddressNVX = reinterpret_cast<PFN_vkGetImageViewAddressNVX>(load(context, "vkGetImageViewAddressNVX"));
	vkGetImageViewHandleNVX = reinterpret_cast<PFN_vkGetImageViewHandleNVX>(load(context, "vkGetImageViewHandleNVX"));
#endif /* defined(VK_NVX_image_view_handle) */
#if defined(VK_NV_clip_space_w_scaling)
	vkCmdSetViewportWScalingNV = reinterpret_cast<PFN_vkCmdSetViewportWScalingNV>(load(context, "vkCmdSetViewportWScalingNV"));
#endif /* defined(VK_NV_clip_space_w_scaling) */
#if defined(VK_NV_copy_memory_indirect)
	vkCmdCopyMemoryIndirectNV = reinterpret_cast<PFN_vkCmdCopyMemoryIndirectNV>(load(context, "vkCmdCopyMemoryIndirectNV"));
	vkCmdCopyMemoryToImageIndirectNV = reinterpret_cast<PFN_vkCmdCopyMemoryToImageIndirectNV>(load(context, "vkCmdCopyMemoryToImageIndirectNV"));
#endif /* defined(VK_NV_copy_memory_indirect) */
#if defined(VK_NV_cuda_kernel_launch)
	vkCmdCudaLaunchKernelNV = reinterpret_cast<PFN_vkCmdCudaLaunchKernelNV>(load(context, "vkCmdCudaLaunchKernelNV"));
	vkCreateCudaFunctionNV = reinterpret_cast<PFN_vkCreateCudaFunctionNV>(load(context, "vkCreateCudaFunctionNV"));
	vkCreateCudaModuleNV = reinterpret_cast<PFN_vkCreateCudaModuleNV>(load(context, "vkCreateCudaModuleNV"));
	vkDestroyCudaFunctionNV = reinterpret_cast<PFN_vkDestroyCudaFunctionNV>(load(context, "vkDestroyCudaFunctionNV"));
	vkDestroyCudaModuleNV = reinterpret_cast<PFN_vkDestroyCudaModuleNV>(load(context, "vkDestroyCudaModuleNV"));
	vkGetCudaModuleCacheNV = reinterpret_cast<PFN_vkGetCudaModuleCacheNV>(load(context, "vkGetCudaModuleCacheNV"));
#endif /* defined(VK_NV_cuda_kernel_launch) */
#if defined(VK_NV_device_diagnostic_checkpoints)
	vkCmdSetCheckpointNV = reinterpret_cast<PFN_vkCmdSetCheckpointNV>(load(context, "vkCmdSetCheckpointNV"));
	vkGetQueueCheckpointDataNV = reinterpret_cast<PFN_vkGetQueueCheckpointDataNV>(load(context, "vkGetQueueCheckpointDataNV"));
#endif /* defined(VK_NV_device_diagnostic_checkpoints) */
#if defined(VK_NV_device_generated_commands)
	vkCmdBindPipelineShaderGroupNV = reinterpret_cast<PFN_vkCmdBindPipelineShaderGroupNV>(load(context, "vkCmdBindPipelineShaderGroupNV"));
	vkCmdExecuteGeneratedCommandsNV = reinterpret_cast<PFN_vkCmdExecuteGeneratedCommandsNV>(load(context, "vkCmdExecuteGeneratedCommandsNV"));
	vkCmdPreprocessGeneratedCommandsNV = reinterpret_cast<PFN_vkCmdPreprocessGeneratedCommandsNV>(load(context, "vkCmdPreprocessGeneratedCommandsNV"));
	vkCreateIndirectCommandsLayoutNV = reinterpret_cast<PFN_vkCreateIndirectCommandsLayoutNV>(load(context, "vkCreateIndirectCommandsLayoutNV"));
	vkDestroyIndirectCommandsLayoutNV = reinterpret_cast<PFN_vkDestroyIndirectCommandsLayoutNV>(load(context, "vkDestroyIndirectCommandsLayoutNV"));
	vkGetGeneratedCommandsMemoryRequirementsNV = reinterpret_cast<PFN_vkGetGeneratedCommandsMemoryRequirementsNV>(load(context, "vkGetGeneratedCommandsMemoryRequirementsNV"));
#endif /* defined(VK_NV_device_generated_commands) */
#if defined(VK_NV_device_generated_commands_compute)
	vkCmdUpdatePipelineIndirectBufferNV = reinterpret_cast<PFN_vkCmdUpdatePipelineIndirectBufferNV>(load(context, "vkCmdUpdatePipelineIndirectBufferNV"));
	vkGetPipelineIndirectDeviceAddressNV = reinterpret_cast<PFN_vkGetPipelineIndirectDeviceAddressNV>(load(context, "vkGetPipelineIndirectDeviceAddressNV"));
	vkGetPipelineIndirectMemoryRequirementsNV = reinterpret_cast<PFN_vkGetPipelineIndirectMemoryRequirementsNV>(load(context, "vkGetPipelineIndirectMemoryRequirementsNV"));
#endif /* defined(VK_NV_device_generated_commands_compute) */
#if defined(VK_NV_external_memory_rdma)
	vkGetMemoryRemoteAddressNV = reinterpret_cast<PFN_vkGetMemoryRemoteAddressNV>(load(context, "vkGetMemoryRemoteAddressNV"));
#endif /* defined(VK_NV_external_memory_rdma) */
#if defined(VK_NV_external_memory_win32)
	vkGetMemoryWin32HandleNV = reinterpret_cast<PFN_vkGetMemoryWin32HandleNV>(load(context, "vkGetMemoryWin32HandleNV"));
#endif /* defined(VK_NV_external_memory_win32) */
#if defined(VK_NV_fragment_shading_rate_enums)
	vkCmdSetFragmentShadingRateEnumNV = reinterpret_cast<PFN_vkCmdSetFragmentShadingRateEnumNV>(load(context, "vkCmdSetFragmentShadingRateEnumNV"));
#endif /* defined(VK_NV_fragment_shading_rate_enums) */
#if defined(VK_NV_low_latency2)
	vkGetLatencyTimingsNV = reinterpret_cast<PFN_vkGetLatencyTimingsNV>(load(context, "vkGetLatencyTimingsNV"));
	vkLatencySleepNV = reinterpret_cast<PFN_vkLatencySleepNV>(load(context, "vkLatencySleepNV"));
	vkQueueNotifyOutOfBandNV = reinterpret_cast<PFN_vkQueueNotifyOutOfBandNV>(load(context, "vkQueueNotifyOutOfBandNV"));
	vkSetLatencyMarkerNV = reinterpret_cast<PFN_vkSetLatencyMarkerNV>(load(context, "vkSetLatencyMarkerNV"));
	vkSetLatencySleepModeNV = reinterpret_cast<PFN_vkSetLatencySleepModeNV>(load(context, "vkSetLatencySleepModeNV"));
#endif /* defined(VK_NV_low_latency2) */
#if defined(VK_NV_memory_decompression)
	vkCmdDecompressMemoryIndirectCountNV = reinterpret_cast<PFN_vkCmdDecompressMemoryIndirectCountNV>(load(context, "vkCmdDecompressMemoryIndirectCountNV"));
	vkCmdDecompressMemoryNV = reinterpret_cast<PFN_vkCmdDecompressMemoryNV>(load(context, "vkCmdDecompressMemoryNV"));
#endif /* defined(VK_NV_memory_decompression) */
#if defined(VK_NV_mesh_shader)
	vkCmdDrawMeshTasksIndirectCountNV = reinterpret_cast<PFN_vkCmdDrawMeshTasksIndirectCountNV>(load(context, "vkCmdDrawMeshTasksIndirectCountNV"));
	vkCmdDrawMeshTasksIndirectNV = reinterpret_cast<PFN_vkCmdDrawMeshTasksIndirectNV>(load(context, "vkCmdDrawMeshTasksIndirectNV"));
	vkCmdDrawMeshTasksNV = reinterpret_cast<PFN_vkCmdDrawMeshTasksNV>(load(context, "vkCmdDrawMeshTasksNV"));
#endif /* defined(VK_NV_mesh_shader) */
#if defined(VK_NV_optical_flow)
	vkBindOpticalFlowSessionImageNV = reinterpret_cast<PFN_vkBindOpticalFlowSessionImageNV>(load(context, "vkBindOpticalFlowSessionImageNV"));
	vkCmdOpticalFlowExecuteNV = reinterpret_cast<PFN_vkCmdOpticalFlowExecuteNV>(load(context, "vkCmdOpticalFlowExecuteNV"));
	vkCreateOpticalFlowSessionNV = reinterpret_cast<PFN_vkCreateOpticalFlowSessionNV>(load(context, "vkCreateOpticalFlowSessionNV"));
	vkDestroyOpticalFlowSessionNV = reinterpret_cast<PFN_vkDestroyOpticalFlowSessionNV>(load(context, "vkDestroyOpticalFlowSessionNV"));
#endif /* defined(VK_NV_optical_flow) */
#if defined(VK_NV_ray_tracing)
	vkBindAccelerationStructureMemoryNV = reinterpret_cast<PFN_vkBindAccelerationStructureMemoryNV>(load(context, "vkBindAccelerationStructureMemoryNV"));
	vkCmdBuildAccelerationStructureNV = reinterpret_cast<PFN_vkCmdBuildAccelerationStructureNV>(load(context, "vkCmdBuildAccelerationStructureNV"));
	vkCmdCopyAccelerationStructureNV = reinterpret_cast<PFN_vkCmdCopyAccelerationStructureNV>(load(context, "vkCmdCopyAccelerationStructureNV"));
	vkCmdTraceRaysNV = reinterpret_cast<PFN_vkCmdTraceRaysNV>(load(context, "vkCmdTraceRaysNV"));
	vkCmdWriteAccelerationStructuresPropertiesNV = reinterpret_cast<PFN_vkCmdWriteAccelerationStructuresPropertiesNV>(load(context, "vkCmdWriteAccelerationStructuresPropertiesNV"));
	vkCompileDeferredNV = reinterpret_cast<PFN_vkCompileDeferredNV>(load(context, "vkCompileDeferredNV"));
	vkCreateAccelerationStructureNV = reinterpret_cast<PFN_vkCreateAccelerationStructureNV>(load(context, "vkCreateAccelerationStructureNV"));
	vkCreateRayTracingPipelinesNV = reinterpret_cast<PFN_vkCreateRayTracingPipelinesNV>(load(context, "vkCreateRayTracingPipelinesNV"));
	vkDestroyAccelerationStructureNV = reinterpret_cast<PFN_vkDestroyAccelerationStructureNV>(load(context, "vkDestroyAccelerationStructureNV"));
	vkGetAccelerationStructureHandleNV = reinterpret_cast<PFN_vkGetAccelerationStructureHandleNV>(load(context, "vkGetAccelerationStructureHandleNV"));
	vkGetAccelerationStructureMemoryRequirementsNV = reinterpret_cast<PFN_vkGetAccelerationStructureMemoryRequirementsNV>(load(context, "vkGetAccelerationStructureMemoryRequirementsNV"));
	vkGetRayTracingShaderGroupHandlesNV = reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesNV>(load(context, "vkGetRayTracingShaderGroupHandlesNV"));
#endif /* defined(VK_NV_ray_tracing) */
#if defined(VK_NV_scissor_exclusive) && VK_NV_SCISSOR_EXCLUSIVE_SPEC_VERSION >= 2
	vkCmdSetExclusiveScissorEnableNV = reinterpret_cast<PFN_vkCmdSetExclusiveScissorEnableNV>(load(context, "vkCmdSetExclusiveScissorEnableNV"));
#endif /* defined(VK_NV_scissor_exclusive) && VK_NV_SCISSOR_EXCLUSIVE_SPEC_VERSION >= 2 */
#if defined(VK_NV_scissor_exclusive)
	vkCmdSetExclusiveScissorNV = reinterpret_cast<PFN_vkCmdSetExclusiveScissorNV>(load(context, "vkCmdSetExclusiveScissorNV"));
#endif /* defined(VK_NV_scissor_exclusive) */
#if defined(VK_NV_shading_rate_image)
	vkCmdBindShadingRateImageNV = reinterpret_cast<PFN_vkCmdBindShadingRateImageNV>(load(context, "vkCmdBindShadingRateImageNV"));
	vkCmdSetCoarseSampleOrderNV = reinterpret_cast<PFN_vkCmdSetCoarseSampleOrderNV>(load(context, "vkCmdSetCoarseSampleOrderNV"));
	vkCmdSetViewportShadingRatePaletteNV = reinterpret_cast<PFN_vkCmdSetViewportShadingRatePaletteNV>(load(context, "vkCmdSetViewportShadingRatePaletteNV"));
#endif /* defined(VK_NV_shading_rate_image) */
#if defined(VK_QCOM_tile_properties)
	vkGetDynamicRenderingTilePropertiesQCOM = reinterpret_cast<PFN_vkGetDynamicRenderingTilePropertiesQCOM>(load(context, "vkGetDynamicRenderingTilePropertiesQCOM"));
	vkGetFramebufferTilePropertiesQCOM = reinterpret_cast<PFN_vkGetFramebufferTilePropertiesQCOM>(load(context, "vkGetFramebufferTilePropertiesQCOM"));
#endif /* defined(VK_QCOM_tile_properties) */
#if defined(VK_QNX_external_memory_screen_buffer)
	vkGetScreenBufferPropertiesQNX = reinterpret_cast<PFN_vkGetScreenBufferPropertiesQNX>(load(context, "vkGetScreenBufferPropertiesQNX"));
#endif /* defined(VK_QNX_external_memory_screen_buffer) */
#if defined(VK_VALVE_descriptor_set_host_mapping)
	vkGetDescriptorSetHostMappingVALVE = reinterpret_cast<PFN_vkGetDescriptorSetHostMappingVALVE>(load(context, "vkGetDescriptorSetHostMappingVALVE"));
	vkGetDescriptorSetLayoutHostMappingInfoVALVE = reinterpret_cast<PFN_vkGetDescriptorSetLayoutHostMappingInfoVALVE>(load(context, "vkGetDescriptorSetLayoutHostMappingInfoVALVE"));
#endif /* defined(VK_VALVE_descriptor_set_host_mapping) */
#if (defined(VK_EXT_extended_dynamic_state)) || (defined(VK_EXT_shader_object))
	vkCmdBindVertexBuffers2EXT = reinterpret_cast<PFN_vkCmdBindVertexBuffers2EXT>(load(context, "vkCmdBindVertexBuffers2EXT"));
	vkCmdSetCullModeEXT = reinterpret_cast<PFN_vkCmdSetCullModeEXT>(load(context, "vkCmdSetCullModeEXT"));
	vkCmdSetDepthBoundsTestEnableEXT = reinterpret_cast<PFN_vkCmdSetDepthBoundsTestEnableEXT>(load(context, "vkCmdSetDepthBoundsTestEnableEXT"));
	vkCmdSetDepthCompareOpEXT = reinterpret_cast<PFN_vkCmdSetDepthCompareOpEXT>(load(context, "vkCmdSetDepthCompareOpEXT"));
	vkCmdSetDepthTestEnableEXT = reinterpret_cast<PFN_vkCmdSetDepthTestEnableEXT>(load(context, "vkCmdSetDepthTestEnableEXT"));
	vkCmdSetDepthWriteEnableEXT = reinterpret_cast<PFN_vkCmdSetDepthWriteEnableEXT>(load(context, "vkCmdSetDepthWriteEnableEXT"));
	vkCmdSetFrontFaceEXT = reinterpret_cast<PFN_vkCmdSetFrontFaceEXT>(load(context, "vkCmdSetFrontFaceEXT"));
	vkCmdSetPrimitiveTopologyEXT = reinterpret_cast<PFN_vkCmdSetPrimitiveTopologyEXT>(load(context, "vkCmdSetPrimitiveTopologyEXT"));
	vkCmdSetScissorWithCountEXT = reinterpret_cast<PFN_vkCmdSetScissorWithCountEXT>(load(context, "vkCmdSetScissorWithCountEXT"));
	vkCmdSetStencilOpEXT = reinterpret_cast<PFN_vkCmdSetStencilOpEXT>(load(context, "vkCmdSetStencilOpEXT"));
	vkCmdSetStencilTestEnableEXT = reinterpret_cast<PFN_vkCmdSetStencilTestEnableEXT>(load(context, "vkCmdSetStencilTestEnableEXT"));
	vkCmdSetViewportWithCountEXT = reinterpret_cast<PFN_vkCmdSetViewportWithCountEXT>(load(context, "vkCmdSetViewportWithCountEXT"));
#endif /* (defined(VK_EXT_extended_dynamic_state)) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state2)) || (defined(VK_EXT_shader_object))
	vkCmdSetDepthBiasEnableEXT = reinterpret_cast<PFN_vkCmdSetDepthBiasEnableEXT>(load(context, "vkCmdSetDepthBiasEnableEXT"));
	vkCmdSetLogicOpEXT = reinterpret_cast<PFN_vkCmdSetLogicOpEXT>(load(context, "vkCmdSetLogicOpEXT"));
	vkCmdSetPatchControlPointsEXT = reinterpret_cast<PFN_vkCmdSetPatchControlPointsEXT>(load(context, "vkCmdSetPatchControlPointsEXT"));
	vkCmdSetPrimitiveRestartEnableEXT = reinterpret_cast<PFN_vkCmdSetPrimitiveRestartEnableEXT>(load(context, "vkCmdSetPrimitiveRestartEnableEXT"));
	vkCmdSetRasterizerDiscardEnableEXT = reinterpret_cast<PFN_vkCmdSetRasterizerDiscardEnableEXT>(load(context, "vkCmdSetRasterizerDiscardEnableEXT"));
#endif /* (defined(VK_EXT_extended_dynamic_state2)) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state3)) || (defined(VK_EXT_shader_object))
	vkCmdSetAlphaToCoverageEnableEXT = reinterpret_cast<PFN_vkCmdSetAlphaToCoverageEnableEXT>(load(context, "vkCmdSetAlphaToCoverageEnableEXT"));
	vkCmdSetAlphaToOneEnableEXT = reinterpret_cast<PFN_vkCmdSetAlphaToOneEnableEXT>(load(context, "vkCmdSetAlphaToOneEnableEXT"));
	vkCmdSetColorBlendEnableEXT = reinterpret_cast<PFN_vkCmdSetColorBlendEnableEXT>(load(context, "vkCmdSetColorBlendEnableEXT"));
	vkCmdSetColorBlendEquationEXT = reinterpret_cast<PFN_vkCmdSetColorBlendEquationEXT>(load(context, "vkCmdSetColorBlendEquationEXT"));
	vkCmdSetColorWriteMaskEXT = reinterpret_cast<PFN_vkCmdSetColorWriteMaskEXT>(load(context, "vkCmdSetColorWriteMaskEXT"));
	vkCmdSetDepthClampEnableEXT = reinterpret_cast<PFN_vkCmdSetDepthClampEnableEXT>(load(context, "vkCmdSetDepthClampEnableEXT"));
	vkCmdSetLogicOpEnableEXT = reinterpret_cast<PFN_vkCmdSetLogicOpEnableEXT>(load(context, "vkCmdSetLogicOpEnableEXT"));
	vkCmdSetPolygonModeEXT = reinterpret_cast<PFN_vkCmdSetPolygonModeEXT>(load(context, "vkCmdSetPolygonModeEXT"));
	vkCmdSetRasterizationSamplesEXT = reinterpret_cast<PFN_vkCmdSetRasterizationSamplesEXT>(load(context, "vkCmdSetRasterizationSamplesEXT"));
	vkCmdSetSampleMaskEXT = reinterpret_cast<PFN_vkCmdSetSampleMaskEXT>(load(context, "vkCmdSetSampleMaskEXT"));
#endif /* (defined(VK_EXT_extended_dynamic_state3)) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state3) && (defined(VK_KHR_maintenance2) || defined(VK_VERSION_1_1))) || (defined(VK_EXT_shader_object))
	vkCmdSetTessellationDomainOriginEXT = reinterpret_cast<PFN_vkCmdSetTessellationDomainOriginEXT>(load(context, "vkCmdSetTessellationDomainOriginEXT"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && (defined(VK_KHR_maintenance2) || defined(VK_VERSION_1_1))) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_transform_feedback)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_transform_feedback))
	vkCmdSetRasterizationStreamEXT = reinterpret_cast<PFN_vkCmdSetRasterizationStreamEXT>(load(context, "vkCmdSetRasterizationStreamEXT"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_transform_feedback)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_transform_feedback)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_conservative_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_conservative_rasterization))
	vkCmdSetConservativeRasterizationModeEXT = reinterpret_cast<PFN_vkCmdSetConservativeRasterizationModeEXT>(load(context, "vkCmdSetConservativeRasterizationModeEXT"));
	vkCmdSetExtraPrimitiveOverestimationSizeEXT = reinterpret_cast<PFN_vkCmdSetExtraPrimitiveOverestimationSizeEXT>(load(context, "vkCmdSetExtraPrimitiveOverestimationSizeEXT"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_conservative_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_conservative_rasterization)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_enable)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_enable))
	vkCmdSetDepthClipEnableEXT = reinterpret_cast<PFN_vkCmdSetDepthClipEnableEXT>(load(context, "vkCmdSetDepthClipEnableEXT"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_enable)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_enable)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_sample_locations)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_sample_locations))
	vkCmdSetSampleLocationsEnableEXT = reinterpret_cast<PFN_vkCmdSetSampleLocationsEnableEXT>(load(context, "vkCmdSetSampleLocationsEnableEXT"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_sample_locations)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_sample_locations)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_blend_operation_advanced)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_blend_operation_advanced))
	vkCmdSetColorBlendAdvancedEXT = reinterpret_cast<PFN_vkCmdSetColorBlendAdvancedEXT>(load(context, "vkCmdSetColorBlendAdvancedEXT"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_blend_operation_advanced)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_blend_operation_advanced)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_provoking_vertex)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_provoking_vertex))
	vkCmdSetProvokingVertexModeEXT = reinterpret_cast<PFN_vkCmdSetProvokingVertexModeEXT>(load(context, "vkCmdSetProvokingVertexModeEXT"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_provoking_vertex)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_provoking_vertex)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_line_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_line_rasterization))
	vkCmdSetLineRasterizationModeEXT = reinterpret_cast<PFN_vkCmdSetLineRasterizationModeEXT>(load(context, "vkCmdSetLineRasterizationModeEXT"));
	vkCmdSetLineStippleEnableEXT = reinterpret_cast<PFN_vkCmdSetLineStippleEnableEXT>(load(context, "vkCmdSetLineStippleEnableEXT"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_line_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_line_rasterization)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_control)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_control))
	vkCmdSetDepthClipNegativeOneToOneEXT = reinterpret_cast<PFN_vkCmdSetDepthClipNegativeOneToOneEXT>(load(context, "vkCmdSetDepthClipNegativeOneToOneEXT"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_control)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_control)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_clip_space_w_scaling)) || (defined(VK_EXT_shader_object) && defined(VK_NV_clip_space_w_scaling))
	vkCmdSetViewportWScalingEnableNV = reinterpret_cast<PFN_vkCmdSetViewportWScalingEnableNV>(load(context, "vkCmdSetViewportWScalingEnableNV"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_clip_space_w_scaling)) || (defined(VK_EXT_shader_object) && defined(VK_NV_clip_space_w_scaling)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_viewport_swizzle)) || (defined(VK_EXT_shader_object) && defined(VK_NV_viewport_swizzle))
	vkCmdSetViewportSwizzleNV = reinterpret_cast<PFN_vkCmdSetViewportSwizzleNV>(load(context, "vkCmdSetViewportSwizzleNV"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_viewport_swizzle)) || (defined(VK_EXT_shader_object) && defined(VK_NV_viewport_swizzle)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_fragment_coverage_to_color)) || (defined(VK_EXT_shader_object) && defined(VK_NV_fragment_coverage_to_color))
	vkCmdSetCoverageToColorEnableNV = reinterpret_cast<PFN_vkCmdSetCoverageToColorEnableNV>(load(context, "vkCmdSetCoverageToColorEnableNV"));
	vkCmdSetCoverageToColorLocationNV = reinterpret_cast<PFN_vkCmdSetCoverageToColorLocationNV>(load(context, "vkCmdSetCoverageToColorLocationNV"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_fragment_coverage_to_color)) || (defined(VK_EXT_shader_object) && defined(VK_NV_fragment_coverage_to_color)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_framebuffer_mixed_samples)) || (defined(VK_EXT_shader_object) && defined(VK_NV_framebuffer_mixed_samples))
	vkCmdSetCoverageModulationModeNV = reinterpret_cast<PFN_vkCmdSetCoverageModulationModeNV>(load(context, "vkCmdSetCoverageModulationModeNV"));
	vkCmdSetCoverageModulationTableEnableNV = reinterpret_cast<PFN_vkCmdSetCoverageModulationTableEnableNV>(load(context, "vkCmdSetCoverageModulationTableEnableNV"));
	vkCmdSetCoverageModulationTableNV = reinterpret_cast<PFN_vkCmdSetCoverageModulationTableNV>(load(context, "vkCmdSetCoverageModulationTableNV"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_framebuffer_mixed_samples)) || (defined(VK_EXT_shader_object) && defined(VK_NV_framebuffer_mixed_samples)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_shading_rate_image)) || (defined(VK_EXT_shader_object) && defined(VK_NV_shading_rate_image))
	vkCmdSetShadingRateImageEnableNV = reinterpret_cast<PFN_vkCmdSetShadingRateImageEnableNV>(load(context, "vkCmdSetShadingRateImageEnableNV"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_shading_rate_image)) || (defined(VK_EXT_shader_object) && defined(VK_NV_shading_rate_image)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_representative_fragment_test)) || (defined(VK_EXT_shader_object) && defined(VK_NV_representative_fragment_test))
	vkCmdSetRepresentativeFragmentTestEnableNV = reinterpret_cast<PFN_vkCmdSetRepresentativeFragmentTestEnableNV>(load(context, "vkCmdSetRepresentativeFragmentTestEnableNV"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_representative_fragment_test)) || (defined(VK_EXT_shader_object) && defined(VK_NV_representative_fragment_test)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_coverage_reduction_mode)) || (defined(VK_EXT_shader_object) && defined(VK_NV_coverage_reduction_mode))
	vkCmdSetCoverageReductionModeNV = reinterpret_cast<PFN_vkCmdSetCoverageReductionModeNV>(load(context, "vkCmdSetCoverageReductionModeNV"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_coverage_reduction_mode)) || (defined(VK_EXT_shader_object) && defined(VK_NV_coverage_reduction_mode)) */
#if (defined(VK_EXT_full_screen_exclusive) && defined(VK_KHR_device_group)) || (defined(VK_EXT_full_screen_exclusive) && defined(VK_VERSION_1_1))
	vkGetDeviceGroupSurfacePresentModes2EXT = reinterpret_cast<PFN_vkGetDeviceGroupSurfacePresentModes2EXT>(load(context, "vkGetDeviceGroupSurfacePresentModes2EXT"));
#endif /* (defined(VK_EXT_full_screen_exclusive) && defined(VK_KHR_device_group)) || (defined(VK_EXT_full_screen_exclusive) && defined(VK_VERSION_1_1)) */
#if (defined(VK_EXT_host_image_copy)) || (defined(VK_EXT_image_compression_control))
	vkGetImageSubresourceLayout2EXT = reinterpret_cast<PFN_vkGetImageSubresourceLayout2EXT>(load(context, "vkGetImageSubresourceLayout2EXT"));
#endif /* (defined(VK_EXT_host_image_copy)) || (defined(VK_EXT_image_compression_control)) */
#if (defined(VK_EXT_shader_object)) || (defined(VK_EXT_vertex_input_dynamic_state))
	vkCmdSetVertexInputEXT = reinterpret_cast<PFN_vkCmdSetVertexInputEXT>(load(context, "vkCmdSetVertexInputEXT"));
#endif /* (defined(VK_EXT_shader_object)) || (defined(VK_EXT_vertex_input_dynamic_state)) */
#if (defined(VK_KHR_descriptor_update_template) && defined(VK_KHR_push_descriptor)) || (defined(VK_KHR_push_descriptor) && defined(VK_VERSION_1_1)) || (defined(VK_KHR_push_descriptor) && defined(VK_KHR_descriptor_update_template))
	vkCmdPushDescriptorSetWithTemplateKHR = reinterpret_cast<PFN_vkCmdPushDescriptorSetWithTemplateKHR>(load(context, "vkCmdPushDescriptorSetWithTemplateKHR"));
#endif /* (defined(VK_KHR_descriptor_update_template) && defined(VK_KHR_push_descriptor)) || (defined(VK_KHR_push_descriptor) && defined(VK_VERSION_1_1)) || (defined(VK_KHR_push_descriptor) && defined(VK_KHR_descriptor_update_template)) */
#if (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
	vkGetDeviceGroupPresentCapabilitiesKHR = reinterpret_cast<PFN_vkGetDeviceGroupPresentCapabilitiesKHR>(load(context, "vkGetDeviceGroupPresentCapabilitiesKHR"));
	vkGetDeviceGroupSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetDeviceGroupSurfacePresentModesKHR>(load(context, "vkGetDeviceGroupSurfacePresentModesKHR"));
#endif /* (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1)) */
#if (defined(VK_KHR_device_group) && defined(VK_KHR_swapchain)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
	vkAcquireNextImage2KHR = reinterpret_cast<PFN_vkAcquireNextImage2KHR>(load(context, "vkAcquireNextImage2KHR"));
#endif /* (defined(VK_KHR_device_group) && defined(VK_KHR_swapchain)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1)) */
	/* VULKANLOADER_GENERATE_LOAD_DEVICE */
}

static void VkGenLoadDeviceTable(VkDeviceTable& table, VkDevice device, VkGenLoaderFunction load)
{
	/* VULKANLOADER_GENERATE_LOAD_DEVICE_TABLE */
#if defined(VK_VERSION_1_0)
	table.vkAllocateCommandBuffers = reinterpret_cast<PFN_vkAllocateCommandBuffers>(load(device, "vkAllocateCommandBuffers"));
	table.vkAllocateDescriptorSets = reinterpret_cast<PFN_vkAllocateDescriptorSets>(load(device, "vkAllocateDescriptorSets"));
	table.vkAllocateMemory = reinterpret_cast<PFN_vkAllocateMemory>(load(device, "vkAllocateMemory"));
	table.vkBeginCommandBuffer = reinterpret_cast<PFN_vkBeginCommandBuffer>(load(device, "vkBeginCommandBuffer"));
	table.vkBindBufferMemory = reinterpret_cast<PFN_vkBindBufferMemory>(load(device, "vkBindBufferMemory"));
	table.vkBindImageMemory = reinterpret_cast<PFN_vkBindImageMemory>(load(device, "vkBindImageMemory"));
	table.vkCmdBeginQuery = reinterpret_cast<PFN_vkCmdBeginQuery>(load(device, "vkCmdBeginQuery"));
	table.vkCmdBeginRenderPass = reinterpret_cast<PFN_vkCmdBeginRenderPass>(load(device, "vkCmdBeginRenderPass"));
	table.vkCmdBindDescriptorSets = reinterpret_cast<PFN_vkCmdBindDescriptorSets>(load(device, "vkCmdBindDescriptorSets"));
	table.vkCmdBindIndexBuffer = reinterpret_cast<PFN_vkCmdBindIndexBuffer>(load(device, "vkCmdBindIndexBuffer"));
	table.vkCmdBindPipeline = reinterpret_cast<PFN_vkCmdBindPipeline>(load(device, "vkCmdBindPipeline"));
	table.vkCmdBindVertexBuffers = reinterpret_cast<PFN_vkCmdBindVertexBuffers>(load(device, "vkCmdBindVertexBuffers"));
	table.vkCmdBlitImage = reinterpret_cast<PFN_vkCmdBlitImage>(load(device, "vkCmdBlitImage"));
	table.vkCmdClearAttachments = reinterpret_cast<PFN_vkCmdClearAttachments>(load(device, "vkCmdClearAttachments"));
	table.vkCmdClearColorImage = reinterpret_cast<PFN_vkCmdClearColorImage>(load(device, "vkCmdClearColorImage"));
	table.vkCmdClearDepthStencilImage = reinterpret_cast<PFN_vkCmdClearDepthStencilImage>(load(device, "vkCmdClearDepthStencilImage"));
	table.vkCmdCopyBuffer = reinterpret_cast<PFN_vkCmdCopyBuffer>(load(device, "vkCmdCopyBuffer"));
	table.vkCmdCopyBufferToImage = reinterpret_cast<PFN_vkCmdCopyBufferToImage>(load(device, "vkCmdCopyBufferToImage"));
	table.vkCmdCopyImage = reinterpret_cast<PFN_vkCmdCopyImage>(load(device, "vkCmdCopyImage"));
	table.vkCmdCopyImageToBuffer = reinterpret_cast<PFN_vkCmdCopyImageToBuffer>(load(device, "vkCmdCopyImageToBuffer"));
	table.vkCmdCopyQueryPoolResults = reinterpret_cast<PFN_vkCmdCopyQueryPoolResults>(load(device, "vkCmdCopyQueryPoolResults"));
	table.vkCmdDispatch = reinterpret_cast<PFN_vkCmdDispatch>(load(device, "vkCmdDispatch"));
	table.vkCmdDispatchIndirect = reinterpret_cast<PFN_vkCmdDispatchIndirect>(load(device, "vkCmdDispatchIndirect"));
	table.vkCmdDraw = reinterpret_cast<PFN_vkCmdDraw>(load(device, "vkCmdDraw"));
	table.vkCmdDrawIndexed = reinterpret_cast<PFN_vkCmdDrawIndexed>(load(device, "vkCmdDrawIndexed"));
	table.vkCmdDrawIndexedIndirect = reinterpret_cast<PFN_vkCmdDrawIndexedIndirect>(load(device, "vkCmdDrawIndexedIndirect"));
	table.vkCmdDrawIndirect = reinterpret_cast<PFN_vkCmdDrawIndirect>(load(device, "vkCmdDrawIndirect"));
	table.vkCmdEndQuery = reinterpret_cast<PFN_vkCmdEndQuery>(load(device, "vkCmdEndQuery"));
	table.vkCmdEndRenderPass = reinterpret_cast<PFN_vkCmdEndRenderPass>(load(device, "vkCmdEndRenderPass"));
	table.vkCmdExecuteCommands = reinterpret_cast<PFN_vkCmdExecuteCommands>(load(device, "vkCmdExecuteCommands"));
	table.vkCmdFillBuffer = reinterpret_cast<PFN_vkCmdFillBuffer>(load(device, "vkCmdFillBuffer"));
	table.vkCmdNextSubpass = reinterpret_cast<PFN_vkCmdNextSubpass>(load(device, "vkCmdNextSubpass"));
	table.vkCmdPipelineBarrier = reinterpret_cast<PFN_vkCmdPipelineBarrier>(load(device, "vkCmdPipelineBarrier"));
	table.vkCmdPushConstants = reinterpret_cast<PFN_vkCmdPushConstants>(load(device, "vkCmdPushConstants"));
	table.vkCmdResetEvent = reinterpret_cast<PFN_vkCmdResetEvent>(load(device, "vkCmdResetEvent"));
	table.vkCmdResetQueryPool = reinterpret_cast<PFN_vkCmdResetQueryPool>(load(device, "vkCmdResetQueryPool"));
	table.vkCmdResolveImage = reinterpret_cast<PFN_vkCmdResolveImage>(load(device, "vkCmdResolveImage"));
	table.vkCmdSetBlendConstants = reinterpret_cast<PFN_vkCmdSetBlendConstants>(load(device, "vkCmdSetBlendConstants"));
	table.vkCmdSetDepthBias = reinterpret_cast<PFN_vkCmdSetDepthBias>(load(device, "vkCmdSetDepthBias"));
	table.vkCmdSetDepthBounds = reinterpret_cast<PFN_vkCmdSetDepthBounds>(load(device, "vkCmdSetDepthBounds"));
	table.vkCmdSetEvent = reinterpret_cast<PFN_vkCmdSetEvent>(load(device, "vkCmdSetEvent"));
	table.vkCmdSetLineWidth = reinterpret_cast<PFN_vkCmdSetLineWidth>(load(device, "vkCmdSetLineWidth"));
	table.vkCmdSetScissor = reinterpret_cast<PFN_vkCmdSetScissor>(load(device, "vkCmdSetScissor"));
	table.vkCmdSetStencilCompareMask = reinterpret_cast<PFN_vkCmdSetStencilCompareMask>(load(device, "vkCmdSetStencilCompareMask"));
	table.vkCmdSetStencilReference = reinterpret_cast<PFN_vkCmdSetStencilReference>(load(device, "vkCmdSetStencilReference"));
	table.vkCmdSetStencilWriteMask = reinterpret_cast<PFN_vkCmdSetStencilWriteMask>(load(device, "vkCmdSetStencilWriteMask"));
	table.vkCmdSetViewport = reinterpret_cast<PFN_vkCmdSetViewport>(load(device, "vkCmdSetViewport"));
	table.vkCmdUpdateBuffer = reinterpret_cast<PFN_vkCmdUpdateBuffer>(load(device, "vkCmdUpdateBuffer"));
	table.vkCmdWaitEvents = reinterpret_cast<PFN_vkCmdWaitEvents>(load(device, "vkCmdWaitEvents"));
	table.vkCmdWriteTimestamp = reinterpret_cast<PFN_vkCmdWriteTimestamp>(load(device, "vkCmdWriteTimestamp"));
	table.vkCreateBuffer = reinterpret_cast<PFN_vkCreateBuffer>(load(device, "vkCreateBuffer"));
	table.vkCreateBufferView = reinterpret_cast<PFN_vkCreateBufferView>(load(device, "vkCreateBufferView"));
	table.vkCreateCommandPool = reinterpret_cast<PFN_vkCreateCommandPool>(load(device, "vkCreateCommandPool"));
	table.vkCreateComputePipelines = reinterpret_cast<PFN_vkCreateComputePipelines>(load(device, "vkCreateComputePipelines"));
	table.vkCreateDescriptorPool = reinterpret_cast<PFN_vkCreateDescriptorPool>(load(device, "vkCreateDescriptorPool"));
	table.vkCreateDescriptorSetLayout = reinterpret_cast<PFN_vkCreateDescriptorSetLayout>(load(device, "vkCreateDescriptorSetLayout"));
	table.vkCreateEvent = reinterpret_cast<PFN_vkCreateEvent>(load(device, "vkCreateEvent"));
	table.vkCreateFence = reinterpret_cast<PFN_vkCreateFence>(load(device, "vkCreateFence"));
	table.vkCreateFramebuffer = reinterpret_cast<PFN_vkCreateFramebuffer>(load(device, "vkCreateFramebuffer"));
	table.vkCreateGraphicsPipelines = reinterpret_cast<PFN_vkCreateGraphicsPipelines>(load(device, "vkCreateGraphicsPipelines"));
	table.vkCreateImage = reinterpret_cast<PFN_vkCreateImage>(load(device, "vkCreateImage"));
	table.vkCreateImageView = reinterpret_cast<PFN_vkCreateImageView>(load(device, "vkCreateImageView"));
	table.vkCreatePipelineCache = reinterpret_cast<PFN_vkCreatePipelineCache>(load(device, "vkCreatePipelineCache"));
	table.vkCreatePipelineLayout = reinterpret_cast<PFN_vkCreatePipelineLayout>(load(device, "vkCreatePipelineLayout"));
	table.vkCreateQueryPool = reinterpret_cast<PFN_vkCreateQueryPool>(load(device, "vkCreateQueryPool"));
	table.vkCreateRenderPass = reinterpret_cast<PFN_vkCreateRenderPass>(load(device, "vkCreateRenderPass"));
	table.vkCreateSampler = reinterpret_cast<PFN_vkCreateSampler>(load(device, "vkCreateSampler"));
	table.vkCreateSemaphore = reinterpret_cast<PFN_vkCreateSemaphore>(load(device, "vkCreateSemaphore"));
	table.vkCreateShaderModule = reinterpret_cast<PFN_vkCreateShaderModule>(load(device, "vkCreateShaderModule"));
	table.vkDestroyBuffer = reinterpret_cast<PFN_vkDestroyBuffer>(load(device, "vkDestroyBuffer"));
	table.vkDestroyBufferView = reinterpret_cast<PFN_vkDestroyBufferView>(load(device, "vkDestroyBufferView"));
	table.vkDestroyCommandPool = reinterpret_cast<PFN_vkDestroyCommandPool>(load(device, "vkDestroyCommandPool"));
	table.vkDestroyDescriptorPool = reinterpret_cast<PFN_vkDestroyDescriptorPool>(load(device, "vkDestroyDescriptorPool"));
	table.vkDestroyDescriptorSetLayout = reinterpret_cast<PFN_vkDestroyDescriptorSetLayout>(load(device, "vkDestroyDescriptorSetLayout"));
	table.vkDestroyDevice = reinterpret_cast<PFN_vkDestroyDevice>(load(device, "vkDestroyDevice"));
	table.vkDestroyEvent = reinterpret_cast<PFN_vkDestroyEvent>(load(device, "vkDestroyEvent"));
	table.vkDestroyFence = reinterpret_cast<PFN_vkDestroyFence>(load(device, "vkDestroyFence"));
	table.vkDestroyFramebuffer = reinterpret_cast<PFN_vkDestroyFramebuffer>(load(device, "vkDestroyFramebuffer"));
	table.vkDestroyImage = reinterpret_cast<PFN_vkDestroyImage>(load(device, "vkDestroyImage"));
	table.vkDestroyImageView = reinterpret_cast<PFN_vkDestroyImageView>(load(device, "vkDestroyImageView"));
	table.vkDestroyPipeline = reinterpret_cast<PFN_vkDestroyPipeline>(load(device, "vkDestroyPipeline"));
	table.vkDestroyPipelineCache = reinterpret_cast<PFN_vkDestroyPipelineCache>(load(device, "vkDestroyPipelineCache"));
	table.vkDestroyPipelineLayout = reinterpret_cast<PFN_vkDestroyPipelineLayout>(load(device, "vkDestroyPipelineLayout"));
	table.vkDestroyQueryPool = reinterpret_cast<PFN_vkDestroyQueryPool>(load(device, "vkDestroyQueryPool"));
	table.vkDestroyRenderPass = reinterpret_cast<PFN_vkDestroyRenderPass>(load(device, "vkDestroyRenderPass"));
	table.vkDestroySampler = reinterpret_cast<PFN_vkDestroySampler>(load(device, "vkDestroySampler"));
	table.vkDestroySemaphore = reinterpret_cast<PFN_vkDestroySemaphore>(load(device, "vkDestroySemaphore"));
	table.vkDestroyShaderModule = reinterpret_cast<PFN_vkDestroyShaderModule>(load(device, "vkDestroyShaderModule"));
	table.vkDeviceWaitIdle = reinterpret_cast<PFN_vkDeviceWaitIdle>(load(device, "vkDeviceWaitIdle"));
	table.vkEndCommandBuffer = reinterpret_cast<PFN_vkEndCommandBuffer>(load(device, "vkEndCommandBuffer"));
	table.vkFlushMappedMemoryRanges = reinterpret_cast<PFN_vkFlushMappedMemoryRanges>(load(device, "vkFlushMappedMemoryRanges"));
	table.vkFreeCommandBuffers = reinterpret_cast<PFN_vkFreeCommandBuffers>(load(device, "vkFreeCommandBuffers"));
	table.vkFreeDescriptorSets = reinterpret_cast<PFN_vkFreeDescriptorSets>(load(device, "vkFreeDescriptorSets"));
	table.vkFreeMemory = reinterpret_cast<PFN_vkFreeMemory>(load(device, "vkFreeMemory"));
	table.vkGetBufferMemoryRequirements = reinterpret_cast<PFN_vkGetBufferMemoryRequirements>(load(device, "vkGetBufferMemoryRequirements"));
	table.vkGetDeviceMemoryCommitment = reinterpret_cast<PFN_vkGetDeviceMemoryCommitment>(load(device, "vkGetDeviceMemoryCommitment"));
	table.vkGetDeviceQueue = reinterpret_cast<PFN_vkGetDeviceQueue>(load(device, "vkGetDeviceQueue"));
	table.vkGetEventStatus = reinterpret_cast<PFN_vkGetEventStatus>(load(device, "vkGetEventStatus"));
	table.vkGetFenceStatus = reinterpret_cast<PFN_vkGetFenceStatus>(load(device, "vkGetFenceStatus"));
	table.vkGetImageMemoryRequirements = reinterpret_cast<PFN_vkGetImageMemoryRequirements>(load(device, "vkGetImageMemoryRequirements"));
	table.vkGetImageSparseMemoryRequirements = reinterpret_cast<PFN_vkGetImageSparseMemoryRequirements>(load(device, "vkGetImageSparseMemoryRequirements"));
	table.vkGetImageSubresourceLayout = reinterpret_cast<PFN_vkGetImageSubresourceLayout>(load(device, "vkGetImageSubresourceLayout"));
	table.vkGetPipelineCacheData = reinterpret_cast<PFN_vkGetPipelineCacheData>(load(device, "vkGetPipelineCacheData"));
	table.vkGetQueryPoolResults = reinterpret_cast<PFN_vkGetQueryPoolResults>(load(device, "vkGetQueryPoolResults"));
	table.vkGetRenderAreaGranularity = reinterpret_cast<PFN_vkGetRenderAreaGranularity>(load(device, "vkGetRenderAreaGranularity"));
	table.vkInvalidateMappedMemoryRanges = reinterpret_cast<PFN_vkInvalidateMappedMemoryRanges>(load(device, "vkInvalidateMappedMemoryRanges"));
	table.vkMapMemory = reinterpret_cast<PFN_vkMapMemory>(load(device, "vkMapMemory"));
	table.vkMergePipelineCaches = reinterpret_cast<PFN_vkMergePipelineCaches>(load(device, "vkMergePipelineCaches"));
	table.vkQueueBindSparse = reinterpret_cast<PFN_vkQueueBindSparse>(load(device, "vkQueueBindSparse"));
	table.vkQueueSubmit = reinterpret_cast<PFN_vkQueueSubmit>(load(device, "vkQueueSubmit"));
	table.vkQueueWaitIdle = reinterpret_cast<PFN_vkQueueWaitIdle>(load(device, "vkQueueWaitIdle"));
	table.vkResetCommandBuffer = reinterpret_cast<PFN_vkResetCommandBuffer>(load(device, "vkResetCommandBuffer"));
	table.vkResetCommandPool = reinterpret_cast<PFN_vkResetCommandPool>(load(device, "vkResetCommandPool"));
	table.vkResetDescriptorPool = reinterpret_cast<PFN_vkResetDescriptorPool>(load(device, "vkResetDescriptorPool"));
	table.vkResetEvent = reinterpret_cast<PFN_vkResetEvent>(load(device, "vkResetEvent"));
	table.vkResetFences = reinterpret_cast<PFN_vkResetFences>(load(device, "vkResetFences"));
	table.vkSetEvent = reinterpret_cast<PFN_vkSetEvent>(load(device, "vkSetEvent"));
	table.vkUnmapMemory = reinterpret_cast<PFN_vkUnmapMemory>(load(device, "vkUnmapMemory"));
	table.vkUpdateDescriptorSets = reinterpret_cast<PFN_vkUpdateDescriptorSets>(load(device, "vkUpdateDescriptorSets"));
	table.vkWaitForFences = reinterpret_cast<PFN_vkWaitForFences>(load(device, "vkWaitForFences"));
#endif /* defined(VK_VERSION_1_0) */
#if defined(VK_VERSION_1_1)
	table.vkBindBufferMemory2 = reinterpret_cast<PFN_vkBindBufferMemory2>(load(device, "vkBindBufferMemory2"));
	table.vkBindImageMemory2 = reinterpret_cast<PFN_vkBindImageMemory2>(load(device, "vkBindImageMemory2"));
	table.vkCmdDispatchBase = reinterpret_cast<PFN_vkCmdDispatchBase>(load(device, "vkCmdDispatchBase"));
	table.vkCmdSetDeviceMask = reinterpret_cast<PFN_vkCmdSetDeviceMask>(load(device, "vkCmdSetDeviceMask"));
	table.vkCreateDescriptorUpdateTemplate = reinterpret_cast<PFN_vkCreateDescriptorUpdateTemplate>(load(device, "vkCreateDescriptorUpdateTemplate"));
	table.vkCreateSamplerYcbcrConversion = reinterpret_cast<PFN_vkCreateSamplerYcbcrConversion>(load(device, "vkCreateSamplerYcbcrConversion"));
	table.vkDestroyDescriptorUpdateTemplate = reinterpret_cast<PFN_vkDestroyDescriptorUpdateTemplate>(load(device, "vkDestroyDescriptorUpdateTemplate"));
	table.vkDestroySamplerYcbcrConversion = reinterpret_cast<PFN_vkDestroySamplerYcbcrConversion>(load(device, "vkDestroySamplerYcbcrConversion"));
	table.vkGetBufferMemoryRequirements2 = reinterpret_cast<PFN_vkGetBufferMemoryRequirements2>(load(device, "vkGetBufferMemoryRequirements2"));
	table.vkGetDescriptorSetLayoutSupport = reinterpret_cast<PFN_vkGetDescriptorSetLayoutSupport>(load(device, "vkGetDescriptorSetLayoutSupport"));
	table.vkGetDeviceGroupPeerMemoryFeatures = reinterpret_cast<PFN_vkGetDeviceGroupPeerMemoryFeatures>(load(device, "vkGetDeviceGroupPeerMemoryFeatures"));
	table.vkGetDeviceQueue2 = reinterpret_cast<PFN_vkGetDeviceQueue2>(load(device, "vkGetDeviceQueue2"));
	table.vkGetImageMemoryRequirements2 = reinterpret_cast<PFN_vkGetImageMemoryRequirements2>(load(device, "vkGetImageMemoryRequirements2"));
	table.vkGetImageSparseMemoryRequirements2 = reinterpret_cast<PFN_vkGetImageSparseMemoryRequirements2>(load(device, "vkGetImageSparseMemoryRequirements2"));
	table.vkTrimCommandPool = reinterpret_cast<PFN_vkTrimCommandPool>(load(device, "vkTrimCommandPool"));
	table.vkUpdateDescriptorSetWithTemplate = reinterpret_cast<PFN_vkUpdateDescriptorSetWithTemplate>(load(device, "vkUpdateDescriptorSetWithTemplate"));
#endif /* defined(VK_VERSION_1_1) */
#if defined(VK_VERSION_1_2)
	table.vkCmdBeginRenderPass2 = reinterpret_cast<PFN_vkCmdBeginRenderPass2>(load(device, "vkCmdBeginRenderPass2"));
	table.vkCmdDrawIndexedIndirectCount = reinterpret_cast<PFN_vkCmdDrawIndexedIndirectCount>(load(device, "vkCmdDrawIndexedIndirectCount"));
	table.vkCmdDrawIndirectCount = reinterpret_cast<PFN_vkCmdDrawIndirectCount>(load(device, "vkCmdDrawIndirectCount"));
	table.vkCmdEndRenderPass2 = reinterpret_cast<PFN_vkCmdEndRenderPass2>(load(device, "vkCmdEndRenderPass2"));
	table.vkCmdNextSubpass2 = reinterpret_cast<PFN_vkCmdNextSubpass2>(load(device, "vkCmdNextSubpass2"));
	table.vkCreateRenderPass2 = reinterpret_cast<PFN_vkCreateRenderPass2>(load(device, "vkCreateRenderPass2"));
	table.vkGetBufferDeviceAddress = reinterpret_cast<PFN_vkGetBufferDeviceAddress>(load(device, "vkGetBufferDeviceAddress"));
	table.vkGetBufferOpaqueCaptureAddress = reinterpret_cast<PFN_vkGetBufferOpaqueCaptureAddress>(load(device, "vkGetBufferOpaqueCaptureAddress"));
	table.vkGetDeviceMemoryOpaqueCaptureAddress = reinterpret_cast<PFN_vkGetDeviceMemoryOpaqueCaptureAddress>(load(device, "vkGetDeviceMemoryOpaqueCaptureAddress"));
	table.vkGetSemaphoreCounterValue = reinterpret_cast<PFN_vkGetSemaphoreCounterValue>(load(device, "vkGetSemaphoreCounterValue"));
	table.vkResetQueryPool = reinterpret_cast<PFN_vkResetQueryPool>(load(device, "vkResetQueryPool"));
	table.vkSignalSemaphore = reinterpret_cast<PFN_vkSignalSemaphore>(load(device, "vkSignalSemaphore"));
	table.vkWaitSemaphores = reinterpret_cast<PFN_vkWaitSemaphores>(load(device, "vkWaitSemaphores"));
#endif /* defined(VK_VERSION_1_2) */
#if defined(VK_VERSION_1_3)
	table.vkCmdBeginRendering = reinterpret_cast<PFN_vkCmdBeginRendering>(load(device, "vkCmdBeginRendering"));
	table.vkCmdBindVertexBuffers2 = reinterpret_cast<PFN_vkCmdBindVertexBuffers2>(load(device, "vkCmdBindVertexBuffers2"));
	table.vkCmdBlitImage2 = reinterpret_cast<PFN_vkCmdBlitImage2>(load(device, "vkCmdBlitImage2"));
	table.vkCmdCopyBuffer2 = reinterpret_cast<PFN_vkCmdCopyBuffer2>(load(device, "vkCmdCopyBuffer2"));
	table.vkCmdCopyBufferToImage2 = reinterpret_cast<PFN_vkCmdCopyBufferToImage2>(load(device, "vkCmdCopyBufferToImage2"));
	table.vkCmdCopyImage2 = reinterpret_cast<PFN_vkCmdCopyImage2>(load(device, "vkCmdCopyImage2"));
	table.vkCmdCopyImageToBuffer2 = reinterpret_cast<PFN_vkCmdCopyImageToBuffer2>(load(device, "vkCmdCopyImageToBuffer2"));
	table.vkCmdEndRendering = reinterpret_cast<PFN_vkCmdEndRendering>(load(device, "vkCmdEndRendering"));
	table.vkCmdPipelineBarrier2 = reinterpret_cast<PFN_vkCmdPipelineBarrier2>(load(device, "vkCmdPipelineBarrier2"));
	table.vkCmdResetEvent2 = reinterpret_cast<PFN_vkCmdResetEvent2>(load(device, "vkCmdResetEvent2"));
	table.vkCmdResolveImage2 = reinterpret_cast<PFN_vkCmdResolveImage2>(load(device, "vkCmdResolveImage2"));
	table.vkCmdSetCullMode = reinterpret_cast<PFN_vkCmdSetCullMode>(load(device, "vkCmdSetCullMode"));
	table.vkCmdSetDepthBiasEnable = reinterpret_cast<PFN_vkCmdSetDepthBiasEnable>(load(device, "vkCmdSetDepthBiasEnable"));
	table.vkCmdSetDepthBoundsTestEnable = reinterpret_cast<PFN_vkCmdSetDepthBoundsTestEnable>(load(device, "vkCmdSetDepthBoundsTestEnable"));
	table.vkCmdSetDepthCompareOp = reinterpret_cast<PFN_vkCmdSetDepthCompareOp>(load(device, "vkCmdSetDepthCompareOp"));
	table.vkCmdSetDepthTestEnable = reinterpret_cast<PFN_vkCmdSetDepthTestEnable>(load(device, "vkCmdSetDepthTestEnable"));
	table.vkCmdSetDepthWriteEnable = reinterpret_cast<PFN_vkCmdSetDepthWriteEnable>(load(device, "vkCmdSetDepthWriteEnable"));
	table.vkCmdSetEvent2 = reinterpret_cast<PFN_vkCmdSetEvent2>(load(device, "vkCmdSetEvent2"));
	table.vkCmdSetFrontFace = reinterpret_cast<PFN_vkCmdSetFrontFace>(load(device, "vkCmdSetFrontFace"));
	table.vkCmdSetPrimitiveRestartEnable = reinterpret_cast<PFN_vkCmdSetPrimitiveRestartEnable>(load(device, "vkCmdSetPrimitiveRestartEnable"));
	table.vkCmdSetPrimitiveTopology = reinterpret_cast<PFN_vkCmdSetPrimitiveTopology>(load(device, "vkCmdSetPrimitiveTopology"));
	table.vkCmdSetRasterizerDiscardEnable = reinterpret_cast<PFN_vkCmdSetRasterizerDiscardEnable>(load(device, "vkCmdSetRasterizerDiscardEnable"));
	table.vkCmdSetScissorWithCount = reinterpret_cast<PFN_vkCmdSetScissorWithCount>(load(device, "vkCmdSetScissorWithCount"));
	table.vkCmdSetStencilOp = reinterpret_cast<PFN_vkCmdSetStencilOp>(load(device, "vkCmdSetStencilOp"));
	table.vkCmdSetStencilTestEnable = reinterpret_cast<PFN_vkCmdSetStencilTestEnable>(load(device, "vkCmdSetStencilTestEnable"));
	table.vkCmdSetViewportWithCount = reinterpret_cast<PFN_vkCmdSetViewportWithCount>(load(device, "vkCmdSetViewportWithCount"));
	table.vkCmdWaitEvents2 = reinterpret_cast<PFN_vkCmdWaitEvents2>(load(device, "vkCmdWaitEvents2"));
	table.vkCmdWriteTimestamp2 = reinterpret_cast<PFN_vkCmdWriteTimestamp2>(load(device, "vkCmdWriteTimestamp2"));
	table.vkCreatePrivateDataSlot = reinterpret_cast<PFN_vkCreatePrivateDataSlot>(load(device, "vkCreatePrivateDataSlot"));
	table.vkDestroyPrivateDataSlot = reinterpret_cast<PFN_vkDestroyPrivateDataSlot>(load(device, "vkDestroyPrivateDataSlot"));
	table.vkGetDeviceBufferMemoryRequirements = reinterpret_cast<PFN_vkGetDeviceBufferMemoryRequirements>(load(device, "vkGetDeviceBufferMemoryRequirements"));
	table.vkGetDeviceImageMemoryRequirements = reinterpret_cast<PFN_vkGetDeviceImageMemoryRequirements>(load(device, "vkGetDeviceImageMemoryRequirements"));
	table.vkGetDeviceImageSparseMemoryRequirements = reinterpret_cast<PFN_vkGetDeviceImageSparseMemoryRequirements>(load(device, "vkGetDeviceImageSparseMemoryRequirements"));
	table.vkGetPrivateData = reinterpret_cast<PFN_vkGetPrivateData>(load(device, "vkGetPrivateData"));
	table.vkQueueSubmit2 = reinterpret_cast<PFN_vkQueueSubmit2>(load(device, "vkQueueSubmit2"));
	table.vkSetPrivateData = reinterpret_cast<PFN_vkSetPrivateData>(load(device, "vkSetPrivateData"));
#endif /* defined(VK_VERSION_1_3) */
#if defined(VK_AMDX_shader_enqueue)
	table.vkCmdDispatchGraphAMDX = reinterpret_cast<PFN_vkCmdDispatchGraphAMDX>(load(device, "vkCmdDispatchGraphAMDX"));
	table.vkCmdDispatchGraphIndirectAMDX = reinterpret_cast<PFN_vkCmdDispatchGraphIndirectAMDX>(load(device, "vkCmdDispatchGraphIndirectAMDX"));
	table.vkCmdDispatchGraphIndirectCountAMDX = reinterpret_cast<PFN_vkCmdDispatchGraphIndirectCountAMDX>(load(device, "vkCmdDispatchGraphIndirectCountAMDX"));
	table.vkCmdInitializeGraphScratchMemoryAMDX = reinterpret_cast<PFN_vkCmdInitializeGraphScratchMemoryAMDX>(load(device, "vkCmdInitializeGraphScratchMemoryAMDX"));
	table.vkCreateExecutionGraphPipelinesAMDX = reinterpret_cast<PFN_vkCreateExecutionGraphPipelinesAMDX>(load(device, "vkCreateExecutionGraphPipelinesAMDX"));
	table.vkGetExecutionGraphPipelineNodeIndexAMDX = reinterpret_cast<PFN_vkGetExecutionGraphPipelineNodeIndexAMDX>(load(device, "vkGetExecutionGraphPipelineNodeIndexAMDX"));
	table.vkGetExecutionGraphPipelineScratchSizeAMDX = reinterpret_cast<PFN_vkGetExecutionGraphPipelineScratchSizeAMDX>(load(device, "vkGetExecutionGraphPipelineScratchSizeAMDX"));
#endif /* defined(VK_AMDX_shader_enqueue) */
#if defined(VK_AMD_buffer_marker)
	table.vkCmdWriteBufferMarkerAMD = reinterpret_cast<PFN_vkCmdWriteBufferMarkerAMD>(load(device, "vkCmdWriteBufferMarkerAMD"));
#endif /* defined(VK_AMD_buffer_marker) */
#if defined(VK_AMD_display_native_hdr)
	table.vkSetLocalDimmingAMD = reinterpret_cast<PFN_vkSetLocalDimmingAMD>(load(device, "vkSetLocalDimmingAMD"));
#endif /* defined(VK_AMD_display_native_hdr) */
#if defined(VK_AMD_draw_indirect_count)
	table.vkCmdDrawIndexedIndirectCountAMD = reinterpret_cast<PFN_vkCmdDrawIndexedIndirectCountAMD>(load(device, "vkCmdDrawIndexedIndirectCountAMD"));
	table.vkCmdDrawIndirectCountAMD = reinterpret_cast<PFN_vkCmdDrawIndirectCountAMD>(load(device, "vkCmdDrawIndirectCountAMD"));
#endif /* defined(VK_AMD_draw_indirect_count) */
#if defined(VK_AMD_shader_info)
	table.vkGetShaderInfoAMD = reinterpret_cast<PFN_vkGetShaderInfoAMD>(load(device, "vkGetShaderInfoAMD"));
#endif /* defined(VK_AMD_shader_info) */
#if defined(VK_ANDROID_external_memory_android_hardware_buffer)
	table.vkGetAndroidHardwareBufferPropertiesANDROID = reinterpret_cast<PFN_vkGetAndroidHardwareBufferPropertiesANDROID>(load(device, "vkGetAndroidHardwareBufferPropertiesANDROID"));
	table.vkGetMemoryAndroidHardwareBufferANDROID = reinterpret_cast<PFN_vkGetMemoryAndroidHardwareBufferANDROID>(load(device, "vkGetMemoryAndroidHardwareBufferANDROID"));
#endif /* defined(VK_ANDROID_external_memory_android_hardware_buffer) */
#if defined(VK_EXT_attachment_feedback_loop_dynamic_state)
	table.vkCmdSetAttachmentFeedbackLoopEnableEXT = reinterpret_cast<PFN_vkCmdSetAttachmentFeedbackLoopEnableEXT>(load(device, "vkCmdSetAttachmentFeedbackLoopEnableEXT"));
#endif /* defined(VK_EXT_attachment_feedback_loop_dynamic_state) */
#if defined(VK_EXT_buffer_device_address)
	table.vkGetBufferDeviceAddressEXT = reinterpret_cast<PFN_vkGetBufferDeviceAddressEXT>(load(device, "vkGetBufferDeviceAddressEXT"));
#endif /* defined(VK_EXT_buffer_device_address) */
#if defined(VK_EXT_calibrated_timestamps)
	table.vkGetCalibratedTimestampsEXT = reinterpret_cast<PFN_vkGetCalibratedTimestampsEXT>(load(device, "vkGetCalibratedTimestampsEXT"));
#endif /* defined(VK_EXT_calibrated_timestamps) */
#if defined(VK_EXT_color_write_enable)
	table.vkCmdSetColorWriteEnableEXT = reinterpret_cast<PFN_vkCmdSetColorWriteEnableEXT>(load(device, "vkCmdSetColorWriteEnableEXT"));
#endif /* defined(VK_EXT_color_write_enable) */
#if defined(VK_EXT_conditional_rendering)
	table.vkCmdBeginConditionalRenderingEXT = reinterpret_cast<PFN_vkCmdBeginConditionalRenderingEXT>(load(device, "vkCmdBeginConditionalRenderingEXT"));
	table.vkCmdEndConditionalRenderingEXT = reinterpret_cast<PFN_vkCmdEndConditionalRenderingEXT>(load(device, "vkCmdEndConditionalRenderingEXT"));
#endif /* defined(VK_EXT_conditional_rendering) */
#if defined(VK_EXT_debug_marker)
	table.vkCmdDebugMarkerBeginEXT = reinterpret_cast<PFN_vkCmdDebugMarkerBeginEXT>(load(device, "vkCmdDebugMarkerBeginEXT"));
	table.vkCmdDebugMarkerEndEXT = reinterpret_cast<PFN_vkCmdDebugMarkerEndEXT>(load(device, "vkCmdDebugMarkerEndEXT"));
	table.vkCmdDebugMarkerInsertEXT = reinterpret_cast<PFN_vkCmdDebugMarkerInsertEXT>(load(device, "vkCmdDebugMarkerInsertEXT"));
	table.vkDebugMarkerSetObjectNameEXT = reinterpret_cast<PFN_vkDebugMarkerSetObjectNameEXT>(load(device, "vkDebugMarkerSetObjectNameEXT"));
	table.vkDebugMarkerSetObjectTagEXT = reinterpret_cast<PFN_vkDebugMarkerSetObjectTagEXT>(load(device, "vkDebugMarkerSetObjectTagEXT"));
#endif /* defined(VK_EXT_debug_marker) */
#if defined(VK_EXT_depth_bias_control)
	table.vkCmdSetDepthBias2EXT = reinterpret_cast<PFN_vkCmdSetDepthBias2EXT>(load(device, "vkCmdSetDepthBias2EXT"));
#endif /* defined(VK_EXT_depth_bias_control) */
#if defined(VK_EXT_descriptor_buffer)
	table.vkCmdBindDescriptorBufferEmbeddedSamplersEXT = reinterpret_cast<PFN_vkCmdBindDescriptorBufferEmbeddedSamplersEXT>(load(device, "vkCmdBindDescriptorBufferEmbeddedSamplersEXT"));
	table.vkCmdBindDescriptorBuffersEXT = reinterpret_cast<PFN_vkCmdBindDescriptorBuffersEXT>(load(device, "vkCmdBindDescriptorBuffersEXT"));
	table.vkCmdSetDescriptorBufferOffsetsEXT = reinterpret_cast<PFN_vkCmdSetDescriptorBufferOffsetsEXT>(load(device, "vkCmdSetDescriptorBufferOffsetsEXT"));
	table.vkGetBufferOpaqueCaptureDescriptorDataEXT = reinterpret_cast<PFN_vkGetBufferOpaqueCaptureDescriptorDataEXT>(load(device, "vkGetBufferOpaqueCaptureDescriptorDataEXT"));
	table.vkGetDescriptorEXT = reinterpret_cast<PFN_vkGetDescriptorEXT>(load(device, "vkGetDescriptorEXT"));
	table.vkGetDescriptorSetLayoutBindingOffsetEXT = reinterpret_cast<PFN_vkGetDescriptorSetLayoutBindingOffsetEXT>(load(device, "vkGetDescriptorSetLayoutBindingOffsetEXT"));
	table.vkGetDescriptorSetLayoutSizeEXT = reinterpret_cast<PFN_vkGetDescriptorSetLayoutSizeEXT>(load(device, "vkGetDescriptorSetLayoutSizeEXT"));
	table.vkGetImageOpaqueCaptureDescriptorDataEXT = reinterpret_cast<PFN_vkGetImageOpaqueCaptureDescriptorDataEXT>(load(device, "vkGetImageOpaqueCaptureDescriptorDataEXT"));
	table.vkGetImageViewOpaqueCaptureDescriptorDataEXT = reinterpret_cast<PFN_vkGetImageViewOpaqueCaptureDescriptorDataEXT>(load(device, "vkGetImageViewOpaqueCaptureDescriptorDataEXT"));
	table.vkGetSamplerOpaqueCaptureDescriptorDataEXT = reinterpret_cast<PFN_vkGetSamplerOpaqueCaptureDescriptorDataEXT>(load(device, "vkGetSamplerOpaqueCaptureDescriptorDataEXT"));
#endif /* defined(VK_EXT_descriptor_buffer) */
#if defined(VK_EXT_descriptor_buffer) && (defined(VK_KHR_acceleration_structure) || defined(VK_NV_ray_tracing))
	table.vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT = reinterpret_cast<PFN_vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT>(load(device, "vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT"));
#endif /* defined(VK_EXT_descriptor_buffer) && (defined(VK_KHR_acceleration_structure) || defined(VK_NV_ray_tracing)) */
#if defined(VK_EXT_device_fault)
	table.vkGetDeviceFaultInfoEXT = reinterpret_cast<PFN_vkGetDeviceFaultInfoEXT>(load(device, "vkGetDeviceFaultInfoEXT"));
#endif /* defined(VK_EXT_device_fault) */
#if defined(VK_EXT_discard_rectangles)
	table.vkCmdSetDiscardRectangleEXT = reinterpret_cast<PFN_vkCmdSetDiscardRectangleEXT>(load(device, "vkCmdSetDiscardRectangleEXT"));
#endif /* defined(VK_EXT_discard_rectangles) */
#if defined(VK_EXT_discard_rectangles) && VK_EXT_DISCARD_RECTANGLES_SPEC_VERSION >= 2
	table.vkCmdSetDiscardRectangleEnableEXT = reinterpret_cast<PFN_vkCmdSetDiscardRectangleEnableEXT>(load(device, "vkCmdSetDiscardRectangleEnableEXT"));
	table.vkCmdSetDiscardRectangleModeEXT = reinterpret_cast<PFN_vkCmdSetDiscardRectangleModeEXT>(load(device, "vkCmdSetDiscardRectangleModeEXT"));
#endif /* defined(VK_EXT_discard_rectangles) && VK_EXT_DISCARD_RECTANGLES_SPEC_VERSION >= 2 */
#if defined(VK_EXT_display_control)
	table.vkDisplayPowerControlEXT = reinterpret_cast<PFN_vkDisplayPowerControlEXT>(load(device, "vkDisplayPowerControlEXT"));
	table.vkGetSwapchainCounterEXT = reinterpret_cast<PFN_vkGetSwapchainCounterEXT>(load(device, "vkGetSwapchainCounterEXT"));
	table.vkRegisterDeviceEventEXT = reinterpret_cast<PFN_vkRegisterDeviceEventEXT>(load(device, "vkRegisterDeviceEventEXT"));
	table.vkRegisterDisplayEventEXT = reinterpret_cast<PFN_vkRegisterDisplayEventEXT>(load(device, "vkRegisterDisplayEventEXT"));
#endif /* defined(VK_EXT_display_control) */
#if defined(VK_EXT_external_memory_host)
	table.vkGetMemoryHostPointerPropertiesEXT = reinterpret_cast<PFN_vkGetMemoryHostPointerPropertiesEXT>(load(device, "vkGetMemoryHostPointerPropertiesEXT"));
#endif /* defined(VK_EXT_external_memory_host) */
#if defined(VK_EXT_full_screen_exclusive)
	table.vkAcquireFullScreenExclusiveModeEXT = reinterpret_cast<PFN_vkAcquireFullScreenExclusiveModeEXT>(load(device, "vkAcquireFullScreenExclusiveModeEXT"));
	table.vkReleaseFullScreenExclusiveModeEXT = reinterpret_cast<PFN_vkReleaseFullScreenExclusiveModeEXT>(load(device, "vkReleaseFullScreenExclusiveModeEXT"));
#endif /* defined(VK_EXT_full_screen_exclusive) */
#if defined(VK_EXT_hdr_metadata)
	table.vkSetHdrMetadataEXT = reinterpret_cast<PFN_vkSetHdrMetadataEXT>(load(device, "vkSetHdrMetadataEXT"));
#endif /* defined(VK_EXT_hdr_metadata) */
#if defined(VK_EXT_host_image_copy)
	table.vkCopyImageToImageEXT = reinterpret_cast<PFN_vkCopyImageToImageEXT>(load(device, "vkCopyImageToImageEXT"));
	table.vkCopyImageToMemoryEXT = reinterpret_cast<PFN_vkCopyImageToMemoryEXT>(load(device, "vkCopyImageToMemoryEXT"));
	table.vkCopyMemoryToImageEXT = reinterpret_cast<PFN_vkCopyMemoryToImageEXT>(load(device, "vkCopyMemoryToImageEXT"));
	table.vkTransitionImageLayoutEXT = reinterpret_cast<PFN_vkTransitionImageLayoutEXT>(load(device, "vkTransitionImageLayoutEXT"));
#endif /* defined(VK_EXT_host_image_copy) */
#if defined(VK_EXT_host_query_reset)
	table.vkResetQueryPoolEXT = reinterpret_cast<PFN_vkResetQueryPoolEXT>(load(device, "vkResetQueryPoolEXT"));
#endif /* defined(VK_EXT_host_query_reset) */
#if defined(VK_EXT_image_drm_format_modifier)
	table.vkGetImageDrmFormatModifierPropertiesEXT = reinterpret_cast<PFN_vkGetImageDrmFormatModifierPropertiesEXT>(load(device, "vkGetImageDrmFormatModifierPropertiesEXT"));
#endif /* defined(VK_EXT_image_drm_format_modifier) */
#if defined(VK_EXT_line_rasterization)
	table.vkCmdSetLineStippleEXT = reinterpret_cast<PFN_vkCmdSetLineStippleEXT>(load(device, "vkCmdSetLineStippleEXT"));
#endif /* defined(VK_EXT_line_rasterization) */
#if defined(VK_EXT_mesh_shader)
	table.vkCmdDrawMeshTasksEXT = reinterpret_cast<PFN_vkCmdDrawMeshTasksEXT>(load(device, "vkCmdDrawMeshTasksEXT"));
	table.vkCmdDrawMeshTasksIndirectCountEXT = reinterpret_cast<PFN_vkCmdDrawMeshTasksIndirectCountEXT>(load(device, "vkCmdDrawMeshTasksIndirectCountEXT"));
	table.vkCmdDrawMeshTasksIndirectEXT = reinterpret_cast<PFN_vkCmdDrawMeshTasksIndirectEXT>(load(device, "vkCmdDrawMeshTasksIndirectEXT"));
#endif /* defined(VK_EXT_mesh_shader) */
#if defined(VK_EXT_metal_objects)
	table.vkExportMetalObjectsEXT = reinterpret_cast<PFN_vkExportMetalObjectsEXT>(load(device, "vkExportMetalObjectsEXT"));
#endif /* defined(VK_EXT_metal_objects) */
#if defined(VK_EXT_multi_draw)
	table.vkCmdDrawMultiEXT = reinterpret_cast<PFN_vkCmdDrawMultiEXT>(load(device, "vkCmdDrawMultiEXT"));
	table.vkCmdDrawMultiIndexedEXT = reinterpret_cast<PFN_vkCmdDrawMultiIndexedEXT>(load(device, "vkCmdDrawMultiIndexedEXT"));
#endif /* defined(VK_EXT_multi_draw) */
#if defined(VK_EXT_opacity_micromap)
	table.vkBuildMicromapsEXT = reinterpret_cast<PFN_vkBuildMicromapsEXT>(load(device, "vkBuildMicromapsEXT"));
	table.vkCmdBuildMicromapsEXT = reinterpret_cast<PFN_vkCmdBuildMicromapsEXT>(load(device, "vkCmdBuildMicromapsEXT"));
	table.vkCmdCopyMemoryToMicromapEXT = reinterpret_cast<PFN_vkCmdCopyMemoryToMicromapEXT>(load(device, "vkCmdCopyMemoryToMicromapEXT"));
	table.vkCmdCopyMicromapEXT = reinterpret_cast<PFN_vkCmdCopyMicromapEXT>(load(device, "vkCmdCopyMicromapEXT"));
	table.vkCmdCopyMicromapToMemoryEXT = reinterpret_cast<PFN_vkCmdCopyMicromapToMemoryEXT>(load(device, "vkCmdCopyMicromapToMemoryEXT"));
	table.vkCmdWriteMicromapsPropertiesEXT = reinterpret_cast<PFN_vkCmdWriteMicromapsPropertiesEXT>(load(device, "vkCmdWriteMicromapsPropertiesEXT"));
	table.vkCopyMemoryToMicromapEXT = reinterpret_cast<PFN_vkCopyMemoryToMicromapEXT>(load(device, "vkCopyMemoryToMicromapEXT"));
	table.vkCopyMicromapEXT = reinterpret_cast<PFN_vkCopyMicromapEXT>(load(device, "vkCopyMicromapEXT"));
	table.vkCopyMicromapToMemoryEXT = reinterpret_cast<PFN_vkCopyMicromapToMemoryEXT>(load(device, "vkCopyMicromapToMemoryEXT"));
	table.vkCreateMicromapEXT = reinterpret_cast<PFN_vkCreateMicromapEXT>(load(device, "vkCreateMicromapEXT"));
	table.vkDestroyMicromapEXT = reinterpret_cast<PFN_vkDestroyMicromapEXT>(load(device, "vkDestroyMicromapEXT"));
	table.vkGetDeviceMicromapCompatibilityEXT = reinterpret_cast<PFN_vkGetDeviceMicromapCompatibilityEXT>(load(device, "vkGetDeviceMicromapCompatibilityEXT"));
	table.vkGetMicromapBuildSizesEXT = reinterpret_cast<PFN_vkGetMicromapBuildSizesEXT>(load(device, "vkGetMicromapBuildSizesEXT"));
	table.vkWriteMicromapsPropertiesEXT = reinterpret_cast<PFN_vkWriteMicromapsPropertiesEXT>(load(device, "vkWriteMicromapsPropertiesEXT"));
#endif /* defined(VK_EXT_opacity_micromap) */
#if defined(VK_EXT_pageable_device_local_memory)
	table.vkSetDeviceMemoryPriorityEXT = reinterpret_cast<PFN_vkSetDeviceMemoryPriorityEXT>(load(device, "vkSetDeviceMemoryPriorityEXT"));
#endif /* defined(VK_EXT_pageable_device_local_memory) */
#if defined(VK_EXT_pipeline_properties)
	table.vkGetPipelinePropertiesEXT = reinterpret_cast<PFN_vkGetPipelinePropertiesEXT>(load(device, "vkGetPipelinePropertiesEXT"));
#endif /* defined(VK_EXT_pipeline_properties) */
#if defined(VK_EXT_private_data)
	table.vkCreatePrivateDataSlotEXT = reinterpret_cast<PFN_vkCreatePrivateDataSlotEXT>(load(device, "vkCreatePrivateDataSlotEXT"));
	table.vkDestroyPrivateDataSlotEXT = reinterpret_cast<PFN_vkDestroyPrivateDataSlotEXT>(load(device, "vkDestroyPrivateDataSlotEXT"));
	table.vkGetPrivateDataEXT = reinterpret_cast<PFN_vkGetPrivateDataEXT>(load(device, "vkGetPrivateDataEXT"));
	table.vkSetPrivateDataEXT = reinterpret_cast<PFN_vkSetPrivateDataEXT>(load(device, "vkSetPrivateDataEXT"));
#endif /* defined(VK_EXT_private_data) */
#if defined(VK_EXT_sample_locations)
	table.vkCmdSetSampleLocationsEXT = reinterpret_cast<PFN_vkCmdSetSampleLocationsEXT>(load(device, "vkCmdSetSampleLocationsEXT"));
#endif /* defined(VK_EXT_sample_locations) */
#if defined(VK_EXT_shader_module_identifier)
	table.vkGetShaderModuleCreateInfoIdentifierEXT = reinterpret_cast<PFN_vkGetShaderModuleCreateInfoIdentifierEXT>(load(device, "vkGetShaderModuleCreateInfoIdentifierEXT"));
	table.vkGetShaderModuleIdentifierEXT = reinterpret_cast<PFN_vkGetShaderModuleIdentifierEXT>(load(device, "vkGetShaderModuleIdentifierEXT"));
#endif /* defined(VK_EXT_shader_module_identifier) */
#if defined(VK_EXT_shader_object)
	table.vkCmdBindShadersEXT = reinterpret_cast<PFN_vkCmdBindShadersEXT>(load(device, "vkCmdBindShadersEXT"));
	table.vkCreateShadersEXT = reinterpret_cast<PFN_vkCreateShadersEXT>(load(device, "vkCreateShadersEXT"));
	table.vkDestroyShaderEXT = reinterpret_cast<PFN_vkDestroyShaderEXT>(load(device, "vkDestroyShaderEXT"));
	table.vkGetShaderBinaryDataEXT = reinterpret_cast<PFN_vkGetShaderBinaryDataEXT>(load(device, "vkGetShaderBinaryDataEXT"));
#endif /* defined(VK_EXT_shader_object) */
#if defined(VK_EXT_swapchain_maintenance1)
	table.vkReleaseSwapchainImagesEXT = reinterpret_cast<PFN_vkReleaseSwapchainImagesEXT>(load(device, "vkReleaseSwapchainImagesEXT"));
#endif /* defined(VK_EXT_swapchain_maintenance1) */
#if defined(VK_EXT_transform_feedback)
	table.vkCmdBeginQueryIndexedEXT = reinterpret_cast<PFN_vkCmdBeginQueryIndexedEXT>(load(device, "vkCmdBeginQueryIndexedEXT"));
	table.vkCmdBeginTransformFeedbackEXT = reinterpret_cast<PFN_vkCmdBeginTransformFeedbackEXT>(load(device, "vkCmdBeginTransformFeedbackEXT"));
	table.vkCmdBindTransformFeedbackBuffersEXT = reinterpret_cast<PFN_vkCmdBindTransformFeedbackBuffersEXT>(load(device, "vkCmdBindTransformFeedbackBuffersEXT"));
	table.vkCmdDrawIndirectByteCountEXT = reinterpret_cast<PFN_vkCmdDrawIndirectByteCountEXT>(load(device, "vkCmdDrawIndirectByteCountEXT"));
	table.vkCmdEndQueryIndexedEXT = reinterpret_cast<PFN_vkCmdEndQueryIndexedEXT>(load(device, "vkCmdEndQueryIndexedEXT"));
	table.vkCmdEndTransformFeedbackEXT = reinterpret_cast<PFN_vkCmdEndTransformFeedbackEXT>(load(device, "vkCmdEndTransformFeedbackEXT"));
#endif /* defined(VK_EXT_transform_feedback) */
#if defined(VK_EXT_validation_cache)
	table.vkCreateValidationCacheEXT = reinterpret_cast<PFN_vkCreateValidationCacheEXT>(load(device, "vkCreateValidationCacheEXT"));
	table.vkDestroyValidationCacheEXT = reinterpret_cast<PFN_vkDestroyValidationCacheEXT>(load(device, "vkDestroyValidationCacheEXT"));
	table.vkGetValidationCacheDataEXT = reinterpret_cast<PFN_vkGetValidationCacheDataEXT>(load(device, "vkGetValidationCacheDataEXT"));
	table.vkMergeValidationCachesEXT = reinterpret_cast<PFN_vkMergeValidationCachesEXT>(load(device, "vkMergeValidationCachesEXT"));
#endif /* defined(VK_EXT_validation_cache) */
#if defined(VK_FUCHSIA_buffer_collection)
	table.vkCreateBufferCollectionFUCHSIA = reinterpret_cast<PFN_vkCreateBufferCollectionFUCHSIA>(load(device, "vkCreateBufferCollectionFUCHSIA"));
	table.vkDestroyBufferCollectionFUCHSIA = reinterpret_cast<PFN_vkDestroyBufferCollectionFUCHSIA>(load(device, "vkDestroyBufferCollectionFUCHSIA"));
	table.vkGetBufferCollectionPropertiesFUCHSIA = reinterpret_cast<PFN_vkGetBufferCollectionPropertiesFUCHSIA>(load(device, "vkGetBufferCollectionPropertiesFUCHSIA"));
	table.vkSetBufferCollectionBufferConstraintsFUCHSIA = reinterpret_cast<PFN_vkSetBufferCollectionBufferConstraintsFUCHSIA>(load(device, "vkSetBufferCollectionBufferConstraintsFUCHSIA"));
	table.vkSetBufferCollectionImageConstraintsFUCHSIA = reinterpret_cast<PFN_vkSetBufferCollectionImageConstraintsFUCHSIA>(load(device, "vkSetBufferCollectionImageConstraintsFUCHSIA"));
#endif /* defined(VK_FUCHSIA_buffer_collection) */
#if defined(VK_FUCHSIA_external_memory)
	table.vkGetMemoryZirconHandleFUCHSIA = reinterpret_cast<PFN_vkGetMemoryZirconHandleFUCHSIA>(load(device, "vkGetMemoryZirconHandleFUCHSIA"));
	table.vkGetMemoryZirconHandlePropertiesFUCHSIA = reinterpret_cast<PFN_vkGetMemoryZirconHandlePropertiesFUCHSIA>(load(device, "vkGetMemoryZirconHandlePropertiesFUCHSIA"));
#endif /* defined(VK_FUCHSIA_external_memory) */
#if defined(VK_FUCHSIA_external_semaphore)
	table.vkGetSemaphoreZirconHandleFUCHSIA = reinterpret_cast<PFN_vkGetSemaphoreZirconHandleFUCHSIA>(load(device, "vkGetSemaphoreZirconHandleFUCHSIA"));
	table.vkImportSemaphoreZirconHandleFUCHSIA = reinterpret_cast<PFN_vkImportSemaphoreZirconHandleFUCHSIA>(load(device, "vkImportSemaphoreZirconHandleFUCHSIA"));
#endif /* defined(VK_FUCHSIA_external_semaphore) */
#if defined(VK_GOOGLE_display_timing)
	table.vkGetPastPresentationTimingGOOGLE = reinterpret_cast<PFN_vkGetPastPresentationTimingGOOGLE>(load(device, "vkGetPastPresentationTimingGOOGLE"));
	table.vkGetRefreshCycleDurationGOOGLE = reinterpret_cast<PFN_vkGetRefreshCycleDurationGOOGLE>(load(device, "vkGetRefreshCycleDurationGOOGLE"));
#endif /* defined(VK_GOOGLE_display_timing) */
#if defined(VK_HUAWEI_cluster_culling_shader)
	table.vkCmdDrawClusterHUAWEI = reinterpret_cast<PFN_vkCmdDrawClusterHUAWEI>(load(device, "vkCmdDrawClusterHUAWEI"));
	table.vkCmdDrawClusterIndirectHUAWEI = reinterpret_cast<PFN_vkCmdDrawClusterIndirectHUAWEI>(load(device, "vkCmdDrawClusterIndirectHUAWEI"));
#endif /* defined(VK_HUAWEI_cluster_culling_shader) */
#if defined(VK_HUAWEI_invocation_mask)
	table.vkCmdBindInvocationMaskHUAWEI = reinterpret_cast<PFN_vkCmdBindInvocationMaskHUAWEI>(load(device, "vkCmdBindInvocationMaskHUAWEI"));
#endif /* defined(VK_HUAWEI_invocation_mask) */
#if defined(VK_HUAWEI_subpass_shading)
	table.vkCmdSubpassShadingHUAWEI = reinterpret_cast<PFN_vkCmdSubpassShadingHUAWEI>(load(device, "vkCmdSubpassShadingHUAWEI"));
	table.vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI = reinterpret_cast<PFN_vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI>(load(device, "vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI"));
#endif /* defined(VK_HUAWEI_subpass_shading) */
#if defined(VK_INTEL_performance_query)
	table.vkAcquirePerformanceConfigurationINTEL = reinterpret_cast<PFN_vkAcquirePerformanceConfigurationINTEL>(load(device, "vkAcquirePerformanceConfigurationINTEL"));
	table.vkCmdSetPerformanceMarkerINTEL = reinterpret_cast<PFN_vkCmdSetPerformanceMarkerINTEL>(load(device, "vkCmdSetPerformanceMarkerINTEL"));
	table.vkCmdSetPerformanceOverrideINTEL = reinterpret_cast<PFN_vkCmdSetPerformanceOverrideINTEL>(load(device, "vkCmdSetPerformanceOverrideINTEL"));
	table.vkCmdSetPerformanceStreamMarkerINTEL = reinterpret_cast<PFN_vkCmdSetPerformanceStreamMarkerINTEL>(load(device, "vkCmdSetPerformanceStreamMarkerINTEL"));
	table.vkGetPerformanceParameterINTEL = reinterpret_cast<PFN_vkGetPerformanceParameterINTEL>(load(device, "vkGetPerformanceParameterINTEL"));
	table.vkInitializePerformanceApiINTEL = reinterpret_cast<PFN_vkInitializePerformanceApiINTEL>(load(device, "vkInitializePerformanceApiINTEL"));
	table.vkQueueSetPerformanceConfigurationINTEL = reinterpret_cast<PFN_vkQueueSetPerformanceConfigurationINTEL>(load(device, "vkQueueSetPerformanceConfigurationINTEL"));
	table.vkReleasePerformanceConfigurationINTEL = reinterpret_cast<PFN_vkReleasePerformanceConfigurationINTEL>(load(device, "vkReleasePerformanceConfigurationINTEL"));
	table.vkUninitializePerformanceApiINTEL = reinterpret_cast<PFN_vkUninitializePerformanceApiINTEL>(load(device, "vkUninitializePerformanceApiINTEL"));
#endif /* defined(VK_INTEL_performance_query) */
#if defined(VK_KHR_acceleration_structure)
	table.vkBuildAccelerationStructuresKHR = reinterpret_cast<PFN_vkBuildAccelerationStructuresKHR>(load(device, "vkBuildAccelerationStructuresKHR"));
	table.vkCmdBuildAccelerationStructuresIndirectKHR = reinterpret_cast<PFN_vkCmdBuildAccelerationStructuresIndirectKHR>(load(device, "vkCmdBuildAccelerationStructuresIndirectKHR"));
	table.vkCmdBuildAccelerationStructuresKHR = reinterpret_cast<PFN_vkCmdBuildAccelerationStructuresKHR>(load(device, "vkCmdBuildAccelerationStructuresKHR"));
	table.vkCmdCopyAccelerationStructureKHR = reinterpret_cast<PFN_vkCmdCopyAccelerationStructureKHR>(load(device, "vkCmdCopyAccelerationStructureKHR"));
	table.vkCmdCopyAccelerationStructureToMemoryKHR = reinterpret_cast<PFN_vkCmdCopyAccelerationStructureToMemoryKHR>(load(device, "vkCmdCopyAccelerationStructureToMemoryKHR"));
	table.vkCmdCopyMemoryToAccelerationStructureKHR = reinterpret_cast<PFN_vkCmdCopyMemoryToAccelerationStructureKHR>(load(device, "vkCmdCopyMemoryToAccelerationStructureKHR"));
	table.vkCmdWriteAccelerationStructuresPropertiesKHR = reinterpret_cast<PFN_vkCmdWriteAccelerationStructuresPropertiesKHR>(load(device, "vkCmdWriteAccelerationStructuresPropertiesKHR"));
	table.vkCopyAccelerationStructureKHR = reinterpret_cast<PFN_vkCopyAccelerationStructureKHR>(load(device, "vkCopyAccelerationStructureKHR"));
	table.vkCopyAccelerationStructureToMemoryKHR = reinterpret_cast<PFN_vkCopyAccelerationStructureToMemoryKHR>(load(device, "vkCopyAccelerationStructureToMemoryKHR"));
	table.vkCopyMemoryToAccelerationStructureKHR = reinterpret_cast<PFN_vkCopyMemoryToAccelerationStructureKHR>(load(device, "vkCopyMemoryToAccelerationStructureKHR"));
	table.vkCreateAccelerationStructureKHR = reinterpret_cast<PFN_vkCreateAccelerationStructureKHR>(load(device, "vkCreateAccelerationStructureKHR"));
	table.vkDestroyAccelerationStructureKHR = reinterpret_cast<PFN_vkDestroyAccelerationStructureKHR>(load(device, "vkDestroyAccelerationStructureKHR"));
	table.vkGetAccelerationStructureBuildSizesKHR = reinterpret_cast<PFN_vkGetAccelerationStructureBuildSizesKHR>(load(device, "vkGetAccelerationStructureBuildSizesKHR"));
	table.vkGetAccelerationStructureDeviceAddressKHR = reinterpret_cast<PFN_vkGetAccelerationStructureDeviceAddressKHR>(load(device, "vkGetAccelerationStructureDeviceAddressKHR"));
	table.vkGetDeviceAccelerationStructureCompatibilityKHR = reinterpret_cast<PFN_vkGetDeviceAccelerationStructureCompatibilityKHR>(load(device, "vkGetDeviceAccelerationStructureCompatibilityKHR"));
	table.vkWriteAccelerationStructuresPropertiesKHR = reinterpret_cast<PFN_vkWriteAccelerationStructuresPropertiesKHR>(load(device, "vkWriteAccelerationStructuresPropertiesKHR"));
#endif /* defined(VK_KHR_acceleration_structure) */
#if defined(VK_KHR_bind_memory2)
	table.vkBindBufferMemory2KHR = reinterpret_cast<PFN_vkBindBufferMemory2KHR>(load(device, "vkBindBufferMemory2KHR"));
	table.vkBindImageMemory2KHR = reinterpret_cast<PFN_vkBindImageMemory2KHR>(load(device, "vkBindImageMemory2KHR"));
#endif /* defined(VK_KHR_bind_memory2) */
#if defined(VK_KHR_buffer_device_address)
	table.vkGetBufferDeviceAddressKHR = reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(load(device, "vkGetBufferDeviceAddressKHR"));
	table.vkGetBufferOpaqueCaptureAddressKHR = reinterpret_cast<PFN_vkGetBufferOpaqueCaptureAddressKHR>(load(device, "vkGetBufferOpaqueCaptureAddressKHR"));
	table.vkGetDeviceMemoryOpaqueCaptureAddressKHR = reinterpret_cast<PFN_vkGetDeviceMemoryOpaqueCaptureAddressKHR>(load(device, "vkGetDeviceMemoryOpaqueCaptureAddressKHR"));
#endif /* defined(VK_KHR_buffer_device_address) */
#if defined(VK_KHR_calibrated_timestamps)
	table.vkGetCalibratedTimestampsKHR = reinterpret_cast<PFN_vkGetCalibratedTimestampsKHR>(load(device, "vkGetCalibratedTimestampsKHR"));
#endif /* defined(VK_KHR_calibrated_timestamps) */
#if defined(VK_KHR_copy_commands2)
	table.vkCmdBlitImage2KHR = reinterpret_cast<PFN_vkCmdBlitImage2KHR>(load(device, "vkCmdBlitImage2KHR"));
	table.vkCmdCopyBuffer2KHR = reinterpret_cast<PFN_vkCmdCopyBuffer2KHR>(load(device, "vkCmdCopyBuffer2KHR"));
	table.vkCmdCopyBufferToImage2KHR = reinterpret_cast<PFN_vkCmdCopyBufferToImage2KHR>(load(device, "vkCmdCopyBufferToImage2KHR"));
	table.vkCmdCopyImage2KHR = reinterpret_cast<PFN_vkCmdCopyImage2KHR>(load(device, "vkCmdCopyImage2KHR"));
	table.vkCmdCopyImageToBuffer2KHR = reinterpret_cast<PFN_vkCmdCopyImageToBuffer2KHR>(load(device, "vkCmdCopyImageToBuffer2KHR"));
	table.vkCmdResolveImage2KHR = reinterpret_cast<PFN_vkCmdResolveImage2KHR>(load(device, "vkCmdResolveImage2KHR"));
#endif /* defined(VK_KHR_copy_commands2) */
#if defined(VK_KHR_create_renderpass2)
	table.vkCmdBeginRenderPass2KHR = reinterpret_cast<PFN_vkCmdBeginRenderPass2KHR>(load(device, "vkCmdBeginRenderPass2KHR"));
	table.vkCmdEndRenderPass2KHR = reinterpret_cast<PFN_vkCmdEndRenderPass2KHR>(load(device, "vkCmdEndRenderPass2KHR"));
	table.vkCmdNextSubpass2KHR = reinterpret_cast<PFN_vkCmdNextSubpass2KHR>(load(device, "vkCmdNextSubpass2KHR"));
	table.vkCreateRenderPass2KHR = reinterpret_cast<PFN_vkCreateRenderPass2KHR>(load(device, "vkCreateRenderPass2KHR"));
#endif /* defined(VK_KHR_create_renderpass2) */
#if defined(VK_KHR_deferred_host_operations)
	table.vkCreateDeferredOperationKHR = reinterpret_cast<PFN_vkCreateDeferredOperationKHR>(load(device, "vkCreateDeferredOperationKHR"));
	table.vkDeferredOperationJoinKHR = reinterpret_cast<PFN_vkDeferredOperationJoinKHR>(load(device, "vkDeferredOperationJoinKHR"));
	table.vkDestroyDeferredOperationKHR = reinterpret_cast<PFN_vkDestroyDeferredOperationKHR>(load(device, "vkDestroyDeferredOperationKHR"));
	table.vkGetDeferredOperationMaxConcurrencyKHR = reinterpret_cast<PFN_vkGetDeferredOperationMaxConcurrencyKHR>(load(device, "vkGetDeferredOperationMaxConcurrencyKHR"));
	table.vkGetDeferredOperationResultKHR = reinterpret_cast<PFN_vkGetDeferredOperationResultKHR>(load(device, "vkGetDeferredOperationResultKHR"));
#endif /* defined(VK_KHR_deferred_host_operations) */
#if defined(VK_KHR_descriptor_update_template)
	table.vkCreateDescriptorUpdateTemplateKHR = reinterpret_cast<PFN_vkCreateDescriptorUpdateTemplateKHR>(load(device, "vkCreateDescriptorUpdateTemplateKHR"));
	table.vkDestroyDescriptorUpdateTemplateKHR = reinterpret_cast<PFN_vkDestroyDescriptorUpdateTemplateKHR>(load(device, "vkDestroyDescriptorUpdateTemplateKHR"));
	table.vkUpdateDescriptorSetWithTemplateKHR = reinterpret_cast<PFN_vkUpdateDescriptorSetWithTemplateKHR>(load(device, "vkUpdateDescriptorSetWithTemplateKHR"));
#endif /* defined(VK_KHR_descriptor_update_template) */
#if defined(VK_KHR_device_group)
	table.vkCmdDispatchBaseKHR = reinterpret_cast<PFN_vkCmdDispatchBaseKHR>(load(device, "vkCmdDispatchBaseKHR"));
	table.vkCmdSetDeviceMaskKHR = reinterpret_cast<PFN_vkCmdSetDeviceMaskKHR>(load(device, "vkCmdSetDeviceMaskKHR"));
	table.vkGetDeviceGroupPeerMemoryFeaturesKHR = reinterpret_cast<PFN_vkGetDeviceGroupPeerMemoryFeaturesKHR>(load(device, "vkGetDeviceGroupPeerMemoryFeaturesKHR"));
#endif /* defined(VK_KHR_device_group) */
#if defined(VK_KHR_display_swapchain)
	table.vkCreateSharedSwapchainsKHR = reinterpret_cast<PFN_vkCreateSharedSwapchainsKHR>(load(device, "vkCreateSharedSwapchainsKHR"));
#endif /* defined(VK_KHR_display_swapchain) */
#if defined(VK_KHR_draw_indirect_count)
	table.vkCmdDrawIndexedIndirectCountKHR = reinterpret_cast<PFN_vkCmdDrawIndexedIndirectCountKHR>(load(device, "vkCmdDrawIndexedIndirectCountKHR"));
	table.vkCmdDrawIndirectCountKHR = reinterpret_cast<PFN_vkCmdDrawIndirectCountKHR>(load(device, "vkCmdDrawIndirectCountKHR"));
#endif /* defined(VK_KHR_draw_indirect_count) */
#if defined(VK_KHR_dynamic_rendering)
	table.vkCmdBeginRenderingKHR = reinterpret_cast<PFN_vkCmdBeginRenderingKHR>(load(device, "vkCmdBeginRenderingKHR"));
	table.vkCmdEndRenderingKHR = reinterpret_cast<PFN_vkCmdEndRenderingKHR>(load(device, "vkCmdEndRenderingKHR"));
#endif /* defined(VK_KHR_dynamic_rendering) */
#if defined(VK_KHR_dynamic_rendering_local_read)
	table.vkCmdSetRenderingAttachmentLocationsKHR = reinterpret_cast<PFN_vkCmdSetRenderingAttachmentLocationsKHR>(load(device, "vkCmdSetRenderingAttachmentLocationsKHR"));
	table.vkCmdSetRenderingInputAttachmentIndicesKHR = reinterpret_cast<PFN_vkCmdSetRenderingInputAttachmentIndicesKHR>(load(device, "vkCmdSetRenderingInputAttachmentIndicesKHR"));
#endif /* defined(VK_KHR_dynamic_rendering_local_read) */
#if defined(VK_KHR_external_fence_fd)
	table.vkGetFenceFdKHR = reinterpret_cast<PFN_vkGetFenceFdKHR>(load(device, "vkGetFenceFdKHR"));
	table.vkImportFenceFdKHR = reinterpret_cast<PFN_vkImportFenceFdKHR>(load(device, "vkImportFenceFdKHR"));
#endif /* defined(VK_KHR_external_fence_fd) */
#if defined(VK_KHR_external_fence_win32)
	table.vkGetFenceWin32HandleKHR = reinterpret_cast<PFN_vkGetFenceWin32HandleKHR>(load(device, "vkGetFenceWin32HandleKHR"));
	table.vkImportFenceWin32HandleKHR = reinterpret_cast<PFN_vkImportFenceWin32HandleKHR>(load(device, "vkImportFenceWin32HandleKHR"));
#endif /* defined(VK_KHR_external_fence_win32) */
#if defined(VK_KHR_external_memory_fd)
	table.vkGetMemoryFdKHR = reinterpret_cast<PFN_vkGetMemoryFdKHR>(load(device, "vkGetMemoryFdKHR"));
	table.vkGetMemoryFdPropertiesKHR = reinterpret_cast<PFN_vkGetMemoryFdPropertiesKHR>(load(device, "vkGetMemoryFdPropertiesKHR"));
#endif /* defined(VK_KHR_external_memory_fd) */
#if defined(VK_KHR_external_memory_win32)
	table.vkGetMemoryWin32HandleKHR = reinterpret_cast<PFN_vkGetMemoryWin32HandleKHR>(load(device, "vkGetMemoryWin32HandleKHR"));
	table.vkGetMemoryWin32HandlePropertiesKHR = reinterpret_cast<PFN_vkGetMemoryWin32HandlePropertiesKHR>(load(device, "vkGetMemoryWin32HandlePropertiesKHR"));
#endif /* defined(VK_KHR_external_memory_win32) */
#if defined(VK_KHR_external_semaphore_fd)
	table.vkGetSemaphoreFdKHR = reinterpret_cast<PFN_vkGetSemaphoreFdKHR>(load(device, "vkGetSemaphoreFdKHR"));
	table.vkImportSemaphoreFdKHR = reinterpret_cast<PFN_vkImportSemaphoreFdKHR>(load(device, "vkImportSemaphoreFdKHR"));
#endif /* defined(VK_KHR_external_semaphore_fd) */
#if defined(VK_KHR_external_semaphore_win32)
	table.vkGetSemaphoreWin32HandleKHR = reinterpret_cast<PFN_vkGetSemaphoreWin32HandleKHR>(load(device, "vkGetSemaphoreWin32HandleKHR"));
	table.vkImportSemaphoreWin32HandleKHR = reinterpret_cast<PFN_vkImportSemaphoreWin32HandleKHR>(load(device, "vkImportSemaphoreWin32HandleKHR"));
#endif /* defined(VK_KHR_external_semaphore_win32) */
#if defined(VK_KHR_fragment_shading_rate)
	table.vkCmdSetFragmentShadingRateKHR = reinterpret_cast<PFN_vkCmdSetFragmentShadingRateKHR>(load(device, "vkCmdSetFragmentShadingRateKHR"));
#endif /* defined(VK_KHR_fragment_shading_rate) */
#if defined(VK_KHR_get_memory_requirements2)
	table.vkGetBufferMemoryRequirements2KHR = reinterpret_cast<PFN_vkGetBufferMemoryRequirements2KHR>(load(device, "vkGetBufferMemoryRequirements2KHR"));
	table.vkGetImageMemoryRequirements2KHR = reinterpret_cast<PFN_vkGetImageMemoryRequirements2KHR>(load(device, "vkGetImageMemoryRequirements2KHR"));
	table.vkGetImageSparseMemoryRequirements2KHR = reinterpret_cast<PFN_vkGetImageSparseMemoryRequirements2KHR>(load(device, "vkGetImageSparseMemoryRequirements2KHR"));
#endif /* defined(VK_KHR_get_memory_requirements2) */
#if defined(VK_KHR_line_rasterization)
	table.vkCmdSetLineStippleKHR = reinterpret_cast<PFN_vkCmdSetLineStippleKHR>(load(device, "vkCmdSetLineStippleKHR"));
#endif /* defined(VK_KHR_line_rasterization) */
#if defined(VK_KHR_maintenance1)
	table.vkTrimCommandPoolKHR = reinterpret_cast<PFN_vkTrimCommandPoolKHR>(load(device, "vkTrimCommandPoolKHR"));
#endif /* defined(VK_KHR_maintenance1) */
#if defined(VK_KHR_maintenance3)
	table.vkGetDescriptorSetLayoutSupportKHR = reinterpret_cast<PFN_vkGetDescriptorSetLayoutSupportKHR>(load(device, "vkGetDescriptorSetLayoutSupportKHR"));
#endif /* defined(VK_KHR_maintenance3) */
#if defined(VK_KHR_maintenance4)
	table.vkGetDeviceBufferMemoryRequirementsKHR = reinterpret_cast<PFN_vkGetDeviceBufferMemoryRequirementsKHR>(load(device, "vkGetDeviceBufferMemoryRequirementsKHR"));
	table.vkGetDeviceImageMemoryRequirementsKHR = reinterpret_cast<PFN_vkGetDeviceImageMemoryRequirementsKHR>(load(device, "vkGetDeviceImageMemoryRequirementsKHR"));
	table.vkGetDeviceImageSparseMemoryRequirementsKHR = reinterpret_cast<PFN_vkGetDeviceImageSparseMemoryRequirementsKHR>(load(device, "vkGetDeviceImageSparseMemoryRequirementsKHR"));
#endif /* defined(VK_KHR_maintenance4) */
#if defined(VK_KHR_maintenance5)
	table.vkCmdBindIndexBuffer2KHR = reinterpret_cast<PFN_vkCmdBindIndexBuffer2KHR>(load(device, "vkCmdBindIndexBuffer2KHR"));
	table.vkGetDeviceImageSubresourceLayoutKHR = reinterpret_cast<PFN_vkGetDeviceImageSubresourceLayoutKHR>(load(device, "vkGetDeviceImageSubresourceLayoutKHR"));
	table.vkGetImageSubresourceLayout2KHR = reinterpret_cast<PFN_vkGetImageSubresourceLayout2KHR>(load(device, "vkGetImageSubresourceLayout2KHR"));
	table.vkGetRenderingAreaGranularityKHR = reinterpret_cast<PFN_vkGetRenderingAreaGranularityKHR>(load(device, "vkGetRenderingAreaGranularityKHR"));
#endif /* defined(VK_KHR_maintenance5) */
#if defined(VK_KHR_maintenance6)
	table.vkCmdBindDescriptorSets2KHR = reinterpret_cast<PFN_vkCmdBindDescriptorSets2KHR>(load(device, "vkCmdBindDescriptorSets2KHR"));
	table.vkCmdPushConstants2KHR = reinterpret_cast<PFN_vkCmdPushConstants2KHR>(load(device, "vkCmdPushConstants2KHR"));
#endif /* defined(VK_KHR_maintenance6) */
#if defined(VK_KHR_maintenance6) && defined(VK_KHR_push_descriptor)
	table.vkCmdPushDescriptorSet2KHR = reinterpret_cast<PFN_vkCmdPushDescriptorSet2KHR>(load(device, "vkCmdPushDescriptorSet2KHR"));
	table.vkCmdPushDescriptorSetWithTemplate2KHR = reinterpret_cast<PFN_vkCmdPushDescriptorSetWithTemplate2KHR>(load(device, "vkCmdPushDescriptorSetWithTemplate2KHR"));
#endif /* defined(VK_KHR_maintenance6) && defined(VK_KHR_push_descriptor) */
#if defined(VK_KHR_maintenance6) && defined(VK_EXT_descriptor_buffer)
	table.vkCmdBindDescriptorBufferEmbeddedSamplers2EXT = reinterpret_cast<PFN_vkCmdBindDescriptorBufferEmbeddedSamplers2EXT>(load(device, "vkCmdBindDescriptorBufferEmbeddedSamplers2EXT"));
	table.vkCmdSetDescriptorBufferOffsets2EXT = reinterpret_cast<PFN_vkCmdSetDescriptorBufferOffsets2EXT>(load(device, "vkCmdSetDescriptorBufferOffsets2EXT"));
#endif /* defined(VK_KHR_maintenance6) && defined(VK_EXT_descriptor_buffer) */
#if defined(VK_KHR_map_memory2)
	table.vkMapMemory2KHR = reinterpret_cast<PFN_vkMapMemory2KHR>(load(device, "vkMapMemory2KHR"));
	table.vkUnmapMemory2KHR = reinterpret_cast<PFN_vkUnmapMemory2KHR>(load(device, "vkUnmapMemory2KHR"));
#endif /* defined(VK_KHR_map_memory2) */
#if defined(VK_KHR_performance_query)
	table.vkAcquireProfilingLockKHR = reinterpret_cast<PFN_vkAcquireProfilingLockKHR>(load(device, "vkAcquireProfilingLockKHR"));
	table.vkReleaseProfilingLockKHR = reinterpret_cast<PFN_vkReleaseProfilingLockKHR>(load(device, "vkReleaseProfilingLockKHR"));
#endif /* defined(VK_KHR_performance_query) */
#if defined(VK_KHR_pipeline_executable_properties)
	table.vkGetPipelineExecutableInternalRepresentationsKHR = reinterpret_cast<PFN_vkGetPipelineExecutableInternalRepresentationsKHR>(load(device, "vkGetPipelineExecutableInternalRepresentationsKHR"));
	table.vkGetPipelineExecutablePropertiesKHR = reinterpret_cast<PFN_vkGetPipelineExecutablePropertiesKHR>(load(device, "vkGetPipelineExecutablePropertiesKHR"));
	table.vkGetPipelineExecutableStatisticsKHR = reinterpret_cast<PFN_vkGetPipelineExecutableStatisticsKHR>(load(device, "vkGetPipelineExecutableStatisticsKHR"));
#endif /* defined(VK_KHR_pipeline_executable_properties) */
#if defined(VK_KHR_present_wait)
	table.vkWaitForPresentKHR = reinterpret_cast<PFN_vkWaitForPresentKHR>(load(device, "vkWaitForPresentKHR"));
#endif /* defined(VK_KHR_present_wait) */
#if defined(VK_KHR_push_descriptor)
	table.vkCmdPushDescriptorSetKHR = reinterpret_cast<PFN_vkCmdPushDescriptorSetKHR>(load(device, "vkCmdPushDescriptorSetKHR"));
#endif /* defined(VK_KHR_push_descriptor) */
#if defined(VK_KHR_ray_tracing_maintenance1) && defined(VK_KHR_ray_tracing_pipeline)
	table.vkCmdTraceRaysIndirect2KHR = reinterpret_cast<PFN_vkCmdTraceRaysIndirect2KHR>(load(device, "vkCmdTraceRaysIndirect2KHR"));
#endif /* defined(VK_KHR_ray_tracing_maintenance1) && defined(VK_KHR_ray_tracing_pipeline) */
#if defined(VK_KHR_ray_tracing_pipeline)
	table.vkCmdSetRayTracingPipelineStackSizeKHR = reinterpret_cast<PFN_vkCmdSetRayTracingPipelineStackSizeKHR>(load(device, "vkCmdSetRayTracingPipelineStackSizeKHR"));
	table.vkCmdTraceRaysIndirectKHR = reinterpret_cast<PFN_vkCmdTraceRaysIndirectKHR>(load(device, "vkCmdTraceRaysIndirectKHR"));
	table.vkCmdTraceRaysKHR = reinterpret_cast<PFN_vkCmdTraceRaysKHR>(load(device, "vkCmdTraceRaysKHR"));
	table.vkCreateRayTracingPipelinesKHR = reinterpret_cast<PFN_vkCreateRayTracingPipelinesKHR>(load(device, "vkCreateRayTracingPipelinesKHR"));
	table.vkGetRayTracingCaptureReplayShaderGroupHandlesKHR = reinterpret_cast<PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR>(load(device, "vkGetRayTracingCaptureReplayShaderGroupHandlesKHR"));
	table.vkGetRayTracingShaderGroupHandlesKHR = reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesKHR>(load(device, "vkGetRayTracingShaderGroupHandlesKHR"));
	table.vkGetRayTracingShaderGroupStackSizeKHR = reinterpret_cast<PFN_vkGetRayTracingShaderGroupStackSizeKHR>(load(device, "vkGetRayTracingShaderGroupStackSizeKHR"));
#endif /* defined(VK_KHR_ray_tracing_pipeline) */
#if defined(VK_KHR_sampler_ycbcr_conversion)
	table.vkCreateSamplerYcbcrConversionKHR = reinterpret_cast<PFN_vkCreateSamplerYcbcrConversionKHR>(load(device, "vkCreateSamplerYcbcrConversionKHR"));
	table.vkDestroySamplerYcbcrConversionKHR = reinterpret_cast<PFN_vkDestroySamplerYcbcrConversionKHR>(load(device, "vkDestroySamplerYcbcrConversionKHR"));
#endif /* defined(VK_KHR_sampler_ycbcr_conversion) */
#if defined(VK_KHR_shared_presentable_image)
	table.vkGetSwapchainStatusKHR = reinterpret_cast<PFN_vkGetSwapchainStatusKHR>(load(device, "vkGetSwapchainStatusKHR"));
#endif /* defined(VK_KHR_shared_presentable_image) */
#if defined(VK_KHR_swapchain)
	table.vkAcquireNextImageKHR = reinterpret_cast<PFN_vkAcquireNextImageKHR>(load(device, "vkAcquireNextImageKHR"));
	table.vkCreateSwapchainKHR = reinterpret_cast<PFN_vkCreateSwapchainKHR>(load(device, "vkCreateSwapchainKHR"));
	table.vkDestroySwapchainKHR = reinterpret_cast<PFN_vkDestroySwapchainKHR>(load(device, "vkDestroySwapchainKHR"));
	table.vkGetSwapchainImagesKHR = reinterpret_cast<PFN_vkGetSwapchainImagesKHR>(load(device, "vkGetSwapchainImagesKHR"));
	table.vkQueuePresentKHR = reinterpret_cast<PFN_vkQueuePresentKHR>(load(device, "vkQueuePresentKHR"));
#endif /* defined(VK_KHR_swapchain) */
#if defined(VK_KHR_synchronization2)
	table.vkCmdPipelineBarrier2KHR = reinterpret_cast<PFN_vkCmdPipelineBarrier2KHR>(load(device, "vkCmdPipelineBarrier2KHR"));
	table.vkCmdResetEvent2KHR = reinterpret_cast<PFN_vkCmdResetEvent2KHR>(load(device, "vkCmdResetEvent2KHR"));
	table.vkCmdSetEvent2KHR = reinterpret_cast<PFN_vkCmdSetEvent2KHR>(load(device, "vkCmdSetEvent2KHR"));
	table.vkCmdWaitEvents2KHR = reinterpret_cast<PFN_vkCmdWaitEvents2KHR>(load(device, "vkCmdWaitEvents2KHR"));
	table.vkCmdWriteTimestamp2KHR = reinterpret_cast<PFN_vkCmdWriteTimestamp2KHR>(load(device, "vkCmdWriteTimestamp2KHR"));
	table.vkQueueSubmit2KHR = reinterpret_cast<PFN_vkQueueSubmit2KHR>(load(device, "vkQueueSubmit2KHR"));
#endif /* defined(VK_KHR_synchronization2) */
#if defined(VK_KHR_synchronization2) && defined(VK_AMD_buffer_marker)
	table.vkCmdWriteBufferMarker2AMD = reinterpret_cast<PFN_vkCmdWriteBufferMarker2AMD>(load(device, "vkCmdWriteBufferMarker2AMD"));
#endif /* defined(VK_KHR_synchronization2) && defined(VK_AMD_buffer_marker) */
#if defined(VK_KHR_synchronization2) && defined(VK_NV_device_diagnostic_checkpoints)
	table.vkGetQueueCheckpointData2NV = reinterpret_cast<PFN_vkGetQueueCheckpointData2NV>(load(device, "vkGetQueueCheckpointData2NV"));
#endif /* defined(VK_KHR_synchronization2) && defined(VK_NV_device_diagnostic_checkpoints) */
#if defined(VK_KHR_timeline_semaphore)
	table.vkGetSemaphoreCounterValueKHR = reinterpret_cast<PFN_vkGetSemaphoreCounterValueKHR>(load(device, "vkGetSemaphoreCounterValueKHR"));
	table.vkSignalSemaphoreKHR = reinterpret_cast<PFN_vkSignalSemaphoreKHR>(load(device, "vkSignalSemaphoreKHR"));
	table.vkWaitSemaphoresKHR = reinterpret_cast<PFN_vkWaitSemaphoresKHR>(load(device, "vkWaitSemaphoresKHR"));
#endif /* defined(VK_KHR_timeline_semaphore) */
#if defined(VK_KHR_video_decode_queue)
	table.vkCmdDecodeVideoKHR = reinterpret_cast<PFN_vkCmdDecodeVideoKHR>(load(device, "vkCmdDecodeVideoKHR"));
#endif /* defined(VK_KHR_video_decode_queue) */
#if defined(VK_KHR_video_encode_queue)
	table.vkCmdEncodeVideoKHR = reinterpret_cast<PFN_vkCmdEncodeVideoKHR>(load(device, "vkCmdEncodeVideoKHR"));
	table.vkGetEncodedVideoSessionParametersKHR = reinterpret_cast<PFN_vkGetEncodedVideoSessionParametersKHR>(load(device, "vkGetEncodedVideoSessionParametersKHR"));
#endif /* defined(VK_KHR_video_encode_queue) */
#if defined(VK_KHR_video_queue)
	table.vkBindVideoSessionMemoryKHR = reinterpret_cast<PFN_vkBindVideoSessionMemoryKHR>(load(device, "vkBindVideoSessionMemoryKHR"));
	table.vkCmdBeginVideoCodingKHR = reinterpret_cast<PFN_vkCmdBeginVideoCodingKHR>(load(device, "vkCmdBeginVideoCodingKHR"));
	table.vkCmdControlVideoCodingKHR = reinterpret_cast<PFN_vkCmdControlVideoCodingKHR>(load(device, "vkCmdControlVideoCodingKHR"));
	table.vkCmdEndVideoCodingKHR = reinterpret_cast<PFN_vkCmdEndVideoCodingKHR>(load(device, "vkCmdEndVideoCodingKHR"));
	table.vkCreateVideoSessionKHR = reinterpret_cast<PFN_vkCreateVideoSessionKHR>(load(device, "vkCreateVideoSessionKHR"));
	table.vkCreateVideoSessionParametersKHR = reinterpret_cast<PFN_vkCreateVideoSessionParametersKHR>(load(device, "vkCreateVideoSessionParametersKHR"));
	table.vkDestroyVideoSessionKHR = reinterpret_cast<PFN_vkDestroyVideoSessionKHR>(load(device, "vkDestroyVideoSessionKHR"));
	table.vkDestroyVideoSessionParametersKHR = reinterpret_cast<PFN_vkDestroyVideoSessionParametersKHR>(load(device, "vkDestroyVideoSessionParametersKHR"));
	table.vkGetVideoSessionMemoryRequirementsKHR = reinterpret_cast<PFN_vkGetVideoSessionMemoryRequirementsKHR>(load(device, "vkGetVideoSessionMemoryRequirementsKHR"));
	table.vkUpdateVideoSessionParametersKHR = reinterpret_cast<PFN_vkUpdateVideoSessionParametersKHR>(load(device, "vkUpdateVideoSessionParametersKHR"));
#endif /* defined(VK_KHR_video_queue) */
#if defined(VK_NVX_binary_import)
	table.vkCmdCuLaunchKernelNVX = reinterpret_cast<PFN_vkCmdCuLaunchKernelNVX>(load(device, "vkCmdCuLaunchKernelNVX"));
	table.vkCreateCuFunctionNVX = reinterpret_cast<PFN_vkCreateCuFunctionNVX>(load(device, "vkCreateCuFunctionNVX"));
	table.vkCreateCuModuleNVX = reinterpret_cast<PFN_vkCreateCuModuleNVX>(load(device, "vkCreateCuModuleNVX"));
	table.vkDestroyCuFunctionNVX = reinterpret_cast<PFN_vkDestroyCuFunctionNVX>(load(device, "vkDestroyCuFunctionNVX"));
	table.vkDestroyCuModuleNVX = reinterpret_cast<PFN_vkDestroyCuModuleNVX>(load(device, "vkDestroyCuModuleNVX"));
#endif /* defined(VK_NVX_binary_import) */
#if defined(VK_NVX_image_view_handle)
	table.vkGetImageViewAddressNVX = reinterpret_cast<PFN_vkGetImageViewAddressNVX>(load(device, "vkGetImageViewAddressNVX"));
	table.vkGetImageViewHandleNVX = reinterpret_cast<PFN_vkGetImageViewHandleNVX>(load(device, "vkGetImageViewHandleNVX"));
#endif /* defined(VK_NVX_image_view_handle) */
#if defined(VK_NV_clip_space_w_scaling)
	table.vkCmdSetViewportWScalingNV = reinterpret_cast<PFN_vkCmdSetViewportWScalingNV>(load(device, "vkCmdSetViewportWScalingNV"));
#endif /* defined(VK_NV_clip_space_w_scaling) */
#if defined(VK_NV_copy_memory_indirect)
	table.vkCmdCopyMemoryIndirectNV = reinterpret_cast<PFN_vkCmdCopyMemoryIndirectNV>(load(device, "vkCmdCopyMemoryIndirectNV"));
	table.vkCmdCopyMemoryToImageIndirectNV = reinterpret_cast<PFN_vkCmdCopyMemoryToImageIndirectNV>(load(device, "vkCmdCopyMemoryToImageIndirectNV"));
#endif /* defined(VK_NV_copy_memory_indirect) */
#if defined(VK_NV_cuda_kernel_launch)
	table.vkCmdCudaLaunchKernelNV = reinterpret_cast<PFN_vkCmdCudaLaunchKernelNV>(load(device, "vkCmdCudaLaunchKernelNV"));
	table.vkCreateCudaFunctionNV = reinterpret_cast<PFN_vkCreateCudaFunctionNV>(load(device, "vkCreateCudaFunctionNV"));
	table.vkCreateCudaModuleNV = reinterpret_cast<PFN_vkCreateCudaModuleNV>(load(device, "vkCreateCudaModuleNV"));
	table.vkDestroyCudaFunctionNV = reinterpret_cast<PFN_vkDestroyCudaFunctionNV>(load(device, "vkDestroyCudaFunctionNV"));
	table.vkDestroyCudaModuleNV = reinterpret_cast<PFN_vkDestroyCudaModuleNV>(load(device, "vkDestroyCudaModuleNV"));
	table.vkGetCudaModuleCacheNV = reinterpret_cast<PFN_vkGetCudaModuleCacheNV>(load(device, "vkGetCudaModuleCacheNV"));
#endif /* defined(VK_NV_cuda_kernel_launch) */
#if defined(VK_NV_device_diagnostic_checkpoints)
	table.vkCmdSetCheckpointNV = reinterpret_cast<PFN_vkCmdSetCheckpointNV>(load(device, "vkCmdSetCheckpointNV"));
	table.vkGetQueueCheckpointDataNV = reinterpret_cast<PFN_vkGetQueueCheckpointDataNV>(load(device, "vkGetQueueCheckpointDataNV"));
#endif /* defined(VK_NV_device_diagnostic_checkpoints) */
#if defined(VK_NV_device_generated_commands)
	table.vkCmdBindPipelineShaderGroupNV = reinterpret_cast<PFN_vkCmdBindPipelineShaderGroupNV>(load(device, "vkCmdBindPipelineShaderGroupNV"));
	table.vkCmdExecuteGeneratedCommandsNV = reinterpret_cast<PFN_vkCmdExecuteGeneratedCommandsNV>(load(device, "vkCmdExecuteGeneratedCommandsNV"));
	table.vkCmdPreprocessGeneratedCommandsNV = reinterpret_cast<PFN_vkCmdPreprocessGeneratedCommandsNV>(load(device, "vkCmdPreprocessGeneratedCommandsNV"));
	table.vkCreateIndirectCommandsLayoutNV = reinterpret_cast<PFN_vkCreateIndirectCommandsLayoutNV>(load(device, "vkCreateIndirectCommandsLayoutNV"));
	table.vkDestroyIndirectCommandsLayoutNV = reinterpret_cast<PFN_vkDestroyIndirectCommandsLayoutNV>(load(device, "vkDestroyIndirectCommandsLayoutNV"));
	table.vkGetGeneratedCommandsMemoryRequirementsNV = reinterpret_cast<PFN_vkGetGeneratedCommandsMemoryRequirementsNV>(load(device, "vkGetGeneratedCommandsMemoryRequirementsNV"));
#endif /* defined(VK_NV_device_generated_commands) */
#if defined(VK_NV_device_generated_commands_compute)
	table.vkCmdUpdatePipelineIndirectBufferNV = reinterpret_cast<PFN_vkCmdUpdatePipelineIndirectBufferNV>(load(device, "vkCmdUpdatePipelineIndirectBufferNV"));
	table.vkGetPipelineIndirectDeviceAddressNV = reinterpret_cast<PFN_vkGetPipelineIndirectDeviceAddressNV>(load(device, "vkGetPipelineIndirectDeviceAddressNV"));
	table.vkGetPipelineIndirectMemoryRequirementsNV = reinterpret_cast<PFN_vkGetPipelineIndirectMemoryRequirementsNV>(load(device, "vkGetPipelineIndirectMemoryRequirementsNV"));
#endif /* defined(VK_NV_device_generated_commands_compute) */
#if defined(VK_NV_external_memory_rdma)
	table.vkGetMemoryRemoteAddressNV = reinterpret_cast<PFN_vkGetMemoryRemoteAddressNV>(load(device, "vkGetMemoryRemoteAddressNV"));
#endif /* defined(VK_NV_external_memory_rdma) */
#if defined(VK_NV_external_memory_win32)
	table.vkGetMemoryWin32HandleNV = reinterpret_cast<PFN_vkGetMemoryWin32HandleNV>(load(device, "vkGetMemoryWin32HandleNV"));
#endif /* defined(VK_NV_external_memory_win32) */
#if defined(VK_NV_fragment_shading_rate_enums)
	table.vkCmdSetFragmentShadingRateEnumNV = reinterpret_cast<PFN_vkCmdSetFragmentShadingRateEnumNV>(load(device, "vkCmdSetFragmentShadingRateEnumNV"));
#endif /* defined(VK_NV_fragment_shading_rate_enums) */
#if defined(VK_NV_low_latency2)
	table.vkGetLatencyTimingsNV = reinterpret_cast<PFN_vkGetLatencyTimingsNV>(load(device, "vkGetLatencyTimingsNV"));
	table.vkLatencySleepNV = reinterpret_cast<PFN_vkLatencySleepNV>(load(device, "vkLatencySleepNV"));
	table.vkQueueNotifyOutOfBandNV = reinterpret_cast<PFN_vkQueueNotifyOutOfBandNV>(load(device, "vkQueueNotifyOutOfBandNV"));
	table.vkSetLatencyMarkerNV = reinterpret_cast<PFN_vkSetLatencyMarkerNV>(load(device, "vkSetLatencyMarkerNV"));
	table.vkSetLatencySleepModeNV = reinterpret_cast<PFN_vkSetLatencySleepModeNV>(load(device, "vkSetLatencySleepModeNV"));
#endif /* defined(VK_NV_low_latency2) */
#if defined(VK_NV_memory_decompression)
	table.vkCmdDecompressMemoryIndirectCountNV = reinterpret_cast<PFN_vkCmdDecompressMemoryIndirectCountNV>(load(device, "vkCmdDecompressMemoryIndirectCountNV"));
	table.vkCmdDecompressMemoryNV = reinterpret_cast<PFN_vkCmdDecompressMemoryNV>(load(device, "vkCmdDecompressMemoryNV"));
#endif /* defined(VK_NV_memory_decompression) */
#if defined(VK_NV_mesh_shader)
	table.vkCmdDrawMeshTasksIndirectCountNV = reinterpret_cast<PFN_vkCmdDrawMeshTasksIndirectCountNV>(load(device, "vkCmdDrawMeshTasksIndirectCountNV"));
	table.vkCmdDrawMeshTasksIndirectNV = reinterpret_cast<PFN_vkCmdDrawMeshTasksIndirectNV>(load(device, "vkCmdDrawMeshTasksIndirectNV"));
	table.vkCmdDrawMeshTasksNV = reinterpret_cast<PFN_vkCmdDrawMeshTasksNV>(load(device, "vkCmdDrawMeshTasksNV"));
#endif /* defined(VK_NV_mesh_shader) */
#if defined(VK_NV_optical_flow)
	table.vkBindOpticalFlowSessionImageNV = reinterpret_cast<PFN_vkBindOpticalFlowSessionImageNV>(load(device, "vkBindOpticalFlowSessionImageNV"));
	table.vkCmdOpticalFlowExecuteNV = reinterpret_cast<PFN_vkCmdOpticalFlowExecuteNV>(load(device, "vkCmdOpticalFlowExecuteNV"));
	table.vkCreateOpticalFlowSessionNV = reinterpret_cast<PFN_vkCreateOpticalFlowSessionNV>(load(device, "vkCreateOpticalFlowSessionNV"));
	table.vkDestroyOpticalFlowSessionNV = reinterpret_cast<PFN_vkDestroyOpticalFlowSessionNV>(load(device, "vkDestroyOpticalFlowSessionNV"));
#endif /* defined(VK_NV_optical_flow) */
#if defined(VK_NV_ray_tracing)
	table.vkBindAccelerationStructureMemoryNV = reinterpret_cast<PFN_vkBindAccelerationStructureMemoryNV>(load(device, "vkBindAccelerationStructureMemoryNV"));
	table.vkCmdBuildAccelerationStructureNV = reinterpret_cast<PFN_vkCmdBuildAccelerationStructureNV>(load(device, "vkCmdBuildAccelerationStructureNV"));
	table.vkCmdCopyAccelerationStructureNV = reinterpret_cast<PFN_vkCmdCopyAccelerationStructureNV>(load(device, "vkCmdCopyAccelerationStructureNV"));
	table.vkCmdTraceRaysNV = reinterpret_cast<PFN_vkCmdTraceRaysNV>(load(device, "vkCmdTraceRaysNV"));
	table.vkCmdWriteAccelerationStructuresPropertiesNV = reinterpret_cast<PFN_vkCmdWriteAccelerationStructuresPropertiesNV>(load(device, "vkCmdWriteAccelerationStructuresPropertiesNV"));
	table.vkCompileDeferredNV = reinterpret_cast<PFN_vkCompileDeferredNV>(load(device, "vkCompileDeferredNV"));
	table.vkCreateAccelerationStructureNV = reinterpret_cast<PFN_vkCreateAccelerationStructureNV>(load(device, "vkCreateAccelerationStructureNV"));
	table.vkCreateRayTracingPipelinesNV = reinterpret_cast<PFN_vkCreateRayTracingPipelinesNV>(load(device, "vkCreateRayTracingPipelinesNV"));
	table.vkDestroyAccelerationStructureNV = reinterpret_cast<PFN_vkDestroyAccelerationStructureNV>(load(device, "vkDestroyAccelerationStructureNV"));
	table.vkGetAccelerationStructureHandleNV = reinterpret_cast<PFN_vkGetAccelerationStructureHandleNV>(load(device, "vkGetAccelerationStructureHandleNV"));
	table.vkGetAccelerationStructureMemoryRequirementsNV = reinterpret_cast<PFN_vkGetAccelerationStructureMemoryRequirementsNV>(load(device, "vkGetAccelerationStructureMemoryRequirementsNV"));
	table.vkGetRayTracingShaderGroupHandlesNV = reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesNV>(load(device, "vkGetRayTracingShaderGroupHandlesNV"));
#endif /* defined(VK_NV_ray_tracing) */
#if defined(VK_NV_scissor_exclusive) && VK_NV_SCISSOR_EXCLUSIVE_SPEC_VERSION >= 2
	table.vkCmdSetExclusiveScissorEnableNV = reinterpret_cast<PFN_vkCmdSetExclusiveScissorEnableNV>(load(device, "vkCmdSetExclusiveScissorEnableNV"));
#endif /* defined(VK_NV_scissor_exclusive) && VK_NV_SCISSOR_EXCLUSIVE_SPEC_VERSION >= 2 */
#if defined(VK_NV_scissor_exclusive)
	table.vkCmdSetExclusiveScissorNV = reinterpret_cast<PFN_vkCmdSetExclusiveScissorNV>(load(device, "vkCmdSetExclusiveScissorNV"));
#endif /* defined(VK_NV_scissor_exclusive) */
#if defined(VK_NV_shading_rate_image)
	table.vkCmdBindShadingRateImageNV = reinterpret_cast<PFN_vkCmdBindShadingRateImageNV>(load(device, "vkCmdBindShadingRateImageNV"));
	table.vkCmdSetCoarseSampleOrderNV = reinterpret_cast<PFN_vkCmdSetCoarseSampleOrderNV>(load(device, "vkCmdSetCoarseSampleOrderNV"));
	table.vkCmdSetViewportShadingRatePaletteNV = reinterpret_cast<PFN_vkCmdSetViewportShadingRatePaletteNV>(load(device, "vkCmdSetViewportShadingRatePaletteNV"));
#endif /* defined(VK_NV_shading_rate_image) */
#if defined(VK_QCOM_tile_properties)
	table.vkGetDynamicRenderingTilePropertiesQCOM = reinterpret_cast<PFN_vkGetDynamicRenderingTilePropertiesQCOM>(load(device, "vkGetDynamicRenderingTilePropertiesQCOM"));
	table.vkGetFramebufferTilePropertiesQCOM = reinterpret_cast<PFN_vkGetFramebufferTilePropertiesQCOM>(load(device, "vkGetFramebufferTilePropertiesQCOM"));
#endif /* defined(VK_QCOM_tile_properties) */
#if defined(VK_QNX_external_memory_screen_buffer)
	table.vkGetScreenBufferPropertiesQNX = reinterpret_cast<PFN_vkGetScreenBufferPropertiesQNX>(load(device, "vkGetScreenBufferPropertiesQNX"));
#endif /* defined(VK_QNX_external_memory_screen_buffer) */
#if defined(VK_VALVE_descriptor_set_host_mapping)
	table.vkGetDescriptorSetHostMappingVALVE = reinterpret_cast<PFN_vkGetDescriptorSetHostMappingVALVE>(load(device, "vkGetDescriptorSetHostMappingVALVE"));
	table.vkGetDescriptorSetLayoutHostMappingInfoVALVE = reinterpret_cast<PFN_vkGetDescriptorSetLayoutHostMappingInfoVALVE>(load(device, "vkGetDescriptorSetLayoutHostMappingInfoVALVE"));
#endif /* defined(VK_VALVE_descriptor_set_host_mapping) */
#if (defined(VK_EXT_extended_dynamic_state)) || (defined(VK_EXT_shader_object))
	table.vkCmdBindVertexBuffers2EXT = reinterpret_cast<PFN_vkCmdBindVertexBuffers2EXT>(load(device, "vkCmdBindVertexBuffers2EXT"));
	table.vkCmdSetCullModeEXT = reinterpret_cast<PFN_vkCmdSetCullModeEXT>(load(device, "vkCmdSetCullModeEXT"));
	table.vkCmdSetDepthBoundsTestEnableEXT = reinterpret_cast<PFN_vkCmdSetDepthBoundsTestEnableEXT>(load(device, "vkCmdSetDepthBoundsTestEnableEXT"));
	table.vkCmdSetDepthCompareOpEXT = reinterpret_cast<PFN_vkCmdSetDepthCompareOpEXT>(load(device, "vkCmdSetDepthCompareOpEXT"));
	table.vkCmdSetDepthTestEnableEXT = reinterpret_cast<PFN_vkCmdSetDepthTestEnableEXT>(load(device, "vkCmdSetDepthTestEnableEXT"));
	table.vkCmdSetDepthWriteEnableEXT = reinterpret_cast<PFN_vkCmdSetDepthWriteEnableEXT>(load(device, "vkCmdSetDepthWriteEnableEXT"));
	table.vkCmdSetFrontFaceEXT = reinterpret_cast<PFN_vkCmdSetFrontFaceEXT>(load(device, "vkCmdSetFrontFaceEXT"));
	table.vkCmdSetPrimitiveTopologyEXT = reinterpret_cast<PFN_vkCmdSetPrimitiveTopologyEXT>(load(device, "vkCmdSetPrimitiveTopologyEXT"));
	table.vkCmdSetScissorWithCountEXT = reinterpret_cast<PFN_vkCmdSetScissorWithCountEXT>(load(device, "vkCmdSetScissorWithCountEXT"));
	table.vkCmdSetStencilOpEXT = reinterpret_cast<PFN_vkCmdSetStencilOpEXT>(load(device, "vkCmdSetStencilOpEXT"));
	table.vkCmdSetStencilTestEnableEXT = reinterpret_cast<PFN_vkCmdSetStencilTestEnableEXT>(load(device, "vkCmdSetStencilTestEnableEXT"));
	table.vkCmdSetViewportWithCountEXT = reinterpret_cast<PFN_vkCmdSetViewportWithCountEXT>(load(device, "vkCmdSetViewportWithCountEXT"));
#endif /* (defined(VK_EXT_extended_dynamic_state)) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state2)) || (defined(VK_EXT_shader_object))
	table.vkCmdSetDepthBiasEnableEXT = reinterpret_cast<PFN_vkCmdSetDepthBiasEnableEXT>(load(device, "vkCmdSetDepthBiasEnableEXT"));
	table.vkCmdSetLogicOpEXT = reinterpret_cast<PFN_vkCmdSetLogicOpEXT>(load(device, "vkCmdSetLogicOpEXT"));
	table.vkCmdSetPatchControlPointsEXT = reinterpret_cast<PFN_vkCmdSetPatchControlPointsEXT>(load(device, "vkCmdSetPatchControlPointsEXT"));
	table.vkCmdSetPrimitiveRestartEnableEXT = reinterpret_cast<PFN_vkCmdSetPrimitiveRestartEnableEXT>(load(device, "vkCmdSetPrimitiveRestartEnableEXT"));
	table.vkCmdSetRasterizerDiscardEnableEXT = reinterpret_cast<PFN_vkCmdSetRasterizerDiscardEnableEXT>(load(device, "vkCmdSetRasterizerDiscardEnableEXT"));
#endif /* (defined(VK_EXT_extended_dynamic_state2)) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state3)) || (defined(VK_EXT_shader_object))
	table.vkCmdSetAlphaToCoverageEnableEXT = reinterpret_cast<PFN_vkCmdSetAlphaToCoverageEnableEXT>(load(device, "vkCmdSetAlphaToCoverageEnableEXT"));
	table.vkCmdSetAlphaToOneEnableEXT = reinterpret_cast<PFN_vkCmdSetAlphaToOneEnableEXT>(load(device, "vkCmdSetAlphaToOneEnableEXT"));
	table.vkCmdSetColorBlendEnableEXT = reinterpret_cast<PFN_vkCmdSetColorBlendEnableEXT>(load(device, "vkCmdSetColorBlendEnableEXT"));
	table.vkCmdSetColorBlendEquationEXT = reinterpret_cast<PFN_vkCmdSetColorBlendEquationEXT>(load(device, "vkCmdSetColorBlendEquationEXT"));
	table.vkCmdSetColorWriteMaskEXT = reinterpret_cast<PFN_vkCmdSetColorWriteMaskEXT>(load(device, "vkCmdSetColorWriteMaskEXT"));
	table.vkCmdSetDepthClampEnableEXT = reinterpret_cast<PFN_vkCmdSetDepthClampEnableEXT>(load(device, "vkCmdSetDepthClampEnableEXT"));
	table.vkCmdSetLogicOpEnableEXT = reinterpret_cast<PFN_vkCmdSetLogicOpEnableEXT>(load(device, "vkCmdSetLogicOpEnableEXT"));
	table.vkCmdSetPolygonModeEXT = reinterpret_cast<PFN_vkCmdSetPolygonModeEXT>(load(device, "vkCmdSetPolygonModeEXT"));
	table.vkCmdSetRasterizationSamplesEXT = reinterpret_cast<PFN_vkCmdSetRasterizationSamplesEXT>(load(device, "vkCmdSetRasterizationSamplesEXT"));
	table.vkCmdSetSampleMaskEXT = reinterpret_cast<PFN_vkCmdSetSampleMaskEXT>(load(device, "vkCmdSetSampleMaskEXT"));
#endif /* (defined(VK_EXT_extended_dynamic_state3)) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state3) && (defined(VK_KHR_maintenance2) || defined(VK_VERSION_1_1))) || (defined(VK_EXT_shader_object))
	table.vkCmdSetTessellationDomainOriginEXT = reinterpret_cast<PFN_vkCmdSetTessellationDomainOriginEXT>(load(device, "vkCmdSetTessellationDomainOriginEXT"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && (defined(VK_KHR_maintenance2) || defined(VK_VERSION_1_1))) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_transform_feedback)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_transform_feedback))
	table.vkCmdSetRasterizationStreamEXT = reinterpret_cast<PFN_vkCmdSetRasterizationStreamEXT>(load(device, "vkCmdSetRasterizationStreamEXT"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_transform_feedback)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_transform_feedback)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_conservative_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_conservative_rasterization))
	table.vkCmdSetConservativeRasterizationModeEXT = reinterpret_cast<PFN_vkCmdSetConservativeRasterizationModeEXT>(load(device, "vkCmdSetConservativeRasterizationModeEXT"));
	table.vkCmdSetExtraPrimitiveOverestimationSizeEXT = reinterpret_cast<PFN_vkCmdSetExtraPrimitiveOverestimationSizeEXT>(load(device, "vkCmdSetExtraPrimitiveOverestimationSizeEXT"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_conservative_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_conservative_rasterization)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_enable)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_enable))
	table.vkCmdSetDepthClipEnableEXT = reinterpret_cast<PFN_vkCmdSetDepthClipEnableEXT>(load(device, "vkCmdSetDepthClipEnableEXT"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_enable)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_enable)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_sample_locations)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_sample_locations))
	table.vkCmdSetSampleLocationsEnableEXT = reinterpret_cast<PFN_vkCmdSetSampleLocationsEnableEXT>(load(device, "vkCmdSetSampleLocationsEnableEXT"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_sample_locations)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_sample_locations)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_blend_operation_advanced)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_blend_operation_advanced))
	table.vkCmdSetColorBlendAdvancedEXT = reinterpret_cast<PFN_vkCmdSetColorBlendAdvancedEXT>(load(device, "vkCmdSetColorBlendAdvancedEXT"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_blend_operation_advanced)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_blend_operation_advanced)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_provoking_vertex)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_provoking_vertex))
	table.vkCmdSetProvokingVertexModeEXT = reinterpret_cast<PFN_vkCmdSetProvokingVertexModeEXT>(load(device, "vkCmdSetProvokingVertexModeEXT"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_provoking_vertex)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_provoking_vertex)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_line_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_line_rasterization))
	table.vkCmdSetLineRasterizationModeEXT = reinterpret_cast<PFN_vkCmdSetLineRasterizationModeEXT>(load(device, "vkCmdSetLineRasterizationModeEXT"));
	table.vkCmdSetLineStippleEnableEXT = reinterpret_cast<PFN_vkCmdSetLineStippleEnableEXT>(load(device, "vkCmdSetLineStippleEnableEXT"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_line_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_line_rasterization)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_control)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_control))
	table.vkCmdSetDepthClipNegativeOneToOneEXT = reinterpret_cast<PFN_vkCmdSetDepthClipNegativeOneToOneEXT>(load(device, "vkCmdSetDepthClipNegativeOneToOneEXT"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_control)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_control)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_clip_space_w_scaling)) || (defined(VK_EXT_shader_object) && defined(VK_NV_clip_space_w_scaling))
	table.vkCmdSetViewportWScalingEnableNV = reinterpret_cast<PFN_vkCmdSetViewportWScalingEnableNV>(load(device, "vkCmdSetViewportWScalingEnableNV"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_clip_space_w_scaling)) || (defined(VK_EXT_shader_object) && defined(VK_NV_clip_space_w_scaling)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_viewport_swizzle)) || (defined(VK_EXT_shader_object) && defined(VK_NV_viewport_swizzle))
	table.vkCmdSetViewportSwizzleNV = reinterpret_cast<PFN_vkCmdSetViewportSwizzleNV>(load(device, "vkCmdSetViewportSwizzleNV"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_viewport_swizzle)) || (defined(VK_EXT_shader_object) && defined(VK_NV_viewport_swizzle)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_fragment_coverage_to_color)) || (defined(VK_EXT_shader_object) && defined(VK_NV_fragment_coverage_to_color))
	table.vkCmdSetCoverageToColorEnableNV = reinterpret_cast<PFN_vkCmdSetCoverageToColorEnableNV>(load(device, "vkCmdSetCoverageToColorEnableNV"));
	table.vkCmdSetCoverageToColorLocationNV = reinterpret_cast<PFN_vkCmdSetCoverageToColorLocationNV>(load(device, "vkCmdSetCoverageToColorLocationNV"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_fragment_coverage_to_color)) || (defined(VK_EXT_shader_object) && defined(VK_NV_fragment_coverage_to_color)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_framebuffer_mixed_samples)) || (defined(VK_EXT_shader_object) && defined(VK_NV_framebuffer_mixed_samples))
	table.vkCmdSetCoverageModulationModeNV = reinterpret_cast<PFN_vkCmdSetCoverageModulationModeNV>(load(device, "vkCmdSetCoverageModulationModeNV"));
	table.vkCmdSetCoverageModulationTableEnableNV = reinterpret_cast<PFN_vkCmdSetCoverageModulationTableEnableNV>(load(device, "vkCmdSetCoverageModulationTableEnableNV"));
	table.vkCmdSetCoverageModulationTableNV = reinterpret_cast<PFN_vkCmdSetCoverageModulationTableNV>(load(device, "vkCmdSetCoverageModulationTableNV"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_framebuffer_mixed_samples)) || (defined(VK_EXT_shader_object) && defined(VK_NV_framebuffer_mixed_samples)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_shading_rate_image)) || (defined(VK_EXT_shader_object) && defined(VK_NV_shading_rate_image))
	table.vkCmdSetShadingRateImageEnableNV = reinterpret_cast<PFN_vkCmdSetShadingRateImageEnableNV>(load(device, "vkCmdSetShadingRateImageEnableNV"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_shading_rate_image)) || (defined(VK_EXT_shader_object) && defined(VK_NV_shading_rate_image)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_representative_fragment_test)) || (defined(VK_EXT_shader_object) && defined(VK_NV_representative_fragment_test))
	table.vkCmdSetRepresentativeFragmentTestEnableNV = reinterpret_cast<PFN_vkCmdSetRepresentativeFragmentTestEnableNV>(load(device, "vkCmdSetRepresentativeFragmentTestEnableNV"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_representative_fragment_test)) || (defined(VK_EXT_shader_object) && defined(VK_NV_representative_fragment_test)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_coverage_reduction_mode)) || (defined(VK_EXT_shader_object) && defined(VK_NV_coverage_reduction_mode))
	table.vkCmdSetCoverageReductionModeNV = reinterpret_cast<PFN_vkCmdSetCoverageReductionModeNV>(load(device, "vkCmdSetCoverageReductionModeNV"));
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_coverage_reduction_mode)) || (defined(VK_EXT_shader_object) && defined(VK_NV_coverage_reduction_mode)) */
#if (defined(VK_EXT_full_screen_exclusive) && defined(VK_KHR_device_group)) || (defined(VK_EXT_full_screen_exclusive) && defined(VK_VERSION_1_1))
	table.vkGetDeviceGroupSurfacePresentModes2EXT = reinterpret_cast<PFN_vkGetDeviceGroupSurfacePresentModes2EXT>(load(device, "vkGetDeviceGroupSurfacePresentModes2EXT"));
#endif /* (defined(VK_EXT_full_screen_exclusive) && defined(VK_KHR_device_group)) || (defined(VK_EXT_full_screen_exclusive) && defined(VK_VERSION_1_1)) */
#if (defined(VK_EXT_host_image_copy)) || (defined(VK_EXT_image_compression_control))
	table.vkGetImageSubresourceLayout2EXT = reinterpret_cast<PFN_vkGetImageSubresourceLayout2EXT>(load(device, "vkGetImageSubresourceLayout2EXT"));
#endif /* (defined(VK_EXT_host_image_copy)) || (defined(VK_EXT_image_compression_control)) */
#if (defined(VK_EXT_shader_object)) || (defined(VK_EXT_vertex_input_dynamic_state))
	table.vkCmdSetVertexInputEXT = reinterpret_cast<PFN_vkCmdSetVertexInputEXT>(load(device, "vkCmdSetVertexInputEXT"));
#endif /* (defined(VK_EXT_shader_object)) || (defined(VK_EXT_vertex_input_dynamic_state)) */
#if (defined(VK_KHR_descriptor_update_template) && defined(VK_KHR_push_descriptor)) || (defined(VK_KHR_push_descriptor) && defined(VK_VERSION_1_1)) || (defined(VK_KHR_push_descriptor) && defined(VK_KHR_descriptor_update_template))
	table.vkCmdPushDescriptorSetWithTemplateKHR = reinterpret_cast<PFN_vkCmdPushDescriptorSetWithTemplateKHR>(load(device, "vkCmdPushDescriptorSetWithTemplateKHR"));
#endif /* (defined(VK_KHR_descriptor_update_template) && defined(VK_KHR_push_descriptor)) || (defined(VK_KHR_push_descriptor) && defined(VK_VERSION_1_1)) || (defined(VK_KHR_push_descriptor) && defined(VK_KHR_descriptor_update_template)) */
#if (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
	table.vkGetDeviceGroupPresentCapabilitiesKHR = reinterpret_cast<PFN_vkGetDeviceGroupPresentCapabilitiesKHR>(load(device, "vkGetDeviceGroupPresentCapabilitiesKHR"));
	table.vkGetDeviceGroupSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetDeviceGroupSurfacePresentModesKHR>(load(device, "vkGetDeviceGroupSurfacePresentModesKHR"));
#endif /* (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1)) */
#if (defined(VK_KHR_device_group) && defined(VK_KHR_swapchain)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
	table.vkAcquireNextImage2KHR = reinterpret_cast<PFN_vkAcquireNextImage2KHR>(load(device, "vkAcquireNextImage2KHR"));
#endif /* (defined(VK_KHR_device_group) && defined(VK_KHR_swapchain)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1)) */
	/* VULKANLOADER_GENERATE_LOAD_DEVICE_TABLE */
}

#ifdef __GNUC__
    #ifdef VULKANLOADER_DEFAULT_VISIBILITY
        #pragma GCC visibility push(default)
    #else
        #pragma GCC visibility push(hidden)
    #endif
#endif /*__GNUC__*/

/* VULKANLOADER_GENERATE_PROTOTYPES_C */
#if defined(VK_VERSION_1_0)
inline PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers;
inline PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets;
inline PFN_vkAllocateMemory vkAllocateMemory;
inline PFN_vkBeginCommandBuffer vkBeginCommandBuffer;
inline PFN_vkBindBufferMemory vkBindBufferMemory;
inline PFN_vkBindImageMemory vkBindImageMemory;
inline PFN_vkCmdBeginQuery vkCmdBeginQuery;
inline PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass;
inline PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets;
inline PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer;
inline PFN_vkCmdBindPipeline vkCmdBindPipeline;
inline PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers;
inline PFN_vkCmdBlitImage vkCmdBlitImage;
inline PFN_vkCmdClearAttachments vkCmdClearAttachments;
inline PFN_vkCmdClearColorImage vkCmdClearColorImage;
inline PFN_vkCmdClearDepthStencilImage vkCmdClearDepthStencilImage;
inline PFN_vkCmdCopyBuffer vkCmdCopyBuffer;
inline PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage;
inline PFN_vkCmdCopyImage vkCmdCopyImage;
inline PFN_vkCmdCopyImageToBuffer vkCmdCopyImageToBuffer;
inline PFN_vkCmdCopyQueryPoolResults vkCmdCopyQueryPoolResults;
inline PFN_vkCmdDispatch vkCmdDispatch;
inline PFN_vkCmdDispatchIndirect vkCmdDispatchIndirect;
inline PFN_vkCmdDraw vkCmdDraw;
inline PFN_vkCmdDrawIndexed vkCmdDrawIndexed;
inline PFN_vkCmdDrawIndexedIndirect vkCmdDrawIndexedIndirect;
inline PFN_vkCmdDrawIndirect vkCmdDrawIndirect;
inline PFN_vkCmdEndQuery vkCmdEndQuery;
inline PFN_vkCmdEndRenderPass vkCmdEndRenderPass;
inline PFN_vkCmdExecuteCommands vkCmdExecuteCommands;
inline PFN_vkCmdFillBuffer vkCmdFillBuffer;
inline PFN_vkCmdNextSubpass vkCmdNextSubpass;
inline PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier;
inline PFN_vkCmdPushConstants vkCmdPushConstants;
inline PFN_vkCmdResetEvent vkCmdResetEvent;
inline PFN_vkCmdResetQueryPool vkCmdResetQueryPool;
inline PFN_vkCmdResolveImage vkCmdResolveImage;
inline PFN_vkCmdSetBlendConstants vkCmdSetBlendConstants;
inline PFN_vkCmdSetDepthBias vkCmdSetDepthBias;
inline PFN_vkCmdSetDepthBounds vkCmdSetDepthBounds;
inline PFN_vkCmdSetEvent vkCmdSetEvent;
inline PFN_vkCmdSetLineWidth vkCmdSetLineWidth;
inline PFN_vkCmdSetScissor vkCmdSetScissor;
inline PFN_vkCmdSetStencilCompareMask vkCmdSetStencilCompareMask;
inline PFN_vkCmdSetStencilReference vkCmdSetStencilReference;
inline PFN_vkCmdSetStencilWriteMask vkCmdSetStencilWriteMask;
inline PFN_vkCmdSetViewport vkCmdSetViewport;
inline PFN_vkCmdUpdateBuffer vkCmdUpdateBuffer;
inline PFN_vkCmdWaitEvents vkCmdWaitEvents;
inline PFN_vkCmdWriteTimestamp vkCmdWriteTimestamp;
inline PFN_vkCreateBuffer vkCreateBuffer;
inline PFN_vkCreateBufferView vkCreateBufferView;
inline PFN_vkCreateCommandPool vkCreateCommandPool;
inline PFN_vkCreateComputePipelines vkCreateComputePipelines;
inline PFN_vkCreateDescriptorPool vkCreateDescriptorPool;
inline PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout;
inline PFN_vkCreateDevice vkCreateDevice;
inline PFN_vkCreateEvent vkCreateEvent;
inline PFN_vkCreateFence vkCreateFence;
inline PFN_vkCreateFramebuffer vkCreateFramebuffer;
inline PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines;
inline PFN_vkCreateImage vkCreateImage;
inline PFN_vkCreateImageView vkCreateImageView;
inline PFN_vkCreateInstance vkCreateInstance;
inline PFN_vkCreatePipelineCache vkCreatePipelineCache;
inline PFN_vkCreatePipelineLayout vkCreatePipelineLayout;
inline PFN_vkCreateQueryPool vkCreateQueryPool;
inline PFN_vkCreateRenderPass vkCreateRenderPass;
inline PFN_vkCreateSampler vkCreateSampler;
inline PFN_vkCreateSemaphore vkCreateSemaphore;
inline PFN_vkCreateShaderModule vkCreateShaderModule;
inline PFN_vkDestroyBuffer vkDestroyBuffer;
inline PFN_vkDestroyBufferView vkDestroyBufferView;
inline PFN_vkDestroyCommandPool vkDestroyCommandPool;
inline PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool;
inline PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout;
inline PFN_vkDestroyDevice vkDestroyDevice;
inline PFN_vkDestroyEvent vkDestroyEvent;
inline PFN_vkDestroyFence vkDestroyFence;
inline PFN_vkDestroyFramebuffer vkDestroyFramebuffer;
inline PFN_vkDestroyImage vkDestroyImage;
inline PFN_vkDestroyImageView vkDestroyImageView;
inline PFN_vkDestroyInstance vkDestroyInstance;
inline PFN_vkDestroyPipeline vkDestroyPipeline;
inline PFN_vkDestroyPipelineCache vkDestroyPipelineCache;
inline PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout;
inline PFN_vkDestroyQueryPool vkDestroyQueryPool;
inline PFN_vkDestroyRenderPass vkDestroyRenderPass;
inline PFN_vkDestroySampler vkDestroySampler;
inline PFN_vkDestroySemaphore vkDestroySemaphore;
inline PFN_vkDestroyShaderModule vkDestroyShaderModule;
inline PFN_vkDeviceWaitIdle vkDeviceWaitIdle;
inline PFN_vkEndCommandBuffer vkEndCommandBuffer;
inline PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties;
inline PFN_vkEnumerateDeviceLayerProperties vkEnumerateDeviceLayerProperties;
inline PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
inline PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
inline PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
inline PFN_vkFlushMappedMemoryRanges vkFlushMappedMemoryRanges;
inline PFN_vkFreeCommandBuffers vkFreeCommandBuffers;
inline PFN_vkFreeDescriptorSets vkFreeDescriptorSets;
inline PFN_vkFreeMemory vkFreeMemory;
inline PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
inline PFN_vkGetDeviceMemoryCommitment vkGetDeviceMemoryCommitment;
inline PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;
inline PFN_vkGetDeviceQueue vkGetDeviceQueue;
inline PFN_vkGetEventStatus vkGetEventStatus;
inline PFN_vkGetFenceStatus vkGetFenceStatus;
inline PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements;
inline PFN_vkGetImageSparseMemoryRequirements vkGetImageSparseMemoryRequirements;
inline PFN_vkGetImageSubresourceLayout vkGetImageSubresourceLayout;
inline PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
inline PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures;
inline PFN_vkGetPhysicalDeviceFormatProperties vkGetPhysicalDeviceFormatProperties;
inline PFN_vkGetPhysicalDeviceImageFormatProperties vkGetPhysicalDeviceImageFormatProperties;
inline PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties;
inline PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
inline PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
inline PFN_vkGetPhysicalDeviceSparseImageFormatProperties vkGetPhysicalDeviceSparseImageFormatProperties;
inline PFN_vkGetPipelineCacheData vkGetPipelineCacheData;
inline PFN_vkGetQueryPoolResults vkGetQueryPoolResults;
inline PFN_vkGetRenderAreaGranularity vkGetRenderAreaGranularity;
inline PFN_vkInvalidateMappedMemoryRanges vkInvalidateMappedMemoryRanges;
inline PFN_vkMapMemory vkMapMemory;
inline PFN_vkMergePipelineCaches vkMergePipelineCaches;
inline PFN_vkQueueBindSparse vkQueueBindSparse;
inline PFN_vkQueueSubmit vkQueueSubmit;
inline PFN_vkQueueWaitIdle vkQueueWaitIdle;
inline PFN_vkResetCommandBuffer vkResetCommandBuffer;
inline PFN_vkResetCommandPool vkResetCommandPool;
inline PFN_vkResetDescriptorPool vkResetDescriptorPool;
inline PFN_vkResetEvent vkResetEvent;
inline PFN_vkResetFences vkResetFences;
inline PFN_vkSetEvent vkSetEvent;
inline PFN_vkUnmapMemory vkUnmapMemory;
inline PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets;
inline PFN_vkWaitForFences vkWaitForFences;
#endif /* defined(VK_VERSION_1_0) */
#if defined(VK_VERSION_1_1)
inline PFN_vkBindBufferMemory2 vkBindBufferMemory2;
inline PFN_vkBindImageMemory2 vkBindImageMemory2;
inline PFN_vkCmdDispatchBase vkCmdDispatchBase;
inline PFN_vkCmdSetDeviceMask vkCmdSetDeviceMask;
inline PFN_vkCreateDescriptorUpdateTemplate vkCreateDescriptorUpdateTemplate;
inline PFN_vkCreateSamplerYcbcrConversion vkCreateSamplerYcbcrConversion;
inline PFN_vkDestroyDescriptorUpdateTemplate vkDestroyDescriptorUpdateTemplate;
inline PFN_vkDestroySamplerYcbcrConversion vkDestroySamplerYcbcrConversion;
inline PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion;
inline PFN_vkEnumeratePhysicalDeviceGroups vkEnumeratePhysicalDeviceGroups;
inline PFN_vkGetBufferMemoryRequirements2 vkGetBufferMemoryRequirements2;
inline PFN_vkGetDescriptorSetLayoutSupport vkGetDescriptorSetLayoutSupport;
inline PFN_vkGetDeviceGroupPeerMemoryFeatures vkGetDeviceGroupPeerMemoryFeatures;
inline PFN_vkGetDeviceQueue2 vkGetDeviceQueue2;
inline PFN_vkGetImageMemoryRequirements2 vkGetImageMemoryRequirements2;
inline PFN_vkGetImageSparseMemoryRequirements2 vkGetImageSparseMemoryRequirements2;
inline PFN_vkGetPhysicalDeviceExternalBufferProperties vkGetPhysicalDeviceExternalBufferProperties;
inline PFN_vkGetPhysicalDeviceExternalFenceProperties vkGetPhysicalDeviceExternalFenceProperties;
inline PFN_vkGetPhysicalDeviceExternalSemaphoreProperties vkGetPhysicalDeviceExternalSemaphoreProperties;
inline PFN_vkGetPhysicalDeviceFeatures2 vkGetPhysicalDeviceFeatures2;
inline PFN_vkGetPhysicalDeviceFormatProperties2 vkGetPhysicalDeviceFormatProperties2;
inline PFN_vkGetPhysicalDeviceImageFormatProperties2 vkGetPhysicalDeviceImageFormatProperties2;
inline PFN_vkGetPhysicalDeviceMemoryProperties2 vkGetPhysicalDeviceMemoryProperties2;
inline PFN_vkGetPhysicalDeviceProperties2 vkGetPhysicalDeviceProperties2;
inline PFN_vkGetPhysicalDeviceQueueFamilyProperties2 vkGetPhysicalDeviceQueueFamilyProperties2;
inline PFN_vkGetPhysicalDeviceSparseImageFormatProperties2 vkGetPhysicalDeviceSparseImageFormatProperties2;
inline PFN_vkTrimCommandPool vkTrimCommandPool;
inline PFN_vkUpdateDescriptorSetWithTemplate vkUpdateDescriptorSetWithTemplate;
#endif /* defined(VK_VERSION_1_1) */
#if defined(VK_VERSION_1_2)
inline PFN_vkCmdBeginRenderPass2 vkCmdBeginRenderPass2;
inline PFN_vkCmdDrawIndexedIndirectCount vkCmdDrawIndexedIndirectCount;
inline PFN_vkCmdDrawIndirectCount vkCmdDrawIndirectCount;
inline PFN_vkCmdEndRenderPass2 vkCmdEndRenderPass2;
inline PFN_vkCmdNextSubpass2 vkCmdNextSubpass2;
inline PFN_vkCreateRenderPass2 vkCreateRenderPass2;
inline PFN_vkGetBufferDeviceAddress vkGetBufferDeviceAddress;
inline PFN_vkGetBufferOpaqueCaptureAddress vkGetBufferOpaqueCaptureAddress;
inline PFN_vkGetDeviceMemoryOpaqueCaptureAddress vkGetDeviceMemoryOpaqueCaptureAddress;
inline PFN_vkGetSemaphoreCounterValue vkGetSemaphoreCounterValue;
inline PFN_vkResetQueryPool vkResetQueryPool;
inline PFN_vkSignalSemaphore vkSignalSemaphore;
inline PFN_vkWaitSemaphores vkWaitSemaphores;
#endif /* defined(VK_VERSION_1_2) */
#if defined(VK_VERSION_1_3)
inline PFN_vkCmdBeginRendering vkCmdBeginRendering;
inline PFN_vkCmdBindVertexBuffers2 vkCmdBindVertexBuffers2;
inline PFN_vkCmdBlitImage2 vkCmdBlitImage2;
inline PFN_vkCmdCopyBuffer2 vkCmdCopyBuffer2;
inline PFN_vkCmdCopyBufferToImage2 vkCmdCopyBufferToImage2;
inline PFN_vkCmdCopyImage2 vkCmdCopyImage2;
inline PFN_vkCmdCopyImageToBuffer2 vkCmdCopyImageToBuffer2;
inline PFN_vkCmdEndRendering vkCmdEndRendering;
inline PFN_vkCmdPipelineBarrier2 vkCmdPipelineBarrier2;
inline PFN_vkCmdResetEvent2 vkCmdResetEvent2;
inline PFN_vkCmdResolveImage2 vkCmdResolveImage2;
inline PFN_vkCmdSetCullMode vkCmdSetCullMode;
inline PFN_vkCmdSetDepthBiasEnable vkCmdSetDepthBiasEnable;
inline PFN_vkCmdSetDepthBoundsTestEnable vkCmdSetDepthBoundsTestEnable;
inline PFN_vkCmdSetDepthCompareOp vkCmdSetDepthCompareOp;
inline PFN_vkCmdSetDepthTestEnable vkCmdSetDepthTestEnable;
inline PFN_vkCmdSetDepthWriteEnable vkCmdSetDepthWriteEnable;
inline PFN_vkCmdSetEvent2 vkCmdSetEvent2;
inline PFN_vkCmdSetFrontFace vkCmdSetFrontFace;
inline PFN_vkCmdSetPrimitiveRestartEnable vkCmdSetPrimitiveRestartEnable;
inline PFN_vkCmdSetPrimitiveTopology vkCmdSetPrimitiveTopology;
inline PFN_vkCmdSetRasterizerDiscardEnable vkCmdSetRasterizerDiscardEnable;
inline PFN_vkCmdSetScissorWithCount vkCmdSetScissorWithCount;
inline PFN_vkCmdSetStencilOp vkCmdSetStencilOp;
inline PFN_vkCmdSetStencilTestEnable vkCmdSetStencilTestEnable;
inline PFN_vkCmdSetViewportWithCount vkCmdSetViewportWithCount;
inline PFN_vkCmdWaitEvents2 vkCmdWaitEvents2;
inline PFN_vkCmdWriteTimestamp2 vkCmdWriteTimestamp2;
inline PFN_vkCreatePrivateDataSlot vkCreatePrivateDataSlot;
inline PFN_vkDestroyPrivateDataSlot vkDestroyPrivateDataSlot;
inline PFN_vkGetDeviceBufferMemoryRequirements vkGetDeviceBufferMemoryRequirements;
inline PFN_vkGetDeviceImageMemoryRequirements vkGetDeviceImageMemoryRequirements;
inline PFN_vkGetDeviceImageSparseMemoryRequirements vkGetDeviceImageSparseMemoryRequirements;
inline PFN_vkGetPhysicalDeviceToolProperties vkGetPhysicalDeviceToolProperties;
inline PFN_vkGetPrivateData vkGetPrivateData;
inline PFN_vkQueueSubmit2 vkQueueSubmit2;
inline PFN_vkSetPrivateData vkSetPrivateData;
#endif /* defined(VK_VERSION_1_3) */
#if defined(VK_AMDX_shader_enqueue)
inline PFN_vkCmdDispatchGraphAMDX vkCmdDispatchGraphAMDX;
inline PFN_vkCmdDispatchGraphIndirectAMDX vkCmdDispatchGraphIndirectAMDX;
inline PFN_vkCmdDispatchGraphIndirectCountAMDX vkCmdDispatchGraphIndirectCountAMDX;
inline PFN_vkCmdInitializeGraphScratchMemoryAMDX vkCmdInitializeGraphScratchMemoryAMDX;
inline PFN_vkCreateExecutionGraphPipelinesAMDX vkCreateExecutionGraphPipelinesAMDX;
inline PFN_vkGetExecutionGraphPipelineNodeIndexAMDX vkGetExecutionGraphPipelineNodeIndexAMDX;
inline PFN_vkGetExecutionGraphPipelineScratchSizeAMDX vkGetExecutionGraphPipelineScratchSizeAMDX;
#endif /* defined(VK_AMDX_shader_enqueue) */
#if defined(VK_AMD_buffer_marker)
inline PFN_vkCmdWriteBufferMarkerAMD vkCmdWriteBufferMarkerAMD;
#endif /* defined(VK_AMD_buffer_marker) */
#if defined(VK_AMD_display_native_hdr)
inline PFN_vkSetLocalDimmingAMD vkSetLocalDimmingAMD;
#endif /* defined(VK_AMD_display_native_hdr) */
#if defined(VK_AMD_draw_indirect_count)
inline PFN_vkCmdDrawIndexedIndirectCountAMD vkCmdDrawIndexedIndirectCountAMD;
inline PFN_vkCmdDrawIndirectCountAMD vkCmdDrawIndirectCountAMD;
#endif /* defined(VK_AMD_draw_indirect_count) */
#if defined(VK_AMD_shader_info)
inline PFN_vkGetShaderInfoAMD vkGetShaderInfoAMD;
#endif /* defined(VK_AMD_shader_info) */
#if defined(VK_ANDROID_external_memory_android_hardware_buffer)
inline PFN_vkGetAndroidHardwareBufferPropertiesANDROID vkGetAndroidHardwareBufferPropertiesANDROID;
inline PFN_vkGetMemoryAndroidHardwareBufferANDROID vkGetMemoryAndroidHardwareBufferANDROID;
#endif /* defined(VK_ANDROID_external_memory_android_hardware_buffer) */
#if defined(VK_EXT_acquire_drm_display)
inline PFN_vkAcquireDrmDisplayEXT vkAcquireDrmDisplayEXT;
inline PFN_vkGetDrmDisplayEXT vkGetDrmDisplayEXT;
#endif /* defined(VK_EXT_acquire_drm_display) */
#if defined(VK_EXT_acquire_xlib_display)
inline PFN_vkAcquireXlibDisplayEXT vkAcquireXlibDisplayEXT;
inline PFN_vkGetRandROutputDisplayEXT vkGetRandROutputDisplayEXT;
#endif /* defined(VK_EXT_acquire_xlib_display) */
#if defined(VK_EXT_attachment_feedback_loop_dynamic_state)
inline PFN_vkCmdSetAttachmentFeedbackLoopEnableEXT vkCmdSetAttachmentFeedbackLoopEnableEXT;
#endif /* defined(VK_EXT_attachment_feedback_loop_dynamic_state) */
#if defined(VK_EXT_buffer_device_address)
inline PFN_vkGetBufferDeviceAddressEXT vkGetBufferDeviceAddressEXT;
#endif /* defined(VK_EXT_buffer_device_address) */
#if defined(VK_EXT_calibrated_timestamps)
inline PFN_vkGetCalibratedTimestampsEXT vkGetCalibratedTimestampsEXT;
inline PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT vkGetPhysicalDeviceCalibrateableTimeDomainsEXT;
#endif /* defined(VK_EXT_calibrated_timestamps) */
#if defined(VK_EXT_color_write_enable)
inline PFN_vkCmdSetColorWriteEnableEXT vkCmdSetColorWriteEnableEXT;
#endif /* defined(VK_EXT_color_write_enable) */
#if defined(VK_EXT_conditional_rendering)
inline PFN_vkCmdBeginConditionalRenderingEXT vkCmdBeginConditionalRenderingEXT;
inline PFN_vkCmdEndConditionalRenderingEXT vkCmdEndConditionalRenderingEXT;
#endif /* defined(VK_EXT_conditional_rendering) */
#if defined(VK_EXT_debug_marker)
inline PFN_vkCmdDebugMarkerBeginEXT vkCmdDebugMarkerBeginEXT;
inline PFN_vkCmdDebugMarkerEndEXT vkCmdDebugMarkerEndEXT;
inline PFN_vkCmdDebugMarkerInsertEXT vkCmdDebugMarkerInsertEXT;
inline PFN_vkDebugMarkerSetObjectNameEXT vkDebugMarkerSetObjectNameEXT;
inline PFN_vkDebugMarkerSetObjectTagEXT vkDebugMarkerSetObjectTagEXT;
#endif /* defined(VK_EXT_debug_marker) */
#if defined(VK_EXT_debug_report)
inline PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT;
inline PFN_vkDebugReportMessageEXT vkDebugReportMessageEXT;
inline PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT;
#endif /* defined(VK_EXT_debug_report) */
#if defined(VK_EXT_debug_utils)
inline PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT;
inline PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT;
inline PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT;
inline PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
inline PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
inline PFN_vkQueueBeginDebugUtilsLabelEXT vkQueueBeginDebugUtilsLabelEXT;
inline PFN_vkQueueEndDebugUtilsLabelEXT vkQueueEndDebugUtilsLabelEXT;
inline PFN_vkQueueInsertDebugUtilsLabelEXT vkQueueInsertDebugUtilsLabelEXT;
inline PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT;
inline PFN_vkSetDebugUtilsObjectTagEXT vkSetDebugUtilsObjectTagEXT;
inline PFN_vkSubmitDebugUtilsMessageEXT vkSubmitDebugUtilsMessageEXT;
#endif /* defined(VK_EXT_debug_utils) */
#if defined(VK_EXT_depth_bias_control)
inline PFN_vkCmdSetDepthBias2EXT vkCmdSetDepthBias2EXT;
#endif /* defined(VK_EXT_depth_bias_control) */
#if defined(VK_EXT_descriptor_buffer)
inline PFN_vkCmdBindDescriptorBufferEmbeddedSamplersEXT vkCmdBindDescriptorBufferEmbeddedSamplersEXT;
inline PFN_vkCmdBindDescriptorBuffersEXT vkCmdBindDescriptorBuffersEXT;
inline PFN_vkCmdSetDescriptorBufferOffsetsEXT vkCmdSetDescriptorBufferOffsetsEXT;
inline PFN_vkGetBufferOpaqueCaptureDescriptorDataEXT vkGetBufferOpaqueCaptureDescriptorDataEXT;
inline PFN_vkGetDescriptorEXT vkGetDescriptorEXT;
inline PFN_vkGetDescriptorSetLayoutBindingOffsetEXT vkGetDescriptorSetLayoutBindingOffsetEXT;
inline PFN_vkGetDescriptorSetLayoutSizeEXT vkGetDescriptorSetLayoutSizeEXT;
inline PFN_vkGetImageOpaqueCaptureDescriptorDataEXT vkGetImageOpaqueCaptureDescriptorDataEXT;
inline PFN_vkGetImageViewOpaqueCaptureDescriptorDataEXT vkGetImageViewOpaqueCaptureDescriptorDataEXT;
inline PFN_vkGetSamplerOpaqueCaptureDescriptorDataEXT vkGetSamplerOpaqueCaptureDescriptorDataEXT;
#endif /* defined(VK_EXT_descriptor_buffer) */
#if defined(VK_EXT_descriptor_buffer) && (defined(VK_KHR_acceleration_structure) || defined(VK_NV_ray_tracing))
inline PFN_vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT;
#endif /* defined(VK_EXT_descriptor_buffer) && (defined(VK_KHR_acceleration_structure) || defined(VK_NV_ray_tracing)) */
#if defined(VK_EXT_device_fault)
inline PFN_vkGetDeviceFaultInfoEXT vkGetDeviceFaultInfoEXT;
#endif /* defined(VK_EXT_device_fault) */
#if defined(VK_EXT_direct_mode_display)
inline PFN_vkReleaseDisplayEXT vkReleaseDisplayEXT;
#endif /* defined(VK_EXT_direct_mode_display) */
#if defined(VK_EXT_directfb_surface)
inline PFN_vkCreateDirectFBSurfaceEXT vkCreateDirectFBSurfaceEXT;
inline PFN_vkGetPhysicalDeviceDirectFBPresentationSupportEXT vkGetPhysicalDeviceDirectFBPresentationSupportEXT;
#endif /* defined(VK_EXT_directfb_surface) */
#if defined(VK_EXT_discard_rectangles)
inline PFN_vkCmdSetDiscardRectangleEXT vkCmdSetDiscardRectangleEXT;
#endif /* defined(VK_EXT_discard_rectangles) */
#if defined(VK_EXT_discard_rectangles) && VK_EXT_DISCARD_RECTANGLES_SPEC_VERSION >= 2
inline PFN_vkCmdSetDiscardRectangleEnableEXT vkCmdSetDiscardRectangleEnableEXT;
inline PFN_vkCmdSetDiscardRectangleModeEXT vkCmdSetDiscardRectangleModeEXT;
#endif /* defined(VK_EXT_discard_rectangles) && VK_EXT_DISCARD_RECTANGLES_SPEC_VERSION >= 2 */
#if defined(VK_EXT_display_control)
inline PFN_vkDisplayPowerControlEXT vkDisplayPowerControlEXT;
inline PFN_vkGetSwapchainCounterEXT vkGetSwapchainCounterEXT;
inline PFN_vkRegisterDeviceEventEXT vkRegisterDeviceEventEXT;
inline PFN_vkRegisterDisplayEventEXT vkRegisterDisplayEventEXT;
#endif /* defined(VK_EXT_display_control) */
#if defined(VK_EXT_display_surface_counter)
inline PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT vkGetPhysicalDeviceSurfaceCapabilities2EXT;
#endif /* defined(VK_EXT_display_surface_counter) */
#if defined(VK_EXT_external_memory_host)
inline PFN_vkGetMemoryHostPointerPropertiesEXT vkGetMemoryHostPointerPropertiesEXT;
#endif /* defined(VK_EXT_external_memory_host) */
#if defined(VK_EXT_full_screen_exclusive)
inline PFN_vkAcquireFullScreenExclusiveModeEXT vkAcquireFullScreenExclusiveModeEXT;
inline PFN_vkGetPhysicalDeviceSurfacePresentModes2EXT vkGetPhysicalDeviceSurfacePresentModes2EXT;
inline PFN_vkReleaseFullScreenExclusiveModeEXT vkReleaseFullScreenExclusiveModeEXT;
#endif /* defined(VK_EXT_full_screen_exclusive) */
#if defined(VK_EXT_hdr_metadata)
inline PFN_vkSetHdrMetadataEXT vkSetHdrMetadataEXT;
#endif /* defined(VK_EXT_hdr_metadata) */
#if defined(VK_EXT_headless_surface)
inline PFN_vkCreateHeadlessSurfaceEXT vkCreateHeadlessSurfaceEXT;
#endif /* defined(VK_EXT_headless_surface) */
#if defined(VK_EXT_host_image_copy)
inline PFN_vkCopyImageToImageEXT vkCopyImageToImageEXT;
inline PFN_vkCopyImageToMemoryEXT vkCopyImageToMemoryEXT;
inline PFN_vkCopyMemoryToImageEXT vkCopyMemoryToImageEXT;
inline PFN_vkTransitionImageLayoutEXT vkTransitionImageLayoutEXT;
#endif /* defined(VK_EXT_host_image_copy) */
#if defined(VK_EXT_host_query_reset)
inline PFN_vkResetQueryPoolEXT vkResetQueryPoolEXT;
#endif /* defined(VK_EXT_host_query_reset) */
#if defined(VK_EXT_image_drm_format_modifier)
inline PFN_vkGetImageDrmFormatModifierPropertiesEXT vkGetImageDrmFormatModifierPropertiesEXT;
#endif /* defined(VK_EXT_image_drm_format_modifier) */
#if defined(VK_EXT_line_rasterization)
inline PFN_vkCmdSetLineStippleEXT vkCmdSetLineStippleEXT;
#endif /* defined(VK_EXT_line_rasterization) */
#if defined(VK_EXT_mesh_shader)
inline PFN_vkCmdDrawMeshTasksEXT vkCmdDrawMeshTasksEXT;
inline PFN_vkCmdDrawMeshTasksIndirectCountEXT vkCmdDrawMeshTasksIndirectCountEXT;
inline PFN_vkCmdDrawMeshTasksIndirectEXT vkCmdDrawMeshTasksIndirectEXT;
#endif /* defined(VK_EXT_mesh_shader) */
#if defined(VK_EXT_metal_objects)
inline PFN_vkExportMetalObjectsEXT vkExportMetalObjectsEXT;
#endif /* defined(VK_EXT_metal_objects) */
#if defined(VK_EXT_metal_surface)
inline PFN_vkCreateMetalSurfaceEXT vkCreateMetalSurfaceEXT;
#endif /* defined(VK_EXT_metal_surface) */
#if defined(VK_EXT_multi_draw)
inline PFN_vkCmdDrawMultiEXT vkCmdDrawMultiEXT;
inline PFN_vkCmdDrawMultiIndexedEXT vkCmdDrawMultiIndexedEXT;
#endif /* defined(VK_EXT_multi_draw) */
#if defined(VK_EXT_opacity_micromap)
inline PFN_vkBuildMicromapsEXT vkBuildMicromapsEXT;
inline PFN_vkCmdBuildMicromapsEXT vkCmdBuildMicromapsEXT;
inline PFN_vkCmdCopyMemoryToMicromapEXT vkCmdCopyMemoryToMicromapEXT;
inline PFN_vkCmdCopyMicromapEXT vkCmdCopyMicromapEXT;
inline PFN_vkCmdCopyMicromapToMemoryEXT vkCmdCopyMicromapToMemoryEXT;
inline PFN_vkCmdWriteMicromapsPropertiesEXT vkCmdWriteMicromapsPropertiesEXT;
inline PFN_vkCopyMemoryToMicromapEXT vkCopyMemoryToMicromapEXT;
inline PFN_vkCopyMicromapEXT vkCopyMicromapEXT;
inline PFN_vkCopyMicromapToMemoryEXT vkCopyMicromapToMemoryEXT;
inline PFN_vkCreateMicromapEXT vkCreateMicromapEXT;
inline PFN_vkDestroyMicromapEXT vkDestroyMicromapEXT;
inline PFN_vkGetDeviceMicromapCompatibilityEXT vkGetDeviceMicromapCompatibilityEXT;
inline PFN_vkGetMicromapBuildSizesEXT vkGetMicromapBuildSizesEXT;
inline PFN_vkWriteMicromapsPropertiesEXT vkWriteMicromapsPropertiesEXT;
#endif /* defined(VK_EXT_opacity_micromap) */
#if defined(VK_EXT_pageable_device_local_memory)
inline PFN_vkSetDeviceMemoryPriorityEXT vkSetDeviceMemoryPriorityEXT;
#endif /* defined(VK_EXT_pageable_device_local_memory) */
#if defined(VK_EXT_pipeline_properties)
inline PFN_vkGetPipelinePropertiesEXT vkGetPipelinePropertiesEXT;
#endif /* defined(VK_EXT_pipeline_properties) */
#if defined(VK_EXT_private_data)
inline PFN_vkCreatePrivateDataSlotEXT vkCreatePrivateDataSlotEXT;
inline PFN_vkDestroyPrivateDataSlotEXT vkDestroyPrivateDataSlotEXT;
inline PFN_vkGetPrivateDataEXT vkGetPrivateDataEXT;
inline PFN_vkSetPrivateDataEXT vkSetPrivateDataEXT;
#endif /* defined(VK_EXT_private_data) */
#if defined(VK_EXT_sample_locations)
inline PFN_vkCmdSetSampleLocationsEXT vkCmdSetSampleLocationsEXT;
inline PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT vkGetPhysicalDeviceMultisamplePropertiesEXT;
#endif /* defined(VK_EXT_sample_locations) */
#if defined(VK_EXT_shader_module_identifier)
inline PFN_vkGetShaderModuleCreateInfoIdentifierEXT vkGetShaderModuleCreateInfoIdentifierEXT;
inline PFN_vkGetShaderModuleIdentifierEXT vkGetShaderModuleIdentifierEXT;
#endif /* defined(VK_EXT_shader_module_identifier) */
#if defined(VK_EXT_shader_object)
inline PFN_vkCmdBindShadersEXT vkCmdBindShadersEXT;
inline PFN_vkCreateShadersEXT vkCreateShadersEXT;
inline PFN_vkDestroyShaderEXT vkDestroyShaderEXT;
inline PFN_vkGetShaderBinaryDataEXT vkGetShaderBinaryDataEXT;
#endif /* defined(VK_EXT_shader_object) */
#if defined(VK_EXT_swapchain_maintenance1)
inline PFN_vkReleaseSwapchainImagesEXT vkReleaseSwapchainImagesEXT;
#endif /* defined(VK_EXT_swapchain_maintenance1) */
#if defined(VK_EXT_tooling_info)
inline PFN_vkGetPhysicalDeviceToolPropertiesEXT vkGetPhysicalDeviceToolPropertiesEXT;
#endif /* defined(VK_EXT_tooling_info) */
#if defined(VK_EXT_transform_feedback)
inline PFN_vkCmdBeginQueryIndexedEXT vkCmdBeginQueryIndexedEXT;
inline PFN_vkCmdBeginTransformFeedbackEXT vkCmdBeginTransformFeedbackEXT;
inline PFN_vkCmdBindTransformFeedbackBuffersEXT vkCmdBindTransformFeedbackBuffersEXT;
inline PFN_vkCmdDrawIndirectByteCountEXT vkCmdDrawIndirectByteCountEXT;
inline PFN_vkCmdEndQueryIndexedEXT vkCmdEndQueryIndexedEXT;
inline PFN_vkCmdEndTransformFeedbackEXT vkCmdEndTransformFeedbackEXT;
#endif /* defined(VK_EXT_transform_feedback) */
#if defined(VK_EXT_validation_cache)
inline PFN_vkCreateValidationCacheEXT vkCreateValidationCacheEXT;
inline PFN_vkDestroyValidationCacheEXT vkDestroyValidationCacheEXT;
inline PFN_vkGetValidationCacheDataEXT vkGetValidationCacheDataEXT;
inline PFN_vkMergeValidationCachesEXT vkMergeValidationCachesEXT;
#endif /* defined(VK_EXT_validation_cache) */
#if defined(VK_FUCHSIA_buffer_collection)
inline PFN_vkCreateBufferCollectionFUCHSIA vkCreateBufferCollectionFUCHSIA;
inline PFN_vkDestroyBufferCollectionFUCHSIA vkDestroyBufferCollectionFUCHSIA;
inline PFN_vkGetBufferCollectionPropertiesFUCHSIA vkGetBufferCollectionPropertiesFUCHSIA;
inline PFN_vkSetBufferCollectionBufferConstraintsFUCHSIA vkSetBufferCollectionBufferConstraintsFUCHSIA;
inline PFN_vkSetBufferCollectionImageConstraintsFUCHSIA vkSetBufferCollectionImageConstraintsFUCHSIA;
#endif /* defined(VK_FUCHSIA_buffer_collection) */
#if defined(VK_FUCHSIA_external_memory)
inline PFN_vkGetMemoryZirconHandleFUCHSIA vkGetMemoryZirconHandleFUCHSIA;
inline PFN_vkGetMemoryZirconHandlePropertiesFUCHSIA vkGetMemoryZirconHandlePropertiesFUCHSIA;
#endif /* defined(VK_FUCHSIA_external_memory) */
#if defined(VK_FUCHSIA_external_semaphore)
inline PFN_vkGetSemaphoreZirconHandleFUCHSIA vkGetSemaphoreZirconHandleFUCHSIA;
inline PFN_vkImportSemaphoreZirconHandleFUCHSIA vkImportSemaphoreZirconHandleFUCHSIA;
#endif /* defined(VK_FUCHSIA_external_semaphore) */
#if defined(VK_FUCHSIA_imagepipe_surface)
inline PFN_vkCreateImagePipeSurfaceFUCHSIA vkCreateImagePipeSurfaceFUCHSIA;
#endif /* defined(VK_FUCHSIA_imagepipe_surface) */
#if defined(VK_GGP_stream_descriptor_surface)
inline PFN_vkCreateStreamDescriptorSurfaceGGP vkCreateStreamDescriptorSurfaceGGP;
#endif /* defined(VK_GGP_stream_descriptor_surface) */
#if defined(VK_GOOGLE_display_timing)
inline PFN_vkGetPastPresentationTimingGOOGLE vkGetPastPresentationTimingGOOGLE;
inline PFN_vkGetRefreshCycleDurationGOOGLE vkGetRefreshCycleDurationGOOGLE;
#endif /* defined(VK_GOOGLE_display_timing) */
#if defined(VK_HUAWEI_cluster_culling_shader)
inline PFN_vkCmdDrawClusterHUAWEI vkCmdDrawClusterHUAWEI;
inline PFN_vkCmdDrawClusterIndirectHUAWEI vkCmdDrawClusterIndirectHUAWEI;
#endif /* defined(VK_HUAWEI_cluster_culling_shader) */
#if defined(VK_HUAWEI_invocation_mask)
inline PFN_vkCmdBindInvocationMaskHUAWEI vkCmdBindInvocationMaskHUAWEI;
#endif /* defined(VK_HUAWEI_invocation_mask) */
#if defined(VK_HUAWEI_subpass_shading)
inline PFN_vkCmdSubpassShadingHUAWEI vkCmdSubpassShadingHUAWEI;
inline PFN_vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI;
#endif /* defined(VK_HUAWEI_subpass_shading) */
#if defined(VK_INTEL_performance_query)
inline PFN_vkAcquirePerformanceConfigurationINTEL vkAcquirePerformanceConfigurationINTEL;
inline PFN_vkCmdSetPerformanceMarkerINTEL vkCmdSetPerformanceMarkerINTEL;
inline PFN_vkCmdSetPerformanceOverrideINTEL vkCmdSetPerformanceOverrideINTEL;
inline PFN_vkCmdSetPerformanceStreamMarkerINTEL vkCmdSetPerformanceStreamMarkerINTEL;
inline PFN_vkGetPerformanceParameterINTEL vkGetPerformanceParameterINTEL;
inline PFN_vkInitializePerformanceApiINTEL vkInitializePerformanceApiINTEL;
inline PFN_vkQueueSetPerformanceConfigurationINTEL vkQueueSetPerformanceConfigurationINTEL;
inline PFN_vkReleasePerformanceConfigurationINTEL vkReleasePerformanceConfigurationINTEL;
inline PFN_vkUninitializePerformanceApiINTEL vkUninitializePerformanceApiINTEL;
#endif /* defined(VK_INTEL_performance_query) */
#if defined(VK_KHR_acceleration_structure)
inline PFN_vkBuildAccelerationStructuresKHR vkBuildAccelerationStructuresKHR;
inline PFN_vkCmdBuildAccelerationStructuresIndirectKHR vkCmdBuildAccelerationStructuresIndirectKHR;
inline PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR;
inline PFN_vkCmdCopyAccelerationStructureKHR vkCmdCopyAccelerationStructureKHR;
inline PFN_vkCmdCopyAccelerationStructureToMemoryKHR vkCmdCopyAccelerationStructureToMemoryKHR;
inline PFN_vkCmdCopyMemoryToAccelerationStructureKHR vkCmdCopyMemoryToAccelerationStructureKHR;
inline PFN_vkCmdWriteAccelerationStructuresPropertiesKHR vkCmdWriteAccelerationStructuresPropertiesKHR;
inline PFN_vkCopyAccelerationStructureKHR vkCopyAccelerationStructureKHR;
inline PFN_vkCopyAccelerationStructureToMemoryKHR vkCopyAccelerationStructureToMemoryKHR;
inline PFN_vkCopyMemoryToAccelerationStructureKHR vkCopyMemoryToAccelerationStructureKHR;
inline PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR;
inline PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR;
inline PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR;
inline PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR;
inline PFN_vkGetDeviceAccelerationStructureCompatibilityKHR vkGetDeviceAccelerationStructureCompatibilityKHR;
inline PFN_vkWriteAccelerationStructuresPropertiesKHR vkWriteAccelerationStructuresPropertiesKHR;
#endif /* defined(VK_KHR_acceleration_structure) */
#if defined(VK_KHR_android_surface)
inline PFN_vkCreateAndroidSurfaceKHR vkCreateAndroidSurfaceKHR;
#endif /* defined(VK_KHR_android_surface) */
#if defined(VK_KHR_bind_memory2)
inline PFN_vkBindBufferMemory2KHR vkBindBufferMemory2KHR;
inline PFN_vkBindImageMemory2KHR vkBindImageMemory2KHR;
#endif /* defined(VK_KHR_bind_memory2) */
#if defined(VK_KHR_buffer_device_address)
inline PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddressKHR;
inline PFN_vkGetBufferOpaqueCaptureAddressKHR vkGetBufferOpaqueCaptureAddressKHR;
inline PFN_vkGetDeviceMemoryOpaqueCaptureAddressKHR vkGetDeviceMemoryOpaqueCaptureAddressKHR;
#endif /* defined(VK_KHR_buffer_device_address) */
#if defined(VK_KHR_calibrated_timestamps)
inline PFN_vkGetCalibratedTimestampsKHR vkGetCalibratedTimestampsKHR;
inline PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsKHR vkGetPhysicalDeviceCalibrateableTimeDomainsKHR;
#endif /* defined(VK_KHR_calibrated_timestamps) */
#if defined(VK_KHR_cooperative_matrix)
inline PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR;
#endif /* defined(VK_KHR_cooperative_matrix) */
#if defined(VK_KHR_copy_commands2)
inline PFN_vkCmdBlitImage2KHR vkCmdBlitImage2KHR;
inline PFN_vkCmdCopyBuffer2KHR vkCmdCopyBuffer2KHR;
inline PFN_vkCmdCopyBufferToImage2KHR vkCmdCopyBufferToImage2KHR;
inline PFN_vkCmdCopyImage2KHR vkCmdCopyImage2KHR;
inline PFN_vkCmdCopyImageToBuffer2KHR vkCmdCopyImageToBuffer2KHR;
inline PFN_vkCmdResolveImage2KHR vkCmdResolveImage2KHR;
#endif /* defined(VK_KHR_copy_commands2) */
#if defined(VK_KHR_create_renderpass2)
inline PFN_vkCmdBeginRenderPass2KHR vkCmdBeginRenderPass2KHR;
inline PFN_vkCmdEndRenderPass2KHR vkCmdEndRenderPass2KHR;
inline PFN_vkCmdNextSubpass2KHR vkCmdNextSubpass2KHR;
inline PFN_vkCreateRenderPass2KHR vkCreateRenderPass2KHR;
#endif /* defined(VK_KHR_create_renderpass2) */
#if defined(VK_KHR_deferred_host_operations)
inline PFN_vkCreateDeferredOperationKHR vkCreateDeferredOperationKHR;
inline PFN_vkDeferredOperationJoinKHR vkDeferredOperationJoinKHR;
inline PFN_vkDestroyDeferredOperationKHR vkDestroyDeferredOperationKHR;
inline PFN_vkGetDeferredOperationMaxConcurrencyKHR vkGetDeferredOperationMaxConcurrencyKHR;
inline PFN_vkGetDeferredOperationResultKHR vkGetDeferredOperationResultKHR;
#endif /* defined(VK_KHR_deferred_host_operations) */
#if defined(VK_KHR_descriptor_update_template)
inline PFN_vkCreateDescriptorUpdateTemplateKHR vkCreateDescriptorUpdateTemplateKHR;
inline PFN_vkDestroyDescriptorUpdateTemplateKHR vkDestroyDescriptorUpdateTemplateKHR;
inline PFN_vkUpdateDescriptorSetWithTemplateKHR vkUpdateDescriptorSetWithTemplateKHR;
#endif /* defined(VK_KHR_descriptor_update_template) */
#if defined(VK_KHR_device_group)
inline PFN_vkCmdDispatchBaseKHR vkCmdDispatchBaseKHR;
inline PFN_vkCmdSetDeviceMaskKHR vkCmdSetDeviceMaskKHR;
inline PFN_vkGetDeviceGroupPeerMemoryFeaturesKHR vkGetDeviceGroupPeerMemoryFeaturesKHR;
#endif /* defined(VK_KHR_device_group) */
#if defined(VK_KHR_device_group_creation)
inline PFN_vkEnumeratePhysicalDeviceGroupsKHR vkEnumeratePhysicalDeviceGroupsKHR;
#endif /* defined(VK_KHR_device_group_creation) */
#if defined(VK_KHR_display)
inline PFN_vkCreateDisplayModeKHR vkCreateDisplayModeKHR;
inline PFN_vkCreateDisplayPlaneSurfaceKHR vkCreateDisplayPlaneSurfaceKHR;
inline PFN_vkGetDisplayModePropertiesKHR vkGetDisplayModePropertiesKHR;
inline PFN_vkGetDisplayPlaneCapabilitiesKHR vkGetDisplayPlaneCapabilitiesKHR;
inline PFN_vkGetDisplayPlaneSupportedDisplaysKHR vkGetDisplayPlaneSupportedDisplaysKHR;
inline PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR vkGetPhysicalDeviceDisplayPlanePropertiesKHR;
inline PFN_vkGetPhysicalDeviceDisplayPropertiesKHR vkGetPhysicalDeviceDisplayPropertiesKHR;
#endif /* defined(VK_KHR_display) */
#if defined(VK_KHR_display_swapchain)
inline PFN_vkCreateSharedSwapchainsKHR vkCreateSharedSwapchainsKHR;
#endif /* defined(VK_KHR_display_swapchain) */
#if defined(VK_KHR_draw_indirect_count)
inline PFN_vkCmdDrawIndexedIndirectCountKHR vkCmdDrawIndexedIndirectCountKHR;
inline PFN_vkCmdDrawIndirectCountKHR vkCmdDrawIndirectCountKHR;
#endif /* defined(VK_KHR_draw_indirect_count) */
#if defined(VK_KHR_dynamic_rendering)
inline PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR;
inline PFN_vkCmdEndRenderingKHR vkCmdEndRenderingKHR;
#endif /* defined(VK_KHR_dynamic_rendering) */
#if defined(VK_KHR_dynamic_rendering_local_read)
inline PFN_vkCmdSetRenderingAttachmentLocationsKHR vkCmdSetRenderingAttachmentLocationsKHR;
inline PFN_vkCmdSetRenderingInputAttachmentIndicesKHR vkCmdSetRenderingInputAttachmentIndicesKHR;
#endif /* defined(VK_KHR_dynamic_rendering_local_read) */
#if defined(VK_KHR_external_fence_capabilities)
inline PFN_vkGetPhysicalDeviceExternalFencePropertiesKHR vkGetPhysicalDeviceExternalFencePropertiesKHR;
#endif /* defined(VK_KHR_external_fence_capabilities) */
#if defined(VK_KHR_external_fence_fd)
inline PFN_vkGetFenceFdKHR vkGetFenceFdKHR;
inline PFN_vkImportFenceFdKHR vkImportFenceFdKHR;
#endif /* defined(VK_KHR_external_fence_fd) */
#if defined(VK_KHR_external_fence_win32)
inline PFN_vkGetFenceWin32HandleKHR vkGetFenceWin32HandleKHR;
inline PFN_vkImportFenceWin32HandleKHR vkImportFenceWin32HandleKHR;
#endif /* defined(VK_KHR_external_fence_win32) */
#if defined(VK_KHR_external_memory_capabilities)
inline PFN_vkGetPhysicalDeviceExternalBufferPropertiesKHR vkGetPhysicalDeviceExternalBufferPropertiesKHR;
#endif /* defined(VK_KHR_external_memory_capabilities) */
#if defined(VK_KHR_external_memory_fd)
inline PFN_vkGetMemoryFdKHR vkGetMemoryFdKHR;
inline PFN_vkGetMemoryFdPropertiesKHR vkGetMemoryFdPropertiesKHR;
#endif /* defined(VK_KHR_external_memory_fd) */
#if defined(VK_KHR_external_memory_win32)
inline PFN_vkGetMemoryWin32HandleKHR vkGetMemoryWin32HandleKHR;
inline PFN_vkGetMemoryWin32HandlePropertiesKHR vkGetMemoryWin32HandlePropertiesKHR;
#endif /* defined(VK_KHR_external_memory_win32) */
#if defined(VK_KHR_external_semaphore_capabilities)
inline PFN_vkGetPhysicalDeviceExternalSemaphorePropertiesKHR vkGetPhysicalDeviceExternalSemaphorePropertiesKHR;
#endif /* defined(VK_KHR_external_semaphore_capabilities) */
#if defined(VK_KHR_external_semaphore_fd)
inline PFN_vkGetSemaphoreFdKHR vkGetSemaphoreFdKHR;
inline PFN_vkImportSemaphoreFdKHR vkImportSemaphoreFdKHR;
#endif /* defined(VK_KHR_external_semaphore_fd) */
#if defined(VK_KHR_external_semaphore_win32)
inline PFN_vkGetSemaphoreWin32HandleKHR vkGetSemaphoreWin32HandleKHR;
inline PFN_vkImportSemaphoreWin32HandleKHR vkImportSemaphoreWin32HandleKHR;
#endif /* defined(VK_KHR_external_semaphore_win32) */
#if defined(VK_KHR_fragment_shading_rate)
inline PFN_vkCmdSetFragmentShadingRateKHR vkCmdSetFragmentShadingRateKHR;
inline PFN_vkGetPhysicalDeviceFragmentShadingRatesKHR vkGetPhysicalDeviceFragmentShadingRatesKHR;
#endif /* defined(VK_KHR_fragment_shading_rate) */
#if defined(VK_KHR_get_display_properties2)
inline PFN_vkGetDisplayModeProperties2KHR vkGetDisplayModeProperties2KHR;
inline PFN_vkGetDisplayPlaneCapabilities2KHR vkGetDisplayPlaneCapabilities2KHR;
inline PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR vkGetPhysicalDeviceDisplayPlaneProperties2KHR;
inline PFN_vkGetPhysicalDeviceDisplayProperties2KHR vkGetPhysicalDeviceDisplayProperties2KHR;
#endif /* defined(VK_KHR_get_display_properties2) */
#if defined(VK_KHR_get_memory_requirements2)
inline PFN_vkGetBufferMemoryRequirements2KHR vkGetBufferMemoryRequirements2KHR;
inline PFN_vkGetImageMemoryRequirements2KHR vkGetImageMemoryRequirements2KHR;
inline PFN_vkGetImageSparseMemoryRequirements2KHR vkGetImageSparseMemoryRequirements2KHR;
#endif /* defined(VK_KHR_get_memory_requirements2) */
#if defined(VK_KHR_get_physical_device_properties2)
inline PFN_vkGetPhysicalDeviceFeatures2KHR vkGetPhysicalDeviceFeatures2KHR;
inline PFN_vkGetPhysicalDeviceFormatProperties2KHR vkGetPhysicalDeviceFormatProperties2KHR;
inline PFN_vkGetPhysicalDeviceImageFormatProperties2KHR vkGetPhysicalDeviceImageFormatProperties2KHR;
inline PFN_vkGetPhysicalDeviceMemoryProperties2KHR vkGetPhysicalDeviceMemoryProperties2KHR;
inline PFN_vkGetPhysicalDeviceProperties2KHR vkGetPhysicalDeviceProperties2KHR;
inline PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR vkGetPhysicalDeviceQueueFamilyProperties2KHR;
inline PFN_vkGetPhysicalDeviceSparseImageFormatProperties2KHR vkGetPhysicalDeviceSparseImageFormatProperties2KHR;
#endif /* defined(VK_KHR_get_physical_device_properties2) */
#if defined(VK_KHR_get_surface_capabilities2)
inline PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR vkGetPhysicalDeviceSurfaceCapabilities2KHR;
inline PFN_vkGetPhysicalDeviceSurfaceFormats2KHR vkGetPhysicalDeviceSurfaceFormats2KHR;
#endif /* defined(VK_KHR_get_surface_capabilities2) */
#if defined(VK_KHR_line_rasterization)
inline PFN_vkCmdSetLineStippleKHR vkCmdSetLineStippleKHR;
#endif /* defined(VK_KHR_line_rasterization) */
#if defined(VK_KHR_maintenance1)
inline PFN_vkTrimCommandPoolKHR vkTrimCommandPoolKHR;
#endif /* defined(VK_KHR_maintenance1) */
#if defined(VK_KHR_maintenance3)
inline PFN_vkGetDescriptorSetLayoutSupportKHR vkGetDescriptorSetLayoutSupportKHR;
#endif /* defined(VK_KHR_maintenance3) */
#if defined(VK_KHR_maintenance4)
inline PFN_vkGetDeviceBufferMemoryRequirementsKHR vkGetDeviceBufferMemoryRequirementsKHR;
inline PFN_vkGetDeviceImageMemoryRequirementsKHR vkGetDeviceImageMemoryRequirementsKHR;
inline PFN_vkGetDeviceImageSparseMemoryRequirementsKHR vkGetDeviceImageSparseMemoryRequirementsKHR;
#endif /* defined(VK_KHR_maintenance4) */
#if defined(VK_KHR_maintenance5)
inline PFN_vkCmdBindIndexBuffer2KHR vkCmdBindIndexBuffer2KHR;
inline PFN_vkGetDeviceImageSubresourceLayoutKHR vkGetDeviceImageSubresourceLayoutKHR;
inline PFN_vkGetImageSubresourceLayout2KHR vkGetImageSubresourceLayout2KHR;
inline PFN_vkGetRenderingAreaGranularityKHR vkGetRenderingAreaGranularityKHR;
#endif /* defined(VK_KHR_maintenance5) */
#if defined(VK_KHR_maintenance6)
inline PFN_vkCmdBindDescriptorSets2KHR vkCmdBindDescriptorSets2KHR;
inline PFN_vkCmdPushConstants2KHR vkCmdPushConstants2KHR;
#endif /* defined(VK_KHR_maintenance6) */
#if defined(VK_KHR_maintenance6) && defined(VK_KHR_push_descriptor)
inline PFN_vkCmdPushDescriptorSet2KHR vkCmdPushDescriptorSet2KHR;
inline PFN_vkCmdPushDescriptorSetWithTemplate2KHR vkCmdPushDescriptorSetWithTemplate2KHR;
#endif /* defined(VK_KHR_maintenance6) && defined(VK_KHR_push_descriptor) */
#if defined(VK_KHR_maintenance6) && defined(VK_EXT_descriptor_buffer)
inline PFN_vkCmdBindDescriptorBufferEmbeddedSamplers2EXT vkCmdBindDescriptorBufferEmbeddedSamplers2EXT;
inline PFN_vkCmdSetDescriptorBufferOffsets2EXT vkCmdSetDescriptorBufferOffsets2EXT;
#endif /* defined(VK_KHR_maintenance6) && defined(VK_EXT_descriptor_buffer) */
#if defined(VK_KHR_map_memory2)
inline PFN_vkMapMemory2KHR vkMapMemory2KHR;
inline PFN_vkUnmapMemory2KHR vkUnmapMemory2KHR;
#endif /* defined(VK_KHR_map_memory2) */
#if defined(VK_KHR_performance_query)
inline PFN_vkAcquireProfilingLockKHR vkAcquireProfilingLockKHR;
inline PFN_vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR;
inline PFN_vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR;
inline PFN_vkReleaseProfilingLockKHR vkReleaseProfilingLockKHR;
#endif /* defined(VK_KHR_performance_query) */
#if defined(VK_KHR_pipeline_executable_properties)
inline PFN_vkGetPipelineExecutableInternalRepresentationsKHR vkGetPipelineExecutableInternalRepresentationsKHR;
inline PFN_vkGetPipelineExecutablePropertiesKHR vkGetPipelineExecutablePropertiesKHR;
inline PFN_vkGetPipelineExecutableStatisticsKHR vkGetPipelineExecutableStatisticsKHR;
#endif /* defined(VK_KHR_pipeline_executable_properties) */
#if defined(VK_KHR_present_wait)
inline PFN_vkWaitForPresentKHR vkWaitForPresentKHR;
#endif /* defined(VK_KHR_present_wait) */
#if defined(VK_KHR_push_descriptor)
inline PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR;
#endif /* defined(VK_KHR_push_descriptor) */
#if defined(VK_KHR_ray_tracing_maintenance1) && defined(VK_KHR_ray_tracing_pipeline)
inline PFN_vkCmdTraceRaysIndirect2KHR vkCmdTraceRaysIndirect2KHR;
#endif /* defined(VK_KHR_ray_tracing_maintenance1) && defined(VK_KHR_ray_tracing_pipeline) */
#if defined(VK_KHR_ray_tracing_pipeline)
inline PFN_vkCmdSetRayTracingPipelineStackSizeKHR vkCmdSetRayTracingPipelineStackSizeKHR;
inline PFN_vkCmdTraceRaysIndirectKHR vkCmdTraceRaysIndirectKHR;
inline PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR;
inline PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR;
inline PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR vkGetRayTracingCaptureReplayShaderGroupHandlesKHR;
inline PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR;
inline PFN_vkGetRayTracingShaderGroupStackSizeKHR vkGetRayTracingShaderGroupStackSizeKHR;
#endif /* defined(VK_KHR_ray_tracing_pipeline) */
#if defined(VK_KHR_sampler_ycbcr_conversion)
inline PFN_vkCreateSamplerYcbcrConversionKHR vkCreateSamplerYcbcrConversionKHR;
inline PFN_vkDestroySamplerYcbcrConversionKHR vkDestroySamplerYcbcrConversionKHR;
#endif /* defined(VK_KHR_sampler_ycbcr_conversion) */
#if defined(VK_KHR_shared_presentable_image)
inline PFN_vkGetSwapchainStatusKHR vkGetSwapchainStatusKHR;
#endif /* defined(VK_KHR_shared_presentable_image) */
#if defined(VK_KHR_surface)
inline PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;
inline PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
inline PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR;
inline PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;
inline PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;
#endif /* defined(VK_KHR_surface) */
#if defined(VK_KHR_swapchain)
inline PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
inline PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
inline PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;
inline PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
inline PFN_vkQueuePresentKHR vkQueuePresentKHR;
#endif /* defined(VK_KHR_swapchain) */
#if defined(VK_KHR_synchronization2)
inline PFN_vkCmdPipelineBarrier2KHR vkCmdPipelineBarrier2KHR;
inline PFN_vkCmdResetEvent2KHR vkCmdResetEvent2KHR;
inline PFN_vkCmdSetEvent2KHR vkCmdSetEvent2KHR;
inline PFN_vkCmdWaitEvents2KHR vkCmdWaitEvents2KHR;
inline PFN_vkCmdWriteTimestamp2KHR vkCmdWriteTimestamp2KHR;
inline PFN_vkQueueSubmit2KHR vkQueueSubmit2KHR;
#endif /* defined(VK_KHR_synchronization2) */
#if defined(VK_KHR_synchronization2) && defined(VK_AMD_buffer_marker)
inline PFN_vkCmdWriteBufferMarker2AMD vkCmdWriteBufferMarker2AMD;
#endif /* defined(VK_KHR_synchronization2) && defined(VK_AMD_buffer_marker) */
#if defined(VK_KHR_synchronization2) && defined(VK_NV_device_diagnostic_checkpoints)
inline PFN_vkGetQueueCheckpointData2NV vkGetQueueCheckpointData2NV;
#endif /* defined(VK_KHR_synchronization2) && defined(VK_NV_device_diagnostic_checkpoints) */
#if defined(VK_KHR_timeline_semaphore)
inline PFN_vkGetSemaphoreCounterValueKHR vkGetSemaphoreCounterValueKHR;
inline PFN_vkSignalSemaphoreKHR vkSignalSemaphoreKHR;
inline PFN_vkWaitSemaphoresKHR vkWaitSemaphoresKHR;
#endif /* defined(VK_KHR_timeline_semaphore) */
#if defined(VK_KHR_video_decode_queue)
inline PFN_vkCmdDecodeVideoKHR vkCmdDecodeVideoKHR;
#endif /* defined(VK_KHR_video_decode_queue) */
#if defined(VK_KHR_video_encode_queue)
inline PFN_vkCmdEncodeVideoKHR vkCmdEncodeVideoKHR;
inline PFN_vkGetEncodedVideoSessionParametersKHR vkGetEncodedVideoSessionParametersKHR;
inline PFN_vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR;
#endif /* defined(VK_KHR_video_encode_queue) */
#if defined(VK_KHR_video_queue)
inline PFN_vkBindVideoSessionMemoryKHR vkBindVideoSessionMemoryKHR;
inline PFN_vkCmdBeginVideoCodingKHR vkCmdBeginVideoCodingKHR;
inline PFN_vkCmdControlVideoCodingKHR vkCmdControlVideoCodingKHR;
inline PFN_vkCmdEndVideoCodingKHR vkCmdEndVideoCodingKHR;
inline PFN_vkCreateVideoSessionKHR vkCreateVideoSessionKHR;
inline PFN_vkCreateVideoSessionParametersKHR vkCreateVideoSessionParametersKHR;
inline PFN_vkDestroyVideoSessionKHR vkDestroyVideoSessionKHR;
inline PFN_vkDestroyVideoSessionParametersKHR vkDestroyVideoSessionParametersKHR;
inline PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR vkGetPhysicalDeviceVideoCapabilitiesKHR;
inline PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR vkGetPhysicalDeviceVideoFormatPropertiesKHR;
inline PFN_vkGetVideoSessionMemoryRequirementsKHR vkGetVideoSessionMemoryRequirementsKHR;
inline PFN_vkUpdateVideoSessionParametersKHR vkUpdateVideoSessionParametersKHR;
#endif /* defined(VK_KHR_video_queue) */
#if defined(VK_KHR_wayland_surface)
inline PFN_vkCreateWaylandSurfaceKHR vkCreateWaylandSurfaceKHR;
inline PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR vkGetPhysicalDeviceWaylandPresentationSupportKHR;
#endif /* defined(VK_KHR_wayland_surface) */
#if defined(VK_KHR_win32_surface)
inline PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR;
inline PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR vkGetPhysicalDeviceWin32PresentationSupportKHR;
#endif /* defined(VK_KHR_win32_surface) */
#if defined(VK_KHR_xcb_surface)
inline PFN_vkCreateXcbSurfaceKHR vkCreateXcbSurfaceKHR;
inline PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR vkGetPhysicalDeviceXcbPresentationSupportKHR;
#endif /* defined(VK_KHR_xcb_surface) */
#if defined(VK_KHR_xlib_surface)
inline PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR;
inline PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR vkGetPhysicalDeviceXlibPresentationSupportKHR;
#endif /* defined(VK_KHR_xlib_surface) */
#if defined(VK_MVK_ios_surface)
inline PFN_vkCreateIOSSurfaceMVK vkCreateIOSSurfaceMVK;
#endif /* defined(VK_MVK_ios_surface) */
#if defined(VK_MVK_macos_surface)
inline PFN_vkCreateMacOSSurfaceMVK vkCreateMacOSSurfaceMVK;
#endif /* defined(VK_MVK_macos_surface) */
#if defined(VK_NN_vi_surface)
inline PFN_vkCreateViSurfaceNN vkCreateViSurfaceNN;
#endif /* defined(VK_NN_vi_surface) */
#if defined(VK_NVX_binary_import)
inline PFN_vkCmdCuLaunchKernelNVX vkCmdCuLaunchKernelNVX;
inline PFN_vkCreateCuFunctionNVX vkCreateCuFunctionNVX;
inline PFN_vkCreateCuModuleNVX vkCreateCuModuleNVX;
inline PFN_vkDestroyCuFunctionNVX vkDestroyCuFunctionNVX;
inline PFN_vkDestroyCuModuleNVX vkDestroyCuModuleNVX;
#endif /* defined(VK_NVX_binary_import) */
#if defined(VK_NVX_image_view_handle)
inline PFN_vkGetImageViewAddressNVX vkGetImageViewAddressNVX;
inline PFN_vkGetImageViewHandleNVX vkGetImageViewHandleNVX;
#endif /* defined(VK_NVX_image_view_handle) */
#if defined(VK_NV_acquire_winrt_display)
inline PFN_vkAcquireWinrtDisplayNV vkAcquireWinrtDisplayNV;
inline PFN_vkGetWinrtDisplayNV vkGetWinrtDisplayNV;
#endif /* defined(VK_NV_acquire_winrt_display) */
#if defined(VK_NV_clip_space_w_scaling)
inline PFN_vkCmdSetViewportWScalingNV vkCmdSetViewportWScalingNV;
#endif /* defined(VK_NV_clip_space_w_scaling) */
#if defined(VK_NV_cooperative_matrix)
inline PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV vkGetPhysicalDeviceCooperativeMatrixPropertiesNV;
#endif /* defined(VK_NV_cooperative_matrix) */
#if defined(VK_NV_copy_memory_indirect)
inline PFN_vkCmdCopyMemoryIndirectNV vkCmdCopyMemoryIndirectNV;
inline PFN_vkCmdCopyMemoryToImageIndirectNV vkCmdCopyMemoryToImageIndirectNV;
#endif /* defined(VK_NV_copy_memory_indirect) */
#if defined(VK_NV_coverage_reduction_mode)
inline PFN_vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV;
#endif /* defined(VK_NV_coverage_reduction_mode) */
#if defined(VK_NV_cuda_kernel_launch)
inline PFN_vkCmdCudaLaunchKernelNV vkCmdCudaLaunchKernelNV;
inline PFN_vkCreateCudaFunctionNV vkCreateCudaFunctionNV;
inline PFN_vkCreateCudaModuleNV vkCreateCudaModuleNV;
inline PFN_vkDestroyCudaFunctionNV vkDestroyCudaFunctionNV;
inline PFN_vkDestroyCudaModuleNV vkDestroyCudaModuleNV;
inline PFN_vkGetCudaModuleCacheNV vkGetCudaModuleCacheNV;
#endif /* defined(VK_NV_cuda_kernel_launch) */
#if defined(VK_NV_device_diagnostic_checkpoints)
inline PFN_vkCmdSetCheckpointNV vkCmdSetCheckpointNV;
inline PFN_vkGetQueueCheckpointDataNV vkGetQueueCheckpointDataNV;
#endif /* defined(VK_NV_device_diagnostic_checkpoints) */
#if defined(VK_NV_device_generated_commands)
inline PFN_vkCmdBindPipelineShaderGroupNV vkCmdBindPipelineShaderGroupNV;
inline PFN_vkCmdExecuteGeneratedCommandsNV vkCmdExecuteGeneratedCommandsNV;
inline PFN_vkCmdPreprocessGeneratedCommandsNV vkCmdPreprocessGeneratedCommandsNV;
inline PFN_vkCreateIndirectCommandsLayoutNV vkCreateIndirectCommandsLayoutNV;
inline PFN_vkDestroyIndirectCommandsLayoutNV vkDestroyIndirectCommandsLayoutNV;
inline PFN_vkGetGeneratedCommandsMemoryRequirementsNV vkGetGeneratedCommandsMemoryRequirementsNV;
#endif /* defined(VK_NV_device_generated_commands) */
#if defined(VK_NV_device_generated_commands_compute)
inline PFN_vkCmdUpdatePipelineIndirectBufferNV vkCmdUpdatePipelineIndirectBufferNV;
inline PFN_vkGetPipelineIndirectDeviceAddressNV vkGetPipelineIndirectDeviceAddressNV;
inline PFN_vkGetPipelineIndirectMemoryRequirementsNV vkGetPipelineIndirectMemoryRequirementsNV;
#endif /* defined(VK_NV_device_generated_commands_compute) */
#if defined(VK_NV_external_memory_capabilities)
inline PFN_vkGetPhysicalDeviceExternalImageFormatPropertiesNV vkGetPhysicalDeviceExternalImageFormatPropertiesNV;
#endif /* defined(VK_NV_external_memory_capabilities) */
#if defined(VK_NV_external_memory_rdma)
inline PFN_vkGetMemoryRemoteAddressNV vkGetMemoryRemoteAddressNV;
#endif /* defined(VK_NV_external_memory_rdma) */
#if defined(VK_NV_external_memory_win32)
inline PFN_vkGetMemoryWin32HandleNV vkGetMemoryWin32HandleNV;
#endif /* defined(VK_NV_external_memory_win32) */
#if defined(VK_NV_fragment_shading_rate_enums)
inline PFN_vkCmdSetFragmentShadingRateEnumNV vkCmdSetFragmentShadingRateEnumNV;
#endif /* defined(VK_NV_fragment_shading_rate_enums) */
#if defined(VK_NV_low_latency2)
inline PFN_vkGetLatencyTimingsNV vkGetLatencyTimingsNV;
inline PFN_vkLatencySleepNV vkLatencySleepNV;
inline PFN_vkQueueNotifyOutOfBandNV vkQueueNotifyOutOfBandNV;
inline PFN_vkSetLatencyMarkerNV vkSetLatencyMarkerNV;
inline PFN_vkSetLatencySleepModeNV vkSetLatencySleepModeNV;
#endif /* defined(VK_NV_low_latency2) */
#if defined(VK_NV_memory_decompression)
inline PFN_vkCmdDecompressMemoryIndirectCountNV vkCmdDecompressMemoryIndirectCountNV;
inline PFN_vkCmdDecompressMemoryNV vkCmdDecompressMemoryNV;
#endif /* defined(VK_NV_memory_decompression) */
#if defined(VK_NV_mesh_shader)
inline PFN_vkCmdDrawMeshTasksIndirectCountNV vkCmdDrawMeshTasksIndirectCountNV;
inline PFN_vkCmdDrawMeshTasksIndirectNV vkCmdDrawMeshTasksIndirectNV;
inline PFN_vkCmdDrawMeshTasksNV vkCmdDrawMeshTasksNV;
#endif /* defined(VK_NV_mesh_shader) */
#if defined(VK_NV_optical_flow)
inline PFN_vkBindOpticalFlowSessionImageNV vkBindOpticalFlowSessionImageNV;
inline PFN_vkCmdOpticalFlowExecuteNV vkCmdOpticalFlowExecuteNV;
inline PFN_vkCreateOpticalFlowSessionNV vkCreateOpticalFlowSessionNV;
inline PFN_vkDestroyOpticalFlowSessionNV vkDestroyOpticalFlowSessionNV;
inline PFN_vkGetPhysicalDeviceOpticalFlowImageFormatsNV vkGetPhysicalDeviceOpticalFlowImageFormatsNV;
#endif /* defined(VK_NV_optical_flow) */
#if defined(VK_NV_ray_tracing)
inline PFN_vkBindAccelerationStructureMemoryNV vkBindAccelerationStructureMemoryNV;
inline PFN_vkCmdBuildAccelerationStructureNV vkCmdBuildAccelerationStructureNV;
inline PFN_vkCmdCopyAccelerationStructureNV vkCmdCopyAccelerationStructureNV;
inline PFN_vkCmdTraceRaysNV vkCmdTraceRaysNV;
inline PFN_vkCmdWriteAccelerationStructuresPropertiesNV vkCmdWriteAccelerationStructuresPropertiesNV;
inline PFN_vkCompileDeferredNV vkCompileDeferredNV;
inline PFN_vkCreateAccelerationStructureNV vkCreateAccelerationStructureNV;
inline PFN_vkCreateRayTracingPipelinesNV vkCreateRayTracingPipelinesNV;
inline PFN_vkDestroyAccelerationStructureNV vkDestroyAccelerationStructureNV;
inline PFN_vkGetAccelerationStructureHandleNV vkGetAccelerationStructureHandleNV;
inline PFN_vkGetAccelerationStructureMemoryRequirementsNV vkGetAccelerationStructureMemoryRequirementsNV;
inline PFN_vkGetRayTracingShaderGroupHandlesNV vkGetRayTracingShaderGroupHandlesNV;
#endif /* defined(VK_NV_ray_tracing) */
#if defined(VK_NV_scissor_exclusive) && VK_NV_SCISSOR_EXCLUSIVE_SPEC_VERSION >= 2
inline PFN_vkCmdSetExclusiveScissorEnableNV vkCmdSetExclusiveScissorEnableNV;
#endif /* defined(VK_NV_scissor_exclusive) && VK_NV_SCISSOR_EXCLUSIVE_SPEC_VERSION >= 2 */
#if defined(VK_NV_scissor_exclusive)
inline PFN_vkCmdSetExclusiveScissorNV vkCmdSetExclusiveScissorNV;
#endif /* defined(VK_NV_scissor_exclusive) */
#if defined(VK_NV_shading_rate_image)
inline PFN_vkCmdBindShadingRateImageNV vkCmdBindShadingRateImageNV;
inline PFN_vkCmdSetCoarseSampleOrderNV vkCmdSetCoarseSampleOrderNV;
inline PFN_vkCmdSetViewportShadingRatePaletteNV vkCmdSetViewportShadingRatePaletteNV;
#endif /* defined(VK_NV_shading_rate_image) */
#if defined(VK_QCOM_tile_properties)
inline PFN_vkGetDynamicRenderingTilePropertiesQCOM vkGetDynamicRenderingTilePropertiesQCOM;
inline PFN_vkGetFramebufferTilePropertiesQCOM vkGetFramebufferTilePropertiesQCOM;
#endif /* defined(VK_QCOM_tile_properties) */
#if defined(VK_QNX_external_memory_screen_buffer)
inline PFN_vkGetScreenBufferPropertiesQNX vkGetScreenBufferPropertiesQNX;
#endif /* defined(VK_QNX_external_memory_screen_buffer) */
#if defined(VK_QNX_screen_surface)
inline PFN_vkCreateScreenSurfaceQNX vkCreateScreenSurfaceQNX;
inline PFN_vkGetPhysicalDeviceScreenPresentationSupportQNX vkGetPhysicalDeviceScreenPresentationSupportQNX;
#endif /* defined(VK_QNX_screen_surface) */
#if defined(VK_VALVE_descriptor_set_host_mapping)
inline PFN_vkGetDescriptorSetHostMappingVALVE vkGetDescriptorSetHostMappingVALVE;
inline PFN_vkGetDescriptorSetLayoutHostMappingInfoVALVE vkGetDescriptorSetLayoutHostMappingInfoVALVE;
#endif /* defined(VK_VALVE_descriptor_set_host_mapping) */
#if (defined(VK_EXT_extended_dynamic_state)) || (defined(VK_EXT_shader_object))
inline PFN_vkCmdBindVertexBuffers2EXT vkCmdBindVertexBuffers2EXT;
inline PFN_vkCmdSetCullModeEXT vkCmdSetCullModeEXT;
inline PFN_vkCmdSetDepthBoundsTestEnableEXT vkCmdSetDepthBoundsTestEnableEXT;
inline PFN_vkCmdSetDepthCompareOpEXT vkCmdSetDepthCompareOpEXT;
inline PFN_vkCmdSetDepthTestEnableEXT vkCmdSetDepthTestEnableEXT;
inline PFN_vkCmdSetDepthWriteEnableEXT vkCmdSetDepthWriteEnableEXT;
inline PFN_vkCmdSetFrontFaceEXT vkCmdSetFrontFaceEXT;
inline PFN_vkCmdSetPrimitiveTopologyEXT vkCmdSetPrimitiveTopologyEXT;
inline PFN_vkCmdSetScissorWithCountEXT vkCmdSetScissorWithCountEXT;
inline PFN_vkCmdSetStencilOpEXT vkCmdSetStencilOpEXT;
inline PFN_vkCmdSetStencilTestEnableEXT vkCmdSetStencilTestEnableEXT;
inline PFN_vkCmdSetViewportWithCountEXT vkCmdSetViewportWithCountEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state)) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state2)) || (defined(VK_EXT_shader_object))
inline PFN_vkCmdSetDepthBiasEnableEXT vkCmdSetDepthBiasEnableEXT;
inline PFN_vkCmdSetLogicOpEXT vkCmdSetLogicOpEXT;
inline PFN_vkCmdSetPatchControlPointsEXT vkCmdSetPatchControlPointsEXT;
inline PFN_vkCmdSetPrimitiveRestartEnableEXT vkCmdSetPrimitiveRestartEnableEXT;
inline PFN_vkCmdSetRasterizerDiscardEnableEXT vkCmdSetRasterizerDiscardEnableEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state2)) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state3)) || (defined(VK_EXT_shader_object))
inline PFN_vkCmdSetAlphaToCoverageEnableEXT vkCmdSetAlphaToCoverageEnableEXT;
inline PFN_vkCmdSetAlphaToOneEnableEXT vkCmdSetAlphaToOneEnableEXT;
inline PFN_vkCmdSetColorBlendEnableEXT vkCmdSetColorBlendEnableEXT;
inline PFN_vkCmdSetColorBlendEquationEXT vkCmdSetColorBlendEquationEXT;
inline PFN_vkCmdSetColorWriteMaskEXT vkCmdSetColorWriteMaskEXT;
inline PFN_vkCmdSetDepthClampEnableEXT vkCmdSetDepthClampEnableEXT;
inline PFN_vkCmdSetLogicOpEnableEXT vkCmdSetLogicOpEnableEXT;
inline PFN_vkCmdSetPolygonModeEXT vkCmdSetPolygonModeEXT;
inline PFN_vkCmdSetRasterizationSamplesEXT vkCmdSetRasterizationSamplesEXT;
inline PFN_vkCmdSetSampleMaskEXT vkCmdSetSampleMaskEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3)) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state3) && (defined(VK_KHR_maintenance2) || defined(VK_VERSION_1_1))) || (defined(VK_EXT_shader_object))
inline PFN_vkCmdSetTessellationDomainOriginEXT vkCmdSetTessellationDomainOriginEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && (defined(VK_KHR_maintenance2) || defined(VK_VERSION_1_1))) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_transform_feedback)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_transform_feedback))
inline PFN_vkCmdSetRasterizationStreamEXT vkCmdSetRasterizationStreamEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_transform_feedback)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_transform_feedback)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_conservative_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_conservative_rasterization))
inline PFN_vkCmdSetConservativeRasterizationModeEXT vkCmdSetConservativeRasterizationModeEXT;
inline PFN_vkCmdSetExtraPrimitiveOverestimationSizeEXT vkCmdSetExtraPrimitiveOverestimationSizeEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_conservative_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_conservative_rasterization)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_enable)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_enable))
inline PFN_vkCmdSetDepthClipEnableEXT vkCmdSetDepthClipEnableEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_enable)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_enable)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_sample_locations)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_sample_locations))
inline PFN_vkCmdSetSampleLocationsEnableEXT vkCmdSetSampleLocationsEnableEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_sample_locations)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_sample_locations)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_blend_operation_advanced)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_blend_operation_advanced))
inline PFN_vkCmdSetColorBlendAdvancedEXT vkCmdSetColorBlendAdvancedEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_blend_operation_advanced)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_blend_operation_advanced)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_provoking_vertex)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_provoking_vertex))
inline PFN_vkCmdSetProvokingVertexModeEXT vkCmdSetProvokingVertexModeEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_provoking_vertex)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_provoking_vertex)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_line_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_line_rasterization))
inline PFN_vkCmdSetLineRasterizationModeEXT vkCmdSetLineRasterizationModeEXT;
inline PFN_vkCmdSetLineStippleEnableEXT vkCmdSetLineStippleEnableEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_line_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_line_rasterization)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_control)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_control))
inline PFN_vkCmdSetDepthClipNegativeOneToOneEXT vkCmdSetDepthClipNegativeOneToOneEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_control)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_control)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_clip_space_w_scaling)) || (defined(VK_EXT_shader_object) && defined(VK_NV_clip_space_w_scaling))
inline PFN_vkCmdSetViewportWScalingEnableNV vkCmdSetViewportWScalingEnableNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_clip_space_w_scaling)) || (defined(VK_EXT_shader_object) && defined(VK_NV_clip_space_w_scaling)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_viewport_swizzle)) || (defined(VK_EXT_shader_object) && defined(VK_NV_viewport_swizzle))
inline PFN_vkCmdSetViewportSwizzleNV vkCmdSetViewportSwizzleNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_viewport_swizzle)) || (defined(VK_EXT_shader_object) && defined(VK_NV_viewport_swizzle)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_fragment_coverage_to_color)) || (defined(VK_EXT_shader_object) && defined(VK_NV_fragment_coverage_to_color))
inline PFN_vkCmdSetCoverageToColorEnableNV vkCmdSetCoverageToColorEnableNV;
inline PFN_vkCmdSetCoverageToColorLocationNV vkCmdSetCoverageToColorLocationNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_fragment_coverage_to_color)) || (defined(VK_EXT_shader_object) && defined(VK_NV_fragment_coverage_to_color)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_framebuffer_mixed_samples)) || (defined(VK_EXT_shader_object) && defined(VK_NV_framebuffer_mixed_samples))
inline PFN_vkCmdSetCoverageModulationModeNV vkCmdSetCoverageModulationModeNV;
inline PFN_vkCmdSetCoverageModulationTableEnableNV vkCmdSetCoverageModulationTableEnableNV;
inline PFN_vkCmdSetCoverageModulationTableNV vkCmdSetCoverageModulationTableNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_framebuffer_mixed_samples)) || (defined(VK_EXT_shader_object) && defined(VK_NV_framebuffer_mixed_samples)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_shading_rate_image)) || (defined(VK_EXT_shader_object) && defined(VK_NV_shading_rate_image))
inline PFN_vkCmdSetShadingRateImageEnableNV vkCmdSetShadingRateImageEnableNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_shading_rate_image)) || (defined(VK_EXT_shader_object) && defined(VK_NV_shading_rate_image)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_representative_fragment_test)) || (defined(VK_EXT_shader_object) && defined(VK_NV_representative_fragment_test))
inline PFN_vkCmdSetRepresentativeFragmentTestEnableNV vkCmdSetRepresentativeFragmentTestEnableNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_representative_fragment_test)) || (defined(VK_EXT_shader_object) && defined(VK_NV_representative_fragment_test)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_coverage_reduction_mode)) || (defined(VK_EXT_shader_object) && defined(VK_NV_coverage_reduction_mode))
inline PFN_vkCmdSetCoverageReductionModeNV vkCmdSetCoverageReductionModeNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_coverage_reduction_mode)) || (defined(VK_EXT_shader_object) && defined(VK_NV_coverage_reduction_mode)) */
#if (defined(VK_EXT_full_screen_exclusive) && defined(VK_KHR_device_group)) || (defined(VK_EXT_full_screen_exclusive) && defined(VK_VERSION_1_1))
inline PFN_vkGetDeviceGroupSurfacePresentModes2EXT vkGetDeviceGroupSurfacePresentModes2EXT;
#endif /* (defined(VK_EXT_full_screen_exclusive) && defined(VK_KHR_device_group)) || (defined(VK_EXT_full_screen_exclusive) && defined(VK_VERSION_1_1)) */
#if (defined(VK_EXT_host_image_copy)) || (defined(VK_EXT_image_compression_control))
inline PFN_vkGetImageSubresourceLayout2EXT vkGetImageSubresourceLayout2EXT;
#endif /* (defined(VK_EXT_host_image_copy)) || (defined(VK_EXT_image_compression_control)) */
#if (defined(VK_EXT_shader_object)) || (defined(VK_EXT_vertex_input_dynamic_state))
inline PFN_vkCmdSetVertexInputEXT vkCmdSetVertexInputEXT;
#endif /* (defined(VK_EXT_shader_object)) || (defined(VK_EXT_vertex_input_dynamic_state)) */
#if (defined(VK_KHR_descriptor_update_template) && defined(VK_KHR_push_descriptor)) || (defined(VK_KHR_push_descriptor) && defined(VK_VERSION_1_1)) || (defined(VK_KHR_push_descriptor) && defined(VK_KHR_descriptor_update_template))
inline PFN_vkCmdPushDescriptorSetWithTemplateKHR vkCmdPushDescriptorSetWithTemplateKHR;
#endif /* (defined(VK_KHR_descriptor_update_template) && defined(VK_KHR_push_descriptor)) || (defined(VK_KHR_push_descriptor) && defined(VK_VERSION_1_1)) || (defined(VK_KHR_push_descriptor) && defined(VK_KHR_descriptor_update_template)) */
#if (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
inline PFN_vkGetDeviceGroupPresentCapabilitiesKHR vkGetDeviceGroupPresentCapabilitiesKHR;
inline PFN_vkGetDeviceGroupSurfacePresentModesKHR vkGetDeviceGroupSurfacePresentModesKHR;
inline PFN_vkGetPhysicalDevicePresentRectanglesKHR vkGetPhysicalDevicePresentRectanglesKHR;
#endif /* (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1)) */
#if (defined(VK_KHR_device_group) && defined(VK_KHR_swapchain)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
inline PFN_vkAcquireNextImage2KHR vkAcquireNextImage2KHR;
#endif /* (defined(VK_KHR_device_group) && defined(VK_KHR_swapchain)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1)) */
/* VULKANLOADER_GENERATE_PROTOTYPES_C */

#ifdef __GNUC__
    #pragma GCC visibility pop
#endif /*__GNUC__*/

#ifdef _MSC_VER
	#pragma warning(pop)
#endif /*_MSC_VER*/

#endif /*VULKANLOADER_IMPLEMENTATION*/

/*
Copyright (c) 2018-2023 Arseny Kapoulkine

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Modified by: Jan "GamesTrap" Schuerkamp
*/
