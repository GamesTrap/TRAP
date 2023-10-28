#include "TRAPPCH.h"
#include "VulkanInits.h"

#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

[[nodiscard]] VkApplicationInfo TRAP::Graphics::API::VulkanInits::ApplicationInfo(const std::string_view appName)
{
	TRAP_ASSERT(!appName.empty(), "VulkanInits::ApplicationInfo(): Application name can't be empty!");

	return VkApplicationInfo
	{
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext = nullptr,
		.pApplicationName = appName.data(),
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "TRAP",
		.engineVersion = TRAP_VERSION,
		.apiVersion = VK_API_VERSION_1_1
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkDebugUtilsMessengerCreateInfoEXT TRAP::Graphics::API::VulkanInits::DebugUtilsMessengerCreateInfo(const PFN_vkDebugUtilsMessengerCallbackEXT callback)
{
	TRAP_ASSERT(callback, "VulkanInits::DebugUtilsMessengerCreateInfo(): Callback can't be nullptr!");

	return VkDebugUtilsMessengerCreateInfoEXT
	{
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.pNext = nullptr,
		.flags = 0,
		.messageSeverity = static_cast<u32>(VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) |
						   static_cast<u32>(VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) |
						   static_cast<u32>(VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT),
		.messageType = static_cast<u32>(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) |
					   static_cast<u32>(VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) |
					   static_cast<u32>(VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT),
		.pfnUserCallback = callback,
		.pUserData = nullptr
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkDebugReportCallbackCreateInfoEXT TRAP::Graphics::API::VulkanInits::DebugReportCallbackCreateInfo(const PFN_vkDebugReportCallbackEXT callback)
{
	TRAP_ASSERT(callback, "VulkanInits::DebugReportCallbackCreateInfo(): Callback can't be nullptr!");

	return VkDebugReportCallbackCreateInfoEXT
	{
		.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
		.pNext = nullptr,
		.flags = static_cast<u32>(VK_DEBUG_REPORT_WARNING_BIT_EXT) | static_cast<u32>(VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) |
				 static_cast<u32>(VK_DEBUG_REPORT_ERROR_BIT_EXT) | static_cast<u32>(VK_DEBUG_REPORT_DEBUG_BIT_EXT),
		.pfnCallback = callback,
		.pUserData = nullptr
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkDeviceCreateInfo TRAP::Graphics::API::VulkanInits::DeviceCreateInfo(const void* const pNext,
                                                                                    const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos,
																	                const std::vector<const char*>& deviceExtensions)
{
	TRAP_ASSERT(!queueCreateInfos.empty(), "VulkanInits::DeviceCreateInfo(): QueueCreateInfos can't be empty!");

	return VkDeviceCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = pNext,
		.flags = 0,
		.queueCreateInfoCount = NumericCast<u32>(queueCreateInfos.size()),
		.pQueueCreateInfos = queueCreateInfos.data(),
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = nullptr,
		.enabledExtensionCount = NumericCast<u32>(deviceExtensions.size()),
		.ppEnabledExtensionNames = deviceExtensions.data(),
		.pEnabledFeatures = nullptr
	};
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

	VmaAllocatorCreateInfo info
	{
		.flags = VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT,
		.physicalDevice = physicalDevice,
		.device = device,
		.preferredLargeHeapBlockSize = 0,
		.pAllocationCallbacks = nullptr,
		.pDeviceMemoryCallbacks = nullptr,
		.pHeapSizeLimit = nullptr,
		.pVulkanFunctions = &vulkanFunctions,
		.instance = instance,
		.vulkanApiVersion = VK_API_VERSION_1_1,
#if VMA_EXTERNAL_MEMORY
		.pTypeExternalMemoryHandleTypes = nullptr
#endif
	};

	if(VulkanRenderer::s_bufferDeviceAddressExtension)
		info.flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkDescriptorPoolCreateInfo TRAP::Graphics::API::VulkanInits::DescriptorPoolCreateInfo(const std::vector<VkDescriptorPoolSize>& descriptorPoolSizes,
                                                                                                    const u32 numDescriptorSets)
{
	TRAP_ASSERT(numDescriptorSets != 0, "VulkanInits::DescriptorPoolCreateInfo(): NumDescriptorSets can't be 0!");

	return VkDescriptorPoolCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.maxSets = numDescriptorSets,
		.poolSizeCount = NumericCast<u32>(descriptorPoolSizes.size()),
		.pPoolSizes = descriptorPoolSizes.data()
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkDescriptorSetAllocateInfo TRAP::Graphics::API::VulkanInits::DescriptorSetAllocateInfo(VkDescriptorPool descriptorPool,
	                                                                                                  const VkDescriptorSetLayout& descriptorLayout)
{
	TRAP_ASSERT(descriptorPool != VK_NULL_HANDLE, "VulkanInits::DescriptorSetAllocateInfo(): DescriptorPool can't be VK_NULL_HANDLE!");
	TRAP_ASSERT(descriptorLayout != VK_NULL_HANDLE, "VulkanInits::DescriptorSetAllocateInfo(): DescriptorLayout can't be VK_NULL_HANDLE!");

	return VkDescriptorSetAllocateInfo
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = nullptr,
		.descriptorPool = descriptorPool,
		.descriptorSetCount = 1,
		.pSetLayouts = &descriptorLayout
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkDescriptorUpdateTemplateCreateInfo TRAP::Graphics::API::VulkanInits::DescriptorUpdateTemplateCreateInfo(VkDescriptorSetLayout descriptorSetLayout,
	                                                                                                                    const u32 entryCount,
	                                                                                                                    const VkDescriptorUpdateTemplateEntry* const entries,
	                                                                                                                    const VkPipelineBindPoint bindPoint,
	                                                                                                                    VkPipelineLayout pipelineLayout,
	                                                                                                                    const u32 setIndex)
{
	TRAP_ASSERT(descriptorSetLayout != VK_NULL_HANDLE, "VulkanInits::DescriptorUpdateTemplateCreateInfo(): DescriptorSetLayout can't be VK_NULL_HANDLE!");
	TRAP_ASSERT(entryCount != 0, "VulkanInits::DescriptorUpdateTemplateCreateInfo(): EntryCount can't be 0!");
	TRAP_ASSERT(entries, "VulkanInits::DescriptorUpdateTemplateCreateInfo(): Entries can't be nullptr!");

	return VkDescriptorUpdateTemplateCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.descriptorUpdateEntryCount = entryCount,
		.pDescriptorUpdateEntries = entries,
		.templateType = VK_DESCRIPTOR_UPDATE_TEMPLATE_TYPE_DESCRIPTOR_SET,
		.descriptorSetLayout = descriptorSetLayout,
		.pipelineBindPoint = bindPoint,
		.pipelineLayout = pipelineLayout,
		.set = setIndex
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkSemaphoreWaitInfoKHR TRAP::Graphics::API::VulkanInits::SemaphoreWaitInfo(VkSemaphore& semaphore, u64& value)
{
	TRAP_ASSERT(semaphore != VK_NULL_HANDLE, "VulkanInits::SemaphoreWaitInfo(): Semaphore can't be VK_NULL_HANDLE!");

	return VkSemaphoreWaitInfoKHR
	{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO_KHR,
		.pNext = nullptr,
		.flags = 0,
		.semaphoreCount = 1,
		.pSemaphores = &semaphore,
		.pValues = &value
	};
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

	return VkFragmentShadingRateAttachmentInfoKHR
	{
		.sType = VK_STRUCTURE_TYPE_FRAGMENT_SHADING_RATE_ATTACHMENT_INFO_KHR,
		.pNext = nullptr,
		.pFragmentShadingRateAttachment = &shadingRateAttachment,
		.shadingRateAttachmentTexelSize = texelSize
	};
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

	return VkAttachmentDescription
	{
		.flags = 0,
		.format = format,
		.samples = sampleCount,
		.loadOp = loadOp,
		.storeOp = storeOp,
		.stencilLoadOp = stencilLoadOp,
		.stencilStoreOp = stencilStoreOp,
		.initialLayout = layout,
		.finalLayout = finalLayout
	};
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

	return VkAttachmentDescription2KHR
	{
		.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2_KHR,
		.pNext = nullptr,
		.flags = 0,
		.format = format,
		.samples = sampleCount,
		.loadOp = loadOp,
		.storeOp = storeOp,
		.stencilLoadOp = stencilLoadOp,
		.stencilStoreOp = stencilStoreOp,
		.initialLayout = layout,
		.finalLayout = finalLayout
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkRenderPassBeginInfo TRAP::Graphics::API::VulkanInits::RenderPassBeginInfo(VkRenderPass renderPass,
	                                                                                      VkFramebuffer frameBuffer,
	                                                                                      const VkRect2D renderArea,
	                                                                                      const std::vector<VkClearValue>& colorValues)
{
	TRAP_ASSERT(renderPass != VK_NULL_HANDLE, "VulkanInits::RenderPassBeginInfo(): RenderPass can't be VK_NULL_HANDLE!");
	TRAP_ASSERT(frameBuffer != VK_NULL_HANDLE, "VulkanInits::RenderPassBeginInfo(): FrameBuffer can't be VK_NULL_HANDLE!");

	return VkRenderPassBeginInfo
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext = nullptr,
		.renderPass = renderPass,
		.framebuffer = frameBuffer,
		.renderArea = renderArea,
		.clearValueCount = NumericCast<u32>(colorValues.size()),
		.pClearValues = colorValues.data()
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkFramebufferCreateInfo TRAP::Graphics::API::VulkanInits::FramebufferCreateInfo(VkRenderPass renderPass,
	                                                                                          const std::vector<VkImageView>& attachments,
	                                                                                          const u32 width,
	                                                                                          const u32 height,
	                                                                                          const u32 layerCount)
{
	TRAP_ASSERT(width != 0, "VulkanInits::FramebufferCreateInfo(): Width can't be 0!");
	TRAP_ASSERT(height != 0, "VulkanInits::FramebufferCreateInfo(): Height can't be 0!");
	TRAP_ASSERT(layerCount != 0, "VulkanInits::FramebufferCreateInfo(): LayerCount can't be 0!");
	TRAP_ASSERT(renderPass != VK_NULL_HANDLE, "VulkanInits::FramebufferCreateInfo(): RenderPass can't be VK_NULL_HANDLE!");

	return VkFramebufferCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.renderPass = renderPass,
		.attachmentCount = NumericCast<u32>(attachments.size()),
		.pAttachments = attachments.data(),
		.width = width,
		.height = height,
		.layers = layerCount
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkImageViewCreateInfo TRAP::Graphics::API::VulkanInits::ImageViewCreateInfo(VkImage image,
	                                                                                      const VkImageViewType imageViewType,
	                                                                                      const VkFormat format,
																			              const u32 levelCount,
	                                                                                      const u32 layerCount)
{
	TRAP_ASSERT(image != VK_NULL_HANDLE, "VulkanInits::ImageViewCreateInfo(): Image can't be VK_NULL_HANDLE!");

	static constexpr VkComponentMapping componentMapping
	{
        .r = VK_COMPONENT_SWIZZLE_R,
		.g = VK_COMPONENT_SWIZZLE_G,
		.b = VK_COMPONENT_SWIZZLE_B,
		.a = VK_COMPONENT_SWIZZLE_A
	};

	const VkImageSubresourceRange subresourceRange
	{
        .aspectMask = DetermineAspectMask(format, true),
		.baseMipLevel = 0,
		.levelCount = levelCount,
		.baseArrayLayer = 0,
		.layerCount = layerCount
	};

	return VkImageViewCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.image = image,
		.viewType = imageViewType,
		.format = format,
		.components = componentMapping,
		.subresourceRange = subresourceRange
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkImageCreateInfo TRAP::Graphics::API::VulkanInits::ImageCreateInfo(const VkImageType imageType,
                                                                                  const VkFormat imageFormat, const u32 width,
                                                                                  const u32 height, const u32 depth,
                                                                                  const u32 mipLevels,
																	              const u32 arrayLayers,
																	              const VkSampleCountFlagBits sampleCount,
																	              const VkImageTiling tiling,
																	              const VkImageUsageFlags usage)
{
	TRAP_ASSERT(width != 0, "VulkanInits::ImageCreateInfo(): Width can't be 0!");
	TRAP_ASSERT(height != 0, "VulkanInits::ImageCreateInfo(): Height can't be 0!");
	TRAP_ASSERT(depth != 0, "VulkanInits::ImageCreateInfo(): Depth can't be 0!");
	TRAP_ASSERT(mipLevels != 0, "VulkanInits::ImageCreateInfo(): MipLevels can't be 0!");
	TRAP_ASSERT(arrayLayers != 0, "VulkanInits::ImageCreateInfo(): ArrayLayers can't be 0!");

	const VkExtent3D extent
	{
		.width = width,
		.height = height,
		.depth = depth
	};

	return VkImageCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.imageType = imageType,
		.format = imageFormat,
		.extent = extent,
		.mipLevels = mipLevels,
		.arrayLayers = arrayLayers,
		.samples = sampleCount,
		.tiling = tiling,
		.usage = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkBufferImageCopy TRAP::Graphics::API::VulkanInits::ImageCopy(const u32 bufferRowLength, const u32 width,
															                const u32 height, const u32 depth,
							    							                const VkImageSubresourceLayers layers)
{
	TRAP_ASSERT(width != 0, "VulkanInits::ImageCopy(): Width can't be 0!");
	TRAP_ASSERT(height != 0, "VulkanInits::ImageCopy(): Height can't be 0!");
	TRAP_ASSERT(depth != 0, "VulkanInits::ImageCopy(): Depth can't be 0!");

	static constexpr VkOffset3D offset
	{
		.x = 0,
		.y = 0,
		.z = 0
	};

	const VkExtent3D extent
	{
		.width = width,
		.height = height,
		.depth = depth
	};

	return VkBufferImageCopy
	{
		.bufferOffset = 0,
		.bufferRowLength = bufferRowLength,
		.bufferImageHeight = 0,
		.imageSubresource = layers,
		.imageOffset = offset,
		.imageExtent = extent
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkBufferImageCopy TRAP::Graphics::API::VulkanInits::ImageCopy(const u64 bufferOffset, const u32 bufferRowLength,
 															                const u32 bufferImageHeight, const u32 width,
															                const u32 height, const u32 depth,
							    							                const VkImageSubresourceLayers layers)
{
	TRAP_ASSERT(width != 0, "VulkanInits::ImageCopy(): Width can't be 0!");
	TRAP_ASSERT(height != 0, "VulkanInits::ImageCopy(): Height can't be 0!");
	TRAP_ASSERT(depth != 0, "VulkanInits::ImageCopy(): Depth can't be 0!");

	static constexpr VkOffset3D offset
	{
		.x = 0,
		.y = 0,
		.z = 0
	};

	const VkExtent3D extent
	{
		.width = width,
		.height = height,
		.depth = depth
	};

	return VkBufferImageCopy
	{
		.bufferOffset = bufferOffset,
		.bufferRowLength = bufferRowLength,
		.bufferImageHeight = bufferImageHeight,
		.imageSubresource = layers,
		.imageOffset = offset,
		.imageExtent = extent
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkBufferCreateInfo TRAP::Graphics::API::VulkanInits::BufferCreateInfo(const u64 allocationSize,
                                                                                    const VkBufferUsageFlags usageFlags)
{
	TRAP_ASSERT(allocationSize != 0, "VulkanInits::BufferCreateInfo(): AllocationSize can't be 0!");
	TRAP_ASSERT(usageFlags != 0, "VulkanInits::BufferCreateInfo(): UsageFlags can't be 0!");

	return VkBufferCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.size = allocationSize,
		.usage = usageFlags,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkBufferViewCreateInfo TRAP::Graphics::API::VulkanInits::BufferViewCreateInfo(VkBuffer buffer,
                                                                                            const VkFormat format,
	                                                                                        const u64 offset,
	                                                                                        const u64 range)
{
	TRAP_ASSERT(buffer != VK_NULL_HANDLE, "VulkanInits::BufferViewCreateInfo(): Buffer can't be VK_NULL_HANDLE!");

	return VkBufferViewCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.buffer = buffer,
		.format = format,
		.offset = offset,
		.range = range
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkCommandBufferAllocateInfo TRAP::Graphics::API::VulkanInits::CommandBufferAllocateInfo(VkCommandPool commandPool,
                                                                                                      const bool secondary)
{
	TRAP_ASSERT(commandPool != VK_NULL_HANDLE, "VulkanInits::CommandBufferAllocateInfo(): CommandPool can't be VK_NULL_HANDLE!");

	return VkCommandBufferAllocateInfo
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = nullptr,
		.commandPool = commandPool,
		.level = secondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY : VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkPipelineShaderStageCreateInfo TRAP::Graphics::API::VulkanInits::PipelineShaderStageCreateInfo(const VkShaderStageFlagBits stage,
	                                                                                                          VkShaderModule module,
	                                                                                                          const std::string_view name)
{
	TRAP_ASSERT(!name.empty(), "VulkanInits::PipelineShaderStageCreateInfo(): Shader name can not be empty!");

	return VkPipelineShaderStageCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.stage = stage,
		.module = module,
		.pName = name.data(),
		.pSpecializationInfo = nullptr
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkComputePipelineCreateInfo TRAP::Graphics::API::VulkanInits::ComputePipelineCreateInfo(const VkPipelineShaderStageCreateInfo& stage,
	                                                                                                  VkPipelineLayout layout)
{
	TRAP_ASSERT(layout, "VulkanInits::ComputePipelineCreateInfo(): Invalid Pipeline Layout!");

	return VkComputePipelineCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.stage = stage,
		.layout = layout,
		.basePipelineHandle = nullptr,
		.basePipelineIndex = 0
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkSwapchainCreateInfoKHR TRAP::Graphics::API::VulkanInits::SwapchainCreateInfoKHR(VkSurfaceKHR surface,
	                                                                                            const u32 imageCount,
	                                                                                            const VkSurfaceFormatKHR surfaceFormat,
	                                                                                            const VkExtent2D imageExtent,
	                                                                                            const VkSharingMode sharingMode,
	                                                                                            const std::vector<u32>& queueFamilyIndices,
	                                                                                            const VkSurfaceTransformFlagBitsKHR preTransform,
	                                                                                            const VkCompositeAlphaFlagBitsKHR compositeAlpha,
	                                                                                            const VkPresentModeKHR presentMode,
																								VkSwapchainKHR oldSwapchain)
{
	TRAP_ASSERT(surface, "VulkanInits::SwapchainCreateInfoKHR(): Invalid Surface!");

	return VkSwapchainCreateInfoKHR
	{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext = nullptr,
		.flags = 0,
		.surface = surface,
		.minImageCount = imageCount,
		.imageFormat = surfaceFormat.format,
		.imageColorSpace = surfaceFormat.colorSpace,
		.imageExtent = imageExtent,
		.imageArrayLayers = 1,
		.imageUsage = static_cast<u32>(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) |
		              static_cast<u32>(VK_IMAGE_USAGE_TRANSFER_SRC_BIT) |
					  static_cast<u32>(VK_IMAGE_USAGE_TRANSFER_DST_BIT),
		.imageSharingMode = sharingMode,
		.queueFamilyIndexCount = NumericCast<u32>(queueFamilyIndices.size()),
		.pQueueFamilyIndices = queueFamilyIndices.data(),
		.preTransform = preTransform,
		.compositeAlpha = compositeAlpha,
		.presentMode = presentMode,
		.clipped = VK_TRUE,
		.oldSwapchain = oldSwapchain
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkDebugUtilsObjectNameInfoEXT TRAP::Graphics::API::VulkanInits::DebugUtilsObjectNameInfo(const VkObjectType type,
	                                                                                                   const u64 handle,
	                                                                                                   const std::string_view name)
{
	TRAP_ASSERT(!name.empty(), "VulkanInits::DebugUtilsObjectNameInfo(): Name can't be empty!");
	TRAP_ASSERT(!(type == VK_OBJECT_TYPE_UNKNOWN && handle == 0), "VulkanInits::DebugUtilsObjectNameInfo(): Type and Handle can't be VK_OBJECT_TYPE_UNKNOWN and VK_NULL_HANDLE!");

	return VkDebugUtilsObjectNameInfoEXT
	{
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
		.pNext = nullptr,
		.objectType = type,
		.objectHandle = handle,
		.pObjectName = name.data()
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkDebugMarkerObjectNameInfoEXT TRAP::Graphics::API::VulkanInits::DebugMarkerObjectNameInfo(const VkDebugReportObjectTypeEXT type,
	                                                                                                     const u64 handle,
	                                                                                                     const std::string_view name)
{
	TRAP_ASSERT(!name.empty(), "VulkanInits::DebugMarkerObjectNameInfo(): Name can't be empty!");
	TRAP_ASSERT(type != VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT, "VulkanInits::DebugMarkerObjectNameInfo(): Type can't be VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT!");
	TRAP_ASSERT(handle != 0, "VulkanInits::DebugMarkerObjectNameInfo(): Handle can't be VK_NULL_HANDLE!");

	return VkDebugMarkerObjectNameInfoEXT
	{
		.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT,
		.pNext = nullptr,
		.objectType = type,
		.object = handle,
		.pObjectName = name.data()
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkDebugUtilsLabelEXT TRAP::Graphics::API::VulkanInits::DebugUtilsLabelExt(const f32 r, const f32 g,
                                                                                        const f32 b, const std::string_view name)
{
	TRAP_ASSERT(!name.empty(), "VulkanInits::DebugUtilsLabelExt(): Name can't be empty!");

	return VkDebugUtilsLabelEXT
	{
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
		.pNext = nullptr,
		.pLabelName = name.data(),
		.color = {r, g, b, 1.0f}
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkDebugMarkerMarkerInfoEXT TRAP::Graphics::API::VulkanInits::DebugMarkerMarkerInfo(const f32 r, const f32 g,
                                                                                                 const f32 b, const std::string_view name)
{
	TRAP_ASSERT(!name.empty(), "VulkanInits::DebugMarkerMarkerInfo(): Name can't be empty!");

	return VkDebugMarkerMarkerInfoEXT
	{
		.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT,
		.pNext = nullptr,
		.pMarkerName = name.data(),
		.color = {r, g, b, 1.0f},
	};
}
