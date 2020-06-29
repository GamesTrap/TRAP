#include "TRAPPCH.h"

#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "VulkanSurface.h"
#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "Graphics/API/Vulkan/Internals/VulkanQueueFamilyIndices.h"

TRAP::Graphics::API::Vulkan::Surface::Surface(const Scope<Instance>& instance, PhysicalDevice& device, INTERNAL::WindowingAPI::InternalWindow* window)
	: m_device(&device), m_surface(nullptr), m_surfaceCapabilities(), m_instance(instance.get()), m_usedSurfaceFormat(), m_usedPresentMode(), m_supportDepthStencil(false)
{	
	VkCall(INTERNAL::WindowingAPI::CreateWindowSurface(instance->GetInstance(), window, nullptr, m_surface));

	VkCall(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.GetPhysicalDevice(), m_surface, &m_surfaceCapabilities));

	uint32_t surfaceFormatCount = 0;
	VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(device.GetPhysicalDevice(), m_surface, &surfaceFormatCount, nullptr));
	m_surfaceFormats.resize(surfaceFormatCount);
	VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(device.GetPhysicalDevice(), m_surface, &surfaceFormatCount, m_surfaceFormats.data()));

	uint32_t surfacePresentModeCount = 0;
	VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(device.GetPhysicalDevice(), m_surface, &surfacePresentModeCount, nullptr));
	m_presentModes.resize(surfacePresentModeCount);
	VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(device.GetPhysicalDevice(), m_surface, &surfacePresentModeCount, m_presentModes.data()));

	//Get all Queue families
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device.GetPhysicalDevice(), &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> availableQueueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device.GetPhysicalDevice(), &queueFamilyCount, availableQueueFamilies.data());

	//Reset QueueFamilyIndices
	device.GetQueueFamilyIndices().GraphicsIndices = std::numeric_limits<uint32_t>::max();
	device.GetQueueFamilyIndices().PresentIndices = std::numeric_limits<uint32_t>::max();
	device.GetQueueFamilyIndices().ComputeIndices = std::numeric_limits<uint32_t>::max();
	device.GetQueueFamilyIndices().TransferIndices = std::numeric_limits<uint32_t>::max();

	//Get new QueueFamilyIndices
	for (uint32_t i = 0; i < availableQueueFamilies.size(); i++)
	{
		if (availableQueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			device.GetQueueFamilyIndices().GraphicsIndices = i;

		if (availableQueueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
			device.GetQueueFamilyIndices().ComputeIndices = i;

		if (availableQueueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT && !(availableQueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
			device.GetQueueFamilyIndices().TransferIndices = i;

		VkBool32 presentSupport = false;
		VkCall(vkGetPhysicalDeviceSurfaceSupportKHR(device.GetPhysicalDevice(), i, m_surface, &presentSupport));
		if (presentSupport)
			device.GetQueueFamilyIndices().PresentIndices = i;

		if (device.GetQueueFamilyIndices().GraphicsIndices != std::numeric_limits<uint32_t>::max() &&
			device.GetQueueFamilyIndices().PresentIndices != std::numeric_limits<uint32_t>::max() &&
			device.GetQueueFamilyIndices().ComputeIndices != std::numeric_limits<uint32_t>::max() &&
			device.GetQueueFamilyIndices().TransferIndices != std::numeric_limits<uint32_t>::max())
			break;
	}

	//Select Surface Format
	for(auto& format : m_surfaceFormats)
	{
		if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			m_usedSurfaceFormat = &format;
			break;
		}
	}
	if (!m_usedSurfaceFormat)
		m_usedSurfaceFormat = &m_surfaceFormats[0];

	//Select Surface Present Mode
	for(auto& presentMode : m_presentModes)
	{
		if(presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			m_usedPresentMode = presentMode;
			break;
		}
	}
	if (!m_usedPresentMode)
		m_usedPresentMode = VK_PRESENT_MODE_FIFO_KHR;

	//Select Depth Format
	//Since all depth formats may be optional, we need to find a suitable depth format to use
	//Start with the highest precision packed format
	std::array<VkFormat, 5> depthFormats
	{
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM
	};
	for(VkFormat format : depthFormats)
	{
		const VkFormatProperties formatProperties = device.GetPhysicalDeviceFormatProperties(format);

		//Format must support depth stencil attachment for optional tiling
		if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			m_supportDepthStencil = true;
			m_depthFormat = format;
			break;
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::Vulkan::Surface::~Surface()
{
	if(m_surface)
	{
		vkDestroySurfaceKHR(m_instance->GetInstance(), m_surface, nullptr);
		m_surface = nullptr;
	}
};

//-------------------------------------------------------------------------------------------------------------------//

VkSurfaceKHR TRAP::Graphics::API::Vulkan::Surface::GetSurface() const
{
	return m_surface;
}

//-------------------------------------------------------------------------------------------------------------------//

VkSurfaceCapabilitiesKHR TRAP::Graphics::API::Vulkan::Surface::GetSurfaceCapabilities()
{
    VkCall(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_device->GetPhysicalDevice(), m_surface, &m_surfaceCapabilities));
    
	return m_surfaceCapabilities;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<VkSurfaceFormatKHR>& TRAP::Graphics::API::Vulkan::Surface::GetSurfaceFormats() const
{
	return m_surfaceFormats;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<VkPresentModeKHR>& TRAP::Graphics::API::Vulkan::Surface::GetPresentModes() const
{
	return m_presentModes;
}

//-------------------------------------------------------------------------------------------------------------------//

VkSurfaceFormatKHR& TRAP::Graphics::API::Vulkan::Surface::GetOptimalSurfaceFormat() const
{
	return *m_usedSurfaceFormat;
}

//-------------------------------------------------------------------------------------------------------------------//

VkFormat* TRAP::Graphics::API::Vulkan::Surface::GetOptimalDepthFormat()
{
	if(m_supportDepthStencil)
		return &m_depthFormat;

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

VkPresentModeKHR& TRAP::Graphics::API::Vulkan::Surface::GetOptimalPresentMode()
{
	return m_usedPresentMode;
}
