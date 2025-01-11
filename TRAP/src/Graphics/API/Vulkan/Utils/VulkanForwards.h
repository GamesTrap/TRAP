#ifndef TRAP_VULKANFORWARDS_H
#define TRAP_VULKANFORWARDS_H

#include <cstdint>

using VkFlags = std::uint32_t;
using VkQueueFlags = VkFlags;
using VkImageUsageFlags = VkFlags;

using VkCommandPool = struct VkCommandPool_T*;
using VkCommandBuffer = struct VkCommandBuffer_T*;
using VkRenderPass = struct VkRenderPass_T*;
using VkPipelineLayout = struct VkPipelineLayout_T*;
using VkSampler = struct VkSampler_T*;
using VkSamplerYcbcrConversion = struct VkSamplerYcbcrConversion_T*;
using VkImageView = struct VkImageView_T*;
using VkDebugUtilsMessengerEXT = struct VkDebugUtilsMessengerEXT_T*;
using VkDebugReportCallbackEXT = struct VkDebugReportCallbackEXT_T*;
using VkDescriptorPool = struct VkDescriptorPool_T*;
using VkDescriptorSet = struct VkDescriptorSet_T*;
using VkDescriptorSetLayout = struct VkDescriptorSetLayout_T*;
using VkDevice = struct VkDevice_T*;
using VkInstance = struct VkInstance_T*;
using VkPipeline = struct VkPipeline_T*;
using VkPipelineCache = struct VkPipelineCache_T*;
using VkQueryPool = struct VkQueryPool_T*;
using VkSwapchainKHR = struct VkSwapchainKHR_T*;
using VkQueue = struct VkQueue_T*;
using VkPhysicalDevice = struct VkPhysicalDevice_T*;
using VkShaderModule = struct VkShaderModule_T*;
using VkBuffer = struct VkBuffer_T*;
using VkBufferView = struct VkBufferView_T*;
using VmaAllocation = struct VmaAllocation_T*;

struct VkDescriptorPoolSize;
struct VkImageBlit;
struct VkQueueFamilyProperties;
struct VkLayerProperties;
struct VkExtensionProperties;
struct VkFormatProperties;
struct VkImageFormatProperties;
struct VkPhysicalDeviceProperties;
struct VkPhysicalDeviceSubgroupProperties;
struct VkPhysicalDeviceIDProperties;
struct VkPhysicalDeviceMemoryProperties;
struct VkPhysicalDeviceFeatures;
struct VkPhysicalDeviceDriverProperties;
struct VmaVulkanFunctions;
struct VmaAllocatorCreateInfo;

#endif /*TRAP_VULKANFORWARDS_H*/
