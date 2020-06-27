#include "TRAPPCH.h"

#include "VulkanPhysicalDevice.h"

#include "Graphics/API/Vulkan/Internals/VulkanQueueFamilyIndices.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"

std::multimap<int32_t, TRAP::Graphics::API::Vulkan::PhysicalDevice> TRAP::Graphics::API::Vulkan::PhysicalDevice::s_availableGraphicPhysicalDevices{};
TRAP::Graphics::API::Vulkan::Instance* TRAP::Graphics::API::Vulkan::PhysicalDevice::s_instance = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::Vulkan::PhysicalDevice::PhysicalDevice(VkPhysicalDevice device, const QueueFamilyIndices indices)
	: m_device(device), m_familyIndices(indices), m_deviceProperties(), m_deviceProperties11(), m_deviceFeatures(), m_deviceMemoryProperties(), m_rayTracing(false)
{
	vkGetPhysicalDeviceProperties(m_device, &m_deviceProperties);
	vkGetPhysicalDeviceFeatures(m_device, &m_deviceFeatures);
	vkGetPhysicalDeviceMemoryProperties(m_device, &m_deviceMemoryProperties);

	m_deviceProperties11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
	VkPhysicalDeviceProperties2 props2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, &m_deviceProperties11, {} };
	vkGetPhysicalDeviceProperties2(device, &props2);

	uint32_t extensionCount = 0;
	VkCall(vkEnumerateDeviceExtensionProperties(m_device, nullptr, &extensionCount, nullptr));
	m_deviceExtensions.resize(extensionCount);
	VkCall(vkEnumerateDeviceExtensionProperties(m_device, nullptr, &extensionCount, m_deviceExtensions.data()));

	//Get GPU UUID
	VkPhysicalDeviceIDProperties propID;
	propID.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES;
	propID.pNext = nullptr;
	VkPhysicalDeviceProperties2 props21{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, &propID, {} };
	vkGetPhysicalDeviceProperties2(device, &props21);
	m_deviceUUID.resize(16, 0);
	for(uint8_t i = 0; i < 16; i++)
		m_deviceUUID[i] = propID.deviceUUID[i];
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::Vulkan::PhysicalDevice::~PhysicalDevice() = default;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::Vulkan::QueueFamilyIndices& TRAP::Graphics::API::Vulkan::PhysicalDevice::GetQueueFamilyIndices()
{
	return m_familyIndices;
}

//-------------------------------------------------------------------------------------------------------------------//

VkPhysicalDevice& TRAP::Graphics::API::Vulkan::PhysicalDevice::GetPhysicalDevice()
{
	return m_device;
}

//-------------------------------------------------------------------------------------------------------------------//

VkPhysicalDeviceProperties& TRAP::Graphics::API::Vulkan::PhysicalDevice::GetPhysicalDeviceProperties()
{
	return m_deviceProperties;
}

//-------------------------------------------------------------------------------------------------------------------//

VkPhysicalDeviceVulkan11Properties& TRAP::Graphics::API::Vulkan::PhysicalDevice::GetPhysicalDeviceVulkan11Properties()
{
	return m_deviceProperties11;
}

//-------------------------------------------------------------------------------------------------------------------//

VkPhysicalDeviceFeatures& TRAP::Graphics::API::Vulkan::PhysicalDevice::GetPhysicalDeviceFeatures()
{
	return m_deviceFeatures;
}

//-------------------------------------------------------------------------------------------------------------------//

VkPhysicalDeviceMemoryProperties& TRAP::Graphics::API::Vulkan::PhysicalDevice::GetPhysicalDeviceMemoryProperties()
{
	return m_deviceMemoryProperties;
}

//-------------------------------------------------------------------------------------------------------------------//

