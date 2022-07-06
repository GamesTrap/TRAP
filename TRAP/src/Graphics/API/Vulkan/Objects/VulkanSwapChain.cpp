#include "TRAPPCH.h"
#include "VulkanSwapChain.h"

#include "VulkanSemaphore.h"
#include "VulkanFence.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanInits.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanSurface.h"
#include "VulkanRenderTarget.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"


TRAP::Graphics::API::VulkanSwapChain::VulkanSwapChain(RendererAPI::SwapChainDesc& desc)
	: m_vma(dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetVMA()),
	  m_instance(dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetInstance()),
	  m_device(dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice()),
	  m_presentQueue(),
	  m_swapChain(),
	  m_presentQueueFamilyIndex(),
	  m_imageCount(),
	  m_enableVSync()
{
	TRAP_ASSERT(m_device);
	TRAP_ASSERT(desc.ImageCount >= RendererAPI::ImageCount);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanSwapChainPrefix, "Creating SwapChain");
#endif

	InitSwapchain(desc);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanSwapChain::~VulkanSwapChain()
{
	TRAP_ASSERT(m_swapChain);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanSwapChainPrefix, "Destroying SwapChain");
#endif

	DeInitSwapchain();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanSwapChain::InitSwapchain(RendererAPI::SwapChainDesc& desc)
{
	//////////////////
	//Create Surface//
	//////////////////
	TRAP::Ref<VulkanSurface> surface = TRAP::MakeRef<VulkanSurface>(m_instance, m_device, desc.Window);

	////////////////////
	//Create SwapChain//
	////////////////////
	//Image Count
	if (desc.ImageCount == 0)
		desc.ImageCount = 2;

	const VkSurfaceCapabilitiesKHR caps = surface->GetVkSurfaceCapabilities();
	if ((caps.maxImageCount > 0) && (desc.ImageCount > caps.maxImageCount))
	{
		TP_WARN(Log::RendererVulkanSwapChainPrefix, "Changed requested SwapChain images ", desc.ImageCount,
		        " to maximum allowed SwapChain images ", caps.maxImageCount);
		desc.ImageCount = caps.maxImageCount;
	}
	if(desc.ImageCount < caps.minImageCount)
	{
		TP_WARN(Log::RendererVulkanSwapChainPrefix, "Changed requested SwapChain images ", desc.ImageCount,
		        " to minimum required SwapChain images ", caps.minImageCount);
		desc.ImageCount = caps.minImageCount;
	}

	//Surface format
	//Select a surface format, depending on whether HDR is available.
	const VkSurfaceFormatKHR HDRSurfaceFormat = { VK_FORMAT_A2B10G10R10_UNORM_PACK32,
	                                              VK_COLOR_SPACE_HDR10_ST2084_EXT };

	VkSurfaceFormatKHR surfaceFormat{};
	surfaceFormat.format = VK_FORMAT_UNDEFINED;
	const std::vector<VkSurfaceFormatKHR>& formats = surface->GetVkSurfaceFormats();

	if ((formats.size() == 1) && (formats[0].format == VK_FORMAT_UNDEFINED))
	{
		surfaceFormat.format = VK_FORMAT_B8G8R8A8_SRGB;
		// surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
		surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	}
	else
	{
		const VkFormat requestedFormat = ImageFormatToVkFormat(desc.ColorFormat);
		VkColorSpaceKHR requestedColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		if(requestedFormat == HDRSurfaceFormat.format)
			requestedColorSpace = HDRSurfaceFormat.colorSpace;

		for(const VkSurfaceFormatKHR& format : formats)
		{
			if ((requestedFormat == format.format) && (requestedColorSpace == format.colorSpace))
			{
				surfaceFormat.format = requestedFormat;
				surfaceFormat.colorSpace = requestedColorSpace;
				break;
			}
		}

		//Default to VK_FORMAT_B8G8R8A8_UNORM if requested format isn't found
		if (surfaceFormat.format == VK_FORMAT_UNDEFINED)
		{
			// surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
			surfaceFormat.format = VK_FORMAT_B8G8R8A8_SRGB;
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

	for (uint32_t j = preferredModeStartIndex; j < preferredModeList.size(); ++j)
	{
		const VkPresentModeKHR mode = preferredModeList[j];
		uint32_t i = 0;
		for (; i < modes.size(); ++i)
		{
			if (modes[i] == mode)
				break;
		}
		if (i < modes.size())
		{
			presentMode = mode;
			break;
		}
	}

	//SwapChain
	VkExtent2D extent{};
	extent.width = TRAP::Math::Clamp(desc.Width, caps.minImageExtent.width, caps.maxImageExtent.width);
	extent.height = TRAP::Math::Clamp(desc.Height, caps.minImageExtent.height, caps.maxImageExtent.height);

	desc.Width = extent.width;
	desc.Height = extent.height;

	VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	uint32_t queueFamilyIndexCount = 0;
	std::array<uint32_t, 2> queueFamilyIndices =
	{
		static_cast<uint32_t>(dynamic_cast<VulkanQueue*>(desc.PresentQueues[0].get())->GetQueueFamilyIndex()),
		0
	};
	uint32_t presentQueueFamilyIndex = std::numeric_limits<uint32_t>::max();

	const std::vector<VkQueueFamilyProperties>& queueFamilyProperties = m_device->GetPhysicalDevice()->GetQueueFamilyProperties();

	//Check if hardware provides dedicated present queue
	if (!queueFamilyProperties.empty())
	{
		for (uint32_t index = 0; index < queueFamilyProperties.size(); ++index)
		{
			VkBool32 supportsPresent = VK_FALSE;
			const VkResult res = vkGetPhysicalDeviceSurfaceSupportKHR(m_device->GetPhysicalDevice()->GetVkPhysicalDevice(),
			                                                          index, surface->GetVkSurface(),
																	  &supportsPresent);
			if ((res == VK_SUCCESS) && (supportsPresent == VK_TRUE) &&
			    dynamic_cast<VulkanQueue*>(desc.PresentQueues[0].get())->GetQueueFamilyIndex() != index)
			{
				presentQueueFamilyIndex = index;
				break;
			}
		}

		//If there is no dedicated present queue, just find the first available queue which supports present
		if (presentQueueFamilyIndex == std::numeric_limits<uint32_t>::max())
		{
			for (uint32_t index = 0; index < queueFamilyProperties.size(); ++index)
			{
				VkBool32 supportsPresent = VK_FALSE;
				const VkResult res = vkGetPhysicalDeviceSurfaceSupportKHR(m_device->GetPhysicalDevice()->GetVkPhysicalDevice(),
				                                                          index, surface->GetVkSurface(),
																		  &supportsPresent);
				if ((res == VK_SUCCESS) && (supportsPresent == VK_TRUE))
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
	VkQueue presentQueue = VK_NULL_HANDLE;
	uint32_t finalPresentQueueFamilyIndex = 0;
	if (presentQueueFamilyIndex != std::numeric_limits<uint32_t>::max() &&
	    queueFamilyIndices[0] != presentQueueFamilyIndex)
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

	VkSurfaceTransformFlagBitsKHR preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
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
	for (VkCompositeAlphaFlagBitsKHR flag : compositeAlphaFlags)
	{
		if (caps.supportedCompositeAlpha & flag)
		{
			compositeAlpha = flag;
			break;
		}
	}

	TRAP_ASSERT(compositeAlpha != VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR);

	VkSwapchainKHR swapChain = VK_NULL_HANDLE;
	VkSwapchainCreateInfoKHR swapChainCreateInfo = VulkanInits::SwapchainCreateInfoKHR(surface->GetVkSurface(),
		                                                                               desc.ImageCount,
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
	VkCall(vkGetSwapchainImagesKHR(m_device->GetVkDevice(), swapChain, &imageCount, nullptr));

	TRAP_ASSERT(desc.ImageCount == imageCount);

	std::vector<VkImage> images(imageCount);
	VkCall(vkGetSwapchainImagesKHR(m_device->GetVkDevice(), swapChain, &imageCount, images.data()));

	RendererAPI::RenderTargetDesc descColor = {};
	descColor.Width = desc.Width;
	descColor.Height = desc.Height;
	descColor.Depth = 1;
	descColor.ArraySize = 1;
	descColor.Format = desc.ColorFormat;
	descColor.ClearColor = desc.ClearColor;
	descColor.ClearDepth = desc.ClearDepth;
	descColor.ClearStencil = desc.ClearStencil;
	descColor.SampleCount = RendererAPI::SampleCount::SampleCount1;
	descColor.SampleQuality = 0;
	descColor.StartState = RendererAPI::ResourceState::Present;

	//Populate the vk_image field and add the Vulkan texture objects
	for (uint32_t i = 0; i < imageCount; ++i)
	{
		descColor.NativeHandle = images[i];
		m_renderTargets.push_back(TRAP::MakeRef<VulkanRenderTarget>(descColor));
	}

	//Create MSAA resolve images if needed
	if(desc.SampleCount != RendererAPI::SampleCount::SampleCount1)
	{
		descColor.NativeHandle = nullptr;
		descColor.SampleCount = desc.SampleCount;
		for (uint32_t i = 0; i < imageCount; ++i)
			m_renderTargetsMSAAResolve.push_back(RenderTarget::Create(descColor));
	}

	//////////////

	m_desc = desc;
	m_enableVSync = desc.EnableVSync;
	m_imageCount = imageCount;
	m_surface = std::move(surface);
	m_presentQueueFamilyIndex = finalPresentQueueFamilyIndex;
	m_presentQueue = presentQueue;
	m_swapChain = swapChain;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanSwapChain::DeInitSwapchain()
{
	m_device->WaitIdle();

	m_renderTargetsMSAAResolve.clear();
	m_renderTargets.clear();

	vkDestroySwapchainKHR(m_device->GetVkDevice(), m_swapChain, nullptr);
	m_surface.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanSwapChain::AcquireNextImage(const TRAP::Ref<Semaphore>& signalSemaphore,
                                                                const TRAP::Ref<Fence>& fence) const
{
	TRAP_ASSERT(m_device != VK_NULL_HANDLE);
	TRAP_ASSERT(m_swapChain != VK_NULL_HANDLE);
	TRAP_ASSERT(signalSemaphore || fence);

	uint32_t imageIndex = std::numeric_limits<uint32_t>::max();
	VkResult res{};

	if(fence != nullptr)
	{
		VulkanFence* fen = dynamic_cast<VulkanFence*>(fence.get());
		res = vkAcquireNextImageKHR(m_device->GetVkDevice(), m_swapChain, std::numeric_limits<uint64_t>::max(),
		                            VK_NULL_HANDLE, fen->GetVkFence(), &imageIndex);

		//If SwapChain is out of date, let caller know by returning -1
		if(res == VK_ERROR_OUT_OF_DATE_KHR)
		{
			VkFence vkF = fen->GetVkFence();
			VkCall(vkResetFences(m_device->GetVkDevice(), 1, &vkF));
			fen->m_submitted = false;
			return std::numeric_limits<uint32_t>::max();
		}

		fen->m_submitted = true;
	}
	else
	{
		VulkanSemaphore* sema = dynamic_cast<VulkanSemaphore*>(signalSemaphore.get());
		res = vkAcquireNextImageKHR(m_device->GetVkDevice(), m_swapChain, std::numeric_limits<uint64_t>::max(),
		                            sema->GetVkSemaphore(), VK_NULL_HANDLE, &imageIndex);

		//If SwapChain is out of date, let caller know by returning -1
		if(res == VK_ERROR_OUT_OF_DATE_KHR)
		{
			sema->m_signaled = false;
			return std::numeric_limits<uint32_t>::max();
		}

		VkCall(res);
		sema->m_signaled = true;
	}

	return imageIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanSwapChain::ToggleVSync()
{
	RendererAPI::SwapChainDesc desc = m_desc;
	desc.EnableVSync = !desc.EnableVSync;

	//Toggle VSync on or off
	//For Vulkan we need to remove the SwapChain and recreate it with correct VSync option
	DeInitSwapchain();
	InitSwapchain(desc);
}

//-------------------------------------------------------------------------------------------------------------------//

VkSwapchainKHR TRAP::Graphics::API::VulkanSwapChain::GetVkSwapChain() const
{
	return m_swapChain;
}

//-------------------------------------------------------------------------------------------------------------------//

VkQueue TRAP::Graphics::API::VulkanSwapChain::GetPresentVkQueue() const
{
	return m_presentQueue;
}