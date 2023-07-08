#include "TRAPPCH.h"
#include "VulkanInits.h"

#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

[[nodiscard]] VkApplicationInfo TRAP::Graphics::API::VulkanInits::ApplicationInfo(const std::string_view appName)
{
	TRAP_ASSERT(!appName.empty(), "VulkanInits::ApplicationInfo(): Application name can't be empty!");

	VkApplicationInfo info{};

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

[[nodiscard]] VkDebugUtilsMessengerCreateInfoEXT TRAP::Graphics::API::VulkanInits::DebugUtilsMessengerCreateInfo(const PFN_vkDebugUtilsMessengerCallbackEXT callback)
{
	TRAP_ASSERT(callback, "VulkanInits::DebugUtilsMessengerCreateInfo(): Callback can't be nullptr!");

	VkDebugUtilsMessengerCreateInfoEXT info{};

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

[[nodiscard]] VkDebugReportCallbackCreateInfoEXT TRAP::Graphics::API::VulkanInits::DebugReportCallbackCreateInfo(const PFN_vkDebugReportCallbackEXT callback)
{
	TRAP_ASSERT(callback, "VulkanInits::DebugReportCallbackCreateInfo(): Callback can't be nullptr!");

	VkDebugReportCallbackCreateInfoEXT info{};

	info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	info.pNext = nullptr;
	info.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
	             VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT;
	info.pfnCallback = callback;
	info.pUserData = nullptr;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkDeviceCreateInfo TRAP::Graphics::API::VulkanInits::DeviceCreateInfo(const void* const pNext,
                                                                                              const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos,
																	                          const std::vector<const char*>& deviceExtensions)
{
	TRAP_ASSERT(!queueCreateInfos.empty(), "VulkanInits::DeviceCreateInfo(): QueueCreateInfos can't be empty!");

	VkDeviceCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	info.pNext = pNext;
	info.flags = 0;
	info.queueCreateInfoCount = NumericCast<uint32_t>(queueCreateInfos.size());
	info.pQueueCreateInfos = queueCreateInfos.data();
	info.enabledLayerCount = 0;
	info.ppEnabledLayerNames = nullptr;
	info.enabledExtensionCount = NumericCast<uint32_t>(deviceExtensions.size());
	info.ppEnabledExtensionNames = deviceExtensions.data();
	info.pEnabledFeatures = nullptr;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VmaAllocatorCreateInfo TRAP::Graphics::API::VulkanInits::VMAAllocatorCreateInfo(VkDevice device,
	                                                                                          VkPhysicalDevice physicalDevice,
	                                                                                          VkInstance instance,
	                                                                                          const VmaVulkanFunctions& vulkanFunctions)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(device != VK_NULL_HANDLE, "VulkanInits::VMAAllocatorCreateInfo(): Device can't be VK_NULL_HANDLE!");
	TRAP_ASSERT(physicalDevice != VK_NULL_HANDLE, "VulkanInits::VMAAllocatorCreateInfo(): PhysicalDevice can't be VK_NULL_HANDLE!");
	TRAP_ASSERT(instance != VK_NULL_HANDLE, "VulkanInits::VMAAllocatorCreateInfo(): Instance can't be VK_NULL_HANDLE!");

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

[[nodiscard]] VkDescriptorPoolCreateInfo TRAP::Graphics::API::VulkanInits::DescriptorPoolCreateInfo(const std::vector<VkDescriptorPoolSize>& descriptorPoolSizes,
                                                                                                    const uint32_t numDescriptorSets)
{
	TRAP_ASSERT(numDescriptorSets != 0, "VulkanInits::DescriptorPoolCreateInfo(): NumDescriptorSets can't be 0!");

	VkDescriptorPoolCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.poolSizeCount = NumericCast<uint32_t>(descriptorPoolSizes.size());
	info.pPoolSizes = descriptorPoolSizes.data();
	info.maxSets = numDescriptorSets;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkDescriptorSetAllocateInfo TRAP::Graphics::API::VulkanInits::DescriptorSetAllocateInfo(VkDescriptorPool descriptorPool,
	                                                                                                  const VkDescriptorSetLayout& descriptorLayout)
{
	TRAP_ASSERT(descriptorPool != VK_NULL_HANDLE, "VulkanInits::DescriptorSetAllocateInfo(): DescriptorPool can't be VK_NULL_HANDLE!");
	TRAP_ASSERT(descriptorLayout != VK_NULL_HANDLE, "VulkanInits::DescriptorSetAllocateInfo(): DescriptorLayout can't be VK_NULL_HANDLE!");

	VkDescriptorSetAllocateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	info.pNext = nullptr;
	info.descriptorPool = descriptorPool;
	info.descriptorSetCount = 1;
	info.pSetLayouts = &descriptorLayout;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkDescriptorUpdateTemplateCreateInfo TRAP::Graphics::API::VulkanInits::DescriptorUpdateTemplateCreateInfo(VkDescriptorSetLayout descriptorSetLayout,
	                                                                                                                    const uint32_t entryCount,
	                                                                                                                    const VkDescriptorUpdateTemplateEntry* const entries,
	                                                                                                                    const VkPipelineBindPoint bindPoint,
	                                                                                                                    VkPipelineLayout pipelineLayout,
	                                                                                                                    const uint32_t setIndex)
{
	TRAP_ASSERT(descriptorSetLayout != VK_NULL_HANDLE, "VulkanInits::DescriptorUpdateTemplateCreateInfo(): DescriptorSetLayout can't be VK_NULL_HANDLE!");
	TRAP_ASSERT(entryCount != 0, "VulkanInits::DescriptorUpdateTemplateCreateInfo(): EntryCount can't be 0!");
	TRAP_ASSERT(entries, "VulkanInits::DescriptorUpdateTemplateCreateInfo(): Entries can't be nullptr!");

	VkDescriptorUpdateTemplateCreateInfo info{};

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

[[nodiscard]] VkSemaphoreWaitInfoKHR TRAP::Graphics::API::VulkanInits::SemaphoreWaitInfo(VkSemaphore& semaphore, uint64_t& value)
{
	TRAP_ASSERT(semaphore != VK_NULL_HANDLE, "VulkanInits::SemaphoreWaitInfo(): Semaphore can't be VK_NULL_HANDLE!");

	VkSemaphoreWaitInfoKHR info{};

	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO_KHR;
	info.pNext = nullptr;
	info.flags = 0;
	info.semaphoreCount = 1;
	info.pSemaphores = &semaphore;
	info.pValues = &value;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkFragmentShadingRateAttachmentInfoKHR TRAP::Graphics::API::VulkanInits::FragmentShadingRateAttachmentInfo(VkAttachmentReference2KHR& shadingRateAttachment,
	                                                                                                                     VkExtent2D texelSize)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(shadingRateAttachment.attachment != VK_ATTACHMENT_UNUSED, "VulkanInits::FragmentShadingRateAttachmentInfo(): ShadingRateAttachment can't be VK_ATTACHMENT_UNUSED!");
	TRAP_ASSERT(shadingRateAttachment.layout == VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR || shadingRateAttachment.layout == VK_IMAGE_LAYOUT_GENERAL, "VulkanInits::FragmentShadingRateAttachmentInfo(): ShadingRateAttachment Layout must be VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR or VK_IMAGE_LAYOUT_GENERAL!");
	TRAP_ASSERT(Math::IsPowerOfTwo(texelSize.width) && Math::IsPowerOfTwo(texelSize.height), "VulkanInits::FragmentShadingRateAttachmentInfo(): TexelSize must be a power of two!");
	TRAP_ASSERT(texelSize.width <= RendererAPI::GPUSettings.ShadingRateTexelWidth && texelSize.height <= RendererAPI::GPUSettings.ShadingRateTexelHeight, "VulkanInits::FragmentShadingRateAttachmentInfo(): TexelSize must be smaller than or equal to RendererAPI::GPUSettings.ShadingRateTexelWidth and RendererAPI::GPUSettings.ShadingRateTexelHeight!");

	VkFragmentShadingRateAttachmentInfoKHR info{};

	info.sType = VK_STRUCTURE_TYPE_FRAGMENT_SHADING_RATE_ATTACHMENT_INFO_KHR;
	info.pNext = nullptr;
	info.pFragmentShadingRateAttachment = &shadingRateAttachment;
	info.shadingRateAttachmentTexelSize = texelSize;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkAttachmentDescription TRAP::Graphics::API::VulkanInits::AttachmentDescription(const VkFormat format,
	                                                                                          const VkSampleCountFlagBits sampleCount,
	                                                                                          const VkAttachmentLoadOp loadOp,
	                                                                                          const VkAttachmentStoreOp storeOp,
	                                                                                          const VkAttachmentLoadOp stencilLoadOp,
	                                                                                          const VkAttachmentStoreOp stencilStoreOp,
	                                                                                          const VkImageLayout layout,
	                                                                                          const VkImageLayout finalLayout)
{
	TRAP_ASSERT(format != VK_FORMAT_UNDEFINED, "VulkanInits::AttachmentDescription(): Format can't be VK_FORMAT_UNDEFINED!");
	TRAP_ASSERT(finalLayout != VK_IMAGE_LAYOUT_UNDEFINED, "VulkanInits::AttachmentDescription(): FinalLayout can't be VK_IMAGE_LAYOUT_UNDEFINED!");
	TRAP_ASSERT(finalLayout != VK_IMAGE_LAYOUT_PREINITIALIZED, "VulkanInits::AttachmentDescription(): FinalLayout can't be VK_IMAGE_LAYOUT_PREINITIALIZED!");

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

[[nodiscard]] VkAttachmentDescription2KHR TRAP::Graphics::API::VulkanInits::AttachmentDescription2(const VkFormat format,
	                                                                                               const VkSampleCountFlagBits sampleCount,
	                                                                                               const VkAttachmentLoadOp loadOp,
	                                                                                               const VkAttachmentStoreOp storeOp,
	                                                                                               const VkAttachmentLoadOp stencilLoadOp,
	                                                                                               const VkAttachmentStoreOp stencilStoreOp,
	                                                                                               const VkImageLayout layout,
	                                                                                               const VkImageLayout finalLayout)
{
	TRAP_ASSERT(format != VK_FORMAT_UNDEFINED, "VulkanInits::AttachmentDescription2(): Format can't be VK_FORMAT_UNDEFINED!");
	TRAP_ASSERT(finalLayout != VK_IMAGE_LAYOUT_UNDEFINED, "VulkanInits::AttachmentDescription2(): FinalLayout can't be VK_IMAGE_LAYOUT_UNDEFINED!");
	TRAP_ASSERT(finalLayout != VK_IMAGE_LAYOUT_PREINITIALIZED, "VulkanInits::AttachmentDescription2(): FinalLayout can't be VK_IMAGE_LAYOUT_PREINITIALIZED!");

	VkAttachmentDescription2KHR desc{};

	desc.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2_KHR;
	desc.pNext = nullptr;
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

[[nodiscard]] VkRenderPassBeginInfo TRAP::Graphics::API::VulkanInits::RenderPassBeginInfo(VkRenderPass renderPass,
	                                                                                      VkFramebuffer frameBuffer,
	                                                                                      const VkRect2D renderArea,
	                                                                                      const std::vector<VkClearValue>& colorValues)
{
	TRAP_ASSERT(renderPass != VK_NULL_HANDLE, "VulkanInits::RenderPassBeginInfo(): RenderPass can't be VK_NULL_HANDLE!");
	TRAP_ASSERT(frameBuffer != VK_NULL_HANDLE, "VulkanInits::RenderPassBeginInfo(): FrameBuffer can't be VK_NULL_HANDLE!");

	VkRenderPassBeginInfo info{};

	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	info.pNext = nullptr;
	info.renderPass = renderPass;
	info.framebuffer = frameBuffer;
	info.renderArea = renderArea;
	info.clearValueCount = NumericCast<uint32_t>(colorValues.size());
	info.pClearValues = colorValues.data();

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkFramebufferCreateInfo TRAP::Graphics::API::VulkanInits::FramebufferCreateInfo(VkRenderPass renderPass,
	                                                                                          const std::vector<VkImageView>& attachments,
	                                                                                          const uint32_t width,
	                                                                                          const uint32_t height,
	                                                                                          const uint32_t layerCount)
{
	TRAP_ASSERT(width != 0, "VulkanInits::FramebufferCreateInfo(): Width can't be 0!");
	TRAP_ASSERT(height != 0, "VulkanInits::FramebufferCreateInfo(): Height can't be 0!");
	TRAP_ASSERT(layerCount != 0, "VulkanInits::FramebufferCreateInfo(): LayerCount can't be 0!");
	TRAP_ASSERT(renderPass != VK_NULL_HANDLE, "VulkanInits::FramebufferCreateInfo(): RenderPass can't be VK_NULL_HANDLE!");

	VkFramebufferCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.renderPass = renderPass;
	info.attachmentCount = NumericCast<uint32_t>(attachments.size());
	info.pAttachments = attachments.data();
	info.width = width;
	info.height = height;
	info.layers = layerCount;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkImageViewCreateInfo TRAP::Graphics::API::VulkanInits::ImageViewCreateInfo(VkImage image,
	                                                                                      const VkImageViewType imageViewType,
	                                                                                      const VkFormat format,
																			              const uint32_t levelCount,
	                                                                                      const uint32_t layerCount)
{
	TRAP_ASSERT(image != VK_NULL_HANDLE, "VulkanInits::ImageViewCreateInfo(): Image can't be VK_NULL_HANDLE!");

	VkImageViewCreateInfo info{};

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

[[nodiscard]] VkImageCreateInfo TRAP::Graphics::API::VulkanInits::ImageCreateInfo(const VkImageType imageType,
                                                                                  const VkFormat imageFormat, const uint32_t width,
                                                                                  const uint32_t height, const uint32_t depth,
                                                                                  const uint32_t mipLevels,
																	              const uint32_t arrayLayers,
																	              const VkSampleCountFlagBits sampleCount,
																	              const VkImageTiling tiling,
																	              const VkImageUsageFlags usage)
{
	TRAP_ASSERT(width != 0, "VulkanInits::ImageCreateInfo(): Width can't be 0!");
	TRAP_ASSERT(height != 0, "VulkanInits::ImageCreateInfo(): Height can't be 0!");
	TRAP_ASSERT(depth != 0, "VulkanInits::ImageCreateInfo(): Depth can't be 0!");
	TRAP_ASSERT(mipLevels != 0, "VulkanInits::ImageCreateInfo(): MipLevels can't be 0!");
	TRAP_ASSERT(arrayLayers != 0, "VulkanInits::ImageCreateInfo(): ArrayLayers can't be 0!");

	VkImageCreateInfo info{};

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

[[nodiscard]] VkBufferImageCopy TRAP::Graphics::API::VulkanInits::ImageCopy(const uint32_t bufferRowLength, const uint32_t width,
															                const uint32_t height, const uint32_t depth,
							    							                const VkImageSubresourceLayers layers)
{
	TRAP_ASSERT(width != 0, "VulkanInits::ImageCopy(): Width can't be 0!");
	TRAP_ASSERT(height != 0, "VulkanInits::ImageCopy(): Height can't be 0!");
	TRAP_ASSERT(depth != 0, "VulkanInits::ImageCopy(): Depth can't be 0!");

	VkBufferImageCopy copy{};

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

[[nodiscard]] VkBufferImageCopy TRAP::Graphics::API::VulkanInits::ImageCopy(const uint64_t bufferOffset, const uint32_t bufferRowLength,
 															                const uint32_t bufferImageHeight, const uint32_t width,
															                const uint32_t height, const uint32_t depth,
							    							                const VkImageSubresourceLayers layers)
{
	TRAP_ASSERT(width != 0, "VulkanInits::ImageCopy(): Width can't be 0!");
	TRAP_ASSERT(height != 0, "VulkanInits::ImageCopy(): Height can't be 0!");
	TRAP_ASSERT(depth != 0, "VulkanInits::ImageCopy(): Depth can't be 0!");

	VkBufferImageCopy copy{};

	copy.bufferOffset = bufferOffset;
	copy.bufferRowLength = bufferRowLength;
	copy.bufferImageHeight = bufferImageHeight;
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

[[nodiscard]] VkBufferCreateInfo TRAP::Graphics::API::VulkanInits::BufferCreateInfo(const uint64_t allocationSize,
                                                                                    const VkBufferUsageFlags usageFlags)
{
	TRAP_ASSERT(allocationSize != 0, "VulkanInits::BufferCreateInfo(): AllocationSize can't be 0!");
	TRAP_ASSERT(usageFlags != 0, "VulkanInits::BufferCreateInfo(): UsageFlags can't be 0!");

	VkBufferCreateInfo info{};

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

[[nodiscard]] VkBufferViewCreateInfo TRAP::Graphics::API::VulkanInits::BufferViewCreateInfo(VkBuffer buffer,
                                                                                            const VkFormat format,
	                                                                                        const uint64_t offset,
	                                                                                        const uint64_t range)
{
	TRAP_ASSERT(buffer != VK_NULL_HANDLE, "VulkanInits::BufferViewCreateInfo(): Buffer can't be VK_NULL_HANDLE!");

	VkBufferViewCreateInfo info{};

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

[[nodiscard]] VkCommandBufferAllocateInfo TRAP::Graphics::API::VulkanInits::CommandBufferAllocateInfo(VkCommandPool commandPool,
                                                                                                      const bool secondary)
{
	TRAP_ASSERT(commandPool != VK_NULL_HANDLE, "VulkanInits::CommandBufferAllocateInfo(): CommandPool can't be VK_NULL_HANDLE!");

	VkCommandBufferAllocateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	info.pNext = nullptr;
	info.commandPool = commandPool;
	info.level = secondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY : VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	info.commandBufferCount = 1;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkSamplerCreateInfo TRAP::Graphics::API::VulkanInits::SamplerCreateInfo(const VkFilter magFilter,
	                                                                                  const VkFilter minFilter,
	                                                                                  const VkSamplerMipmapMode mipMapMode,
	                                                                                  const VkSamplerAddressMode u,
	                                                                                  const VkSamplerAddressMode v,
	                                                                                  const VkSamplerAddressMode w,
	                                                                                  const float mipLodBias,
																		              const float minLod,
																		              const float maxLod,
	                                                                                  const float maxAnisotropy,
	                                                                                  const VkCompareOp compareOp)
{
	TRAP_ASSERT(minLod <= maxLod, "VulkanInits::SamplerCreateInfo(): minLod can't be greater than maxLod!");

	VkSamplerCreateInfo info{};

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

[[nodiscard]] VkPipelineShaderStageCreateInfo TRAP::Graphics::API::VulkanInits::PipelineShaderStageCreateInfo(const VkShaderStageFlagBits stage,
	                                                                                                          VkShaderModule module,
	                                                                                                          const std::string_view name)
{
	TRAP_ASSERT(!name.empty(), "VulkanInits::PipelineShaderStageCreateInfo(): Shader name can not be empty!");

	VkPipelineShaderStageCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.stage = stage;
	info.module = module;
	info.pName = name.data();
	info.pSpecializationInfo = nullptr;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkComputePipelineCreateInfo TRAP::Graphics::API::VulkanInits::ComputePipelineCreateInfo(const VkPipelineShaderStageCreateInfo& stage,
	                                                                                                  VkPipelineLayout layout)
{
	TRAP_ASSERT(layout, "VulkanInits::ComputePipelineCreateInfo(): Invalid Pipeline Layout!");

	VkComputePipelineCreateInfo info{};

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

[[nodiscard]] VkSwapchainCreateInfoKHR TRAP::Graphics::API::VulkanInits::SwapchainCreateInfoKHR(VkSurfaceKHR surface,
	                                                                                            const uint32_t imageCount,
	                                                                                            const VkSurfaceFormatKHR surfaceFormat,
	                                                                                            const VkExtent2D imageExtent,
	                                                                                            const VkSharingMode sharingMode,
	                                                                                            const uint32_t queueFamilyIndexCount,
	                                                                                            const std::array<uint32_t, 2>& queueFamilyIndices,
	                                                                                            const VkSurfaceTransformFlagBitsKHR preTransform,
	                                                                                            const VkCompositeAlphaFlagBitsKHR compositeAlpha,
	                                                                                            const VkPresentModeKHR presentMode)
{
	TRAP_ASSERT(surface, "VulkanInits::SwapchainCreateInfoKHR(): Invalid Surface!");

	VkSwapchainCreateInfoKHR info{};

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

[[nodiscard]] VkDebugUtilsObjectNameInfoEXT TRAP::Graphics::API::VulkanInits::DebugUtilsObjectNameInfo(const VkObjectType type,
	                                                                                                   const uint64_t handle,
	                                                                                                   const std::string_view name)
{
	TRAP_ASSERT(!name.empty(), "VulkanInits::DebugUtilsObjectNameInfo(): Name can't be empty!");
	TRAP_ASSERT(!(type == VK_OBJECT_TYPE_UNKNOWN && handle == 0), "VulkanInits::DebugUtilsObjectNameInfo(): Type and Handle can't be VK_OBJECT_TYPE_UNKNOWN and VK_NULL_HANDLE!");

	VkDebugUtilsObjectNameInfoEXT info{};

	info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	info.pNext = nullptr;
	info.objectType = type;
	info.objectHandle = handle;
	info.pObjectName = name.data();

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkDebugMarkerObjectNameInfoEXT TRAP::Graphics::API::VulkanInits::DebugMarkerObjectNameInfo(const VkDebugReportObjectTypeEXT type,
	                                                                                                     const uint64_t handle,
	                                                                                                     const std::string_view name)
{
	TRAP_ASSERT(!name.empty(), "VulkanInits::DebugMarkerObjectNameInfo(): Name can't be empty!");
	TRAP_ASSERT(type != VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT, "VulkanInits::DebugMarkerObjectNameInfo(): Type can't be VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT!");
	TRAP_ASSERT(handle != 0, "VulkanInits::DebugMarkerObjectNameInfo(): Handle can't be VK_NULL_HANDLE!");

	VkDebugMarkerObjectNameInfoEXT info{};

	info.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT;
	info.pNext = nullptr;
	info.objectType = type;
	info.object = handle;
	info.pObjectName = name.data();

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkDebugUtilsLabelEXT TRAP::Graphics::API::VulkanInits::DebugUtilsLabelExt(const float r, const float g,
                                                                                        const float b, const std::string_view name)
{
	TRAP_ASSERT(!name.empty(), "VulkanInits::DebugUtilsLabelExt(): Name can't be empty!");

	VkDebugUtilsLabelEXT info{};

	info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
	info.color[0] = r;
	info.color[1] = g;
	info.color[2] = b;
	info.color[3] = 1.0f;
	info.pLabelName = name.data();

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkDebugMarkerMarkerInfoEXT TRAP::Graphics::API::VulkanInits::DebugMarkerMarkerInfo(const float r, const float g,
                                                                                                 const float b, const std::string_view name)
{
	TRAP_ASSERT(!name.empty(), "VulkanInits::DebugMarkerMarkerInfo(): Name can't be empty!");

	VkDebugMarkerMarkerInfoEXT info{};

	info.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
	info.color[0] = r;
	info.color[1] = g;
	info.color[2] = b;
	info.color[3] = 1.0f;
	info.pMarkerName = name.data();

	return info;
}
