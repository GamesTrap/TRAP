#ifndef TRAP_VULKANINITS_H
#define TRAP_VULKANINITS_H

#include "VulkanMemoryAllocator.h"

namespace TRAP::Graphics::API::VulkanInits
{
	VkApplicationInfo ApplicationInfo(const std::string_view appName) noexcept;

	VkInstanceCreateInfo InstanceCreateInfo(const VkApplicationInfo& appInfo,
									        const std::vector<const char*>& instanceLayers,
	                                        const std::vector<const char*>& instanceExtensions) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	VkDebugUtilsMessengerCreateInfoEXT DebugUtilsMessengerCreateInfo(PFN_vkDebugUtilsMessengerCallbackEXT callback) noexcept;

	VkDebugUtilsObjectNameInfoEXT DebugUtilsObjectNameInfo(VkObjectType type, uint64_t handle,
	                                                       const std::string_view name) noexcept;

	VkDebugUtilsLabelEXT DebugUtilsLabelExt(float r, float g, float b, const char* name) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	VkDeviceCreateInfo DeviceCreateInfo(const void* pNext,
	                                    const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos,
	                                    const std::vector<const char*>& deviceExtensions) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	VmaAllocatorCreateInfo VMAAllocatorCreateInfo(VkDevice device, VkPhysicalDevice physicalDevice,
	                                              VkInstance instance, const VmaVulkanFunctions& vulkanFunctions) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo(const std::vector<VkDescriptorPoolSize>& descriptorPoolSizes,
	                                                    uint32_t numDescriptorSets) noexcept;

	VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo(VkDescriptorPool descriptorPool,
														  const VkDescriptorSetLayout& descriptorLayout) noexcept;

	VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo(const std::vector<VkDescriptorSetLayoutBinding>& bindings) noexcept;

	VkDescriptorUpdateTemplateCreateInfo DescriptorUpdateTemplateCreateInfo(VkDescriptorSetLayout descriptorSetLayout,
		                                                                    uint32_t entryCount,
		                                                                    VkDescriptorUpdateTemplateEntry* entries,
		                                                                    VkPipelineBindPoint bindPoint,
		                                                                    VkPipelineLayout pipelineLayout,
		                                                                    uint32_t setIndex) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	VkFenceCreateInfo FenceCreateInfo() noexcept;

	VkSemaphoreCreateInfo SemaphoreCreateInfo() noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	VkAttachmentDescription AttachmentDescription(VkFormat format, VkSampleCountFlagBits sampleCount,
	                                              VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp,
	                                              VkAttachmentLoadOp stencilLoadOp,
												  VkAttachmentStoreOp stencilStoreOp,
	                                              VkImageLayout layout, VkImageLayout finalLayout) noexcept;

	VkSubpassDescription SubPassDescription(VkPipelineBindPoint bindPoint,
	                                        const std::vector<VkAttachmentReference>& inputAttachments,
		                                    const std::vector<VkAttachmentReference>& colorAttachments,
		                                    VkAttachmentReference& depthStencilAttachment) noexcept;

	VkSubpassDescription SubPassDescription(VkPipelineBindPoint bindPoint,
		                                    const std::vector<VkAttachmentReference>& inputAttachments,
		                                    const std::vector<VkAttachmentReference>& colorAttachments) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	VkRenderPassCreateInfo RenderPassCreateInfo(const std::vector<VkAttachmentDescription>& attachmentDescriptions,
		                                        const VkSubpassDescription& subpassDescription) noexcept;

	VkRenderPassCreateInfo RenderPassCreateInfo(const std::vector<VkAttachmentDescription>& attachmentDescriptions) noexcept;

	VkRenderPassBeginInfo RenderPassBeginInfo(VkRenderPass renderPass, VkFramebuffer frameBuffer,
	                                          VkRect2D renderArea, const std::vector<VkClearValue>& colorValues) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	VkFramebufferCreateInfo FramebufferCreateInfo(VkRenderPass renderPass,
	                                              const std::vector<VkImageView>& attachments, uint32_t width,
		                                          uint32_t height, uint32_t layerCount) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	VkImageViewCreateInfo ImageViewCreateInfo(VkImage image, VkImageViewType imageViewType, VkFormat format,
		                                      uint32_t levelCount, uint32_t layerCount) noexcept;

	VkImageCreateInfo ImageCreateInfo(VkImageType imageType, VkFormat imageFormat, uint32_t width, uint32_t height,
		                              uint32_t depth, uint32_t mipLevels, uint32_t arrayLayers,
		                              VkSampleCountFlagBits sampleCount, VkImageTiling tiling,
									  VkImageUsageFlags usage) noexcept;

