#include "TRAPPCH.h"
#include "VulKanRenderer.h"

namespace TRAP::Graphics
{
	VulkanRenderer::VulkanRenderer()
	: m_instance(nullptr),
	  m_physicalDevice(nullptr),
	  m_physicalDeviceProperties(),
	  m_physicalDeviceMemoryProperties(),
	  m_device(nullptr),
	  m_graphicsQueue(nullptr),
	  m_presentQueue(nullptr),
	  m_debugCallbackSupported(false),
	  m_debugReport(),
	  m_context(API::VulkanContext::Get())
	{
	}

	VulkanRenderer::~VulkanRenderer()
	{
		m_context->DeInitImageViews();
		m_context->DeInitSwapchain();
		DeInitDevice();
		DeInitDebug();
		m_context->DeInitSurface();
		DeInitInstance();
		TP_DEBUG("[Renderer][Vulkan] Destroying Renderer");
	}

	void VulkanRenderer::InitInternal()
	{
		if(glfwVulkanSupported())
		{
			SetupInstanceLayersAndExtensions();
			InitInstance();
			m_context->InitSurface();
			InitDebug();
			SetupPhysicalDevice();
			SetupDeviceLayersAndExtensions();
			InitDevice();
			m_context->SetupSwapchain();
			m_context->InitSwapchain();
			m_context->InitImageViews();

			//Not implemented
			SetDepthTesting(true);
			SetBlend(true);
			SetBlendFunction(RendererBlendFunction::SOURCE_ALPHA, RendererBlendFunction::ONE_MINUS_SOURCE_ALPHA);
			//////////////////

			TP_INFO("[Renderer][Vulkan] ----------------------------------");
			TP_INFO("[Renderer][Vulkan] Vulkan:");
			TP_INFO("[Renderer][Vulkan] Version:  ", VK_VERSION_MAJOR(m_physicalDeviceProperties.apiVersion), '.', VK_VERSION_MINOR(m_physicalDeviceProperties.apiVersion), '.', VK_VERSION_PATCH(m_physicalDeviceProperties.apiVersion));
			TP_INFO("[Renderer][Vulkan] Renderer: ", m_physicalDeviceProperties.deviceName);
			TP_INFO("[Renderer][Vulkan] Driver:   ", VK_VERSION_MAJOR(m_physicalDeviceProperties.driverVersion), '.', VK_VERSION_MINOR(m_physicalDeviceProperties.driverVersion), '.', VK_VERSION_PATCH(m_physicalDeviceProperties.driverVersion));
			TP_INFO("[Renderer][Vulkan] ----------------------------------");

			m_rendererTitle = "[Vulkan " + std::to_string(VK_VERSION_MAJOR(m_physicalDeviceProperties.apiVersion)) + '.' + std::to_string(VK_VERSION_MINOR(m_physicalDeviceProperties.apiVersion)) + '.' + std::to_string(VK_VERSION_PATCH(m_physicalDeviceProperties.apiVersion)) + ']';
		}
		else //TODO INFO Switch API(exiting program)
		{
			TP_CRITICAL("[Renderer][Vulkan] Vulkan is unsupported!");
			TP_CRITICAL("[Renderer][Vulkan] Shutting down!");
			exit(-1);
		}
	}

	void VulkanRenderer::ClearInternal(unsigned int buffer)
	{
	}

	void VulkanRenderer::PresentInternal(Window* window)
	{
		m_context->Present(window);
	}

	void VulkanRenderer::SetDepthTestingInternal(bool enabled)
	{
	}

	void VulkanRenderer::SetBlendInternal(bool enabled)
	{
	}

	void VulkanRenderer::SetViewportInternal(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
	}

	void VulkanRenderer::SetBlendFunctionInternal(RendererBlendFunction source, RendererBlendFunction destination)
	{
	}

	void VulkanRenderer::SetBlendEquationInternal(RendererBlendEquation blendEquation)
	{
	}

	std::string_view VulkanRenderer::GetTitleInternal() const
	{
		return m_rendererTitle;
	}

