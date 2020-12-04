#ifndef _TRAP_VULKANINITS_H_
#define _TRAP_VULKANINITS_H_

#include "VulkanMemoryAllocator.h"

namespace TRAP::Graphics::API::VulkanInits
{
	VkApplicationInfo ApplicationInfo(const std::string& appName);
	
	VkInstanceCreateInfo InstanceCreateInfo(const VkApplicationInfo& appInfo, 
									const std::vector<const char*>& instanceLayers,
	                                const std::vector<const char*>& instanceExtensions);
	
	//-------------------------------------------------------------------------------------------------------------------//

	VkDebugUtilsMessengerCreateInfoEXT DebugUtilsMessengerCreateInfo(PFN_vkDebugUtilsMessengerCallbackEXT callback);

	VkDebugUtilsObjectNameInfoEXT DebugUtilsObjectNameInfo(VkObjectType type, uint64_t handle, const char* name);
	
	//-------------------------------------------------------------------------------------------------------------------//

	VkDeviceCreateInfo DeviceCreateInfo(const void* pNext,
	                                    const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos,
	                                    const std::vector<const char*>& deviceExtensions);
	
	//-------------------------------------------------------------------------------------------------------------------//
	
	VmaAllocatorCreateInfo VMAAllocatorCreateInfo(VkDevice device,
	                                              VkPhysicalDevice physicalDevice,
	                                              VkInstance instance,
	                                              const VmaVulkanFunctions& vulkanFunctions);

	//-------------------------------------------------------------------------------------------------------------------//

	VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo(const std::vector<VkDescriptorPoolSize>& descriptorPoolSizes,
	                                                    uint32_t numDescriptorSets);

	VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo(VkDescriptorPool descriptorPool,
														  const VkDescriptorSetLayout& descriptorLayout);

	//-------------------------------------------------------------------------------------------------------------------//

	VkFenceCreateInfo FenceCreateInfo();

	VkSemaphoreCreateInfo SemaphoreCreateInfo();
	
	//-------------------------------------------------------------------------------------------------------------------//

	VkAttachmentDescription AttachmentDescription(VkFormat format,
	                                              VkSampleCountFlagBits sampleCount,
	                                              VkAttachmentLoadOp loadOp,
	                                              VkAttachmentStoreOp storeOp,
	                                              VkAttachmentLoadOp stencilLoadOp,
	                                              VkAttachmentStoreOp stencilStoreOp,
	                                              VkImageLayout layout,
	                                              VkImageLayout finalLayout);

	VkSubpassDescription SubPassDescription(VkPipelineBindPoint bindPoint,
		const std::vector<VkAttachmentReference>& inputAttachments,
		const std::vector<VkAttachmentReference>& colorAttachments,
		VkAttachmentReference& depthStencilAttachment);

	VkSubpassDescription SubPassDescription(VkPipelineBindPoint bindPoint,
		const std::vector<VkAttachmentReference>& inputAttachments,
		const std::vector<VkAttachmentReference>& colorAttachments);
	
	//-------------------------------------------------------------------------------------------------------------------//

	VkRenderPassCreateInfo RenderPassCreateInfo(const std::vector<VkAttachmentDescription>& attachmentDescriptions,
		const std::vector<VkSubpassDescription>& subpassDescriptions);

	VkRenderPassCreateInfo RenderPassCreateInfo(const std::vector<VkAttachmentDescription>& attachmentDescriptions);
	
	//-------------------------------------------------------------------------------------------------------------------//

	VkFramebufferCreateInfo FramebufferCreateInfo(VkRenderPass renderPass,
		const std::vector<VkImageView>& attachments,
		uint32_t width,
		uint32_t height,
		uint32_t layerCount);
	
	//-------------------------------------------------------------------------------------------------------------------//

	VkImageViewCreateInfo ImageViewCreateInfo(VkImage image,
		VkImageViewType imageViewType,
		VkFormat format,
		uint32_t levelCount,
		uint32_t layerCount);

	VkImageCreateInfo ImageCreateInfo(VkImageType imageType,
		VkFormat imageFormat,
		uint32_t width,
		uint32_t height,
		uint32_t depth,
		uint32_t mipLevels,
		uint32_t arrayLayers,
		VkSampleCountFlagBits sampleCount,
		VkImageTiling tiling,
		VkImageUsageFlags usage);
	
	//-------------------------------------------------------------------------------------------------------------------//

	VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t queueFamilyIndex);

	VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool commandPool, bool secondary);
}

#endif /*_TRAP_VULKANINITS_H_*/