VkFormatProperties TRAP::Graphics::API::Vulkan::PhysicalDevice::GetPhysicalDeviceFormatProperties(const VkFormat format) const
{
	VkFormatProperties formatProperties{};
	vkGetPhysicalDeviceFormatProperties(m_device, format, &formatProperties);

	return formatProperties;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::API::Vulkan::PhysicalDevice::GetPhysicalDeviceName() const
{
	return std::string(m_deviceProperties.deviceName);
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<VkExtensionProperties>& TRAP::Graphics::API::Vulkan::PhysicalDevice::GetAvailableExtensions() const
{
	return m_deviceExtensions;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::Graphics::API::Vulkan::PhysicalDevice::GetUUID() const
{
	return m_deviceUUID;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::Vulkan::PhysicalDevice::IsExtensionSupported(const char* extension)
{
	const auto layerResult = std::find_if(m_deviceExtensions.begin(), m_deviceExtensions.end(),
		[extension](const VkExtensionProperties& props) { return strcmp(props.extensionName, extension) == 0; });
	if (layerResult == m_deviceExtensions.end())
	{
		if (strcmp(extension, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0)
			TP_WARN("[Renderer][Vulkan][PhysicalDevice] Extension ", extension, " is not supported(Vulkan SDK installed?)");
		else
			TP_WARN("[Renderer][Vulkan][PhysicalDevice] Extension ", extension, " is not supported");

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::Vulkan::PhysicalDevice::IsRayTracingSupported() const
{
	return m_rayTracing;
}

//-------------------------------------------------------------------------------------------------------------------//

std::multimap<int32_t, TRAP::Graphics::API::Vulkan::PhysicalDevice> TRAP::Graphics::API::Vulkan::PhysicalDevice::GetAllAvailableGraphicPhysicalDevices(const Scope<Instance>& instance,
	INTERNAL::WindowingAPI::InternalWindow* window)
{
	//Needed for surface creation: Both of physicalDevice, and surface must have been created, allocated, or retrieved from the same VkInstance
	if (s_instance && s_instance != instance.get() || !s_instance)
		s_availableGraphicPhysicalDevices.clear();
	
	if (s_availableGraphicPhysicalDevices.empty())
	{
		//Setting up all PhysicalDevice instances
		std::vector<VkPhysicalDevice> devs = GetAvailablePhysicalDevices(instance);
		std::vector<PhysicalDevice> devices;
		for (auto& dev : devs)
		{
			QueueFamilyIndices queueFamily;
			if (PhysicalDeviceSupports(dev, queueFamily))
				devices.emplace_back(dev, queueFamily);
		}

		//Score each Physical Device and insert into multimap
		int32_t score = 0;
		static bool testMessages = true;
		for (PhysicalDevice& dev : devices)
		{			
			//Required: Discrete GPUs have a significant performance advantage
			if (dev.m_deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				score += 5000;
			else if (dev.m_deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
				score += 2500;
			else
			{
				TP_ERROR("[Context][Vulkan] Failed Dedicated/Internal GPU Check!");
				continue;
			}

			//Required: Check If Physical Device is Vulkan 1.2+ capable
			if (dev.m_deviceProperties.apiVersion < VK_API_VERSION_1_2)
			{
				TP_ERROR("[Context][Vulkan] Failed GPU Vulkan 1.2 Version Check!");
				continue;
			}

			//Required: Check If Physical Device supports swapchains
			if (!dev.IsExtensionSupported(VK_KHR_SWAPCHAIN_EXTENSION_NAME))
			{
				TP_ERROR("[Context][Vulkan] Failed GPU Swapchain Extension Check!");
				continue;
			}

			//Required: Check If a VkSurface can be created using the Physical Device
			Surface surface(instance, dev, window);
			if (!surface.GetSurface())
			{
				TP_ERROR("[Context][Vulkan] Failed GPU Surface Creation Check!");
				continue;
			}

			//Required: Check If Physical Device supports graphics queue
			if (dev.GetQueueFamilyIndices().GraphicsIndices == std::numeric_limits<uint32_t>::max())
			{
				TP_ERROR("[Context][Vulkan] Failed GPU Graphics Queue Check!");
				continue;
			}
			
			//Required: Check If Physical Device supports presenting
			if (dev.GetQueueFamilyIndices().PresentIndices == std::numeric_limits<uint32_t>::max())
			{
				TP_ERROR("[Context][Vulkan] Failed GPU Present Queue Check!");
				continue;
			}

			//Required: Check If Surface contains formats
			if (surface.GetSurfaceFormats().empty())
			{
				TP_ERROR("[Context][Vulkan] Failed GPU Surface Format Check!");
				continue;
			}
			
			//Required: Check If Surface contains present modes
			if (surface.GetPresentModes().empty())
			{
				TP_ERROR("[Context][Vulkan] Failed GPU Present Mode Check!");
				continue;
			}

			//Optionally: Check if Physical Device supports optimal surface format
			if (surface.GetOptimalSurfaceFormat().format == VK_FORMAT_B8G8R8A8_UNORM &&
				surface.GetOptimalSurfaceFormat().colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				score += 100;
			else
				TP_WARN("[Context][Vulkan] Surface doesnt support optimal format!");

			//Optionally: Check if Physical Device supports ray tracing
			if (dev.IsExtensionSupported("VK_KHR_ray_tracing") && dev.IsExtensionSupported("VK_KHR_deferred_host_operations") &&
				dev.IsExtensionSupported("VK_KHR_pipeline_library"))
			{
				score += 2000;
				dev.m_rayTracing = true;
			}
			else
				TP_WARN("[Context][Vulkan] GPU Does Not Support RayTracing!");

			//Optionally: Check VRAM size (1e+9 == Bytes to Gigabytes)
			for (uint32_t i = 0; i < dev.m_deviceMemoryProperties.memoryHeapCount; i++)
				score += static_cast<uint32_t>(dev.m_deviceMemoryProperties.memoryHeaps[i].size / 1e+9 * 100);

			//Optionally: Check 2D & Cube Images Max Size
			score += dev.m_deviceProperties.limits.maxImageDimension2D;
			score += dev.m_deviceProperties.limits.maxImageDimensionCube;

			//Optionally: Check if Physical Device has WireFrame capability
			if (dev.m_deviceFeatures.fillModeNonSolid)
				score += 1000;
			else
				TP_WARN("[Context][Vulkan] GPU Does Not Support WireFrame!");

			//Optionally: Check if Physical Device has geometry shader capability
			if (dev.m_deviceFeatures.geometryShader)
				score += 1000;
			else
				TP_WARN("[Context][Vulkan] GPU Does Not Support Geometry Shaders!");

			//Optionally: Check if Physical Device has tessellation shader capability
			if (dev.m_deviceFeatures.tessellationShader)
				score += 1000;
			else
				TP_WARN("[Context][Vulkan] GPU Does Not Support Tessellation Shaders");

			//Optionally: Check if Physical Device has Anisotropy Filtering capability
			if(dev.m_deviceFeatures.samplerAnisotropy)
				score += 1000;
			else
				TP_WARN("[Context][Vulkan] GPU Does Not Support Anisotropic Filtering");

			if(testMessages)
				TP_DEBUG("[Context][Vulkan] Found GPU: \"", dev.GetPhysicalDeviceName(), "\" Score: ", score);
			s_availableGraphicPhysicalDevices.insert({ score, std::move(dev) });
			score = 0;
		}
		
		testMessages = false;
		s_instance = instance.get();
	}

	return s_availableGraphicPhysicalDevices;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<VkPhysicalDevice> TRAP::Graphics::API::Vulkan::PhysicalDevice::GetAvailablePhysicalDevices(const Scope<Instance>& instance)
{
	//Get Physical Device Count
	uint32_t deviceCount = 0;
	VkCall(vkEnumeratePhysicalDevices(instance->GetInstance(), &deviceCount, nullptr));

	//Get All Physical Devices
	std::vector<VkPhysicalDevice> devices(deviceCount);
	VkCall(vkEnumeratePhysicalDevices(instance->GetInstance(), &deviceCount, devices.data()));

	return devices;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::Vulkan::PhysicalDevice::PhysicalDeviceSupports(VkPhysicalDevice& device, QueueFamilyIndices& familyIndices)
{
	return SupportsQueueFamily(device, familyIndices);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::Vulkan::PhysicalDevice::SupportsQueueFamily(VkPhysicalDevice& device, QueueFamilyIndices& familyIndices)
{
	//Get Queue Family Count
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	//Get Queue Families Properties
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	uint32_t i = 0;
	for(VkQueueFamilyProperties& queueFamily : queueFamilies)
	{
		if(queueFamily.queueCount > 0 && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
			familyIndices.GraphicsIndices = i;

		if (familyIndices.GraphicsIndices < std::numeric_limits<uint32_t>::max())
			return true;
		
		i++;
	}
	
	return false;
}