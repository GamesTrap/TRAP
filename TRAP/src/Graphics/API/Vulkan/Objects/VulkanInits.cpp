#include "TRAPPCH.h"
#include "VulkanInits.h"

#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

#include "Graphics/API/Vulkan/Objects/VulkanQueue.h" //TODO REMOVE

VkApplicationInfo TRAP::Graphics::API::VulkanInits::ApplicationInfo(const std::string_view appName) noexcept
{
	VkApplicationInfo info;

	info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	info.pNext = nullptr;
	info.pApplicationName = appName.data();
	info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	info.pEngineName = "TRAP";
	info.engineVersion = TRAP_VERSION;
	info.apiVersion = VK_API_VERSION_1_1;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkInstanceCreateInfo TRAP::Graphics::API::VulkanInits::InstanceCreateInfo(const VkApplicationInfo& appInfo,
	                                                                      const std::vector<const char*>& instanceLayers,
	                                                                      const std::vector<const char*>& instanceExtensions) noexcept
{
	VkInstanceCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.pApplicationInfo = &appInfo;
	info.enabledLayerCount = static_cast<uint32_t>(instanceLayers.size());
	info.ppEnabledLayerNames = instanceLayers.data();
	info.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
	info.ppEnabledExtensionNames = instanceExtensions.data();

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkDebugUtilsMessengerCreateInfoEXT TRAP::Graphics::API::VulkanInits::DebugUtilsMessengerCreateInfo(const PFN_vkDebugUtilsMessengerCallbackEXT callback) noexcept
{
	VkDebugUtilsMessengerCreateInfoEXT info;

	info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	info.pNext = nullptr;
	info.flags = 0;
	info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
						   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
						   VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
	info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		               VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
			           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
	info.pfnUserCallback = callback;
	info.pUserData = nullptr;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkDebugReportCallbackCreateInfoEXT TRAP::Graphics::API::VulkanInits::DebugReportCallbackCreateInfo(const PFN_vkDebugReportCallbackEXT callback) noexcept
{
	VkDebugReportCallbackCreateInfoEXT info;

	info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	info.pNext = nullptr;
	info.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
	             VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT;
	info.pfnCallback = callback;
	info.pUserData = nullptr;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkDebugUtilsObjectNameInfoEXT TRAP::Graphics::API::VulkanInits::DebugUtilsObjectNameInfo(const VkObjectType type,
	                                                                                     const uint64_t handle,
	                                                                                     const std::string_view name) noexcept
{
	VkDebugUtilsObjectNameInfoEXT info;

	info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	info.pNext = nullptr;
	info.objectType = type;
	info.objectHandle = handle;
	info.pObjectName = name.data();

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkDebugMarkerObjectNameInfoEXT TRAP::Graphics::API::VulkanInits::DebugMarkerObjectNameInfo(const VkDebugReportObjectTypeEXT type,
	                                                                                       const uint64_t handle,
	                                                                                       const std::string_view name) noexcept
{
	VkDebugMarkerObjectNameInfoEXT info;

	info.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT;
	info.pNext = nullptr;
	info.objectType = type;
	info.object = handle;
	info.pObjectName = name.data();

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkDebugUtilsLabelEXT TRAP::Graphics::API::VulkanInits::DebugUtilsLabelExt(const float r, const float g,
                                                                          const float b, const char* name) noexcept
{
	VkDebugUtilsLabelEXT info{};

	info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
	info.color[0] = r;
	info.color[1] = g;
	info.color[2] = b;
	info.color[3] = 1.0f;
	info.pLabelName = name;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkDebugMarkerMarkerInfoEXT TRAP::Graphics::API::VulkanInits::DebugMarkerMarkerInfo(const float r, const float g,
                                                                                   const float b, const char* name) noexcept
{
	VkDebugMarkerMarkerInfoEXT info{};

	info.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
	info.color[0] = r;
	info.color[1] = g;
	info.color[2] = b;
	info.color[3] = 1.0f;
	info.pMarkerName = name;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkDeviceCreateInfo TRAP::Graphics::API::VulkanInits::DeviceCreateInfo(const void* pNext,
                                                                      const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos,
																	  const std::vector<const char*>& deviceExtensions) noexcept
{
	VkDeviceCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	info.pNext = pNext;
	info.flags = 0;
	info.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	info.pQueueCreateInfos = queueCreateInfos.data();
	info.enabledLayerCount = 0;
	info.ppEnabledLayerNames = nullptr;
	info.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	info.ppEnabledExtensionNames = deviceExtensions.data();
	info.pEnabledFeatures = nullptr;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VmaAllocatorCreateInfo TRAP::Graphics::API::VulkanInits::VMAAllocatorCreateInfo(VkDevice device,
	                                                                            VkPhysicalDevice physicalDevice,
	                                                                            VkInstance instance,
	                                                                            const VmaVulkanFunctions& vulkanFunctions) noexcept
{
	VmaAllocatorCreateInfo info{};

	info.device = device;
	info.physicalDevice = physicalDevice;
	info.instance = instance;
	info.flags = 0;

	info.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
	if(VulkanRenderer::s_bufferDeviceAddressExtension)
		info.flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

	info.pVulkanFunctions = &vulkanFunctions;
	info.pAllocationCallbacks = nullptr;
	info.vulkanApiVersion = VK_API_VERSION_1_1;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkDescriptorPoolCreateInfo TRAP::Graphics::API::VulkanInits::DescriptorPoolCreateInfo(const std::vector<VkDescriptorPoolSize>& descriptorPoolSizes,
                                                                                      const uint32_t numDescriptorSets) noexcept
{
	VkDescriptorPoolCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
	info.pPoolSizes = descriptorPoolSizes.data();
	info.maxSets = numDescriptorSets;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkDescriptorSetAllocateInfo TRAP::Graphics::API::VulkanInits::DescriptorSetAllocateInfo(VkDescriptorPool descriptorPool,
	                                                                                    const VkDescriptorSetLayout& descriptorLayout) noexcept
{
	VkDescriptorSetAllocateInfo info;

	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	info.pNext = nullptr;
	info.descriptorPool = descriptorPool;
	info.descriptorSetCount = 1;
	info.pSetLayouts = &descriptorLayout;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkDescriptorSetLayoutCreateInfo TRAP::Graphics::API::VulkanInits::DescriptorSetLayoutCreateInfo(const std::vector<VkDescriptorSetLayoutBinding>& bindings) noexcept
{
	VkDescriptorSetLayoutCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.bindingCount = static_cast<uint32_t>(bindings.size());
	info.pBindings = bindings.data();

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkDescriptorUpdateTemplateCreateInfo TRAP::Graphics::API::VulkanInits::DescriptorUpdateTemplateCreateInfo(VkDescriptorSetLayout descriptorSetLayout,
	                                                                                                      const uint32_t entryCount,
	                                                                                                      VkDescriptorUpdateTemplateEntry* entries,
	                                                                                                      const VkPipelineBindPoint bindPoint,
	                                                                                                      VkPipelineLayout pipelineLayout,
	                                                                                                      const uint32_t setIndex) noexcept
{
	VkDescriptorUpdateTemplateCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.descriptorSetLayout = descriptorSetLayout;
	info.descriptorUpdateEntryCount = entryCount;
	info.pDescriptorUpdateEntries = entries;
	info.pipelineBindPoint = bindPoint;
	info.pipelineLayout = pipelineLayout;
	info.set = setIndex;
	info.templateType = VK_DESCRIPTOR_UPDATE_TEMPLATE_TYPE_DESCRIPTOR_SET;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkFenceCreateInfo TRAP::Graphics::API::VulkanInits::FenceCreateInfo() noexcept
{
	VkFenceCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkSemaphoreCreateInfo TRAP::Graphics::API::VulkanInits::SemaphoreCreateInfo() noexcept
{
	VkSemaphoreCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkAttachmentDescription TRAP::Graphics::API::VulkanInits::AttachmentDescription(const VkFormat format,
	                                                                            const VkSampleCountFlagBits sampleCount,
	                                                                            const VkAttachmentLoadOp loadOp,
	                                                                            const VkAttachmentStoreOp storeOp,
	                                                                            const VkAttachmentLoadOp stencilLoadOp,
	                                                                            const VkAttachmentStoreOp stencilStoreOp,
	                                                                            const VkImageLayout layout,
	                                                                            const VkImageLayout finalLayout) noexcept
{
	VkAttachmentDescription desc;

	desc.flags = 0;
	desc.format = format;
	desc.samples = sampleCount;
	desc.loadOp = loadOp;
	desc.storeOp = storeOp;
	desc.stencilLoadOp = stencilLoadOp;
	desc.stencilStoreOp = stencilStoreOp;
	desc.initialLayout = layout;
	desc.finalLayout = finalLayout;

	return desc;
}

//-------------------------------------------------------------------------------------------------------------------//

VkSubpassDescription TRAP::Graphics::API::VulkanInits::SubPassDescription(const VkPipelineBindPoint bindPoint,
	                                                                      const std::vector<VkAttachmentReference>& inputAttachments,
	                                                                      const std::vector<VkAttachmentReference>& colorAttachments,
	                                                                      VkAttachmentReference& depthStencilAttachment) noexcept
{
	VkSubpassDescription subpass;

	subpass.flags = 0;
	subpass.pipelineBindPoint = bindPoint;
	subpass.inputAttachmentCount = static_cast<uint32_t>(inputAttachments.size());
	subpass.pInputAttachments = inputAttachments.data();
	subpass.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
	subpass.pColorAttachments = colorAttachments.data();
	subpass.pResolveAttachments = nullptr;
	subpass.pDepthStencilAttachment = &depthStencilAttachment;
	subpass.preserveAttachmentCount = 0;
	subpass.pPreserveAttachments = nullptr;

	return subpass;
}

//-------------------------------------------------------------------------------------------------------------------//

VkSubpassDescription TRAP::Graphics::API::VulkanInits::SubPassDescription(const VkPipelineBindPoint bindPoint,
	                                                                      const std::vector<VkAttachmentReference>& inputAttachments,
	                                                                      const std::vector<VkAttachmentReference>& colorAttachments) noexcept
{
	VkSubpassDescription subpass;

	subpass.flags = 0;
	subpass.pipelineBindPoint = bindPoint;
	subpass.inputAttachmentCount = static_cast<uint32_t>(inputAttachments.size());
	subpass.pInputAttachments = inputAttachments.data();
	subpass.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
	subpass.pColorAttachments = colorAttachments.data();
	subpass.pResolveAttachments = nullptr;
	subpass.pDepthStencilAttachment = nullptr;
	subpass.preserveAttachmentCount = 0;
	subpass.pPreserveAttachments = nullptr;

	return subpass;
}

//-------------------------------------------------------------------------------------------------------------------//

VkRenderPassCreateInfo TRAP::Graphics::API::VulkanInits::RenderPassCreateInfo(const std::vector<VkAttachmentDescription>& attachmentDescriptions,
	                                                                          const VkSubpassDescription& subpassDescription) noexcept
{
	VkRenderPassCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
	info.pAttachments = attachmentDescriptions.data();
	info.subpassCount = 1;
	info.pSubpasses = &subpassDescription;
	info.dependencyCount = 0;
	info.pDependencies = nullptr;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkRenderPassCreateInfo TRAP::Graphics::API::VulkanInits::RenderPassCreateInfo(const std::vector<VkAttachmentDescription>& attachmentDescriptions) noexcept
{
	VkRenderPassCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
	info.pAttachments = attachmentDescriptions.data();
	info.subpassCount = 0;
	info.pSubpasses = nullptr;
	info.dependencyCount = 0;
	info.pDependencies = nullptr;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkRenderPassBeginInfo TRAP::Graphics::API::VulkanInits::RenderPassBeginInfo(VkRenderPass renderPass,
	                                                                        VkFramebuffer frameBuffer,
	                                                                        const VkRect2D renderArea,
	                                                                        const std::vector<VkClearValue>& colorValues) noexcept
{
	VkRenderPassBeginInfo info;

	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	info.pNext = nullptr;
	info.renderPass = renderPass;
	info.framebuffer = frameBuffer;
	info.renderArea = renderArea;
	info.clearValueCount = static_cast<uint32_t>(colorValues.size());
	info.pClearValues = colorValues.data();

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkFramebufferCreateInfo TRAP::Graphics::API::VulkanInits::FramebufferCreateInfo(VkRenderPass renderPass,
	                                                                            const std::vector<VkImageView>& attachments,
	                                                                            const uint32_t width,
	                                                                            const uint32_t height,
	                                                                            const uint32_t layerCount) noexcept
{
	VkFramebufferCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.renderPass = renderPass;
	info.attachmentCount = static_cast<uint32_t>(attachments.size());
	info.pAttachments = attachments.data();
	info.width = width;
	info.height = height;
	info.layers = layerCount;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkImageViewCreateInfo TRAP::Graphics::API::VulkanInits::ImageViewCreateInfo(VkImage image,
	                                                                        const VkImageViewType imageViewType,
	                                                                        const VkFormat format,
																			const uint32_t levelCount,
	                                                                        const uint32_t layerCount) noexcept
{
	VkImageViewCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.image = image;
	info.viewType = imageViewType;
	info.format = format;
	info.components.r = VK_COMPONENT_SWIZZLE_R;
	info.components.g = VK_COMPONENT_SWIZZLE_G;
	info.components.b = VK_COMPONENT_SWIZZLE_B;
	info.components.a = VK_COMPONENT_SWIZZLE_A;
	info.subresourceRange.aspectMask = DetermineAspectMask(info.format, true);
	info.subresourceRange.baseMipLevel = 0;
	info.subresourceRange.levelCount = levelCount;
	info.subresourceRange.baseArrayLayer = 0;
	info.subresourceRange.layerCount = layerCount;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkImageCreateInfo TRAP::Graphics::API::VulkanInits::ImageCreateInfo(const VkImageType imageType,
                                                                    const VkFormat imageFormat, const uint32_t width,
                                                                    const uint32_t height, const uint32_t depth,
                                                                    const uint32_t mipLevels,
																	const uint32_t arrayLayers,
																	const VkSampleCountFlagBits sampleCount,
																	const VkImageTiling tiling,
																	const VkImageUsageFlags usage) noexcept
{
	VkImageCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.imageType = imageType;
	info.format = imageFormat;
	info.extent.width = width;
	info.extent.height = height;
	info.extent.depth = depth;
	info.mipLevels = mipLevels;
	info.arrayLayers = arrayLayers;
	info.samples = sampleCount;
	info.tiling = tiling;
	info.usage = usage;
	info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	info.queueFamilyIndexCount = 0;
	info.pQueueFamilyIndices = nullptr;
	info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkBufferImageCopy TRAP::Graphics::API::VulkanInits::ImageCopy(const uint32_t bufferRowLength, const uint32_t width,
															  const uint32_t height, const uint32_t depth,
							    							  const VkImageSubresourceLayers layers) noexcept
{
	VkBufferImageCopy copy;

	copy.bufferOffset = 0;
	copy.bufferRowLength = bufferRowLength;
	copy.bufferImageHeight = 0;
	copy.imageSubresource = layers;
	copy.imageOffset.x = 0;
	copy.imageOffset.y = 0;
	copy.imageOffset.z = 0;
	copy.imageExtent.width = width;
	copy.imageExtent.height = height;
	copy.imageExtent.depth = depth;

	return copy;
}

//-------------------------------------------------------------------------------------------------------------------//

VkBufferCreateInfo TRAP::Graphics::API::VulkanInits::BufferCreateInfo(const uint64_t allocationSize,
                                                                      const VkBufferUsageFlags usageFlags) noexcept
{
	VkBufferCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.size = allocationSize;
	info.usage = usageFlags;
	info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	info.queueFamilyIndexCount = 0;
	info.pQueueFamilyIndices = nullptr;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkBufferViewCreateInfo TRAP::Graphics::API::VulkanInits::BufferViewCreateInfo(VkBuffer buffer, const VkFormat format,
	                                                                          const uint64_t offset,
	                                                                          const uint64_t range) noexcept
{
	VkBufferViewCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.buffer = buffer;
	info.format = format;
	info.offset = offset;
	info.range = range;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkCommandPoolCreateInfo TRAP::Graphics::API::VulkanInits::CommandPoolCreateInfo(const uint32_t queueFamilyIndex) noexcept
{
	VkCommandPoolCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.queueFamilyIndex = queueFamilyIndex;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkCommandBufferAllocateInfo TRAP::Graphics::API::VulkanInits::CommandBufferAllocateInfo(VkCommandPool commandPool,
                                                                                        const bool secondary) noexcept
{
	VkCommandBufferAllocateInfo info;

	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	info.pNext = nullptr;
	info.commandPool = commandPool;
	info.level = secondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY : VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	info.commandBufferCount = 1;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkCommandBufferBeginInfo TRAP::Graphics::API::VulkanInits::CommandBufferBeginInfo() noexcept
{
	VkCommandBufferBeginInfo info;

	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.pInheritanceInfo = nullptr;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkSamplerCreateInfo TRAP::Graphics::API::VulkanInits::SamplerCreateInfo(const VkFilter magFilter,
	                                                                    const VkFilter minFilter,
	                                                                    const VkSamplerMipmapMode mipMapMode,
	                                                                    const VkSamplerAddressMode u,
	                                                                    const VkSamplerAddressMode v,
	                                                                    const VkSamplerAddressMode w,
	                                                                    const float mipLodBias,
																		const float minLod,
																		const float maxLod,
	                                                                    const float maxAnisotropy,
	                                                                    const VkCompareOp compareOp) noexcept
{
	VkSamplerCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.magFilter = magFilter;
	info.minFilter = minFilter;
	info.mipmapMode = mipMapMode;
	info.addressModeU = u;
	info.addressModeV = v;
	info.addressModeW = w;
	info.mipLodBias = mipLodBias;
	info.anisotropyEnable = (maxAnisotropy > 0.0f) ? VK_TRUE : VK_FALSE;
	info.maxAnisotropy = maxAnisotropy;
	info.compareEnable = (compareOp != VK_COMPARE_OP_NEVER) ? VK_TRUE : VK_FALSE;
	info.compareOp = compareOp;
	info.minLod = minLod;
	info.maxLod = maxLod;
	info.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
	info.unnormalizedCoordinates = VK_FALSE;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineLayoutCreateInfo TRAP::Graphics::API::VulkanInits::PipelineLayoutCreateInfo(const uint32_t layoutCount,
	                                                                                  VkDescriptorSetLayout* layouts,
	                                                                                  const uint32_t pushConstantRangeCount,
	                                                                                  VkPushConstantRange* pushConstants) noexcept
{
	VkPipelineLayoutCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.setLayoutCount = layoutCount;
	info.pSetLayouts = layouts;
	info.pushConstantRangeCount = pushConstantRangeCount;
	info.pPushConstantRanges = pushConstants;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineColorBlendStateCreateInfo TRAP::Graphics::API::VulkanInits::PipelineColorBlendStateCreateInfo(const std::vector<VkPipelineColorBlendAttachmentState>& attachments) noexcept
{
	VkPipelineColorBlendStateCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.logicOpEnable = VK_FALSE;
	info.logicOp = VK_LOGIC_OP_CLEAR;
	info.attachmentCount = static_cast<uint32_t>(attachments.size());
	info.pAttachments = attachments.data();
	info.blendConstants[0] = 0.0f;
	info.blendConstants[1] = 0.0f;
	info.blendConstants[2] = 0.0f;
	info.blendConstants[3] = 0.0f;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineColorBlendStateCreateInfo TRAP::Graphics::API::VulkanInits::PipelineColorBlendStateCreateInfo(const VkLogicOp logicOp,
	                                                                                                    const std::vector<VkPipelineColorBlendAttachmentState>& attachments,
	                                                                                                    const float blendConstR,
	                                                                                                    const float blendConstG,
	                                                                                                    const float blendConstB,
	                                                                                                    const float blendConstA) noexcept
{
	VkPipelineColorBlendStateCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.logicOpEnable = VK_TRUE;
	info.logicOp = logicOp;
	info.attachmentCount = static_cast<uint32_t>(attachments.size());
	info.pAttachments = attachments.data();
	info.blendConstants[0] = blendConstR;
	info.blendConstants[1] = blendConstG;
	info.blendConstants[2] = blendConstB;
	info.blendConstants[3] = blendConstA;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineCacheCreateInfo TRAP::Graphics::API::VulkanInits::PipelineCacheCreateInfo(const std::vector<uint8_t>& data,
	                                                                                const VkPipelineCacheCreateFlags flags) noexcept
{
	VkPipelineCacheCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	info.pNext = nullptr;
	info.initialDataSize = data.size();
	info.pInitialData = data.data();
	info.flags = flags;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineShaderStageCreateInfo TRAP::Graphics::API::VulkanInits::PipelineShaderStageCreateInfo(const VkShaderStageFlagBits stage,
	                                                                                            VkShaderModule module,
	                                                                                            const char* name) noexcept
{
	VkPipelineShaderStageCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.stage = stage;
	info.module = module;
	info.pName = name;
	info.pSpecializationInfo = nullptr;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineVertexInputStateCreateInfo TRAP::Graphics::API::VulkanInits::PipelineVertexInputStateCreateInfo(const uint32_t inputBindingCount,
	                                                                                                      VkVertexInputBindingDescription* inputBindings,
	                                                                                                      const uint32_t inputAttributeCount,
	                                                                                                      VkVertexInputAttributeDescription* inputAttributes) noexcept
{
	VkPipelineVertexInputStateCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.vertexBindingDescriptionCount = inputBindingCount;
	info.pVertexBindingDescriptions = inputBindings;
	info.vertexAttributeDescriptionCount = inputAttributeCount;
	info.pVertexAttributeDescriptions = inputAttributes;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineInputAssemblyStateCreateInfo TRAP::Graphics::API::VulkanInits::PipelineInputAssemblyStateCreateInfo(const VkPrimitiveTopology topology,
                                                                                                              const bool primitiveRestart) noexcept
{
	VkPipelineInputAssemblyStateCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.topology = topology;
	info.primitiveRestartEnable = primitiveRestart;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineTessellationStateCreateInfo TRAP::Graphics::API::VulkanInits::PipelineTessellationStateCreateInfo(const uint32_t patchControlPoints) noexcept
{
	VkPipelineTessellationStateCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.patchControlPoints = patchControlPoints;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineViewportStateCreateInfo TRAP::Graphics::API::VulkanInits::PipelineViewportStateCreateInfo() noexcept
{
	VkPipelineViewportStateCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.viewportCount = 1; //We are using dynamic viewports but we must set the viewportCount to 1
	info.pViewports = nullptr;
	info.scissorCount = 1;
	info.pScissors = nullptr;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineMultisampleStateCreateInfo TRAP::Graphics::API::VulkanInits::PipelineMultisampleStateCreateInfo(const VkSampleCountFlagBits sampleCount,
	                                                                                                      const bool sampleShading,
																										  const float sampleShadingRate) noexcept
{
	VkPipelineMultisampleStateCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.rasterizationSamples = sampleCount;
	info.sampleShadingEnable = sampleShading;
	info.minSampleShading = sampleShading ? sampleShadingRate : 0.0f;
	info.pSampleMask = nullptr;
	info.alphaToCoverageEnable = VK_FALSE;
	info.alphaToOneEnable = VK_FALSE;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineDynamicStateCreateInfo TRAP::Graphics::API::VulkanInits::PipelineDynamicStateCreateInfo(const std::vector<VkDynamicState>& dynamicStates) noexcept
{
	VkPipelineDynamicStateCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	info.pDynamicStates = dynamicStates.data();

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkComputePipelineCreateInfo TRAP::Graphics::API::VulkanInits::ComputePipelineCreateInfo(const VkPipelineShaderStageCreateInfo& stage,
	                                                                                    VkPipelineLayout layout) noexcept
{
	VkComputePipelineCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.stage = stage;
	info.layout = layout;
	info.basePipelineHandle = nullptr;
	info.basePipelineIndex = 0;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkGraphicsPipelineCreateInfo TRAP::Graphics::API::VulkanInits::GraphicsPipelineCreateInfo(const uint32_t stageCount,
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
	                                                                                      VkRenderPass renderPass) noexcept
{
	VkGraphicsPipelineCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.stageCount = stageCount;
	info.pStages = stages;
	info.pVertexInputState = &vi;
	info.pInputAssemblyState = &ia;

	info.pViewportState = &vs;
	info.pRasterizationState = &rs;
	info.pMultisampleState = &ms;
	info.pDepthStencilState = &ds;
	info.pColorBlendState = &cb;
	info.pDynamicState = &dy;
	info.layout = layout;
	info.renderPass = renderPass;
	info.subpass = 0;
	info.basePipelineHandle = VK_NULL_HANDLE;
	info.basePipelineIndex = -1;

	info.pTessellationState = nullptr;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkSubmitInfo TRAP::Graphics::API::VulkanInits::SubmitInfo(const std::vector<VkSemaphore>& waitSemaphores,
														  const uint32_t waitCount,
                                                          const std::vector<VkPipelineStageFlags>& waitMasks,
                                                          const std::vector<VkCommandBuffer>& cmds,
                                                          const std::vector<VkSemaphore>& signalSemaphores,
														  const uint32_t signalCount) noexcept
{
	VkSubmitInfo info;

	info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	info.pNext = nullptr;
	info.waitSemaphoreCount = waitCount;
	info.pWaitSemaphores = waitSemaphores.data();
	info.pWaitDstStageMask = waitMasks.data();
	info.commandBufferCount = static_cast<uint32_t>(cmds.size());
	info.pCommandBuffers = cmds.data();
	info.signalSemaphoreCount = signalCount;
	info.pSignalSemaphores = signalSemaphores.data();

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkQueryPoolCreateInfo TRAP::Graphics::API::VulkanInits::QueryPoolCreateInfo(const uint32_t count,
                                                                            const VkQueryType queryType) noexcept
{
	VkQueryPoolCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.queryCount = count;
	info.queryType = queryType;
	info.pipelineStatistics = 0;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkSwapchainCreateInfoKHR TRAP::Graphics::API::VulkanInits::SwapchainCreateInfoKHR(VkSurfaceKHR surface,
	                                                                              const uint32_t imageCount,
	                                                                              const VkSurfaceFormatKHR surfaceFormat,
	                                                                              const VkExtent2D imageExtent,
	                                                                              const VkSharingMode sharingMode,
	                                                                              const uint32_t queueFamilyIndexCount,
	                                                                              const std::array<uint32_t, 2>& queueFamilyIndices,
	                                                                              const VkSurfaceTransformFlagBitsKHR preTransform,
	                                                                              const VkCompositeAlphaFlagBitsKHR compositeAlpha,
	                                                                              const VkPresentModeKHR presentMode) noexcept
{
	VkSwapchainCreateInfoKHR info;

	info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	info.pNext = nullptr;
	info.flags = 0;
	info.surface = surface;
	info.minImageCount = imageCount;
	info.imageFormat = surfaceFormat.format;
	info.imageColorSpace = surfaceFormat.colorSpace;
	info.imageExtent = imageExtent;
	info.imageArrayLayers = 1;
	info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	info.imageSharingMode = sharingMode;
	info.queueFamilyIndexCount = queueFamilyIndexCount;
	info.pQueueFamilyIndices = queueFamilyIndices.data();
	info.preTransform = preTransform;
	info.compositeAlpha = compositeAlpha;
	info.presentMode = presentMode;
	info.clipped = VK_TRUE;
	info.oldSwapchain = nullptr;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkPresentInfoKHR TRAP::Graphics::API::VulkanInits::PresentInfo(const std::vector<VkSemaphore>& waitSemaphores,
                                                               const VkSwapchainKHR& swapChain,
															   uint32_t& presentIndex) noexcept
{
	VkPresentInfoKHR info;

	info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	info.pNext = nullptr;
	info.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
	info.pWaitSemaphores = !waitSemaphores.empty() ? waitSemaphores.data() : nullptr;
	info.swapchainCount = 1;
	info.pSwapchains = &swapChain;
	info.pImageIndices = &presentIndex;
	info.pResults = nullptr;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkMemoryAllocateInfo TRAP::Graphics::API::VulkanInits::MemoryAllocateInfo(const VkDeviceSize allocSize,
                                                                          const uint32_t memoryTypeIndex) noexcept
{
	VkMemoryAllocateInfo info;

	info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	info.pNext = nullptr;
	info.allocationSize = allocSize;
	info.memoryTypeIndex = memoryTypeIndex;

	return info;
}