	void VulkanRenderer::SetupInstanceLayersAndExtensions()
	{
		TP_DEBUG("[Renderer][Vulkan] Setting up Instance Layers and Extensions");

		//Instance Layers
		const std::vector<VkLayerProperties> availableInstanceLayers = GetAvailableInstanceLayers();
	#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
		AddInstanceLayer(availableInstanceLayers, "VK_LAYER_KHRONOS_validation");
	#endif

		//Instance Extensions
		uint32_t requiredExtensionsCount = 0;
		const char** requiredExtensions = glfwGetRequiredInstanceExtensions(&requiredExtensionsCount);
		const std::vector<VkExtensionProperties> availableInstanceExtensions = GetAvailableInstanceExtensions();
		for (uint32_t i = 0; i < requiredExtensionsCount; i++)
			AddInstanceExtension(availableInstanceExtensions, requiredExtensions[i]);
	#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
		AddInstanceExtension(availableInstanceExtensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	#endif
	}

#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, const VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData)
	{
		std::ostringstream stream;

		stream << "[Renderer][Vulkan]";
		if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
			stream << "[Violation] ";
		else if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
			stream << "[Performance] ";
		else
			stream << ' ';
		stream << callbackData->messageIdNumber << '(' << callbackData->pMessageIdName << ") " << callbackData->pMessage;
		if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
			TP_DEBUG(stream.str());
		if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
			TP_INFO(stream.str());
		if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			TP_WARN(stream.str());
		if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			TP_ERROR(stream.str());

		return false;
	}
#endif

	void VulkanRenderer::SetupDeviceLayersAndExtensions()
	{
		TP_DEBUG("[Renderer][Vulkan] Setting up Device Layers and Extensions");

		//Device Layers only for compatibility(Deprecated See Vulkan Specification)
		const std::vector<VkLayerProperties> availableDeviceLayers = GetAvailableDeviceLayers();
	#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
		AddDeviceLayer(availableDeviceLayers, "VK_LAYER_KHRONOS_validation");
	#endif

		//Device Extensions
		const std::vector<VkExtensionProperties> availableDeviceExtensions = GetAvailableDeviceExtensions();
		AddDeviceExtension(availableDeviceExtensions, VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}

	void VulkanRenderer::SetupPhysicalDevice()
	{
		TP_DEBUG("[Renderer][Vulkan] Setting up Physical Device");

		//Physical Devices
		std::vector<VkPhysicalDevice> physicalDevices = GetAvailablePhysicalDevices();
	#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
		for (const auto& physicalDevice : physicalDevices)
		{
			VkPhysicalDeviceProperties physicalDeviceProps;
			vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProps);
			TP_DEBUG("[Renderer][Vulkan] Found Physical Device: ", physicalDeviceProps.deviceName);
		}
	#endif

		PickPhysicalDevice(physicalDevices); //TODO INFO Critical if no suitable Physical Device was found | Switch RenderAPI
	}

	void VulkanRenderer::SetupQueues()
	{
		TP_DEBUG("[Renderer][Vulkan] Setting up Graphics and Present Queue Family");

		std::vector<VkQueueFamilyProperties> availableQueueFamilies = GetAvailableQueueFamilies();
		for (uint32_t i = 0; i < availableQueueFamilies.size(); i++)
			if (availableQueueFamilies[i].queueCount > 0 && availableQueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				m_graphicsFamilyIndex = i;

		VkBool32 presentSupport = false;
		VkCall(vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice, m_graphicsFamilyIndex.value(), m_context->GetSurface(), &presentSupport));
		for (uint32_t i = 0; i < availableQueueFamilies.size(); i++)
			if (availableQueueFamilies[i].queueCount > 0 && presentSupport)
				m_presentFamilyIndex = i;
	}

	void VulkanRenderer::InitInstance()
	{
		TP_DEBUG("[Renderer][Vulkan] Initializing Instance");

		VkApplicationInfo applicationInfo
		{
			VK_STRUCTURE_TYPE_APPLICATION_INFO,
			nullptr,
			m_context->GetWindow()->GetTitle().data(),
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
			static_cast<uint32_t>(m_instanceLayers.size()),
			!m_instanceLayers.empty() ? m_instanceLayers.data() : nullptr,
			static_cast<uint32_t>(m_instanceExtensions.size()),
			!m_instanceExtensions.empty() ? m_instanceExtensions.data() : nullptr
		};

		VkCall(vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance));
		TP_RENDERER_ASSERT(m_instance, "[Renderer][Vulkan] Couldn't create Instance!"); //TODO INFO Critical switch Render API
	}

