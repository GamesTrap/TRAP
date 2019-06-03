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

void TRAP::Graphics::API::VulkanContext::DeInitSurface(VkInstance instance)
{
	if (m_surface)
	{
		TP_DEBUG("[Renderer][Vulkan] Destroying Surface");
		vkDestroySurfaceKHR(instance, m_surface, nullptr);
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

void TRAP::Graphics::API::VulkanContext::DeInitSwapchain(VkDevice device)
{
	if (m_swapchain)
	{
		TP_DEBUG("[Renderer][Vulkan] Destroying Swapchain");
		vkDestroySwapchainKHR(device, m_swapchain, nullptr);
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

void TRAP::Graphics::API::VulkanContext::DeInitImageViews(VkDevice device)
{
	if (!m_swapchainImageViews.empty())
		TP_DEBUG("[Renderer][Vulkan] Destroying Swapchain Image Views");

	for (VkImageView imageView : m_swapchainImageViews)
		vkDestroyImageView(device, imageView, nullptr);
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

//-------------------------------------------------------------------------------------------------------------------//

//I know i know it's a heavy function but I don't want to use the VulkanRenderer/Context functions because this should execute silently
bool TRAP::Graphics::API::VulkanContext::IsVulkanCapable()
{
	if (glfwVulkanSupported())
	{
		//Instance Extensions
		//Get Required Instance Extensions from GLFW
		uint32_t requiredExtensionsCount = 0;
		const char** requiredExtensions = glfwGetRequiredInstanceExtensions(&requiredExtensionsCount);

		//Get Instance Extensions
		uint32_t extensionsCount = 0;
		VkCall(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr));
		std::vector<VkExtensionProperties> availableInstanceExtensions(extensionsCount);
		VkCall(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, availableInstanceExtensions.data()));

		//No Instance Extensions found
		if (extensionsCount == 0 || availableInstanceExtensions.empty())
			return false; 

		//Check if required Instance Extensions are available
		std::vector<const char*> instanceExtensions;
		for (uint32_t i = 0; i < requiredExtensionsCount; i++)
		{
			for (auto& availableExtension : availableInstanceExtensions)
				if (strcmp(availableExtension.extensionName, requiredExtensions[i]) == 0)
					instanceExtensions.push_back(requiredExtensions[i]);					
		}

		if (instanceExtensions.empty())
			return false;

		//Check if a required Instance Extension is unsupported
		for(uint32_t i = 0; i < requiredExtensionsCount; i++)
		{
			auto extension = std::find(instanceExtensions.begin(), instanceExtensions.end(), requiredExtensions[i]);
			if (extension == instanceExtensions.end())
				return false;			
		}
		//All required Instance Extensions are available

		//Check if instance can be created
		VkApplicationInfo applicationInfo
		{
			VK_STRUCTURE_TYPE_APPLICATION_INFO,
			nullptr,
			"Vulkan capability test",
			VK_MAKE_VERSION(1, 0, 0),
			"TRAP Engine",
			TRAP_VERSION,
			VK_API_VERSION_1_1
		};

		VkInstanceCreateInfo instanceCreateInfo
		{
			VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			nullptr,
			0,
			&applicationInfo,
			0,
			nullptr,
			static_cast<uint32_t>(instanceExtensions.size()),
			instanceExtensions.data()
		};

		VkInstance instance;
		VkCall(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));
		if (!instance)
			return false;

		//Create Vulkan 1.1 Test window
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_VISIBLE, false);
		glfwWindowHint(GLFW_FOCUSED, false);
		glfwWindowHint(GLFW_FOCUS_ON_SHOW, false);
		GLFWwindow* VulkanTestWindow = glfwCreateWindow(800, 600, "Vulkan Tester", nullptr, nullptr);
		glfwDefaultWindowHints();
		if (!VulkanTestWindow)
			return false;

		VkSurfaceKHR surface;
		VkCall(glfwCreateWindowSurface(instance, VulkanTestWindow, nullptr, &surface));

		if(!surface)
		{
			//Needs to be destroyed after testing
			vkDestroyInstance(instance, nullptr);

			return false;
		}

		//Get Physical Devices
		uint32_t physicalDevicesCount = 0;
		VkCall(vkEnumeratePhysicalDevices(instance, &physicalDevicesCount, nullptr));
		std::vector<VkPhysicalDevice> physicalDevicesList(physicalDevicesCount);
		VkCall(vkEnumeratePhysicalDevices(instance, &physicalDevicesCount, physicalDevicesList.data()));

		//No physical Devices found
		if(physicalDevicesCount == 0 || physicalDevicesList.empty())
		{
			//Needs to be destroyed after testing
			vkDestroyInstance(instance, nullptr);

			return false;
		}

		//Check physical Devices for features
		std::multimap<int, VkPhysicalDevice> candidates;

		int highestScore = 0;
		int score = 0;
		for (const auto& physicalDevice : physicalDevicesList)
		{
			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

			//Discrete GPUs have a significant performance advantage
			if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				score += 1000;
			else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
				score += 250;

			//Make sure GPU has a Graphics Queue
			uint32_t graphicsFamilyIndex = 0;

			//Get Queue Families
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
			std::vector<VkQueueFamilyProperties> availableQueueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, availableQueueFamilies.data());

			//Check if physical device support Graphics Queue Family
			for (uint32_t i = 0; i < availableQueueFamilies.size(); i++)
				if (availableQueueFamilies[i].queueCount > 0 && availableQueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					graphicsFamilyIndex = i;
					score += 1000;
				}

			//Make sure GPU has Present support and a Present Queue
			VkBool32 presentSupport = false;
			VkCall(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, graphicsFamilyIndex, surface, &presentSupport));
			for (auto& queueFamilyProperty : availableQueueFamilies)
				if (queueFamilyProperty.queueCount > 0 && presentSupport)
					score += 1000;

			//Maximum possible size of textures affects graphics quality
			score += deviceProperties.limits.maxImageDimension2D;

			//Get available Device Extensions
			uint32_t extensionCount = 0;
			VkCall(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr));
			std::vector<VkExtensionProperties> availableDeviceExtensions(extensionCount);
			VkCall(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableDeviceExtensions.data()));

			//Make sure GPU has Swapchain support
			for (auto& availableExtension : availableDeviceExtensions)
				if (strcmp(availableExtension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
				{
					//GPU Supports Swapchains
					score += 1000;

					//Double Check to make sure GPU really has Swapchain support :D

					//Get available surface formats
					uint32_t surfaceFormatCount = 0;
					std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
					VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr));
					std::vector<VkSurfaceFormatKHR> availableSurfaceFormats{ surfaceFormatCount };
					VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats.data()));
					if (!surfaceFormats.empty())
						score += 1000;

					//Get Available surface present modes
					uint32_t surfacePresentModeCount = 0;
					std::vector<VkPresentModeKHR> presentModes(surfacePresentModeCount);
					VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &surfacePresentModeCount, nullptr));
					std::vector<VkPresentModeKHR> surfacePresentModes{ surfacePresentModeCount };
					VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &surfacePresentModeCount, presentModes.data()));
					if (!presentModes.empty())
						score += 1000;
				}
			if (score > highestScore)
				highestScore = score;

			candidates.insert(std::make_pair(score, physicalDevice));
		}

		if(candidates.empty())
		{
			//Needs to be destroyed after testing
			vkDestroySurfaceKHR(instance, surface, nullptr);

			glfwDestroyWindow(VulkanTestWindow);

			//Needs to be destroyed after testing
			vkDestroyInstance(instance, nullptr);

			return false;
		}

		//Use first physical Device with highest score
		VkPhysicalDevice physicalDevice;
		for (const auto& candidate : candidates)
			if (candidate.first == highestScore)
			{
				physicalDevice = candidate.second;
				break;
			}

		//Get Queue Families
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> availableQueueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, availableQueueFamilies.data());

		//Check if best physical Device supports Graphics Queue Family
		bool supportsGraphicsQueueFamily = false;
		uint32_t graphicsFamilyIndex = 0;
		for (uint32_t i = 0; i < availableQueueFamilies.size(); i++)
			if (availableQueueFamilies[i].queueCount > 0 && availableQueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				graphicsFamilyIndex = i;
				supportsGraphicsQueueFamily = true;
				break;
			}

		if(!supportsGraphicsQueueFamily)
		{
			//Needs to be destroyed after testing
			vkDestroySurfaceKHR(instance, surface, nullptr);

			glfwDestroyWindow(VulkanTestWindow);

			//Needs to be destroyed after testing
			vkDestroyInstance(instance, nullptr);

			return false;
		}

		//Check if physical Device supports presenting and has a present Queue
		bool presentQueue = false;
		VkBool32 presentSupport = false;
		VkCall(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, graphicsFamilyIndex, surface, &presentSupport));
		for (auto& queueFamilyProperty : availableQueueFamilies)
			if (queueFamilyProperty.queueCount > 0 && presentSupport)
				presentQueue = true;

		if(!presentQueue || !presentSupport)
		{
			//Needs to be destroyed after testing
			vkDestroySurfaceKHR(instance, surface, nullptr);

			glfwDestroyWindow(VulkanTestWindow);

			//Needs to be destroyed after testing
			vkDestroyInstance(instance, nullptr);

			return false;
		}

		//Get available Device Extensions
		uint32_t extensionCount = 0;
		VkCall(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr));
		std::vector<VkExtensionProperties> availableDeviceExtensions(extensionCount);
		VkCall(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableDeviceExtensions.data()));

		//Check if physical Device has Swapchain support
		bool swapchainSupported = false;
		for (auto& availableExtension : availableDeviceExtensions)
			if (strcmp(availableExtension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
				swapchainSupported = true;

		if (!swapchainSupported)
		{
			//Needs to be destroyed after testing
			vkDestroySurfaceKHR(instance, surface, nullptr);

			glfwDestroyWindow(VulkanTestWindow);

			//Needs to be destroyed after testing
			vkDestroyInstance(instance, nullptr);

			return false;
		}

		//Needs to be destroyed after testing
		vkDestroySurfaceKHR(instance, surface, nullptr);

		glfwDestroyWindow(VulkanTestWindow);

		//Needs to be destroyed after testing
		vkDestroyInstance(instance, nullptr);

		return true;
	}

	return false;
}