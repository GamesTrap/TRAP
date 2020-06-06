#include "TRAPPCH.h"
#include "VulkanInstance.h"
#include "Graphics/API/Vulkan/Internals/VulkanInitializers.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

std::vector<VkLayerProperties> TRAP::Graphics::API::Vulkan::Instance::s_availableLayers{};
std::vector<VkExtensionProperties> TRAP::Graphics::API::Vulkan::Instance::s_availableExtensions{};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::Vulkan::Instance::Instance(const char* gameName, const uint32_t gameVersion)
	: m_instance(nullptr)
{
	VkApplicationInfo appInfo = Initializers::ApplicationInfo(gameName, gameVersion);
	VkInstanceCreateInfo instanceInfo = Initializers::InstanceCreateInfo(appInfo);

	VkCall(vkCreateInstance(&instanceInfo, nullptr, &m_instance));

	//Get All Available Layers
	uint32_t layersCount = 0;
	VkCall(vkEnumerateInstanceLayerProperties(&layersCount, nullptr));
	s_availableLayers.resize(layersCount);
	VkCall(vkEnumerateInstanceLayerProperties(&layersCount, s_availableLayers.data()));

	//Get All Available Extensions
	uint32_t extensionsCount = 0;
	VkCall(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr));
	s_availableExtensions.resize(extensionsCount);
	VkCall(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, s_availableExtensions.data()));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::Vulkan::Instance::Instance(const char* gameName, const uint32_t gameVersion, std::vector<std::string>& instanceLayers, std::vector<std::string>& instanceExtensions)
	: m_instance(nullptr), m_layers(instanceLayers), m_extensions(instanceExtensions)
{	
	VkApplicationInfo appInfo = Initializers::ApplicationInfo(gameName, gameVersion);

	std::vector<const char*> layers(instanceLayers.size());
	std::vector<const char*> extensions(instanceExtensions.size());
	std::transform(instanceLayers.begin(), instanceLayers.end(), layers.begin(), [](const std::string& s) {return s.c_str(); });
	std::transform(instanceExtensions.begin(), instanceExtensions.end(), extensions.begin(), [](const std::string& s) {return s.c_str(); });
	
	VkInstanceCreateInfo instanceInfo = Initializers::InstanceCreateInfo(appInfo, layers, extensions);

	VkCall(vkCreateInstance(&instanceInfo, nullptr, &m_instance));

	//Get All Available Layers
	uint32_t layersCount = 0;
	VkCall(vkEnumerateInstanceLayerProperties(&layersCount, nullptr));
	s_availableLayers.resize(layersCount);
	VkCall(vkEnumerateInstanceLayerProperties(&layersCount, s_availableLayers.data()));

	//Get All Available Extensions
	uint32_t extensionsCount = 0;
	VkCall(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr));
	s_availableExtensions.resize(extensionsCount);
	VkCall(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, s_availableExtensions.data()));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::Vulkan::Instance::~Instance()
{
	if(m_instance)
	{
		vkDestroyInstance(m_instance, nullptr);
		m_instance = nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VkInstance& TRAP::Graphics::API::Vulkan::Instance::GetInstance()
{
	return m_instance;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<std::string>& TRAP::Graphics::API::Vulkan::Instance::GetUsedLayers() const
{
	return m_layers;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<std::string>& TRAP::Graphics::API::Vulkan::Instance::GetUsedExtensions() const
{
	return m_extensions;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<VkLayerProperties>& TRAP::Graphics::API::Vulkan::Instance::GetAvailableLayers()
{
	return s_availableLayers;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<VkExtensionProperties>& TRAP::Graphics::API::Vulkan::Instance::GetAvailableExtensions()
{
	return s_availableExtensions;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::Vulkan::Instance::IsLayerSupported(const char* layer)
{
	if(s_availableLayers.empty())
	{
		uint32_t layersCount = 0;
		VkCall(vkEnumerateInstanceLayerProperties(&layersCount, nullptr));
		s_availableLayers.resize(layersCount);
		VkCall(vkEnumerateInstanceLayerProperties(&layersCount, s_availableLayers.data()));
	}
	
	const auto layerResult = std::find_if(s_availableLayers.begin(), s_availableLayers.end(),
	                                      [layer](const VkLayerProperties& props) { return strcmp(props.layerName, layer) == 0; });
	if (layerResult == s_availableLayers.end())
	{
		if (strcmp(layer, "VK_LAYER_KHRONOS_validation") == 0)
			TP_WARN("[Renderer][Vulkan][Instance] Layer ", layer, " is not supported(Vulkan SDK installed?)");
		else
			TP_WARN("[Renderer][Vulkan][Instance] Layer ", layer, " is not supported");
		
		return false;
	}
	
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::Vulkan::Instance::IsExtensionSupported(const char* extension)
{
	if(s_availableExtensions.empty())
	{
		uint32_t extensionsCount = 0;
		VkCall(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr));
		s_availableExtensions.resize(extensionsCount);
		VkCall(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, s_availableExtensions.data()));
	}
	
	const auto extensionResult = std::find_if(s_availableExtensions.begin(), s_availableExtensions.end(),
		[extension](const VkExtensionProperties& props) { return strcmp(props.extensionName, extension) == 0; });
	if (extensionResult == s_availableExtensions.end())
	{
		if (strcmp(extension, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0)
			TP_WARN("[Renderer][Vulkan] Extension ", extension, " is not supported(Vulkan SDK installed?)");
		else
			TP_WARN("[Renderer][Vulkan] Extension ", extension, " is not supported");
		
		return false;
	}

	return true;
}