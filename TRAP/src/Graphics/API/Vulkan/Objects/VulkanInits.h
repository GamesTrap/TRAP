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

	VkDebugUtilsLabelEXT DebugUtilsLabelExt(float r, float g, float b, const char* name);
	
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

	VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo(const std::vector<VkDescriptorSetLayoutBinding>& bindings);

	VkDescriptorUpdateTemplateCreateInfo DescriptorUpdateTemplateCreateInfo(
		VkDescriptorSetLayout descriptorSetLayout,
		uint32_t entryCount,
		VkDescriptorUpdateTemplateEntry* entries,
		VkPipelineBindPoint bindPoint,
		VkPipelineLayout pipelineLayout,
		uint32_t setIndex);
	
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

	VkBufferCreateInfo BufferCreateInfo(uint64_t allocationSize, VkBufferUsageFlags usageFlags);

	VkBufferViewCreateInfo BufferViewCreateInfo(VkBuffer buffer, VkFormat format, uint64_t offset, uint64_t range);
	
	//-------------------------------------------------------------------------------------------------------------------//

	VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t queueFamilyIndex);

	VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool commandPool, bool secondary);

	VkCommandBufferBeginInfo CommandBufferBeginInfo();
	
	//-------------------------------------------------------------------------------------------------------------------//

	VkSamplerCreateInfo SamplerCreateInfo(VkFilter magFilter,
		VkFilter minFilter,
		VkSamplerMipmapMode mipMapMode,
		VkSamplerAddressMode u,
		VkSamplerAddressMode v,
		VkSamplerAddressMode w,
		float mipLodBias,
		float maxAnisotropy,
		VkCompareOp compareOp);
	
	//-------------------------------------------------------------------------------------------------------------------//

	VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo(uint32_t layoutCount,
	                                                    VkDescriptorSetLayout* layouts,
	                                                    uint32_t pushConstantRangeCount,
	                                                    VkPushConstantRange* pushConstants);

	VkPipelineColorBlendStateCreateInfo PipelineColorBlendStateCreateInfo(const std::vector<VkPipelineColorBlendAttachmentState>& attachments);
	
	VkPipelineColorBlendStateCreateInfo PipelineColorBlendStateCreateInfo(VkLogicOp logicOp,
		const std::vector<VkPipelineColorBlendAttachmentState>& attachments,
		float blendConstR,
		float blendConstG,
		float blendConstB,
		float blendConstA);

	VkPipelineCacheCreateInfo PipelineCacheCreateInfo(std::size_t size, void* data, VkPipelineCacheCreateFlags flags);

	VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule module, const char* name);

	VkPipelineVertexInputStateCreateInfo PipelineVertexInputStateCreateInfo(uint32_t inputBindingCount,
		VkVertexInputBindingDescription* inputBindings,
		uint32_t inputAttributeCount,
		VkVertexInputAttributeDescription* inputAttributes);
	
	VkPipelineInputAssemblyStateCreateInfo PipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology topology, bool primitiveRestart);

	VkPipelineTessellationStateCreateInfo PipelineTessellationStateCreateInfo(uint32_t patchControlPoints);

	VkPipelineViewportStateCreateInfo PipelineViewportStateCreateInfo();

	VkPipelineMultisampleStateCreateInfo PipelineMultisampleStateCreateInfo(VkSampleCountFlagBits sampleCount, bool sampleShading);

	VkPipelineDynamicStateCreateInfo PipelineDynamicStateCreateInfo(const std::vector<VkDynamicState>& dynamicStates);
	
	VkComputePipelineCreateInfo ComputePipelineCreateInfo(const VkPipelineShaderStageCreateInfo& stage, VkPipelineLayout layout);

	VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo(uint32_t stageCount,
															VkPipelineShaderStageCreateInfo* stages,
															const VkPipelineVertexInputStateCreateInfo& vi,
															const VkPipelineInputAssemblyStateCreateInfo& ia,
															const VkPipelineViewportStateCreateInfo& vs,
															const VkPipelineRasterizationStateCreateInfo& rs,
															const VkPipelineMultisampleStateCreateInfo& ms,
															const VkPipelineDepthStencilStateCreateInfo& ds,
															const VkPipelineColorBlendStateCreateInfo& cb,
															const VkPipelineDynamicStateCreateInfo& dy,
															VkPipelineLayout layout,
															VkRenderPass renderPass
	);
	
	//-------------------------------------------------------------------------------------------------------------------//

	VkSubmitInfo SubmitInfo(const std::vector<VkSemaphore>& waitSemaphores,
	                        const std::vector<VkPipelineStageFlags>& waitMasks,
	                        const std::vector<VkCommandBuffer>& cmds,
	                        const std::vector<VkSemaphore>& signalSemaphore);
}

#endif /*_TRAP_VULKANINITS_H_*/