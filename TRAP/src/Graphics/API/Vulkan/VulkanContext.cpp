#include "TRAPPCH.h"
#include "VulkanContext.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanContext::VulkanContext(Window* window)
	: m_swapchain(),
	m_extent(),
	m_capabilities(),
	m_format(),
	m_presentMode(),
	m_surface(),
	m_window(window),
	m_vsync(false)
{
	if (m_window == nullptr)
	{
		TP_ERROR("[Context][Vulkan] Window is nullptr");
		exit(-1);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanContext::~VulkanContext()
{
	TP_DEBUG("[Context][Vulkan] Destroying Context");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanContext::Present(Window* window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanContext::InitSurface()
{
	TP_DEBUG("[Renderer][Vulkan] Initializing Surface");

	VkCall(glfwCreateWindowSurface(VulkanRenderer::Get()->GetInstance(), static_cast<GLFWwindow*>(m_window->GetNativeWindow()), nullptr, &m_surface));
	TP_RENDERER_ASSERT(m_surface, "[Renderer][Vulkan] Couldn't create Surface!");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanContext::DeInitSurface()
{
	if (m_surface)
	{
		TP_DEBUG("[Renderer][Vulkan] Destroying Surface");
		vkDestroySurfaceKHR(VulkanRenderer::Get()->GetInstance(), m_surface, nullptr);
		m_surface = nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanContext::SetupSwapchain()
{
	TP_DEBUG("[Renderer][Vulkan] Setting up Swapchain");

	const std::vector<VkSurfaceFormatKHR> availableSurfaceFormats = GetAvailableSurfaceFormats(VulkanRenderer::Get()->GetPhysicalDevice());
	m_format = ChooseSwapchainSurfaceFormat(availableSurfaceFormats);

	if (!m_vsync)
	{
		const std::vector<VkPresentModeKHR> availableSurfacePresentModes = GetAvailableSurfacePresentModes(VulkanRenderer::Get()->GetPhysicalDevice());
		m_presentMode = ChooseSwapchainSurfacePresentMode(availableSurfacePresentModes);
	}
	else
		m_presentMode = VK_PRESENT_MODE_FIFO_KHR;

	VkCall(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VulkanRenderer::Get()->GetPhysicalDevice(), m_surface, &m_capabilities));

	m_extent = ChooseSwapchainExtent();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanContext::InitSwapchain()
{
	TP_DEBUG("[Renderer][Vulkan] Initializing Swapchain");

	uint32_t imageCount = m_capabilities.minImageCount + 1;
	if (m_capabilities.maxImageCount > 0 && imageCount > m_capabilities.maxImageCount)
		imageCount = m_capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.surface = m_surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = m_format.format;
	createInfo.imageColorSpace = m_format.colorSpace;
	createInfo.imageExtent = m_extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t queueFamilyIndexes[] = { VulkanRenderer::Get()->GetGraphicsQueueFamilyIndex().value(), VulkanRenderer::Get()->GetPresentQueueFamilyIndex().value() };

	if (queueFamilyIndexes[0] != queueFamilyIndexes[1])
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndexes;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; //Optional
		createInfo.pQueueFamilyIndices = nullptr; //Optional
	}

	createInfo.preTransform = m_capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; //Blending ???
	createInfo.presentMode = m_presentMode;
	createInfo.clipped = true;
	createInfo.oldSwapchain = nullptr;

	VkCall(vkCreateSwapchainKHR(VulkanRenderer::Get()->GetDevice(), &createInfo, nullptr, &m_swapchain));
	TP_RENDERER_ASSERT(m_swapchain, "[Renderer][Vulkan] Couldn't create Swapchain!");

	VkCall(vkGetSwapchainImagesKHR(VulkanRenderer::Get()->GetDevice(), m_swapchain, &imageCount, nullptr));
	m_swapchainImages.resize(imageCount);
	VkCall(vkGetSwapchainImagesKHR(VulkanRenderer::Get()->GetDevice(), m_swapchain, &imageCount, m_swapchainImages.data()));
	TP_RENDERER_ASSERT(!m_swapchainImages.empty(), "[Renderer][Vulkan] Couldn't get Swapchain Images!");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanContext::DeInitSwapchain()
{
	if (m_swapchain)
	{
		TP_DEBUG("[Renderer][Vulkan] Destroying Swapchain");
		vkDestroySwapchainKHR(VulkanRenderer::Get()->GetDevice(), m_swapchain, nullptr);
		m_swapchain = nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanContext::InitImageViews()
{
	TP_DEBUG("[Renderer][Vulkan] Initializing Swapchain Image Views");

	m_swapchainImageViews.resize(m_swapchainImages.size());
	for (std::size_t i = 0; i < m_swapchainImages.size(); i++)
	{
		VkImageViewCreateInfo createInfo
		{
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			nullptr,
			0,
			m_swapchainImages[i],
			VK_IMAGE_VIEW_TYPE_2D,
			m_format.format,
			{
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY
			},
			{
				VK_IMAGE_ASPECT_COLOR_BIT,
				0,
				1,
				0,
				1
			}
		};

		VkCall(vkCreateImageView(VulkanRenderer::Get()->GetDevice(), &createInfo, nullptr, &m_swapchainImageViews[i]));
	}
	TP_RENDERER_ASSERT(!m_swapchainImageViews.empty(), "[Renderer][Vulkan] Couldn't create Image Views!");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanContext::DeInitImageViews()
{
	if (!m_swapchainImageViews.empty())
		TP_DEBUG("[Renderer][Vulkan] Destroying Swapchain Image Views");

	for (VkImageView imageView : m_swapchainImageViews)
		vkDestroyImageView(VulkanRenderer::Get()->GetDevice(), imageView, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<VkSurfaceFormatKHR> TRAP::Graphics::API::VulkanContext::GetAvailableSurfaceFormats(VkPhysicalDevice physicalDevice) const
{
	uint32_t surfaceFormatCount = 0;
	VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &surfaceFormatCount, nullptr));
	std::vector<VkSurfaceFormatKHR> availableSurfaceFormats{ surfaceFormatCount };
	VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &surfaceFormatCount, availableSurfaceFormats.data()));
	TP_RENDERER_ASSERT(!availableSurfaceFormats.empty(), "[Renderer][Vulkan] Couldn't get available Surface Formats!");

	return availableSurfaceFormats;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<VkSurfaceFormatKHR> TRAP::Graphics::API::VulkanContext::GetAvailableSurfaceFormats() const
{
	TP_DEBUG("[Renderer][Vulkan] Getting available Surface Formats");

	uint32_t surfaceFormatCount = 0;
	VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(VulkanRenderer::Get()->GetPhysicalDevice(), m_surface, &surfaceFormatCount, nullptr));
	std::vector<VkSurfaceFormatKHR> availableSurfaceFormats{ surfaceFormatCount };
	VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(VulkanRenderer::Get()->GetPhysicalDevice(), m_surface, &surfaceFormatCount, availableSurfaceFormats.data()));
	TP_RENDERER_ASSERT(!availableSurfaceFormats.empty(), "[Renderer][Vulkan] Couldn't get available Surface Formats!");

	return availableSurfaceFormats;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<VkPresentModeKHR> TRAP::Graphics::API::VulkanContext::GetAvailableSurfacePresentModes(VkPhysicalDevice physicalDevice) const
{
	uint32_t surfacePresentModeCount = 0;
	VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &surfacePresentModeCount, nullptr));
	std::vector<VkPresentModeKHR> surfacePresentModes{ surfacePresentModeCount };
	VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &surfacePresentModeCount, surfacePresentModes.data()));
	TP_RENDERER_ASSERT(!surfacePresentModes.empty(), "[Renderer][Vulkan] Couldn't get available Surface Present Modes!");

	return surfacePresentModes;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<VkPresentModeKHR> TRAP::Graphics::API::VulkanContext::GetAvailableSurfacePresentModes() const
{
	TP_DEBUG("[Renderer][Vulkan] Getting available Surface Present Modes");

	uint32_t surfacePresentModeCount = 0;
	VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(VulkanRenderer::Get()->GetPhysicalDevice(), m_surface, &surfacePresentModeCount, nullptr));
	std::vector<VkPresentModeKHR> surfacePresentModes{ surfacePresentModeCount };
	VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(VulkanRenderer::Get()->GetPhysicalDevice(), m_surface, &surfacePresentModeCount, surfacePresentModes.data()));
	TP_RENDERER_ASSERT(!surfacePresentModes.empty(), "[Renderer][Vulkan] Couldn't get available Surface Present Modes!");

	return surfacePresentModes;
}

//-------------------------------------------------------------------------------------------------------------------//

VkSurfaceFormatKHR TRAP::Graphics::API::VulkanContext::ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableSurfaceFormats)
{
	TP_DEBUG("[Renderer][Vulkan] Choosing Swapchain Surface Format");

	if (availableSurfaceFormats.size() == 1 && availableSurfaceFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		TP_DEBUG("[Renderer][Vulkan] Using Surface Format: VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR");
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& availableSurfaceFormat : availableSurfaceFormats)
		if (availableSurfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableSurfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			TP_DEBUG("[Renderer][Vulkan] Using Surface Format: VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR");
			return availableSurfaceFormat;
		}

	TP_DEBUG("[Renderer][Vulkan] Using Surface Format: Physical Device Default Format, Physical Device Default Color Space");
	return availableSurfaceFormats[0];
}

//-------------------------------------------------------------------------------------------------------------------//

VkPresentModeKHR TRAP::Graphics::API::VulkanContext::ChooseSwapchainSurfacePresentMode(const std::vector<VkPresentModeKHR>& availableSurfacePresentModes)
{
	TP_DEBUG("[Renderer][Vulkan] Choosing Swapchain Surface Present Mode");

	for (const auto& availableSurfacePresentMode : availableSurfacePresentModes)
	{
		if (availableSurfacePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			TP_DEBUG("[Renderer][Vulkan] Using Surface Present Mode: VK_PRESENT_MODE_MAILBOX_KHR(Triple Buffered)");
			return availableSurfacePresentMode;
		}
		if (availableSurfacePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
		{
			TP_DEBUG("[Renderer][Vulkan] Using Surface Present Mode: VK_PRESENT_MODE_IMMEDIATE_KHR");
			return availableSurfacePresentMode; //Immediate Swaps High Tearing(Preferred over VSync because of driver incompatibilities)
		}
	}

	TP_DEBUG("[Renderer][Vulkan] Using Surface Present Mode: VK_PRESENT_MOE_FIFO_KHR(VSync Double Buffered");
	return VK_PRESENT_MODE_FIFO_KHR; //VSync mode(Required to be available by Vulkan Specification)
}

//-------------------------------------------------------------------------------------------------------------------//

VkExtent2D TRAP::Graphics::API::VulkanContext::ChooseSwapchainExtent() const
{
	TP_DEBUG("[Renderer][Vulkan] Choosing Swapchain Extent");

	if (m_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		TP_DEBUG("[Renderer][Vulkan] Using Swapchain Extent: ", m_capabilities.currentExtent.width, 'x', m_capabilities.currentExtent.height);
		return m_capabilities.currentExtent;
	}

	VkExtent2D actualExtent = { m_window->GetWidth(), m_window->GetHeight() };

	actualExtent.width = std::max(m_capabilities.minImageExtent.width, std::min(m_capabilities.maxImageExtent.width, actualExtent.width));
	actualExtent.height = std::max(m_capabilities.minImageExtent.height, std::min(m_capabilities.maxImageExtent.height, actualExtent.height));

	TP_DEBUG("[Renderer][Vulkan] Using Swapchain Extent: ", actualExtent.width, 'x', actualExtent.height);

	return actualExtent;
}