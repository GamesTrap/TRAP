#include "TRAPPCH.h"
#include "VulkanInits.h"

#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

[[nodiscard]] VkApplicationInfo TRAP::Graphics::API::VulkanInits::ApplicationInfo(const std::string_view appName)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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

[[nodiscard]] VkInstanceCreateInfo TRAP::Graphics::API::VulkanInits::InstanceCreateInfo(const VkApplicationInfo& appInfo,
	                                                                                    const std::vector<const char*>& instanceLayers,
	                                                                                    const std::vector<const char*>& instanceExtensions) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkInstanceCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.pApplicationInfo = &appInfo;
	info.enabledLayerCount = NumericCast<uint32_t>(instanceLayers.size());
	info.ppEnabledLayerNames = instanceLayers.data();
	info.enabledExtensionCount = NumericCast<uint32_t>(instanceExtensions.size());
	info.ppEnabledExtensionNames = instanceExtensions.data();

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkDebugUtilsMessengerCreateInfoEXT TRAP::Graphics::API::VulkanInits::DebugUtilsMessengerCreateInfo(const PFN_vkDebugUtilsMessengerCallbackEXT callback)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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

[[nodiscard]] VkDebugUtilsObjectNameInfoEXT TRAP::Graphics::API::VulkanInits::DebugUtilsObjectNameInfo(const VkObjectType type,
	                                                                                                   const uint64_t handle,
	                                                                                                   const std::string_view name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkDeviceCreateInfo TRAP::Graphics::API::VulkanInits::DeviceCreateInfo(const void* const pNext,
                                                                                    const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos,
																	                const std::vector<const char*>& deviceExtensions)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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

[[nodiscard]] VkDescriptorSetLayoutCreateInfo TRAP::Graphics::API::VulkanInits::DescriptorSetLayoutCreateInfo(const std::vector<VkDescriptorSetLayoutBinding>& bindings) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkDescriptorSetLayoutCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.bindingCount = NumericCast<uint32_t>(bindings.size());
	info.pBindings = bindings.data();

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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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

[[nodiscard]] VkFenceCreateInfo TRAP::Graphics::API::VulkanInits::FenceCreateInfo() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkFenceCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkSemaphoreCreateInfo TRAP::Graphics::API::VulkanInits::SemaphoreCreateInfo() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkSemaphoreCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkSemaphoreWaitInfoKHR TRAP::Graphics::API::VulkanInits::SemaphoreWaitInfo(VkSemaphore& semaphore, uint64_t& value)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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

[[nodiscard]] VkSubpassDescription TRAP::Graphics::API::VulkanInits::SubPassDescription(const VkPipelineBindPoint bindPoint,
	                                                                                    const std::vector<VkAttachmentReference>& inputAttachments,
	                                                                                    const std::vector<VkAttachmentReference>& colorAttachments,
	                                                                                    VkAttachmentReference& depthStencilAttachment) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkSubpassDescription subpass{};

	subpass.flags = 0;
	subpass.pipelineBindPoint = bindPoint;
	subpass.inputAttachmentCount = NumericCast<uint32_t>(inputAttachments.size());
	subpass.pInputAttachments = inputAttachments.data();
	subpass.colorAttachmentCount = NumericCast<uint32_t>(colorAttachments.size());
	subpass.pColorAttachments = colorAttachments.data();
	subpass.pResolveAttachments = nullptr;
	subpass.pDepthStencilAttachment = &depthStencilAttachment;
	subpass.preserveAttachmentCount = 0;
	subpass.pPreserveAttachments = nullptr;

	return subpass;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkSubpassDescription TRAP::Graphics::API::VulkanInits::SubPassDescription(const VkPipelineBindPoint bindPoint,
	                                                                                    const std::vector<VkAttachmentReference>& inputAttachments,
	                                                                                    const std::vector<VkAttachmentReference>& colorAttachments) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkSubpassDescription subpass{};

	subpass.flags = 0;
	subpass.pipelineBindPoint = bindPoint;
	subpass.inputAttachmentCount = NumericCast<uint32_t>(inputAttachments.size());
	subpass.pInputAttachments = inputAttachments.data();
	subpass.colorAttachmentCount = NumericCast<uint32_t>(colorAttachments.size());
	subpass.pColorAttachments = colorAttachments.data();
	subpass.pResolveAttachments = nullptr;
	subpass.pDepthStencilAttachment = nullptr;
	subpass.preserveAttachmentCount = 0;
	subpass.pPreserveAttachments = nullptr;

	return subpass;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkSubpassDescription2KHR TRAP::Graphics::API::VulkanInits::SubPassDescription(const VkPipelineBindPoint bindPoint,
	                                                                                        const std::vector<VkAttachmentReference2KHR>& inputAttachments,
	                                                                                        const std::vector<VkAttachmentReference2KHR>& colorAttachments,
	                                                                                        VkAttachmentReference2KHR& depthStencilAttachment,
												                                            const VkFragmentShadingRateAttachmentInfoKHR* const shadingRateAttachment) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkSubpassDescription2KHR subpass{};

	subpass.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2_KHR;
	subpass.pNext = shadingRateAttachment != nullptr ? shadingRateAttachment : nullptr;
	subpass.flags = 0;
	subpass.pipelineBindPoint = bindPoint;
	subpass.viewMask = 0;
	subpass.inputAttachmentCount = NumericCast<uint32_t>(inputAttachments.size());
	subpass.pInputAttachments = inputAttachments.data();
	subpass.colorAttachmentCount = NumericCast<uint32_t>(colorAttachments.size());
	subpass.pColorAttachments = colorAttachments.data();
	subpass.pResolveAttachments = nullptr;
	subpass.pDepthStencilAttachment = &depthStencilAttachment;
	subpass.preserveAttachmentCount = 0;
	subpass.pPreserveAttachments = nullptr;

	return subpass;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkSubpassDescription2KHR TRAP::Graphics::API::VulkanInits::SubPassDescription(const VkPipelineBindPoint bindPoint,
	                                                                                        const std::vector<VkAttachmentReference2KHR>& inputAttachments,
	                                                                                        const std::vector<VkAttachmentReference2KHR>& colorAttachments,
																			                const VkFragmentShadingRateAttachmentInfoKHR* const shadingRateAttachment) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkSubpassDescription2KHR subpass{};

	subpass.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2_KHR;
	subpass.pNext = shadingRateAttachment != nullptr ? shadingRateAttachment : nullptr;
	subpass.flags = 0;
	subpass.pipelineBindPoint = bindPoint;
	subpass.viewMask = 0;
	subpass.inputAttachmentCount = NumericCast<uint32_t>(inputAttachments.size());
	subpass.pInputAttachments = inputAttachments.data();
	subpass.colorAttachmentCount = NumericCast<uint32_t>(colorAttachments.size());
	subpass.pColorAttachments = colorAttachments.data();
	subpass.pResolveAttachments = nullptr;
	subpass.pDepthStencilAttachment = nullptr;
	subpass.preserveAttachmentCount = 0;
	subpass.pPreserveAttachments = nullptr;

	return subpass;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkRenderPassCreateInfo TRAP::Graphics::API::VulkanInits::RenderPassCreateInfo(const std::vector<VkAttachmentDescription>& attachmentDescriptions,
	                                                                                        const VkSubpassDescription& subpassDescription) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkRenderPassCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.attachmentCount = NumericCast<uint32_t>(attachmentDescriptions.size());
	info.pAttachments = attachmentDescriptions.data();
	info.subpassCount = 1;
	info.pSubpasses = &subpassDescription;
	info.dependencyCount = 0;
	info.pDependencies = nullptr;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkRenderPassCreateInfo TRAP::Graphics::API::VulkanInits::RenderPassCreateInfo(const std::vector<VkAttachmentDescription>& attachmentDescriptions) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkRenderPassCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.attachmentCount = NumericCast<uint32_t>(attachmentDescriptions.size());
	info.pAttachments = attachmentDescriptions.data();
	info.subpassCount = 0;
	info.pSubpasses = nullptr;
	info.dependencyCount = 0;
	info.pDependencies = nullptr;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkRenderPassCreateInfo2KHR TRAP::Graphics::API::VulkanInits::RenderPassCreateInfo(const std::vector<VkAttachmentDescription2KHR>& attachmentDescriptions,
	                                                                                            const VkSubpassDescription2KHR& subpassDescription) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkRenderPassCreateInfo2KHR info{};

	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2_KHR;
	info.pNext = nullptr;
	info.flags = 0;
	info.attachmentCount = NumericCast<uint32_t>(attachmentDescriptions.size());
	info.pAttachments = attachmentDescriptions.data();
	info.subpassCount = 1;
	info.pSubpasses = &subpassDescription;
	info.dependencyCount = 0;
	info.pDependencies = nullptr;
	info.correlatedViewMaskCount = 0;
	info.pCorrelatedViewMasks = nullptr;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkRenderPassCreateInfo2KHR TRAP::Graphics::API::VulkanInits::RenderPassCreateInfo(const std::vector<VkAttachmentDescription2KHR>& attachmentDescriptions) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkRenderPassCreateInfo2KHR info{};

	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2_KHR;
	info.pNext = nullptr;
	info.flags = 0;
	info.attachmentCount = NumericCast<uint32_t>(attachmentDescriptions.size());
	info.pAttachments = attachmentDescriptions.data();
	info.subpassCount = 0;
	info.pSubpasses = nullptr;
	info.dependencyCount = 0;
	info.pDependencies = nullptr;
	info.correlatedViewMaskCount = 0;
	info.pCorrelatedViewMasks = nullptr;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkRenderPassBeginInfo TRAP::Graphics::API::VulkanInits::RenderPassBeginInfo(VkRenderPass renderPass,
	                                                                                      VkFramebuffer frameBuffer,
	                                                                                      const VkRect2D renderArea,
	                                                                                      const std::vector<VkClearValue>& colorValues)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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

[[nodiscard]] VkBufferViewCreateInfo TRAP::Graphics::API::VulkanInits::BufferViewCreateInfo(VkBuffer buffer, const VkFormat format,
	                                                                                        const uint64_t offset,
	                                                                                        const uint64_t range)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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

[[nodiscard]] VkCommandPoolCreateInfo TRAP::Graphics::API::VulkanInits::CommandPoolCreateInfo(const uint32_t queueFamilyIndex) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkCommandPoolCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.queueFamilyIndex = queueFamilyIndex;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkCommandBufferAllocateInfo TRAP::Graphics::API::VulkanInits::CommandBufferAllocateInfo(VkCommandPool commandPool,
                                                                                                      const bool secondary)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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

[[nodiscard]] VkCommandBufferBeginInfo TRAP::Graphics::API::VulkanInits::CommandBufferBeginInfo() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkCommandBufferBeginInfo info{};

	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.pInheritanceInfo = nullptr;

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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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

[[nodiscard]] VkPipelineLayoutCreateInfo TRAP::Graphics::API::VulkanInits::PipelineLayoutCreateInfo(const uint32_t layoutCount,
	                                                                                                const VkDescriptorSetLayout* const layouts,
	                                                                                                const uint32_t pushConstantRangeCount,
	                                                                                                const VkPushConstantRange* const pushConstants) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkPipelineLayoutCreateInfo info{};

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

[[nodiscard]] VkPipelineColorBlendStateCreateInfo TRAP::Graphics::API::VulkanInits::PipelineColorBlendStateCreateInfo(const std::vector<VkPipelineColorBlendAttachmentState>& attachments) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkPipelineColorBlendStateCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.logicOpEnable = VK_FALSE;
	info.logicOp = VK_LOGIC_OP_CLEAR;
	info.attachmentCount = NumericCast<uint32_t>(attachments.size());
	info.pAttachments = attachments.data();
	info.blendConstants[0] = 0.0f;
	info.blendConstants[1] = 0.0f;
	info.blendConstants[2] = 0.0f;
	info.blendConstants[3] = 0.0f;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkPipelineColorBlendStateCreateInfo TRAP::Graphics::API::VulkanInits::PipelineColorBlendStateCreateInfo(const VkLogicOp logicOp,
	                                                                                                                  const std::vector<VkPipelineColorBlendAttachmentState>& attachments,
	                                                                                                                  const float blendConstR,
	                                                                                                                  const float blendConstG,
	                                                                                                                  const float blendConstB,
	                                                                                                                  const float blendConstA) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkPipelineColorBlendStateCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.logicOpEnable = VK_TRUE;
	info.logicOp = logicOp;
	info.attachmentCount = NumericCast<uint32_t>(attachments.size());
	info.pAttachments = attachments.data();
	info.blendConstants[0] = blendConstR;
	info.blendConstants[1] = blendConstG;
	info.blendConstants[2] = blendConstB;
	info.blendConstants[3] = blendConstA;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkPipelineCacheCreateInfo TRAP::Graphics::API::VulkanInits::PipelineCacheCreateInfo(const std::vector<uint8_t>& data,
	                                                                                              const VkPipelineCacheCreateFlags flags) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkPipelineCacheCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	info.pNext = nullptr;
	info.initialDataSize = data.size();
	info.pInitialData = data.data();
	info.flags = flags;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkPipelineShaderStageCreateInfo TRAP::Graphics::API::VulkanInits::PipelineShaderStageCreateInfo(const VkShaderStageFlagBits stage,
	                                                                                                          VkShaderModule module,
	                                                                                                          const std::string_view name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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

[[nodiscard]] VkPipelineVertexInputStateCreateInfo TRAP::Graphics::API::VulkanInits::PipelineVertexInputStateCreateInfo(const uint32_t inputBindingCount,
	                                                                                                                    const VkVertexInputBindingDescription* const inputBindings,
	                                                                                                                    const uint32_t inputAttributeCount,
	                                                                                                                    const VkVertexInputAttributeDescription* const inputAttributes) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkPipelineVertexInputStateCreateInfo info{};

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

[[nodiscard]] VkPipelineInputAssemblyStateCreateInfo TRAP::Graphics::API::VulkanInits::PipelineInputAssemblyStateCreateInfo(const VkPrimitiveTopology topology,
                                                                                                                            const bool primitiveRestart) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkPipelineInputAssemblyStateCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.topology = topology;
	info.primitiveRestartEnable = static_cast<VkBool32>(primitiveRestart);

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkPipelineTessellationStateCreateInfo TRAP::Graphics::API::VulkanInits::PipelineTessellationStateCreateInfo(const uint32_t patchControlPoints) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkPipelineTessellationStateCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.patchControlPoints = patchControlPoints;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkPipelineViewportStateCreateInfo TRAP::Graphics::API::VulkanInits::PipelineViewportStateCreateInfo() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkPipelineViewportStateCreateInfo info{};

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

[[nodiscard]] VkPipelineMultisampleStateCreateInfo TRAP::Graphics::API::VulkanInits::PipelineMultisampleStateCreateInfo(const VkSampleCountFlagBits sampleCount,
	                                                                                                                    const bool sampleShading,
																										                const float sampleShadingRate) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkPipelineMultisampleStateCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.rasterizationSamples = sampleCount;
	info.sampleShadingEnable = static_cast<VkBool32>(sampleShading);
	info.minSampleShading = sampleShading ? sampleShadingRate : 0.0f;
	info.pSampleMask = nullptr;
	info.alphaToCoverageEnable = VK_FALSE;
	info.alphaToOneEnable = VK_FALSE;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkPipelineFragmentShadingRateStateCreateInfoKHR TRAP::Graphics::API::VulkanInits::PipelineFragmentShadingRateStateCreateInfo(const VkExtent2D fragmentSize,
	                                                                                                                                       const std::array<VkFragmentShadingRateCombinerOpKHR, 2>& rateCombiners) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkPipelineFragmentShadingRateStateCreateInfoKHR info{};

	info.sType = VK_STRUCTURE_TYPE_PIPELINE_FRAGMENT_SHADING_RATE_STATE_CREATE_INFO_KHR;
	info.pNext = nullptr;
	info.fragmentSize = fragmentSize;
	info.combinerOps[0] = std::get<0>(rateCombiners);
	info.combinerOps[1] = std::get<1>(rateCombiners);

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkPipelineDynamicStateCreateInfo TRAP::Graphics::API::VulkanInits::PipelineDynamicStateCreateInfo(const std::vector<VkDynamicState>& dynamicStates) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkPipelineDynamicStateCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.dynamicStateCount = NumericCast<uint32_t>(dynamicStates.size());
	info.pDynamicStates = dynamicStates.data();

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkComputePipelineCreateInfo TRAP::Graphics::API::VulkanInits::ComputePipelineCreateInfo(const VkPipelineShaderStageCreateInfo& stage,
	                                                                                                  VkPipelineLayout layout)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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

[[nodiscard]] VkGraphicsPipelineCreateInfo TRAP::Graphics::API::VulkanInits::GraphicsPipelineCreateInfo(const uint32_t stageCount,
	                                                                                                    const VkPipelineShaderStageCreateInfo* const stages,
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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkGraphicsPipelineCreateInfo info{};

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

[[nodiscard]] VkSubmitInfo TRAP::Graphics::API::VulkanInits::SubmitInfo(const std::vector<VkSemaphore>& waitSemaphores,
														                const uint32_t waitCount,
                                                                        const std::vector<VkPipelineStageFlags>& waitMasks,
                                                                        const std::vector<VkCommandBuffer>& cmds,
                                                                        const std::vector<VkSemaphore>& signalSemaphores,
														                const uint32_t signalCount) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkSubmitInfo info{};

	info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	info.pNext = nullptr;
	info.waitSemaphoreCount = waitCount;
	info.pWaitSemaphores = waitSemaphores.data();
	info.pWaitDstStageMask = waitMasks.data();
	info.commandBufferCount = NumericCast<uint32_t>(cmds.size());
	info.pCommandBuffers = cmds.data();
	info.signalSemaphoreCount = signalCount;
	info.pSignalSemaphores = signalSemaphores.data();

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkQueryPoolCreateInfo TRAP::Graphics::API::VulkanInits::QueryPoolCreateInfo(const uint32_t count,
                                                                                          const VkQueryType queryType) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);


	VkQueryPoolCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.queryCount = count;
	info.queryType = queryType;
	info.pipelineStatistics = 0;

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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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

[[nodiscard]] VkPresentInfoKHR TRAP::Graphics::API::VulkanInits::PresentInfo(const std::vector<VkSemaphore>& waitSemaphores,
                                                                             const VkSwapchainKHR& swapChain,
															                 uint32_t& presentIndex) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkPresentInfoKHR info{};

	info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	info.pNext = nullptr;
	info.waitSemaphoreCount = NumericCast<uint32_t>(waitSemaphores.size());
	info.pWaitSemaphores = !waitSemaphores.empty() ? waitSemaphores.data() : nullptr;
	info.swapchainCount = 1;
	info.pSwapchains = &swapChain;
	info.pImageIndices = &presentIndex;
	info.pResults = nullptr;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkMemoryAllocateInfo TRAP::Graphics::API::VulkanInits::MemoryAllocateInfo(const VkDeviceSize allocSize,
                                                                                        const uint32_t memoryTypeIndex) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkMemoryAllocateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	info.pNext = nullptr;
	info.allocationSize = allocSize;
	info.memoryTypeIndex = memoryTypeIndex;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkClearColorValue TRAP::Graphics::API::VulkanInits::ClearColorValue(const RendererAPI::Color& color,
                                                                                  const ImageFormat format)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkClearColorValue clearColor{};

	if(ImageFormatIsNormalized(format))
	{
		if(!ImageFormatIsSigned(format))
		{
			clearColor.float32[0] = TRAP::Math::Clamp(NumericCast<float>(color.Red), 0.0f, 1.0f);
			clearColor.float32[1] = TRAP::Math::Clamp(NumericCast<float>(color.Green), 0.0f, 1.0f);
			clearColor.float32[2] = TRAP::Math::Clamp(NumericCast<float>(color.Blue), 0.0f, 1.0f);
			clearColor.float32[3] = TRAP::Math::Clamp(NumericCast<float>(color.Alpha), 0.0f, 1.0f);
		}
		else /*if(ImageFormatIsSigned(format))*/
		{
			clearColor.float32[0] = TRAP::Math::Clamp(NumericCast<float>(color.Red), -1.0f, 1.0f);
			clearColor.float32[1] = TRAP::Math::Clamp(NumericCast<float>(color.Green), -1.0f, 1.0f);
			clearColor.float32[2] = TRAP::Math::Clamp(NumericCast<float>(color.Blue), -1.0f, 1.0f);
			clearColor.float32[3] = TRAP::Math::Clamp(NumericCast<float>(color.Alpha), -1.0f, 1.0f);
		}
	}
	else /*if(!ImageFormatIsNormalized(format))*/
	{
		if(!ImageFormatIsFloat(format))
		{
			if(!ImageFormatIsSigned(format))
			{
				clearColor.uint32[0] = NumericCast<uint32_t>(TRAP::Math::Round(color.Red));
				clearColor.uint32[1] = NumericCast<uint32_t>(TRAP::Math::Round(color.Green));
				clearColor.uint32[2] = NumericCast<uint32_t>(TRAP::Math::Round(color.Blue));
				clearColor.uint32[3] = NumericCast<uint32_t>(TRAP::Math::Round(color.Alpha));
			}
			else /*if(ImageFormatIsSigned(format))*/
			{
				clearColor.int32[0] = NumericCast<int32_t>(TRAP::Math::Round(color.Red));
				clearColor.int32[1] = NumericCast<int32_t>(TRAP::Math::Round(color.Green));
				clearColor.int32[2] = NumericCast<int32_t>(TRAP::Math::Round(color.Blue));
				clearColor.int32[3] = NumericCast<int32_t>(TRAP::Math::Round(color.Alpha));
			}
		}
		else
		{
			clearColor.float32[0] = NumericCast<float>(color.Red);
			clearColor.float32[1] = NumericCast<float>(color.Green);
			clearColor.float32[2] = NumericCast<float>(color.Blue);
			clearColor.float32[3] = NumericCast<float>(color.Alpha);
		}
	}

	return clearColor;
}
