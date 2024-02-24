#include "TRAPPCH.h"
#include "VulkanSwapChain.h"

#ifndef TRAP_HEADLESS_MODE

#include "VulkanSemaphore.h"
#include "VulkanFence.h"
#include "VulkanCommandPool.h"
#include "VulkanInits.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanSurface.h"
#include "VulkanRenderTarget.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

TRAP::Graphics::API::VulkanSwapChain::VulkanSwapChain(RendererAPI::SwapChainDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_device, "VulkanSwapChain(): Vulkan Device is nullptr!");
	TRAP_ASSERT(desc.ImageCount >= RendererAPI::ImageCount, "VulkanSwapChain(): ImageCount is too low!");
	TRAP_ASSERT(desc.Window, "VulkanSwapChain(): Window is nullptr!");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanSwapChainPrefix, "Creating SwapChain");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	InitSwapchain(desc);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanSwapChain::~VulkanSwapChain()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanSwapChainPrefix, "Destroying SwapChain");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	m_device->WaitIdle();

	vkDestroySwapchainKHR(m_device->GetVkDevice(), m_swapChain, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanSwapChain::InitSwapchain(RendererAPI::SwapChainDesc& desc,
                                                         VkSwapchainKHR oldSwapChain)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	//////////////////
	//Create Surface//
	//////////////////
	if(m_surface == nullptr)
		m_surface = TRAP::MakeRef<VulkanSurface>(m_instance, m_device, desc.Window);

	////////////////////
	//Create SwapChain//
	////////////////////
	//Image Count
	if (desc.ImageCount == 0)
		desc.ImageCount = 2;

	const VkSurfaceCapabilitiesKHR caps = m_surface->GetVkSurfaceCapabilities();
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
	const std::vector<VkSurfaceFormatKHR>& formats = m_surface->GetVkSurfaceFormats();

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

	TRAP_ASSERT(surfaceFormat.format != VK_FORMAT_UNDEFINED, "VulkanSwapChain::InitSwapchain(): No suitable surface format found!");

	//The VK_PRESENT_MODE_FIFO_KHR mode must always be present as per spec
	//This mode waits for the vertical blank ("VSync")
	VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
	const std::vector<VkPresentModeKHR>& modes = m_surface->GetVkSurfacePresentModes();

	static constexpr std::array<VkPresentModeKHR, 4> preferredModeList =
	{
		VK_PRESENT_MODE_IMMEDIATE_KHR,
		VK_PRESENT_MODE_MAILBOX_KHR,
		VK_PRESENT_MODE_FIFO_RELAXED_KHR,
		VK_PRESENT_MODE_FIFO_KHR
	};
	const u32 preferredModeStartIndex = desc.EnableVSync ? 2 : 0;

	for (u32 j = preferredModeStartIndex; j < preferredModeList.size(); ++j)
	{
		const VkPresentModeKHR mode = preferredModeList[j];
		u32 i = 0;
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

	const VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	std::vector<u32> queueFamilyIndices
	{
		NumericCast<u32>(std::dynamic_pointer_cast<VulkanQueue>(desc.PresentQueues[0])->GetQueueFamilyIndex())
	};
	u32 presentQueueFamilyIndex = std::numeric_limits<u32>::max();

	const std::vector<VkQueueFamilyProperties>& queueFamilyProperties = m_device->GetPhysicalDevice().GetQueueFamilyProperties();

	//Check if hardware provides dedicated present queue
	if (!queueFamilyProperties.empty())
	{
		for (u32 index = 0; index < queueFamilyProperties.size(); ++index)
		{
			VkBool32 supportsPresent = VK_FALSE;
			const VkResult res = vkGetPhysicalDeviceSurfaceSupportKHR(m_device->GetPhysicalDevice().GetVkPhysicalDevice(),
			                                                          index, m_surface->GetVkSurface(),
																	  &supportsPresent);
			if ((res == VK_SUCCESS) && (supportsPresent == VK_TRUE) &&
			    std::dynamic_pointer_cast<VulkanQueue>(desc.PresentQueues[0])->GetQueueFamilyIndex() != index)
			{
				presentQueueFamilyIndex = index;
				break;
			}
		}

		//If there is no dedicated present queue, just find the first available queue which supports present
		if (presentQueueFamilyIndex == std::numeric_limits<u32>::max())
		{
			for (u32 index = 0; index < queueFamilyProperties.size(); ++index)
			{
				VkBool32 supportsPresent = VK_FALSE;
				const VkResult res = vkGetPhysicalDeviceSurfaceSupportKHR(m_device->GetPhysicalDevice().GetVkPhysicalDevice(),
				                                                          index, m_surface->GetVkSurface(),
																		  &supportsPresent);
				if ((res == VK_SUCCESS) && (supportsPresent == VK_TRUE))
				{
					presentQueueFamilyIndex = index;
					break;
				}

				//No present queue family available. Something goes wrong.
				TRAP_ASSERT(false, "VulkanSwapChain::InitSwapchain(): No present queue family available!");
			}
		}
	}

	//Find if GPU has a dedicated present queue
	VkQueue presentQueue = VK_NULL_HANDLE;
	u32 finalPresentQueueFamilyIndex = 0;
	if (presentQueueFamilyIndex != std::numeric_limits<u32>::max() &&
	    queueFamilyIndices[0] != presentQueueFamilyIndex)
	{
		queueFamilyIndices[0] = presentQueueFamilyIndex;
		vkGetDeviceQueue(m_device->GetVkDevice(), queueFamilyIndices[0], 0, &presentQueue);
		finalPresentQueueFamilyIndex = presentQueueFamilyIndex;
	}
	else
	{
		finalPresentQueueFamilyIndex = queueFamilyIndices[0];
		presentQueue = VK_NULL_HANDLE;
	}

	VkSurfaceTransformFlagBitsKHR preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	if ((caps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) != 0u)
		preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	else
		preTransform = caps.currentTransform;

	static constexpr std::array<VkCompositeAlphaFlagBitsKHR, 4> compositeAlphaFlags =
	{
		VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR
	};
	VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR;
	for (VkCompositeAlphaFlagBitsKHR flag : compositeAlphaFlags)
	{
		if ((caps.supportedCompositeAlpha & flag) != 0u)
		{
			compositeAlpha = flag;
			break;
		}
	}

	TRAP_ASSERT(compositeAlpha != VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR, "VulkanSwapChain::InitSwapchain(): No composite alpha flag available!");

	if(auto* oldVkSwapChain = dynamic_cast<VulkanSwapChain*>(desc.OldSwapChain); oldSwapChain == nullptr && desc.OldSwapChain != nullptr)
		oldSwapChain = oldVkSwapChain->GetVkSwapChain();

	VkSwapchainKHR swapChain = VK_NULL_HANDLE;
	const VkSwapchainCreateInfoKHR swapChainCreateInfo = VulkanInits::SwapchainCreateInfoKHR(m_surface->GetVkSurface(),
		                                                                                     desc.ImageCount,
		                                                                                     surfaceFormat,
		                                                                                     extent,
		                                                                                     sharingMode,
		                                                                                     queueFamilyIndices,
		                                                                                     preTransform,
		                                                                                     compositeAlpha,
		                                                                                     presentMode,
																							 oldSwapChain);

	m_desc.OldSwapChain = nullptr;

	VkCall(vkCreateSwapchainKHR(m_device->GetVkDevice(), &swapChainCreateInfo, nullptr, &swapChain));
	TRAP_ASSERT(swapChain, "VulkanSwapChain::InitSwapchain(): Vulkan SwapChain is nullptr!");

	desc.ColorFormat = ImageFormatFromVkFormat(surfaceFormat.format);

	//Create RenderTargets from SwapChain
	u32 imageCount = 0;
	VkCall(vkGetSwapchainImagesKHR(m_device->GetVkDevice(), swapChain, &imageCount, nullptr));

	if(desc.ImageCount != imageCount)
		TP_WARN(Log::RendererVulkanSwapChainPrefix, "vkGetSwapchainImagesKHR returned more images than were requested by vkCreateSwapchainKHR (", imageCount, " instead of ", desc.ImageCount, ")!");

	std::vector<VkImage> images(imageCount);
	VkCall(vkGetSwapchainImagesKHR(m_device->GetVkDevice(), swapChain, &imageCount, images.data()));

	RendererAPI::RenderTargetDesc descColor = {};
	descColor.Width = desc.Width;
	descColor.Height = desc.Height;
	descColor.Depth = 1;
	descColor.ArraySize = 1;
	descColor.Format = desc.ColorFormat;
	descColor.ClearValue = desc.ClearValue;
	descColor.SampleCount = RendererAPI::SampleCount::One;
	descColor.SampleQuality = 0;
	descColor.StartState = RendererAPI::ResourceState::Present;

	//Populate the vk_image field and add the Vulkan texture objects
	m_renderTargets.resize(imageCount);
	for (u32 i = 0; i < imageCount; ++i)
	{
		descColor.NativeHandle = images[i];
		m_renderTargets[i] = TRAP::MakeRef<VulkanRenderTarget>(descColor);
	}

	//////////////

	m_desc = desc;
	m_enableVSync = desc.EnableVSync;
	m_imageCount = imageCount;
	m_presentQueueFamilyIndex = finalPresentQueueFamilyIndex;
	m_presentQueue = presentQueue;
	m_swapChain = swapChain;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanSwapChain::DeInitSwapchain(const bool allowSwapChainReuse)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	m_device->WaitIdle();

	m_renderTargets.clear();

	if(!allowSwapChainReuse)
	{
		vkDestroySwapchainKHR(m_device->GetVkDevice(), m_swapChain, nullptr);
		m_surface.reset();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanSwapChain::ReInitSwapChain()
{
	VkSwapchainKHR oldSwapChain = m_swapChain;

	DeInitSwapchain(true);
	InitSwapchain(m_desc, m_swapChain);

	vkDestroySwapchainKHR(m_device->GetVkDevice(), oldSwapChain, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::optional<u32> TRAP::Graphics::API::VulkanSwapChain::AcquireNextImage(const TRAP::Ref<Semaphore>& signalSemaphore,
                                                                                             const TRAP::Ref<Fence>& fence) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_swapChain != VK_NULL_HANDLE, "VulkanSwapChain::AcquireNextImage(): Vulkan SwapChain is nullptr!");
	TRAP_ASSERT(signalSemaphore || fence, "VulkanSwapChain::AcquireNextImage(): Semaphore and Fence are nullptr!");

	u32 imageIndex = std::numeric_limits<u32>::max();
	VkResult res{};

	if(fence != nullptr)
	{
		Ref<VulkanFence> fen = std::dynamic_pointer_cast<VulkanFence>(fence);
		res = vkAcquireNextImageKHR(m_device->GetVkDevice(), m_swapChain, std::numeric_limits<u64>::max(),
		                            VK_NULL_HANDLE, fen->GetVkFence(), &imageIndex);

		if(res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
		{
			VkFence vkF = fen->GetVkFence();
			VkCall(vkResetFences(m_device->GetVkDevice(), 1, &vkF));
			fen->m_submitted = false;
			return std::nullopt;
		}

		fen->m_submitted = true;
	}
	else
	{
		Ref<VulkanSemaphore> sema = std::dynamic_pointer_cast<VulkanSemaphore>(signalSemaphore);
		res = vkAcquireNextImageKHR(m_device->GetVkDevice(), m_swapChain, std::numeric_limits<u64>::max(),
		                            sema->GetVkSemaphore(), VK_NULL_HANDLE, &imageIndex);

		if(res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
		{
			sema->m_signaled = false;
			return std::nullopt;
		}

		VkCall(res);
		sema->m_signaled = true;
	}

	if(imageIndex == std::numeric_limits<u32>::max())
		return std::nullopt;

	return imageIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanSwapChain::ToggleVSync()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	m_desc.EnableVSync = !m_desc.EnableVSync;

	//Toggle VSync on or off
	//For Vulkan we need to remove the SwapChain and recreate it with correct VSync option
	ReInitSwapChain();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanSwapChain::UpdateFramebufferSize()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const auto fbSize = m_desc.Window->GetFrameBufferSize();

	if (fbSize.x() == 0 || fbSize.y() == 0) //0x0 is an invalid framebuffer size
		return;

	m_desc.Width = fbSize.x();
	m_desc.Height = fbSize.y();

	ReInitSwapChain();
}

#endif /*TRAP_HEADLESS_MODE*/