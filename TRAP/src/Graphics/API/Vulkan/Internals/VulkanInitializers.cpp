#include "TRAPPCH.h"

#include "VulkanInitializers.h"
#include "VulkanQueueFamilyIndices.h"

VkApplicationInfo TRAP::Graphics::API::Vulkan::Initializers::ApplicationInfo(const char* applicationName, const uint32_t version)
{
	return VkApplicationInfo
	{
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr,
		applicationName,
		version,
		"TRAP",
		TRAP_VERSION,
		VK_API_VERSION_1_2
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkInstanceCreateInfo TRAP::Graphics::API::Vulkan::Initializers::InstanceCreateInfo(VkApplicationInfo& appInfo)
{
	return VkInstanceCreateInfo
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		nullptr,
		0,
		&appInfo,
		0,
		nullptr,
		0,
		nullptr
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkInstanceCreateInfo TRAP::Graphics::API::Vulkan::Initializers::InstanceCreateInfo(
	VkApplicationInfo& appInfo,
	std::vector<const char*>& instanceLayers,
	std::vector<const char*>& instanceExtensions)
{	
	return VkInstanceCreateInfo
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		nullptr,
		0,
		&appInfo,
		static_cast<uint32_t>(instanceLayers.size()),
		!instanceLayers.empty() ? instanceLayers.data() : nullptr,
		static_cast<uint32_t>(instanceExtensions.size()),
		!instanceExtensions.empty() ? instanceExtensions.data() : nullptr
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkDebugUtilsMessengerCreateInfoEXT TRAP::Graphics::API::Vulkan::Initializers::DebugUtilsMessengerCreateInfo(PFN_vkDebugUtilsMessengerCallbackEXT callback)
{
	return VkDebugUtilsMessengerCreateInfoEXT
	{
		VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		nullptr,
		0,
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
		callback,
		nullptr
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkDeviceQueueCreateInfo TRAP::Graphics::API::Vulkan::Initializers::DeviceQueueCreateInfo(const uint32_t queueFamilyIndex, float* priority)
{
	return VkDeviceQueueCreateInfo
	{
		VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		nullptr,
		0,
		queueFamilyIndex,
		1,
		priority
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkDeviceCreateInfo TRAP::Graphics::API::Vulkan::Initializers::DeviceCreateInfo(std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos, VkPhysicalDeviceFeatures& physicalDeviceFeatures)
{
	return VkDeviceCreateInfo
	{
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		nullptr,
		0,
		static_cast<uint32_t>(queueCreateInfos.size()),
		queueCreateInfos.data(),
		0,
		nullptr,
		0,
		nullptr,
		&physicalDeviceFeatures
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkDeviceCreateInfo TRAP::Graphics::API::Vulkan::Initializers::DeviceCreateInfo(
	std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos,
	VkPhysicalDeviceFeatures& physicalDeviceFeatures,
	std::vector<const char*>& physicalDeviceExtensions)
{
	return VkDeviceCreateInfo
	{
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		nullptr,
		0,
		static_cast<uint32_t>(queueCreateInfos.size()),
		queueCreateInfos.data(),
		0,
		nullptr,
		static_cast<uint32_t>(physicalDeviceExtensions.size()),
		physicalDeviceExtensions.data(),
		&physicalDeviceFeatures
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkCommandPoolCreateInfo TRAP::Graphics::API::Vulkan::Initializers::CommandPoolCreateInfo(const uint32_t queueFamilyIndex, const VkCommandPoolCreateFlags flags)
{
	return VkCommandPoolCreateInfo
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		nullptr,
		flags,
		queueFamilyIndex
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkCommandBufferAllocateInfo TRAP::Graphics::API::Vulkan::Initializers::CommandBufferAllocateInfo(VkCommandPool pool, const uint32_t count)
{
	return VkCommandBufferAllocateInfo
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		nullptr,
		pool,
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		count
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkSwapchainCreateInfoKHR TRAP::Graphics::API::Vulkan::Initializers::SwapchainCreateInfo(
	VkSurfaceKHR surface,
	const VkSurfaceFormatKHR surfaceFormat,
	const uint32_t imageCount,
	const VkExtent2D extent,
	const QueueFamilyIndices& queueFamilyIndices,
	const VkSurfaceTransformFlagBitsKHR preTransform,
	const VkPresentModeKHR presentMode,
	const VkCompositeAlphaFlagsKHR supportCompositeAlpha,
	VkSwapchainKHR oldSwapchain)
{
	//Dont really care about the composite alpha mode but make it compatible with every device
	VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	constexpr std::array<VkCompositeAlphaFlagBitsKHR, 4> compositeAlphaFlags
	{
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR
	};
	for(VkCompositeAlphaFlagBitsKHR sCompositeAlpha : compositeAlphaFlags)
	{
		if(supportCompositeAlpha & sCompositeAlpha)
		{
			compositeAlpha = sCompositeAlpha;
			break;
		}
	}
	
	if(queueFamilyIndices.GraphicsIndices != queueFamilyIndices.PresentIndices)
	{
		return VkSwapchainCreateInfoKHR
		{
			VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			nullptr,
			0,
			surface,
			imageCount,
			surfaceFormat.format,
			surfaceFormat.colorSpace,
			extent,
			1,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			VK_SHARING_MODE_CONCURRENT,
			2,
			&queueFamilyIndices.GraphicsIndices,
			preTransform,
			compositeAlpha,
			presentMode,
			true,
			oldSwapchain
		};
	}
	
	return VkSwapchainCreateInfoKHR
	{
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		nullptr,
		0,
		surface,
		imageCount,
		surfaceFormat.format,
		surfaceFormat.colorSpace,
		extent,
		1,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_SHARING_MODE_EXCLUSIVE,
		0,
		nullptr,
		preTransform,
		compositeAlpha,
		presentMode,
		true,
		oldSwapchain
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkImageViewCreateInfo TRAP::Graphics::API::Vulkan::Initializers::ImageViewCreateInfo(VkImage& swapchainImage, const VkImageViewType viewType, const VkFormat format)
{
	return VkImageViewCreateInfo
	{
		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		nullptr,
		0,
		swapchainImage,
		viewType,
		format,
		VkComponentMapping
		{
			VK_COMPONENT_SWIZZLE_IDENTITY,
			VK_COMPONENT_SWIZZLE_IDENTITY,
			VK_COMPONENT_SWIZZLE_IDENTITY,
			VK_COMPONENT_SWIZZLE_IDENTITY
		},
		VkImageSubresourceRange
		{
			VK_IMAGE_ASPECT_COLOR_BIT,
			0,
			1,
			0,
			1
		}
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkRenderPassCreateInfo TRAP::Graphics::API::Vulkan::Initializers::RenderPassCreateInfo(
	std::vector<VkAttachmentDescription>& attachments,
	VkSubpassDescription& subPassDescription,
	std::vector<VkSubpassDependency>& dependencies)
{
	return VkRenderPassCreateInfo
	{
		VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		nullptr,
		0,
		static_cast<uint32_t>(attachments.size()),
		attachments.data(),
		1,
		&subPassDescription,
		static_cast<uint32_t>(dependencies.size()),
		dependencies.data()
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkFenceCreateInfo TRAP::Graphics::API::Vulkan::Initializers::FenceCreateInfo(const bool signaled)
{
	if (signaled)
	{
		return VkFenceCreateInfo
		{
			VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			nullptr,
			VK_FENCE_CREATE_SIGNALED_BIT
		};
	}
	
	return VkFenceCreateInfo
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		nullptr,
		0
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkSemaphoreCreateInfo TRAP::Graphics::API::Vulkan::Initializers::SemaphoreCreateInfo()
{
	return VkSemaphoreCreateInfo
	{
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		nullptr,
		0
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkFramebufferCreateInfo TRAP::Graphics::API::Vulkan::Initializers::FrameBufferCreateInfo(
	VkRenderPass& renderPass,
	std::vector<VkImageView>& attachments,
	const VkExtent2D extent,
	const uint32_t layers)
{
	return VkFramebufferCreateInfo
	{
		VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		nullptr,
		0,
		renderPass,
		static_cast<uint32_t>(attachments.size()),
		attachments.data(),
		extent.width,
		extent.height,
		layers
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkFramebufferCreateInfo TRAP::Graphics::API::Vulkan::Initializers::FrameBufferCreateInfo(
	VkRenderPass& renderPass,
	VkImageView& attachments,
	const VkExtent2D extent,
	const uint32_t layers)
{
	return VkFramebufferCreateInfo
	{
		VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		nullptr,
		0,
		renderPass,
		1,
		&attachments,
		extent.width,
		extent.height,
		layers
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineLayoutCreateInfo TRAP::Graphics::API::Vulkan::Initializers::PipelineLayoutCreateInfo(std::vector<VkDescriptorSetLayout>& descriptorLayouts, std::vector<VkPushConstantRange>& pushConstants)
{
	return VkPipelineLayoutCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		static_cast<uint32_t>(descriptorLayouts.size()),
		descriptorLayouts.data(),
		static_cast<uint32_t>(pushConstants.size()),
		pushConstants.data()
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineLayoutCreateInfo TRAP::Graphics::API::Vulkan::Initializers::PipelineLayoutCreateInfo()
{
	return VkPipelineLayoutCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		0,
		nullptr,
		0,
		nullptr
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineInputAssemblyStateCreateInfo TRAP::Graphics::API::Vulkan::Initializers::PipelineInputAssemblyStateCreateInfo(const VkPrimitiveTopology topology, const VkBool32 primitiveRestart)
{
	return VkPipelineInputAssemblyStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		nullptr,
		0,
		topology,
		primitiveRestart
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineRasterizationStateCreateInfo TRAP::Graphics::API::Vulkan::Initializers::PipelineRasterizationStateCreateInfo(
	const VkPolygonMode polygonMode,
	const VkCullModeFlags cullMode,
	const VkFrontFace frontFace)
{
	return VkPipelineRasterizationStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		nullptr,
		0,
		false,
		false,
		polygonMode,
		cullMode,
		frontFace,
		false,
		0.0f,
		0.0f,
		0.0f,
		1.0f
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineColorBlendAttachmentState TRAP::Graphics::API::Vulkan::Initializers::PipelineColorBlendAttachmentState()
{
	return VkPipelineColorBlendAttachmentState
	{
		false,
		VK_BLEND_FACTOR_ONE,
		VK_BLEND_FACTOR_ZERO,
		VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,
		VK_BLEND_FACTOR_ZERO,
		VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineColorBlendAttachmentState TRAP::Graphics::API::Vulkan::Initializers::PipelineColorBlendAttachmentState(const VkBlendFactor srcColorBlendFactor,
                                                                                                                 const VkBlendFactor srcAlphaBlendFactor,
                                                                                                                 const VkBlendFactor dstColorBlendFactor,
                                                                                                                 const VkBlendFactor dstAlphaBlendFactor,
                                                                                                                 const VkBlendOp colorBlendOp,
                                                                                                                 const VkBlendOp alphaBlendOp)
{
	return VkPipelineColorBlendAttachmentState
	{
		true,
		srcColorBlendFactor,
		dstColorBlendFactor,
		colorBlendOp,
		srcAlphaBlendFactor,
		dstAlphaBlendFactor,
		alphaBlendOp,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineColorBlendStateCreateInfo TRAP::Graphics::API::Vulkan::Initializers::PipelineColorBlendStateCreateInfo(std::vector<VkPipelineColorBlendAttachmentState>& colorBlendAttachments)
{
	return VkPipelineColorBlendStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		nullptr,
		0,
		false,
		VK_LOGIC_OP_COPY,
		static_cast<uint32_t>(colorBlendAttachments.size()),
		colorBlendAttachments.data(),
		{ 0.0f, 0.0f, 0.0f, 0.0f }
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineColorBlendStateCreateInfo TRAP::Graphics::API::Vulkan::Initializers::PipelineColorBlendStateCreateInfo(VkPipelineColorBlendAttachmentState& colorBlendAttachment)
{
	return VkPipelineColorBlendStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		nullptr,
		0,
		false,
		VK_LOGIC_OP_COPY,
		1,
		&colorBlendAttachment,
		{ 0.0f, 0.0f, 0.0f, 0.0f }
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineDepthStencilStateCreateInfo TRAP::Graphics::API::Vulkan::Initializers::PipelineDepthStencilStateCreateInfo(const VkBool32 depthTest, const VkBool32 depthWrite, const VkCompareOp depthCompareOp)
{
	return VkPipelineDepthStencilStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		nullptr,
		0,
		depthTest,
		depthWrite,
		depthCompareOp,
		false,
		false,
		{},
		{},
		0.0f,
		1.0f
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineViewportStateCreateInfo TRAP::Graphics::API::Vulkan::Initializers::PipelineViewportStateCreateInfo(const uint32_t viewportCount, const uint32_t scissorCount)
{
	return VkPipelineViewportStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		nullptr,
		0,
		viewportCount,
		nullptr,
		scissorCount,
		nullptr
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineMultisampleStateCreateInfo TRAP::Graphics::API::Vulkan::Initializers::PipelineMultisampleStateCreateInfo(const VkSampleCountFlagBits rasterizationSamples)
{
	return VkPipelineMultisampleStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		nullptr,
		0,
		rasterizationSamples,
		false,
		1.0f,
		nullptr,
		false,
		false
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineDynamicStateCreateInfo TRAP::Graphics::API::Vulkan::Initializers::PipelineDynamicStateCreateInfo(std::vector<VkDynamicState>& dynamicStates)
{
	return VkPipelineDynamicStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		nullptr,
		0,
		static_cast<uint32_t>(dynamicStates.size()),
		dynamicStates.data()
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineVertexInputStateCreateInfo TRAP::Graphics::API::Vulkan::Initializers::PipelineVertexInputStateCreateInfo(std::vector<VkVertexInputBindingDescription>& vertexInputBindingDescriptions,
	std::vector<VkVertexInputAttributeDescription>& vertexInputAttributeDescriptions)
{
	return VkPipelineVertexInputStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		nullptr,
		0,
		static_cast<uint32_t>(vertexInputBindingDescriptions.size()),
		vertexInputBindingDescriptions.data(),
		static_cast<uint32_t>(vertexInputAttributeDescriptions.size()),
		vertexInputAttributeDescriptions.data()
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineVertexInputStateCreateInfo TRAP::Graphics::API::Vulkan::Initializers::PipelineVertexInputStateCreateInfo()
{
	return VkPipelineVertexInputStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		nullptr,
		0,
		0,
		nullptr,
		0,
		nullptr
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkGraphicsPipelineCreateInfo TRAP::Graphics::API::Vulkan::Initializers::GraphicsPipelineCreateInfo(std::vector<VkPipelineShaderStageCreateInfo>& shaderStageCreateInfos,
	VkPipelineVertexInputStateCreateInfo& vertexInputStateCreateInfo,
	VkPipelineInputAssemblyStateCreateInfo& inputAssemblyStateCreateInfo,
	VkPipelineViewportStateCreateInfo& viewportStateCreateInfo,
	VkPipelineRasterizationStateCreateInfo& rasterizationStateCreateInfo,
	VkPipelineMultisampleStateCreateInfo& multisampleStateCreateInfo,
	VkPipelineDepthStencilStateCreateInfo& depthStencilStateCreateInfo,
	VkPipelineColorBlendStateCreateInfo& colorBlendStateCreateInfo,
	VkPipelineDynamicStateCreateInfo& dynamicStateCreateInfo,
	VkPipelineLayout& layout,
	VkRenderPass& renderPass,
	const uint32_t subPasses)
{
	return VkGraphicsPipelineCreateInfo
	{
		VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		nullptr,
		0,
		static_cast<uint32_t>(shaderStageCreateInfos.size()),
		shaderStageCreateInfos.data(),
		&vertexInputStateCreateInfo,
		&inputAssemblyStateCreateInfo,
		nullptr,
		&viewportStateCreateInfo,
		&rasterizationStateCreateInfo,
		&multisampleStateCreateInfo,
		&depthStencilStateCreateInfo,
		&colorBlendStateCreateInfo,
		&dynamicStateCreateInfo,
		layout,
		renderPass,
		subPasses,
		nullptr,
		-1
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkCommandBufferBeginInfo TRAP::Graphics::API::Vulkan::Initializers::CommandBufferBeginInfo()
{
	return VkCommandBufferBeginInfo
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		nullptr,
		VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		nullptr
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkRenderPassBeginInfo TRAP::Graphics::API::Vulkan::Initializers::RenderPassBeginInfo(VkRenderPass& renderPass,
	VkFramebuffer& frameBuffer,
	const uint32_t width,
	const uint32_t height,
	std::vector<VkClearValue>& clearValues)
{
	return VkRenderPassBeginInfo
	{
		VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		nullptr,
		renderPass,
		frameBuffer,
		VkRect2D{ {0, 0}, {width, height} },
		static_cast<uint32_t>(clearValues.size()),
		clearValues.data()
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkSubmitInfo TRAP::Graphics::API::Vulkan::Initializers::SubmitInfo(VkSemaphore& waitSemaphore, VkPipelineStageFlags& waitMask, VkCommandBuffer& commandBuffer, VkSemaphore& signalSemaphore)
{
	return VkSubmitInfo
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		nullptr,
		1,
		&waitSemaphore,
		&waitMask,
		1,
		&commandBuffer,
		1,
		&signalSemaphore
	};
}

//-------------------------------------------------------------------------------------------------------------------//

VkPresentInfoKHR TRAP::Graphics::API::Vulkan::Initializers::PresentInfo(VkSemaphore& waitSemaphore, VkSwapchainKHR& swapchain, uint32_t& imageIndex)
{
	return VkPresentInfoKHR
	{
		VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		nullptr,
		1,
		&waitSemaphore,
		1,
		&swapchain,
		&imageIndex,
		nullptr
	};
}