	void VulkanRenderer::DeInitInstance()
	{
		if (m_instance)
		{
			TP_DEBUG("[Renderer][Vulkan] Destroying Instance");
			vkDestroyInstance(m_instance, nullptr);
			m_instance = nullptr;
		}
	}

#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
	PFN_vkCreateDebugUtilsMessengerEXT fvkCreateDebugUtilsMessengerEXT = nullptr;
	PFN_vkDestroyDebugUtilsMessengerEXT fvkDestroyDebugUtilsMessengerEXT = nullptr;

	void VulkanRenderer::InitDebug()
	{
		if(m_debugCallbackSupported)
		{
			TP_DEBUG("[Renderer][Vulkan] Initializing Debug Callback");

			fvkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT"));
			fvkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT"));

			if (fvkCreateDebugUtilsMessengerEXT == nullptr || fvkDestroyDebugUtilsMessengerEXT == nullptr)
			{
				TP_ERROR("[Renderer][Vulkan] Couldn't fetch debug function pointers!");
				return;
			}

			VkDebugUtilsMessengerCreateInfoEXT debugCallbackCreateInfo
			{
				VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
				nullptr,
				0,
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
				VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
				VulkanDebugCallback,
				nullptr
			};

			VkCall(fvkCreateDebugUtilsMessengerEXT(m_instance, &debugCallbackCreateInfo, nullptr, &m_debugReport));
			TP_RENDERER_ASSERT(m_debugReport, "[Renderer][Vulkan] Couldn't create Debug Utils Messenger!");
		}		
	}

	void VulkanRenderer::DeInitDebug()
	{
		if (m_debugReport && m_debugCallbackSupported)
		{
			TP_DEBUG("[Renderer][Vulkan] Destroying Debug Callback");
			fvkDestroyDebugUtilsMessengerEXT(m_instance, m_debugReport, nullptr);
			m_debugReport = nullptr;
		}
	}
#else
	void VulkanRenderer::InitDebug() {}
	void VulkanRenderer::DeInitDebug() {}
#endif

	void VulkanRenderer::InitDevice()
	{
		TP_DEBUG("[Renderer][Vulkan] Initializing Device");

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { m_graphicsFamilyIndex.value(), m_presentFamilyIndex.value() };
		float queuePriority = 1.0f;
		for(uint32_t queueFamily : uniqueQueueFamilies)
			queueCreateInfos.emplace_back(VkDeviceQueueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, nullptr, 0, queueFamily, 1, &queuePriority });

		//Enable Device Features here if needed
		VkPhysicalDeviceFeatures deviceFeatures{};

		TP_DEBUG("[Renderer][Vulkan] Initializing Device");

		VkDeviceCreateInfo deviceCreateInfo
		{
			VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			nullptr,
			0,
			static_cast<uint32_t>(queueCreateInfos.size()),
			!queueCreateInfos.empty() ? queueCreateInfos.data() : nullptr,
			static_cast<uint32_t>(m_deviceLayers.size()),
			!m_deviceLayers.empty() ? m_deviceLayers.data() : nullptr,
			static_cast<uint32_t>(m_deviceExtensions.size()),
			!m_deviceExtensions.empty() ? m_deviceExtensions.data() : nullptr,
			&deviceFeatures
		};

