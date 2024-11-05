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
#include "Graphics/Textures/Texture.h"
#include "Utils/ErrorCodes/ErrorCodes.h"

namespace
{
	[[nodiscard]] constexpr VkExtent2D GetSurfaceExtent(const u32 width, const u32 height, const VkSurfaceCapabilitiesKHR& surfaceCaps)
	{
		return
		{
			.width = TRAP::Math::Clamp(width, surfaceCaps.minImageExtent.width, surfaceCaps.maxImageExtent.width),
			.height = TRAP::Math::Clamp(height, surfaceCaps.minImageExtent.height, surfaceCaps.maxImageExtent.height)
		};
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] TRAP::Ref<TRAP::Graphics::API::VulkanSurface> CreateSurface(TRAP::Ref<TRAP::Graphics::API::VulkanSurface> oldSurface,
	                                                                          const TRAP::Ref<TRAP::Graphics::API::VulkanInstance>& instance,
																			  const TRAP::Graphics::API::VulkanDevice& device,
																			  TRAP::Graphics::RendererAPI::SwapChainDesc& desc,
																			  VkSwapchainKHR& oldSwapChain)
	{
		ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

		TRAP_ASSERT(instance, "VulkanSwapChain::CreateSurface(): Vulkan Instance is nullptr!");

		//Fast path if no old surface is present
		if(oldSurface == nullptr)
		{
			const auto surface = TRAP::MakeRef<TRAP::Graphics::API::VulkanSurface>(instance, device, *desc.Window);

			const VkExtent2D size = GetSurfaceExtent(desc.Width, desc.Height, surface->GetVkSurfaceCapabilities());
			desc.Width = size.width;
			desc.Height = size.height;

			return surface;
		}

		VkExtent2D extent = GetSurfaceExtent(desc.Width, desc.Height, oldSurface->GetVkSurfaceCapabilities());

		//Old surface can only be reused if resolution matches that of SwapChainDesc
		if(extent.width != desc.Width || extent.height != desc.Height)
		{
			//Old SwapChain must be destroyed before we can destroy the old surface
			TRAP_ASSERT(oldSwapChain, "VulkanSwapChain::CreateSurface(): Old SwapChain is nullptr!");
			vkDestroySwapchainKHR(device.GetVkDevice(), oldSwapChain, nullptr);
			oldSwapChain = VK_NULL_HANDLE;

			oldSurface = TRAP::MakeRef<TRAP::Graphics::API::VulkanSurface>(instance, device, *desc.Window);

			extent = GetSurfaceExtent(desc.Width, desc.Height, oldSurface->GetVkSurfaceCapabilities());
		}

		desc.Width = extent.width;
		desc.Height = extent.height;

		return oldSurface;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr u32 GetRequiredImageCount(const TRAP::Graphics::API::VulkanSurface& surface,
	                                                  const TRAP::Graphics::RendererAPI::SwapChainDesc& desc)
	{
		const auto& caps = surface.GetVkSurfaceCapabilities();

		u32 imageCount = desc.ImageCount;

		//If image count is not set then default to double buffering
		if (imageCount == 0)
			imageCount = 2;

		if ((caps.maxImageCount > 0) && (imageCount > caps.maxImageCount)) //caps.maxImageCount <= 0 means no limit
		{
			TP_WARN(TRAP::Log::RendererVulkanSwapChainPrefix, "Changed requested SwapChain images ", imageCount,
					" to maximum allowed SwapChain images ", caps.maxImageCount);
			imageCount = caps.maxImageCount;
		}

		if(imageCount < caps.minImageCount)
		{
			TP_WARN(TRAP::Log::RendererVulkanSwapChainPrefix, "Changed requested SwapChain images ", imageCount,
					" to minimum required SwapChain images ", caps.minImageCount);
			imageCount = caps.minImageCount;
		}

		return imageCount;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] VkSurfaceFormatKHR GetSurfaceFormat(const TRAP::Graphics::API::VulkanSurface& surface,
	                                                  TRAP::Graphics::RendererAPI::SwapChainDesc& desc)
	{
		//TODO HDR support

		const std::vector<VkSurfaceFormatKHR>& formats = surface.GetVkSurfaceFormats();

		if ((formats.size() == 1) && (formats[0].format == VK_FORMAT_UNDEFINED)) //No preferred format, any can be used
		{
			desc.ColorFormat = TRAP::Graphics::API::ImageFormatFromVkFormat(VK_FORMAT_B8G8R8A8_SRGB);

			return VkSurfaceFormatKHR
			{
				.format = VK_FORMAT_B8G8R8A8_SRGB,
				.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
			};
		}

		VkSurfaceFormatKHR requestedSurfaceFormat
		{
			.format = ImageFormatToVkFormat(desc.ColorFormat),
			.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
		};

		auto found = std::ranges::find_if(formats, [requestedSurfaceFormat](const auto& fmt){return fmt.format == requestedSurfaceFormat.format && fmt.colorSpace == requestedSurfaceFormat.colorSpace;});
		if(found != formats.end()) //Found requested format
		{
			desc.ColorFormat = TRAP::Graphics::API::ImageFormatFromVkFormat(found->format);
			return *found;
		}

		TP_WARN("Requested surface format is not available, using fallback!");

		//Check for B8G8R8A8 as fallback
		requestedSurfaceFormat.format = VK_FORMAT_B8G8R8A8_SRGB;
		found = std::ranges::find_if(formats, [requestedSurfaceFormat](const auto& fmt){return fmt.format == requestedSurfaceFormat.format && fmt.colorSpace == requestedSurfaceFormat.colorSpace;});
		if(found != formats.end())
		{
			desc.ColorFormat = TRAP::Graphics::API::ImageFormatFromVkFormat(found->format);
			return *found;
		}

		//Check for R8G8B8A8 as fallback
		requestedSurfaceFormat.format = VK_FORMAT_R8G8B8A8_SRGB;
		found = std::ranges::find_if(formats, [requestedSurfaceFormat](const auto& fmt){return fmt.format == requestedSurfaceFormat.format && fmt.colorSpace == requestedSurfaceFormat.colorSpace;});
		if(found != formats.end())
		{
			desc.ColorFormat = TRAP::Graphics::API::ImageFormatFromVkFormat(found->format);
			return *found;
		}

		TRAP::Utils::DisplayError(TRAP::Utils::ErrorCode::VulkanNoMatchingSurfaceFormatFound);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr VkPresentModeKHR GetPresentMode(const TRAP::Graphics::API::VulkanSurface& surface, const bool vsync)
	{
		const std::vector<VkPresentModeKHR>& modes = surface.GetVkSurfacePresentModes();

		if(!vsync)
		{
			const auto found = std::ranges::find(modes, VK_PRESENT_MODE_IMMEDIATE_KHR);
			if(found != modes.end())
				return *found;

			TP_WARN("Requested present mode is not supported, using fallback");
		}

		VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR; //VSync, no tearing, better efficiency (always supported as stated by spec)

		if(std::ranges::find(modes, VK_PRESENT_MODE_FIFO_RELAXED_KHR) != modes.end()) //VSync, allows tearing, minimize stuttering
			presentMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
		else if(std::ranges::find(modes, VK_PRESENT_MODE_MAILBOX_KHR) != modes.end()) //VSync, no tearing, low latency
			presentMode = VK_PRESENT_MODE_MAILBOX_KHR;

		return presentMode;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	struct PresentQueueInfo
	{
		u32 PresentQueueFamilyIndex;
		u32 PresentQueueIndex;
	};

	[[nodiscard]] PresentQueueInfo GetPresentQueueInfo(const TRAP::Graphics::API::VulkanPhysicalDevice& physicalDevice,
	                                                   const TRAP::Graphics::API::VulkanSurface& surface,
													   const TRAP::Graphics::RendererAPI::SwapChainDesc& desc)
	{
		TRAP_ASSERT(!desc.PresentQueues.empty(), "VulkanSwapChain::GetPresentQueueInfo(): desc.PresentQueues is empty!");

		const std::vector<VkQueueFamilyProperties>& queueFamilyProperties = physicalDevice.GetQueueFamilyProperties();
		TRAP_ASSERT(!queueFamilyProperties.empty(), "VulkanSwapChain::GetPresentQueueInfo(): queueFamilyProperties is empty!");

		//Check if hardware provides dedicated present queue
		for(const auto& presentQueue : desc.PresentQueues)
		{
			const auto* const queue = dynamic_cast<const TRAP::Graphics::API::VulkanQueue*>(presentQueue.get());

			const u32 queueFamilyIndex = queue->GetQueueFamilyIndex();
			const u32 queueIndex = queue->GetQueueIndex();

			if(!physicalDevice.GetPhysicalDeviceSurfaceSupport(surface, queueFamilyIndex))
				continue;

			return PresentQueueInfo
			{
				.PresentQueueFamilyIndex = queueFamilyIndex,
				.PresentQueueIndex = queueIndex
			};
		}

		//If there is no dedicated present queue, find the first available queue which supports presenting
		for(u32 index = 0; index < queueFamilyProperties.size(); ++index)
		{
			if(!physicalDevice.GetPhysicalDeviceSurfaceSupport(surface, index))
				continue;

			return PresentQueueInfo
			{
				.PresentQueueFamilyIndex = index,
				.PresentQueueIndex = 0
			};
		}

		//No present queue family available o.O
		TRAP::Utils::DisplayError(TRAP::Utils::ErrorCode::VulkanNoQueueWithPresentationSupportFound);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] VkQueue GetPresentQueue(const TRAP::Graphics::API::VulkanDevice& device,
	                                      const u32 presentQueueFamilyIndex, const u32 presentQueueIndex)
	{
		VkQueue presentQueue = VK_NULL_HANDLE;
		vkGetDeviceQueue(device.GetVkDevice(), presentQueueFamilyIndex, presentQueueIndex, &presentQueue);
		TRAP_ASSERT(presentQueue, "VulkanSwapChain::GetPresentQueue(): presentQueue is nullptr!");

		return presentQueue;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] VkCompositeAlphaFlagBitsKHR GetCompositeAlphaFlag(const TRAP::Graphics::API::VulkanSurface& surface)
	{
		constexpr std::array<VkCompositeAlphaFlagBitsKHR, 4> compositeAlphaFlags
		{
			VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR
		};

		const auto& caps = surface.GetVkSurfaceCapabilities();

		const auto compositeAlpha = std::ranges::find_if(compositeAlphaFlags, [&caps](const auto flag){return (caps.supportedCompositeAlpha & flag) != 0u;});
		if(compositeAlpha == compositeAlphaFlags.end())
			TRAP::Utils::DisplayError(TRAP::Utils::ErrorCode::VulkanNoSupportedCompositeAlphaFlagFound);

		return *compositeAlpha;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] VkSwapchainKHR CreateVkSwapChain(const TRAP::Graphics::API::VulkanDevice& device,
		                                           const TRAP::Graphics::API::VulkanSurface& surface,
	                                               const VkSurfaceFormatKHR& surfaceFormat,
												   const VkSurfaceTransformFlagBitsKHR surfaceTransform,
												   const VkCompositeAlphaFlagBitsKHR compositeAlpha,
												   const VkPresentModeKHR presentMode,
												   TRAP::Graphics::RendererAPI::SwapChainDesc& desc,
												   VkSwapchainKHR& oldSwapChain)
	{
		VkSwapchainKHR swapChain = VK_NULL_HANDLE;
		const VkSwapchainCreateInfoKHR swapChainCreateInfo = TRAP::Graphics::API::VulkanInits::SwapchainCreateInfoKHR(surface.GetVkSurface(),
																								                      desc.ImageCount,
																								                      surfaceFormat,
																								                      VkExtent2D{.width = desc.Width, .height = desc.Height},
																								                      VK_SHARING_MODE_EXCLUSIVE,
																								                      {},
																								                      surfaceTransform,
																								                      compositeAlpha,
																								                      presentMode,
																								                      oldSwapChain);

		VkCall(vkCreateSwapchainKHR(device.GetVkDevice(), &swapChainCreateInfo, nullptr, &swapChain));
		if(swapChain == VK_NULL_HANDLE)
			TRAP::Utils::DisplayError(TRAP::Utils::ErrorCode::VulkanSwapchainCreationFailed);

		//Dispose of old swapchain
		if(oldSwapChain != nullptr)
			vkDestroySwapchainKHR(device.GetVkDevice(), oldSwapChain, nullptr);
		oldSwapChain = nullptr;
		desc.OldSwapChain = nullptr;

		return swapChain;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] VkSwapchainKHR CreateSwapChain(const TRAP::Graphics::API::VulkanDevice& device,
		                                         const TRAP::Graphics::API::VulkanSurface& surface,
												 VkSwapchainKHR& oldSwapChain,
		                                         TRAP::Graphics::RendererAPI::SwapChainDesc& desc)
	{
		desc.ImageCount = GetRequiredImageCount(surface, desc);

		const VkSurfaceFormatKHR surfaceFormat = GetSurfaceFormat(surface, desc);
		const VkPresentModeKHR presentMode = GetPresentMode(surface, desc.EnableVSync);
		const auto& caps = surface.GetVkSurfaceCapabilities();
		const VkSurfaceTransformFlagBitsKHR preTransform = (caps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) != 0u ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : caps.currentTransform;
		const VkCompositeAlphaFlagBitsKHR compositeAlpha = GetCompositeAlphaFlag(surface);

		if(oldSwapChain == nullptr && desc.OldSwapChain != nullptr)
			oldSwapChain = dynamic_cast<const TRAP::Graphics::API::VulkanSwapChain*>(desc.OldSwapChain)->GetVkSwapChain();

		return CreateVkSwapChain(device, surface, surfaceFormat, preTransform, compositeAlpha, presentMode, desc,
		                         oldSwapChain);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] std::vector<VkImage> GetSwapChainImages(const TRAP::Graphics::API::VulkanDevice& device,
	                                                      VkSwapchainKHR swapChain,
														  const TRAP::Graphics::RendererAPI::SwapChainDesc& desc)
	{
		u32 imageCount = 0;
		VkCall(vkGetSwapchainImagesKHR(device.GetVkDevice(), swapChain, &imageCount, nullptr));

		if(desc.ImageCount != imageCount)
			TP_WARN(TRAP::Log::RendererVulkanSwapChainPrefix, "vkGetSwapchainImagesKHR returned more images than were requested by vkCreateSwapchainKHR (", imageCount, " instead of ", desc.ImageCount, ")!");

		std::vector<VkImage> images(imageCount);
		VkCall(vkGetSwapchainImagesKHR(device.GetVkDevice(), swapChain, &imageCount, images.data()));

		return images;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] std::vector<TRAP::Ref<TRAP::Graphics::RenderTarget>> CreateRenderTargets(const TRAP::Graphics::API::VulkanDevice& device,
	                                                                                       VkSwapchainKHR swapChain,
																						   const TRAP::Graphics::RendererAPI::SwapChainDesc& desc)
	{
		const std::vector<VkImage> images = GetSwapChainImages(device, swapChain, desc);

		TRAP::Graphics::RendererAPI::RenderTargetDesc descColor
		{
			.Flags = TRAP::Graphics::TextureCreationFlags::None,
			.Width = desc.Width,
			.Height = desc.Height,
			.Depth = 1,
			.ArraySize = 1,
			.MipLevels = 0,
			.SampleCount = TRAP::Graphics::RendererAPI::SampleCount::One,
			.Format = desc.ColorFormat,
			.StartState = TRAP::Graphics::RendererAPI::ResourceState::Present,
			.ClearValue = desc.ClearValue,
			.SampleQuality = 0,
			.Descriptors = TRAP::Graphics::RendererAPI::DescriptorType::Undefined,
			.Name = "",
			.NativeHandle = nullptr
		};

		std::vector<TRAP::Ref<TRAP::Graphics::RenderTarget>> renderTargets(images.size());
		for (usize i = 0; i < images.size(); ++i)
		{
			descColor.NativeHandle = images[i];
			descColor.Name = fmt::format("RenderTarget (Index: {})", i);

			renderTargets[i] = TRAP::MakeRef<TRAP::Graphics::API::VulkanRenderTarget>(descColor);
		}

		return renderTargets;
	}
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

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

	if(m_swapChain != nullptr)
		vkDestroySwapchainKHR(m_device->GetVkDevice(), m_swapChain, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanSwapChain::InitSwapchain(RendererAPI::SwapChainDesc& desc,
                                                         VkSwapchainKHR oldSwapChain)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(desc.Window, "VulkanSwapChain::InitSwapchain(): desc.Window is nullptr!");
	TRAP_ASSERT(!desc.PresentQueues.empty(), "VulkanSwapChain::InitSwapchain(): desc.PresentQueues is empty!");

	m_surface = CreateSurface(m_surface, m_instance, *m_device, desc, oldSwapChain);

	const auto [presentQueueFamilyIndex, presentQueueIndex] = GetPresentQueueInfo(m_device->GetPhysicalDevice(), *m_surface, desc);
	m_presentQueue = GetPresentQueue(*m_device, presentQueueFamilyIndex, presentQueueIndex);

	m_swapChain = CreateSwapChain(*m_device, *m_surface, oldSwapChain, desc);

	m_renderTargets = CreateRenderTargets(*m_device, m_swapChain, desc);

	m_desc = desc;

#ifdef ENABLE_GRAPHICS_DEBUG
	const std::string swapChainName = fmt::format("SwapChain ({}x{}, Window: {})", desc.Width, desc.Height, desc.Window->GetTitle());
	TRAP::Graphics::API::VkSetObjectName(*m_device, std::bit_cast<u64>(m_swapChain), VK_OBJECT_TYPE_SWAPCHAIN_KHR, swapChainName);
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanSwapChain::DeInitSwapchain()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	m_device->WaitIdle();
	m_renderTargets.clear();

	m_presentCounter = 0;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanSwapChain::ReInitSwapChain()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	DeInitSwapchain();
	InitSwapchain(m_desc, m_swapChain);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::optional<u32> TRAP::Graphics::API::VulkanSwapChain::AcquireNextImage(Semaphore& signalSemaphore,
                                                                                        Fence& fence) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_swapChain != VK_NULL_HANDLE, "VulkanSwapChain::AcquireNextImage(): Vulkan SwapChain is nullptr!");

	u32 imageIndex = std::numeric_limits<u32>::max();

	VulkanSemaphore* const sema = dynamic_cast<VulkanSemaphore*>(&signalSemaphore);
	VulkanFence* const fen = dynamic_cast<VulkanFence*>(&fence);

	const VkResult res = vkAcquireNextImageKHR(m_device->GetVkDevice(), m_swapChain, std::numeric_limits<u64>::max(),
	                                           sema->GetVkSemaphore(), fen->GetVkFence(), &imageIndex);

	if(res == VK_ERROR_OUT_OF_DATE_KHR)
	{
		sema->m_signaled = false;
		fen->ResetState();
		return std::nullopt;
	}

	if(res == VK_SUBOPTIMAL_KHR)
		TP_WARN(Log::RendererVulkanSwapChainPrefix, "VulkanSwapChain::AcquireNextImage(): vkAcquireNextImageKHR returned VK_SUBOPTIMAL_KHR. If window was just resized, ignore this message.");
	else
		VkCall(res);

	sema->m_signaled = true;
	fen->m_submitted = true;

	if(imageIndex == std::numeric_limits<u32>::max())
		return std::nullopt;

	return imageIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::optional<u32> TRAP::Graphics::API::VulkanSwapChain::AcquireNextImage(Semaphore& signalSemaphore) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_swapChain != VK_NULL_HANDLE, "VulkanSwapChain::AcquireNextImage(): Vulkan SwapChain is nullptr!");

	u32 imageIndex = std::numeric_limits<u32>::max();
	VulkanSemaphore* const sema = dynamic_cast<VulkanSemaphore*>(&signalSemaphore);

	const VkResult res = vkAcquireNextImageKHR(m_device->GetVkDevice(), m_swapChain, std::numeric_limits<u64>::max(),
								               sema->GetVkSemaphore(), VK_NULL_HANDLE, &imageIndex);

	if(res == VK_ERROR_OUT_OF_DATE_KHR)
	{
		sema->m_signaled = false;
		return std::nullopt;
	}
	if(res == VK_SUBOPTIMAL_KHR)
		TP_WARN(Log::RendererVulkanSwapChainPrefix, "VulkanSwapChain::AcquireNextImage(): vkAcquireNextImageKHR returned VK_SUBOPTIMAL_KHR. If window was just resized, ignore this message.");
	else
		VkCall(res);

	sema->m_signaled = true;

	if(imageIndex == std::numeric_limits<u32>::max())
		return std::nullopt;

	return imageIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::optional<u32> TRAP::Graphics::API::VulkanSwapChain::AcquireNextImage(Fence& fence) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_swapChain != VK_NULL_HANDLE, "VulkanSwapChain::AcquireNextImage(): Vulkan SwapChain is nullptr!");

	u32 imageIndex = std::numeric_limits<u32>::max();
	VulkanFence* const fen = dynamic_cast<VulkanFence*>(&fence);

	const VkResult res = vkAcquireNextImageKHR(m_device->GetVkDevice(), m_swapChain, std::numeric_limits<u64>::max(),
								               VK_NULL_HANDLE, fen->GetVkFence(), &imageIndex);

	if(res == VK_ERROR_OUT_OF_DATE_KHR)
	{
		fen->ResetState();
		return std::nullopt;
	}

	VkCall(res);

	fen->m_submitted = true;

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
	//For Vulkan we need to destroy and recreate the SwapChain with the correct VSync option
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

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<VkLatencyTimingsFrameReportNV> TRAP::Graphics::API::VulkanSwapChain::ReflexGetLatency(const u32 numLatencyData) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(RendererAPI::GPUSettings.ReflexSupported, "VulkanSwapChain::ReflexGetLatency(): NVIDIA-Reflex is not supported by the Vulkan device!");
	if(!RendererAPI::GPUSettings.ReflexSupported)
		return {};

	std::vector<VkLatencyTimingsFrameReportNV> latencyReports{};

	VkGetLatencyMarkerInfoNV markerInfoNV
	{
		.sType = VK_STRUCTURE_TYPE_GET_LATENCY_MARKER_INFO_NV,
		.pNext = nullptr,
		.timingCount = numLatencyData,
		.pTimings = nullptr
	};

	if(numLatencyData == 0u) //Query num of max available latency reports
		vkGetLatencyTimingsNV(m_device->GetVkDevice(), m_swapChain, &markerInfoNV);

	latencyReports.resize(markerInfoNV.timingCount);
	for(auto& rep : latencyReports)
		rep.sType = VK_STRUCTURE_TYPE_LATENCY_TIMINGS_FRAME_REPORT_NV;

	//Retrieve the data now
	markerInfoNV.pTimings = latencyReports.data();
	vkGetLatencyTimingsNV(m_device->GetVkDevice(), m_swapChain, &markerInfoNV);

	latencyReports.resize(markerInfoNV.timingCount);

	return latencyReports;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanSwapChain::ReflexSetMarker(const TRAP::Graphics::RendererAPI::NVIDIAReflexLatencyMarker marker)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	if(!RendererAPI::GPUSettings.ReflexSupported)
		return;

	const VkSetLatencyMarkerInfoNV markerInfoNV = VulkanInits::SetLatencyMarkerInfo(m_presentCounter, marker);
	vkSetLatencyMarkerNV(m_device->GetVkDevice(), m_swapChain, &markerInfoNV);

	if(marker == TRAP::Graphics::RendererAPI::NVIDIAReflexLatencyMarker::SimulationEnd)
		++m_presentCounter;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanSwapChain::ReflexSetLatencyMode(const TRAP::Graphics::RendererAPI::NVIDIAReflexLatencyMode latencyMode,
                                                                u32 fpsLimit) const
{
	const VkLatencySleepModeInfoNV latencySleepModeInfoNV = TRAP::Graphics::API::VulkanInits::LatencySleepModeInfo(latencyMode, fpsLimit);
	VkCall(vkSetLatencySleepModeNV(m_device->GetVkDevice(), m_swapChain, &latencySleepModeInfoNV));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanSwapChain::ReflexSleep(const Semaphore& reflexSemaphore) const
{
	const VulkanSemaphore* const vkReflexSemaphore = dynamic_cast<const VulkanSemaphore*>(&reflexSemaphore);

	u64 signalValue = 0u;
	VkCall(vkGetSemaphoreCounterValueKHR(m_device->GetVkDevice(), vkReflexSemaphore->GetVkSemaphore(), &signalValue));
	++signalValue;

	const VkLatencySleepInfoNV latencySleepInfoNV = VulkanInits::LatencySleepInfo(vkReflexSemaphore->GetVkSemaphore(), signalValue);
	VkCall(vkLatencySleepNV(m_device->GetVkDevice(), m_swapChain, &latencySleepInfoNV));

	const VkSemaphoreWaitInfoKHR waitInfo = VulkanInits::SemaphoreWaitInfo(vkReflexSemaphore->GetVkSemaphore(), signalValue);
	VkCall(vkWaitSemaphoresKHR(m_device->GetVkDevice(), &waitInfo, std::numeric_limits<u64>::max()));
}

#endif /*TRAP_HEADLESS_MODE*/
