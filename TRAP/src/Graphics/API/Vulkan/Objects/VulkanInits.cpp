#include "TRAPPCH.h"
#include "VulkanInits.h"

#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

VkApplicationInfo TRAP::Graphics::API::VulkanInits::ApplicationInfo(const std::string& appName)
{
	VkApplicationInfo info;

	info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	info.pNext = nullptr;
	info.pApplicationName = appName.data();
	info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	info.pEngineName = "TRAP";
	info.engineVersion = TRAP_VERSION;
	info.apiVersion = VK_API_VERSION_1_2;
	
	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkInstanceCreateInfo TRAP::Graphics::API::VulkanInits::InstanceCreateInfo(const VkApplicationInfo& appInfo,
	const std::vector<const char*>& instanceLayers,
	const std::vector<const char*>& instanceExtensions)
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

VkDebugUtilsMessengerCreateInfoEXT TRAP::Graphics::API::VulkanInits::DebugUtilsMessengerCreateInfo(
	const PFN_vkDebugUtilsMessengerCallbackEXT callback)
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

VkDebugUtilsObjectNameInfoEXT TRAP::Graphics::API::VulkanInits::DebugUtilsObjectNameInfo(const VkObjectType type,
	const uint64_t handle,
	const char* name)
{
	VkDebugUtilsObjectNameInfoEXT info;

	info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	info.pNext = nullptr;
	info.objectType = type;
	info.objectHandle = handle;
	info.pObjectName = name;
	
	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkDeviceCreateInfo TRAP::Graphics::API::VulkanInits::DeviceCreateInfo(const void* pNext,
                                                                      const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos,
																	  const std::vector<const char*>& deviceExtensions)
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
	const VmaVulkanFunctions& vulkanFunctions)
{
	VmaAllocatorCreateInfo info{};

	info.device = device;
	info.physicalDevice = physicalDevice;
	info.instance = instance;
	info.flags = 0;

	if (!VulkanRenderer::s_renderdocCapture)
		info.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;

	info.pVulkanFunctions = &vulkanFunctions;
	info.pAllocationCallbacks = nullptr;
	info.vulkanApiVersion = VK_API_VERSION_1_2;
	
	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkDescriptorPoolCreateInfo TRAP::Graphics::API::VulkanInits::DescriptorPoolCreateInfo(
	const std::vector<VkDescriptorPoolSize>& descriptorPoolSizes, const uint32_t numDescriptorSets)
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

VkDescriptorSetAllocateInfo TRAP::Graphics::API::VulkanInits::DescriptorSetAllocateInfo(
	VkDescriptorPool descriptorPool,
	const VkDescriptorSetLayout& descriptorLayout)
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

VkFenceCreateInfo TRAP::Graphics::API::VulkanInits::FenceCreateInfo()
{
	VkFenceCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	
	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkSemaphoreCreateInfo TRAP::Graphics::API::VulkanInits::SemaphoreCreateInfo()
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
	const VkImageLayout finalLayout)
{
	VkAttachmentDescription desc{};

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
	VkAttachmentReference& depthStencilAttachment)
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
	const std::vector<VkAttachmentReference>& colorAttachments)
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
	const std::vector<VkSubpassDescription>& subpassDescriptions)
{
	VkRenderPassCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
	info.pAttachments = attachmentDescriptions.data();
	info.subpassCount = static_cast<uint32_t>(subpassDescriptions.size());
	info.pSubpasses = subpassDescriptions.data();
	info.dependencyCount = 0;
	info.pDependencies = nullptr;
	
	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkRenderPassCreateInfo TRAP::Graphics::API::VulkanInits::RenderPassCreateInfo(const std::vector<VkAttachmentDescription>& attachmentDescriptions)
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

VkFramebufferCreateInfo TRAP::Graphics::API::VulkanInits::FramebufferCreateInfo(VkRenderPass renderPass,
	const std::vector<VkImageView>& attachments,
	const uint32_t width,
	const uint32_t height,
	const uint32_t layerCount)
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

VkBufferViewCreateInfo TRAP::Graphics::API::VulkanInits::BufferViewCreateInfo(VkBuffer buffer,
	const VkFormat format,
	const uint64_t offset,
	const uint64_t range)
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

VkImageViewCreateInfo TRAP::Graphics::API::VulkanInits::ImageViewCreateInfo(VkImage image,
	const VkImageViewType imageViewType,
	const VkFormat format,
	const uint32_t levelCount,
	const uint32_t layerCount)
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
                                                                    const VkFormat imageFormat,
                                                                    const uint32_t width,
                                                                    const uint32_t height,
                                                                    const uint32_t depth,
                                                                    const uint32_t mipLevels,
                                                                    const uint32_t arrayLayers,
																	const VkSampleCountFlagBits sampleCount,
																	const VkImageTiling tiling,
																	const VkImageUsageFlags usage)
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

VkBufferCreateInfo TRAP::Graphics::API::VulkanInits::BufferCreateInfo(const uint64_t allocationSize, const VkBufferUsageFlags usageFlags)
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

VkCommandPoolCreateInfo TRAP::Graphics::API::VulkanInits::CommandPoolCreateInfo(const uint32_t queueFamilyIndex)
{
	VkCommandPoolCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.queueFamilyIndex = queueFamilyIndex;
	
	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkCommandBufferAllocateInfo TRAP::Graphics::API::VulkanInits::CommandBufferAllocateInfo(VkCommandPool commandPool, const bool secondary)
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

VkSamplerCreateInfo TRAP::Graphics::API::VulkanInits::SamplerCreateInfo(const VkFilter magFilter,
	const VkFilter minFilter,
	const VkSamplerMipmapMode mipMapMode,
	const VkSamplerAddressMode u,
	const VkSamplerAddressMode v,
	const VkSamplerAddressMode w,
	const float mipLodBias,
	const float maxAnisotropy,
	const VkCompareOp compareOp)
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
	info.minLod = 0.0f;
	info.maxLod = (mipMapMode == VK_SAMPLER_MIPMAP_MODE_LINEAR) ? std::numeric_limits<float>::max() : 0.0f;
	info.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
	info.unnormalizedCoordinates = VK_FALSE;
	
	return info;
}