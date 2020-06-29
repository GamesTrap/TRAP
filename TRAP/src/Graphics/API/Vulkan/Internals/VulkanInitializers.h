#ifndef _TRAP_VULKANINITIALIZERS_H_
#define _TRAP_VULKANINITIALIZERS_H_

namespace TRAP::Graphics::API::Vulkan
{
	struct QueueFamilyIndices;
}

namespace TRAP::Graphics::API::Vulkan::Initializers
{
	VkApplicationInfo ApplicationInfo(const char* applicationName, uint32_t version);
	VkInstanceCreateInfo InstanceCreateInfo(VkApplicationInfo& appInfo);
	VkInstanceCreateInfo InstanceCreateInfo(VkApplicationInfo& appInfo, std::vector<const char*>& instanceLayers, std::vector<const char*>& instanceExtensions);
	VkDebugUtilsMessengerCreateInfoEXT DebugUtilsMessengerCreateInfo(PFN_vkDebugUtilsMessengerCallbackEXT callback);
	VkDeviceQueueCreateInfo DeviceQueueCreateInfo(uint32_t queueFamilyIndex, float* priority);
	VkDeviceCreateInfo DeviceCreateInfo(std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos, VkPhysicalDeviceFeatures& physicalDeviceFeatures);
	VkDeviceCreateInfo DeviceCreateInfo(std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos, VkPhysicalDeviceFeatures& physicalDeviceFeatures, std::vector<const char*>& physicalDeviceExtensions);
	VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);
	VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool pool, uint32_t count);
	VkSwapchainCreateInfoKHR SwapchainCreateInfo(VkSurfaceKHR surface, VkSurfaceFormatKHR surfaceFormat, uint32_t imageCount, VkExtent2D extent, const QueueFamilyIndices& queueFamilyIndices,
		VkSurfaceTransformFlagBitsKHR preTransform, VkPresentModeKHR presentMode, VkCompositeAlphaFlagsKHR supportCompositeAlpha, VkSwapchainKHR oldSwapchain = nullptr);
	VkImageViewCreateInfo ImageViewCreateInfo(VkImage& swapchainImage, VkImageViewType viewType, VkFormat format);
	VkRenderPassCreateInfo RenderPassCreateInfo(std::vector<VkAttachmentDescription>& attachments, VkSubpassDescription& subPassDescription, std::vector<VkSubpassDependency>& dependencies);
	VkFenceCreateInfo FenceCreateInfo(bool signaled);
	VkSemaphoreCreateInfo SemaphoreCreateInfo();
	VkFramebufferCreateInfo FrameBufferCreateInfo(VkRenderPass& renderPass, std::vector<VkImageView>& attachments, VkExtent2D extent, uint32_t layers = 1);
	VkFramebufferCreateInfo FrameBufferCreateInfo(VkRenderPass& renderPass, VkImageView& attachments, VkExtent2D extent, uint32_t layers = 1);
	VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo(std::vector<VkDescriptorSetLayout>& descriptorLayouts, std::vector<VkPushConstantRange>& pushConstants);
	VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo();
	VkPipelineInputAssemblyStateCreateInfo PipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology topology, VkBool32 primitiveRestart);
	VkPipelineRasterizationStateCreateInfo PipelineRasterizationStateCreateInfo(VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace);
	VkPipelineColorBlendAttachmentState PipelineColorBlendAttachmentState();
	VkPipelineColorBlendAttachmentState PipelineColorBlendAttachmentState(VkBlendFactor srcColorBlendFactor,
		VkBlendFactor srcAlphaBlendFactor,
		VkBlendFactor dstColorBlendFactor,
		VkBlendFactor dstAlphaBlendFactor,
		VkBlendOp colorBlendOp,
		VkBlendOp alphaBlendOp);
	VkPipelineColorBlendStateCreateInfo PipelineColorBlendStateCreateInfo(std::vector<VkPipelineColorBlendAttachmentState>& colorBlendAttachments);
	VkPipelineColorBlendStateCreateInfo PipelineColorBlendStateCreateInfo(VkPipelineColorBlendAttachmentState& colorBlendAttachment);
	VkPipelineDepthStencilStateCreateInfo PipelineDepthStencilStateCreateInfo(VkBool32 depthTest, VkBool32 depthWrite, VkCompareOp depthCompareOp);
	VkPipelineViewportStateCreateInfo PipelineViewportStateCreateInfo(uint32_t viewportCount, uint32_t scissorCount);
	VkPipelineMultisampleStateCreateInfo PipelineMultisampleStateCreateInfo(VkSampleCountFlagBits rasterizationSamples);
	VkPipelineDynamicStateCreateInfo PipelineDynamicStateCreateInfo(std::vector<VkDynamicState>& dynamicStates);
	VkPipelineVertexInputStateCreateInfo PipelineVertexInputStateCreateInfo(std::vector<VkVertexInputBindingDescription>& vertexInputBindingDescriptions,
		std::vector<VkVertexInputAttributeDescription>& vertexInputAttributeDescriptions);
	VkPipelineVertexInputStateCreateInfo PipelineVertexInputStateCreateInfo();
	VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo(std::vector<VkPipelineShaderStageCreateInfo>& shaderStageCreateInfos, VkPipelineVertexInputStateCreateInfo& vertexInputStateCreateInfo,
		VkPipelineInputAssemblyStateCreateInfo& inputAssemblyStateCreateInfo, VkPipelineViewportStateCreateInfo& viewportStateCreateInfo, VkPipelineRasterizationStateCreateInfo& rasterizationStateCreateInfo,
		VkPipelineMultisampleStateCreateInfo& multisampleStateCreateInfo, VkPipelineDepthStencilStateCreateInfo& depthStencilStateCreateInfo, VkPipelineColorBlendStateCreateInfo& colorBlendStateCreateInfo,
		VkPipelineDynamicStateCreateInfo& dynamicStateCreateInfo, VkPipelineLayout& layout, VkRenderPass& renderPass, uint32_t subPasses);
	VkCommandBufferBeginInfo CommandBufferBeginInfo();
	VkRenderPassBeginInfo RenderPassBeginInfo(VkRenderPass& renderPass, VkFramebuffer& frameBuffer, uint32_t width, uint32_t height, std::vector<VkClearValue>& clearValues);
	VkSubmitInfo SubmitInfo(VkSemaphore& waitSemaphore, VkPipelineStageFlags& waitMask, VkCommandBuffer& commandBuffer, VkSemaphore& signalSemaphore);
	VkSubmitInfo SubmitInfo(VkCommandBuffer& commandBuffer);
	VkPresentInfoKHR PresentInfo(VkSemaphore& waitSemaphore, VkSwapchainKHR& swapchain, uint32_t& imageIndex);
	VkBufferCreateInfo BufferCreateInfo(VkDeviceSize size, VkBufferUsageFlags usage, std::vector<uint32_t>& queueFamilyIndices);
	VkBufferCreateInfo BufferCreateInfo(VkDeviceSize size, VkBufferUsageFlags usage);
	VkMemoryAllocateInfo MemoryAllocateInfo(VkDeviceSize size, uint32_t memoryTypeIndex);
}

#endif /*_TRAP_VULKANINITIALIZERS_H_*/