	VkBufferImageCopy ImageCopy(uint32_t bufferRowLength, uint32_t width, uint32_t height, uint32_t depth,
							    VkImageSubresourceLayers layers) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	VkBufferCreateInfo BufferCreateInfo(uint64_t allocationSize, VkBufferUsageFlags usageFlags) noexcept;

	VkBufferViewCreateInfo BufferViewCreateInfo(VkBuffer buffer, VkFormat format, uint64_t offset, uint64_t range) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t queueFamilyIndex) noexcept;

	VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool commandPool, bool secondary) noexcept;

	VkCommandBufferBeginInfo CommandBufferBeginInfo() noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	VkSamplerCreateInfo SamplerCreateInfo(VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipMapMode,
		                                  VkSamplerAddressMode u, VkSamplerAddressMode v, VkSamplerAddressMode w,
		                                  float mipLodBias, float maxAnisotropy, VkCompareOp compareOp) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo(uint32_t layoutCount, VkDescriptorSetLayout* layouts,
	                                                    uint32_t pushConstantRangeCount,
														VkPushConstantRange* pushConstants) noexcept;

	VkPipelineColorBlendStateCreateInfo PipelineColorBlendStateCreateInfo(const std::vector<VkPipelineColorBlendAttachmentState>& attachments) noexcept;

	VkPipelineColorBlendStateCreateInfo PipelineColorBlendStateCreateInfo(VkLogicOp logicOp,
		                                                                  const std::vector<VkPipelineColorBlendAttachmentState>& attachments,
		                                                                  float blendConstR, float blendConstG,
		                                                                  float blendConstB, float blendConstA) noexcept;

	VkPipelineCacheCreateInfo PipelineCacheCreateInfo(const std::vector<uint8_t>& data,
	                                                  VkPipelineCacheCreateFlags flags) noexcept;

	VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule module,
	                                                              const char* name) noexcept;

	VkPipelineVertexInputStateCreateInfo PipelineVertexInputStateCreateInfo(uint32_t inputBindingCount,
		                                                                    VkVertexInputBindingDescription* inputBindings,
		                                                                    uint32_t inputAttributeCount,
		                                                                    VkVertexInputAttributeDescription* inputAttributes) noexcept;

	VkPipelineInputAssemblyStateCreateInfo PipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology topology,
	                                                                            bool primitiveRestart) noexcept;

	VkPipelineTessellationStateCreateInfo PipelineTessellationStateCreateInfo(uint32_t patchControlPoints) noexcept;

	VkPipelineViewportStateCreateInfo PipelineViewportStateCreateInfo() noexcept;

	VkPipelineMultisampleStateCreateInfo PipelineMultisampleStateCreateInfo(VkSampleCountFlagBits sampleCount,
	                                                                        bool sampleShading) noexcept;

	VkPipelineDynamicStateCreateInfo PipelineDynamicStateCreateInfo(const std::vector<VkDynamicState>& dynamicStates) noexcept;

	VkComputePipelineCreateInfo ComputePipelineCreateInfo(const VkPipelineShaderStageCreateInfo& stage,
	                                                      VkPipelineLayout layout) noexcept;

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
															VkPipelineLayout layout, VkRenderPass renderPass
	) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	VkSubmitInfo SubmitInfo(const std::vector<VkSemaphore>& waitSemaphores,
	                        const std::vector<VkPipelineStageFlags>& waitMasks,
	                        const std::vector<VkCommandBuffer>& cmds,
	                        const std::vector<VkSemaphore>& signalSemaphore) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	VkQueryPoolCreateInfo QueryPoolCreateInfo(uint32_t count, VkQueryType queryType) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	VkSwapchainCreateInfoKHR SwapchainCreateInfoKHR(VkSurfaceKHR surface, uint32_t imageCount,
	                                                VkSurfaceFormatKHR surfaceFormat, VkExtent2D imageExtent,
	                                                VkSharingMode sharingMode, uint32_t queueFamilyIndexCount,
	                                                const std::array<uint32_t, 2>& queueFamilyIndices,
	                                                VkSurfaceTransformFlagBitsKHR preTransform,
													VkCompositeAlphaFlagBitsKHR compositeAlpha,
													VkPresentModeKHR presentMode) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	VkPresentInfoKHR PresentInfo(const std::vector<VkSemaphore>& waitSemaphores, const VkSwapchainKHR& swapChain,
	                             uint32_t& presentIndex) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	VkMemoryAllocateInfo MemoryAllocateInfo(VkDeviceSize allocSize, uint32_t memoryTypeIndex) noexcept;
}

#endif /*TRAP_VULKANINITS_H*/