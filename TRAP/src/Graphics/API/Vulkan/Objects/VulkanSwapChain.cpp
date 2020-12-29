#include "TRAPPCH.h"
#include "VulkanSwapChain.h"

#include "VulkanFence.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanInits.h"
#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanSurface.h"
#include "VulkanRenderTarget.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

TRAP::Graphics::API::VulkanSwapChain::VulkanSwapChain(const TRAP::Ref<VulkanInstance>& instance,
                                                      TRAP::Ref<VulkanDevice> device,
                                                      const TRAP::Ref<VulkanMemoryAllocator>& vma,
                                                      RendererAPI::SwapChainDesc& desc)
	: m_device(std::move(device)), m_presentQueue(), m_swapChain(), m_presentQueueFamilyIndex(), m_imageCount(), m_enableVSync(), m_desc()
{
	TRAP_ASSERT(m_device);
	TRAP_ASSERT(desc.ImageCount <= 3);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanSwapChainPrefix, "Creating SwapChain");
#endif
	
	//////////////////
	//Create Surface//
	//////////////////
	TRAP::Ref<VulkanSurface> surface = TRAP::MakeRef<VulkanSurface>(instance, m_device, desc.WindowHandle);

	////////////////////
	//Create SwapChain//
	////////////////////
	//Image Count
	if (desc.ImageCount == 0)
		desc.ImageCount = 2;

	const VkSurfaceCapabilitiesKHR caps = surface->GetVkSurfaceCapabilities();
	if ((caps.maxImageCount > 0) && (desc.ImageCount > caps.maxImageCount))
		desc.ImageCount = caps.maxImageCount;

	//Surface format
	//Select a surface format, depending on whether HDR is available.
	const VkSurfaceFormatKHR HDRSurfaceFormat = { VK_FORMAT_A2B10G10R10_UNORM_PACK32, VK_COLOR_SPACE_HDR10_ST2084_EXT };

	VkSurfaceFormatKHR surfaceFormat{};
	surfaceFormat.format = VK_FORMAT_UNDEFINED;
	const std::vector<VkSurfaceFormatKHR>& formats = surface->GetVkSurfaceFormats();

	if((formats.size() == 1) && (formats[0].format == VK_FORMAT_UNDEFINED))
	{
		surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
		surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	}
	else
	{
		const VkFormat requestedFormat = ImageFormatToVkFormat(desc.ColorFormat);
		const VkColorSpaceKHR requestedColorSpace = requestedFormat == HDRSurfaceFormat.format ? HDRSurfaceFormat.colorSpace : VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		for(uint32_t i = 0; i < formats.size(); ++i)
		{
			if((requestedFormat == formats[i].format) && (requestedColorSpace == formats[i].colorSpace))
			{
				surfaceFormat.format = requestedFormat;
				surfaceFormat.colorSpace = requestedColorSpace;
				break;
			}
		}

		//Default to VK_FORMAT_B8G8R8A8_UNORM if requested format isn't found
		if(surfaceFormat.format == VK_FORMAT_UNDEFINED)
		{
			surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
			surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		}
	}

	TRAP_ASSERT(surfaceFormat.format != VK_FORMAT_UNDEFINED);

	//The VK_PRESENT_MODE_FIFO_KHR mode must always be present as per spec
	//This mode waits for the vertical blank ("VSync")
	VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
	const std::vector<VkPresentModeKHR>& modes = surface->GetVkSurfacePresentModes();

	std::array<VkPresentModeKHR, 4> preferredModeList =
	{
		VK_PRESENT_MODE_IMMEDIATE_KHR,
		VK_PRESENT_MODE_MAILBOX_KHR,
		VK_PRESENT_MODE_FIFO_RELAXED_KHR,
		VK_PRESENT_MODE_FIFO_KHR
	};
	const uint32_t preferredModeStartIndex = desc.EnableVSync ? 2 : 0;

	for(uint32_t j = preferredModeStartIndex; j < preferredModeList.size(); ++j)
	{
		const VkPresentModeKHR mode = preferredModeList[j];
		uint32_t i = 0;
		for(; i < modes.size(); ++i)
		{
			if (modes[i] == mode)
				break;
		}
		if(i < modes.size())
		{
			presentMode = mode;
			break;
		}
	}

	//SwapChain
	VkExtent2D extent{};
	extent.width = desc.Width;
	extent.height = desc.Height;

	VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	uint32_t queueFamilyIndexCount = 0;
	std::array<uint32_t, 2> queueFamilyIndices = { static_cast<uint32_t>(desc.PresentQueues[0]->GetQueueFamilyIndex()), 0 };
	uint32_t presentQueueFamilyIndex = -1;

	const std::vector<VkQueueFamilyProperties>& queueFamilyProperties = m_device->GetPhysicalDevice()->GetQueueFamilyProperties();

	//Check if hardware provides dedicated present queue
	if(!queueFamilyProperties.empty())
	{
		for(uint32_t index = 0; index < queueFamilyProperties.size(); ++index)
		{
			VkBool32 supportsPresent = VK_FALSE;
			const VkResult res = vkGetPhysicalDeviceSurfaceSupportKHR(m_device->GetPhysicalDevice()->GetVkPhysicalDevice(), index, surface->GetVkSurface(), &supportsPresent);
			if((res == VK_SUCCESS) && (supportsPresent == VK_TRUE) && desc.PresentQueues[0]->GetQueueFamilyIndex() != index)
			{
				presentQueueFamilyIndex = index;
				break;
			}
		}

		//If there is no dedicated present queue, just find the first available queue which supports present
		if(presentQueueFamilyIndex == -1)
		{
			for(uint32_t index = 0; index < queueFamilyProperties.size(); ++index)
			{
				VkBool32 supportsPresent = VK_FALSE;
				const VkResult res = vkGetPhysicalDeviceSurfaceSupportKHR(m_device->GetPhysicalDevice()->GetVkPhysicalDevice(), index, surface->GetVkSurface(), &supportsPresent);
				if((res == VK_SUCCESS) && (supportsPresent == VK_TRUE))
				{
					presentQueueFamilyIndex = index;
					break;
				}
				
				//No present queue family available. Something goes wrong.
				TRAP_ASSERT(false);
			}
		}
	}

	//Find if GPU has a dedicated present queue
	VkQueue presentQueue;
	uint32_t finalPresentQueueFamilyIndex;
	if(presentQueueFamilyIndex != -1 && queueFamilyIndices[0] != presentQueueFamilyIndex)
	{
		queueFamilyIndices[0] = presentQueueFamilyIndex;
		vkGetDeviceQueue(m_device->GetVkDevice(), queueFamilyIndices[0], 0, &presentQueue);
		queueFamilyIndexCount = 1;
		finalPresentQueueFamilyIndex = presentQueueFamilyIndex;
	}
	else
	{
		finalPresentQueueFamilyIndex = queueFamilyIndices[0];
		presentQueue = VK_NULL_HANDLE;
	}

	VkSurfaceTransformFlagBitsKHR preTransform;
	if (caps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
		preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	else
		preTransform = caps.currentTransform;

	std::array<VkCompositeAlphaFlagBitsKHR, 4> compositeAlphaFlags =
	{
		VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR
	};
	VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR;
	for(VkCompositeAlphaFlagBitsKHR flag : compositeAlphaFlags)
	{
		if(caps.supportedCompositeAlpha & flag)
		{
			compositeAlpha = flag;
			break;
		}
	}

	TRAP_ASSERT(compositeAlpha != VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR);

	VkSwapchainKHR swapChain;
	VkSwapchainCreateInfoKHR swapChainCreateInfo = VulkanInits::SwapchainCreateInfoKHR(surface->GetVkSurface(),
		TRAP::Math::Clamp(desc.ImageCount, caps.minImageCount, caps.maxImageCount),
		surfaceFormat,
		extent,
		sharingMode,
		queueFamilyIndexCount,
		queueFamilyIndices,
		preTransform,
		compositeAlpha,
		presentMode);

	VkCall(vkCreateSwapchainKHR(m_device->GetVkDevice(), &swapChainCreateInfo, nullptr, &swapChain));

	desc.ColorFormat = ImageFormatFromVkFormat(surfaceFormat.format);

	//Create RenderTargets from SwapChain
	uint32_t imageCount = 0;
	vkGetSwapchainImagesKHR(m_device->GetVkDevice(), swapChain, &imageCount, nullptr);

	TRAP_ASSERT(desc.ImageCount == imageCount);

	std::vector<VkImage> images(imageCount);
	vkGetSwapchainImagesKHR(m_device->GetVkDevice(), swapChain, &imageCount, images.data());

	RendererAPI::RenderTargetDesc descColor = {};
	descColor.Width = desc.Width;
	descColor.Height = desc.Height;
	descColor.Depth = 1;
	descColor.ArraySize = 1;
	descColor.Format = desc.ColorFormat;
	descColor.ClearValue = desc.ColorClearValue;
	descColor.SampleCount = RendererAPI::SampleCount::SampleCount1;
	descColor.SampleQuality = 0;

	std::vector<RendererAPI::RenderTargetBarrier> barriers;

	//Populate the vk_image field and add the Vulkan texture objects
	for(uint32_t i = 0; i < imageCount; ++i)
	{
		descColor.NativeHandle = images[i];
		m_renderTargets.push_back(TRAP::MakeRef<VulkanRenderTarget>(m_device, descColor, vma));
		barriers.push_back({ m_renderTargets[i], RendererAPI::ResourceState::Undefined, RendererAPI::ResourceState::Present });
	}

	TRAP::Ref<VulkanQueue> queue = nullptr;
	TRAP::Ref<VulkanCommandPool> cmdPool = nullptr;
	VulkanCommandBuffer* cmd = nullptr;
	TRAP::Ref<VulkanFence> fence = nullptr;
	RendererAPI::QueueDesc queueDesc{};
	queueDesc.Type = RendererAPI::QueueType::Graphics;
	queue = TRAP::MakeRef<VulkanQueue>(m_device, queueDesc);
	cmdPool = TRAP::MakeRef<VulkanCommandPool>(m_device, queue, false);
	cmd = cmdPool->AllocateCommandBuffer(false);
	fence = TRAP::MakeRef<VulkanFence>(m_device);
	cmd->Begin();
	cmd->ResourceBarrier({}, {}, barriers);
	cmd->End();
	RendererAPI::QueueSubmitDesc submitDesc{};
	submitDesc.Cmds.push_back(cmd);
	submitDesc.SignalFence = fence;
	queue->Submit(submitDesc);
	fence->Wait();
	fence.reset();
	cmdPool->FreeCommandBuffer(cmd);
	cmdPool.reset();
	queue.reset();

	//////////////
	
	m_desc = desc;
	m_enableVSync = desc.EnableVSync;
	m_imageCount = imageCount;
	m_surface = surface;
	m_presentQueueFamilyIndex = finalPresentQueueFamilyIndex;
	m_presentQueue = presentQueue;
	m_swapChain = swapChain;
}

TRAP::Graphics::API::VulkanSwapChain::~VulkanSwapChain()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanSwapChainPrefix, "Destroying SwapChain");
#endif

	for (uint32_t i = 0; i < m_renderTargets.size(); ++i)
		m_renderTargets[i].reset();

	vkDestroySwapchainKHR(m_device->GetVkDevice(), m_swapChain, nullptr);
	m_surface.reset();
}