		VkCall(vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_device));
		TP_RENDERER_ASSERT(m_device, "[Renderer][Vulkan] Couldn't create Logical Device!");

		vkGetDeviceQueue(m_device, m_graphicsFamilyIndex.value(), 0, &m_graphicsQueue);

		vkGetDeviceQueue(m_device, m_presentFamilyIndex.value(), 0, &m_presentQueue);
	}

	void VulkanRenderer::DeInitDevice()
	{
		if(m_device)
		{
			TP_DEBUG("[Renderer][Vulkan] Destroying Device");
			vkDestroyDevice(m_device, nullptr);
			m_device = nullptr;
		}
	}

	void VulkanRenderer::PickPhysicalDevice(std::vector<VkPhysicalDevice>& availablePhysicalDevices)
	{
		TP_DEBUG("[Renderer][Vulkan] Selecting Physical Device");

		std::multimap<int, VkPhysicalDevice> candidates;

		int highestScore = 0;
		int score = 0;
		for (const auto& device : availablePhysicalDevices)
		{
			score = RateDeviceSuitability(device);
			if (score > highestScore)
				highestScore = score;

			candidates.insert(std::make_pair(score, device));
		}

		if (!candidates.empty())
		{
			//Use first physical Device with highest score
			for (const auto& candidate : candidates)
				if (candidate.first == highestScore)
				{
					m_physicalDevice = candidate.second;
					break;
				}
			vkGetPhysicalDeviceProperties(m_physicalDevice, &m_physicalDeviceProperties);
			vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_physicalDeviceMemoryProperties);
			TP_DEBUG("[Renderer][Vulkan] Selected Physical Device: ", m_physicalDeviceProperties.deviceName, "(Score: ", candidates.begin()->first, ')');

			SetupQueues();
		}
		else
		{			
			TP_CRITICAL("[Renderer][Vulkan] Could not find a suitable Physical Device"); //TODO INFO Switch RenderAPI
			TP_CRITICAL("[Renderer][Vulkan] Vulkan is unsupported!");
			TP_CRITICAL("[Renderer][Vulkan] Shutting down!");
			exit(-1);
		}
	}

	int VulkanRenderer::RateDeviceSuitability(VkPhysicalDevice physicalDevice) const
	{
		int score = 0;

		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

		//Discrete GPUs have a significant performance advantage
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			score += 1000;
		else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
			score += 250;

		//Make sure GPU has a Graphics Queue
		uint32_t graphicsFamilyIndex = 0;
		std::vector<VkQueueFamilyProperties> availableQueueFamilies = GetAvailableQueueFamilies(physicalDevice);
		for (uint32_t i = 0; i < availableQueueFamilies.size(); i++)
			if (availableQueueFamilies[i].queueCount > 0 && availableQueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				graphicsFamilyIndex = i;
				score += 1000;
			}

		//Make sure GPU has Present support and a Present Queue
		VkBool32 presentSupport = false;
		VkCall(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, graphicsFamilyIndex, m_context->GetSurface(), &presentSupport));
		for (auto& queueFamilyProperty : availableQueueFamilies)
			if (queueFamilyProperty.queueCount > 0 && presentSupport)
				score += 1000;

		//Maximum possible size of textures affects graphics quality
		score += deviceProperties.limits.maxImageDimension2D;

		//Make sure GPU has Swapchain support
		for(auto& availableExtension : GetAvailableDeviceExtensions(physicalDevice))
			if(strcmp(availableExtension.extensionName , VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
			{
				//GPU Supports Swapchains
				score += 1000;

				//Double Check to make sure GPU really has Swapchain support :D
				if (!m_context->GetAvailableSurfaceFormats(physicalDevice).empty())
					score += 1000;

				if(!m_context->GetAvailableSurfacePresentModes(physicalDevice).empty())
					score += 1000;
			}	

		TP_DEBUG("[Renderer][Vulkan] Physical Device: ", deviceProperties.deviceName, " Score: ", score);

		return score;
	}	

	bool VulkanRenderer::IsLayerSupported(const std::vector<VkLayerProperties>& availableLayers, const char* layer)
	{
		for (auto& availableLayer : availableLayers)
			if (strcmp(availableLayer.layerName, layer) == 0)
				return true;

		if (strcmp(layer, "VK_LAYER_KHRONOS_validation") == 0)
			TP_WARN("[Renderer][Vulkan] Layer ", layer, " is not supported(Vulkan SDK installed?)");
		else
			TP_WARN("[Renderer][Vulkan] Layer ", layer, " is not supported");

		return false;
	}

	bool VulkanRenderer::IsExtensionSupported(const std::vector<VkExtensionProperties>& availableExtensions, const char* extension)
	{
		for (auto& availableExtension : availableExtensions)
			if (strcmp(availableExtension.extensionName, extension) == 0)
				return true;

		if (strcmp(extension, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0)
			TP_WARN("[Renderer][Vulkan] Extension ", extension, " is not supported(Vulkan SDK installed?)");
		else
			TP_WARN("[Renderer][Vulkan] Extension ", extension, " is not supported");
		if (strcmp(extension, VK_KHR_SURFACE_EXTENSION_NAME) == 0 || strcmp(extension, "VK_KHR_win32_surface") == 0 || strcmp(extension, "VK_MVK_macos_surface") == 0 || strcmp(extension, "VK_KHR_xlib_surface") == 0 || strcmp(extension, "VK_KHR_xcb_surface") == 0 || strcmp(extension, "VK_KHR_wayland_surface") == 0 || strcmp(extension, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
		{
			TP_CRITICAL("[Renderer][Vulkan] Extension ", extension, " is not supported!"); //TODO INFO Critical Switch RenderAPI
			TP_CRITICAL("[Renderer][Vulkan] Vulkan is unsupported!");
			TP_CRITICAL("[Renderer][Vulkan] Shutting down!");
			exit(-1);
		}

		return false;
	}

	std::vector<VkPhysicalDevice> VulkanRenderer::GetAvailablePhysicalDevices() const
	{
		TP_DEBUG("[Renderer][Vulkan] Getting available Physical Devices");

		uint32_t physicalDevicesCount = 0;
		VkCall(vkEnumeratePhysicalDevices(m_instance, &physicalDevicesCount, nullptr));
		std::vector<VkPhysicalDevice> physicalDevicesList(physicalDevicesCount);
		VkCall(vkEnumeratePhysicalDevices(m_instance, &physicalDevicesCount, physicalDevicesList.data()));
		TP_RENDERER_ASSERT(!physicalDevicesList.empty(), "[Renderer][Vulkan] Couldn't find a Physical Device!");

		return physicalDevicesList;
	}

	std::vector<VkLayerProperties> VulkanRenderer::GetAvailableInstanceLayers()
	{
		TP_DEBUG("[Renderer][Vulkan] Getting available Instance Layers");

		uint32_t layersCount = 0;
		VkCall(vkEnumerateInstanceLayerProperties(&layersCount, nullptr));
		std::vector<VkLayerProperties> availableInstanceLayers(layersCount);
		VkCall(vkEnumerateInstanceLayerProperties(&layersCount, availableInstanceLayers.data()));

		return availableInstanceLayers;
	}

	std::vector<VkExtensionProperties> VulkanRenderer::GetAvailableInstanceExtensions()
	{
		TP_DEBUG("[Renderer][Vulkan] Getting available Instance Extensions");

		uint32_t extensionsCount = 0;
		VkCall(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr));
		std::vector<VkExtensionProperties> availableInstanceExtensions(extensionsCount);
		VkCall(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, availableInstanceExtensions.data()));
		TP_RENDERER_ASSERT(!availableInstanceExtensions.empty(), "[Renderer][Vulkan] Couldn't get Instance Extensions");

		return availableInstanceExtensions;
	}

	std::vector<VkLayerProperties> VulkanRenderer::GetAvailableDeviceLayers()
	{
		TP_DEBUG("[Renderer][Vulkan] Getting available Device Layers(Deprecated)");

		uint32_t layerCount = 0;
		VkCall(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));
		std::vector<VkLayerProperties> availableDeviceLayers(layerCount);
		VkCall(vkEnumerateInstanceLayerProperties(&layerCount, availableDeviceLayers.data()));

		return availableDeviceLayers;
	}

	std::vector<VkExtensionProperties> VulkanRenderer::GetAvailableDeviceExtensions() const
	{
		TP_DEBUG("[Renderer][Vulkan] Getting available Device Extensions");

		uint32_t extensionCount = 0;
		VkCall(vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &extensionCount, nullptr));
		std::vector<VkExtensionProperties> availableDeviceExtensions(extensionCount);
		VkCall(vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &extensionCount, availableDeviceExtensions.data()));
		TP_RENDERER_ASSERT(!availableDeviceExtensions.empty(), "[Renderer][Vulkan] Couldn't get Device Extensions!");

		return availableDeviceExtensions;
	}

	std::vector<VkExtensionProperties> VulkanRenderer::GetAvailableDeviceExtensions(VkPhysicalDevice physicalDevice)
	{
		uint32_t extensionCount = 0;
		VkCall(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr));
		std::vector<VkExtensionProperties> availableDeviceExtensions(extensionCount);
		VkCall(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableDeviceExtensions.data()));

		return availableDeviceExtensions;
	}

	std::vector<VkQueueFamilyProperties> VulkanRenderer::GetAvailableQueueFamilies() const
	{
		TP_DEBUG("[Renderer][Vulkan] Getting available Queue Families");

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> availableQueueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, availableQueueFamilies.data());
		TP_RENDERER_ASSERT(!availableQueueFamilies.empty(), "[Renderer][Vulkan] Couldn't get Queue Families!");

		return availableQueueFamilies;
	}

	std::vector<VkQueueFamilyProperties> VulkanRenderer::GetAvailableQueueFamilies(VkPhysicalDevice physicalDevice)
	{
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
		TP_RENDERER_ASSERT(queueFamilyCount, "Could not get the number of queue families");
		std::vector<VkQueueFamilyProperties> availableQueueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, availableQueueFamilies.data());
		TP_RENDERER_ASSERT(!availableQueueFamilies.empty(), "Could not enumerate queue families");

		return availableQueueFamilies;
	}

	void VulkanRenderer::AddInstanceLayer(const std::vector<VkLayerProperties>& availableInstanceLayers, const char* layer)
	{
		if (IsLayerSupported(availableInstanceLayers, layer))
		{
			m_instanceLayers.push_back(layer);
			TP_DEBUG("[Renderer][Vulkan] Loading Instance Layer: ", layer);
		}
	}

	void VulkanRenderer::AddInstanceExtension(const std::vector<VkExtensionProperties>& availableInstanceExtensions, const char* extension)
	{
		if (IsExtensionSupported(availableInstanceExtensions, extension))
		{
			if (strcmp(extension, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0)
				m_debugCallbackSupported = true;

			m_instanceExtensions.push_back(extension);
			TP_DEBUG("[Renderer][Vulkan] Loading Instance Extension: ", extension);
		}
	}

	void VulkanRenderer::AddDeviceLayer(const std::vector<VkLayerProperties>& availableDeviceLayers, const char* layer)
	{
		if (IsLayerSupported(availableDeviceLayers, layer))
		{
			m_deviceLayers.push_back(layer);
			TP_DEBUG("[Renderer][Vulkan] Loading Device Layer(Deprecated): ", layer);
		}
	}

	void VulkanRenderer::AddDeviceExtension(const std::vector<VkExtensionProperties>& availableDeviceExtensions, const char* extension)
	{
		if (IsExtensionSupported(availableDeviceExtensions, extension))
		{
			m_deviceExtensions.push_back(extension);
			TP_DEBUG("[Renderer][Vulkan] Loading Device Extension: ", extension);
		}
	}
}
