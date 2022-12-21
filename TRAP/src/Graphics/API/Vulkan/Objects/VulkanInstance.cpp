#include "TRAPPCH.h"
#include "VulkanInstance.h"

#include "Application.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Utils/Dialogs/Dialogs.h"

uint32_t TRAP::Graphics::API::VulkanInstance::s_instanceVersion = 0;
std::vector<VkLayerProperties> TRAP::Graphics::API::VulkanInstance::s_availableInstanceLayers{};
std::vector<VkExtensionProperties> TRAP::Graphics::API::VulkanInstance::s_availableInstanceExtensions{};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanInstance::VulkanInstance(const std::string_view appName,
													std::vector<std::string> instanceLayers,
                                                    std::vector<std::string> instanceExtensions)
	: m_instance(VK_NULL_HANDLE),
	  m_instanceLayers(std::move(instanceLayers)),
	  m_instanceExtensions(std::move(instanceExtensions))
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	std::vector<const char*> layers(m_instanceLayers.size());
	for (uint32_t i = 0; i < m_instanceLayers.size(); i++)
	{
		if(IsLayerSupported(m_instanceLayers[i]))
			layers[i] = m_instanceLayers[i].c_str();
		else
			m_instanceLayers.erase(m_instanceLayers.begin() + i);
	}

	std::vector<const char*> extensions(m_instanceExtensions.size());
	for (uint32_t i = 0; i < m_instanceExtensions.size(); i++)
	{
		if (IsExtensionSupported(m_instanceExtensions[i]))
			extensions[i] = m_instanceExtensions[i].c_str();
		else
			m_instanceExtensions.erase(m_instanceExtensions.begin() + i);
	}

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanInstancePrefix, "Creating Instance");

	if (!m_instanceLayers.empty())
	{
		TP_DEBUG(Log::RendererVulkanInstancePrefix, "Loading Instance Layer(s):");
		for (const std::string_view str : m_instanceLayers)
			TP_DEBUG(Log::RendererVulkanInstancePrefix, "    ", str);
	}
	if (!m_instanceExtensions.empty())
	{
		TP_DEBUG(Log::RendererVulkanInstancePrefix, "Loading Instance Extension(s):");
		for (const std::string_view str : m_instanceExtensions)
			TP_DEBUG(Log::RendererVulkanInstancePrefix, "    ", str);
	}
#endif

	const VkApplicationInfo appInfo = VulkanInits::ApplicationInfo(appName);
	VkInstanceCreateInfo info = VulkanInits::InstanceCreateInfo(appInfo, layers, extensions);

#if defined(ENABLE_GRAPHICS_DEBUG) && defined(ENABLE_GPU_BASED_VALIDATION)
	if(VulkanRenderer::s_validationFeaturesExtension)
	{
		VkValidationFeaturesEXT validationFeatures{};
		validationFeatures.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
		VkValidationFeatureEnableEXT enabledValidationFeatures = VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT;
		validationFeatures.enabledValidationFeatureCount = 1;
		validationFeatures.pEnabledValidationFeatures = &enabledValidationFeatures;
		info.pNext = &validationFeatures;
	}
#endif

	VkCall(vkCreateInstance(&info, nullptr, &m_instance));
	TRAP_ASSERT(m_instance, "VulkanInstance(): Vulkan Instance is nullptr!");

	if (m_instance)
		VkLoadInstance(m_instance);
	else
	{
		Utils::Dialogs::ShowMsgBox("Vulkan API error", "Vulkan instance creation failed!\n"
								   "Error code: 0x0005", Utils::Dialogs::Style::Error,
								   Utils::Dialogs::Buttons::Quit);
		TP_CRITICAL(Log::RendererVulkanInstancePrefix, "Instance creation failed!");
		TRAP::Application::Shutdown();
		exit(-1);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanInstance::~VulkanInstance()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanInstancePrefix, "Destroying Instance");
#endif
	vkDestroyInstance(m_instance, nullptr);
	m_instance = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

VkInstance TRAP::Graphics::API::VulkanInstance::GetVkInstance() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_instance;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<std::string>& TRAP::Graphics::API::VulkanInstance::GetUsedInstanceLayers() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_instanceLayers;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<std::string>& TRAP::Graphics::API::VulkanInstance::GetUsedInstanceExtensions() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_instanceExtensions;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanInstance::GetInstanceVersion()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if(!s_instanceVersion)
		s_instanceVersion = VkGetInstanceVersion();

	return s_instanceVersion;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<VkLayerProperties>& TRAP::Graphics::API::VulkanInstance::GetAvailableInstanceLayers()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if (s_availableInstanceLayers.empty())
		LoadAllInstanceLayers();

	return s_availableInstanceLayers;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<VkExtensionProperties>& TRAP::Graphics::API::VulkanInstance::GetAvailableInstanceExtensions()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if (s_availableInstanceExtensions.empty())
		LoadAllInstanceExtensions();

	return s_availableInstanceExtensions;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::VulkanInstance::IsLayerSupported(const std::string_view layer)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	if (s_availableInstanceLayers.empty())
		LoadAllInstanceLayers();

	const auto result = std::find_if(s_availableInstanceLayers.begin(), s_availableInstanceLayers.end(),
		                             [layer](VkLayerProperties prop)
		{
			return std::strcmp(prop.layerName, layer.data()) == 0;
		});

	if (result == s_availableInstanceLayers.end())
	{
		if (layer == "VK_LAYER_KHRONOS_validation")
			TP_WARN(Log::RendererVulkanInstancePrefix, "Layer: \"", layer,
			        "\" is not supported(Vulkan SDK installed?)");
		else
			TP_WARN(Log::RendererVulkanInstancePrefix, "Layer: \"", layer, "\" is not supported");

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::VulkanInstance::IsExtensionSupported(const std::string_view extension)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	if (s_availableInstanceExtensions.empty())
		LoadAllInstanceExtensions();

	const auto result = std::find_if(s_availableInstanceExtensions.begin(),
	                              s_availableInstanceExtensions.end(),
	                              [extension](VkExtensionProperties prop)
								  { return std::strcmp(prop.extensionName, extension.data()) == 0; });
	if (result == s_availableInstanceExtensions.end())
	{
		if (extension == VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
			TP_WARN(Log::RendererVulkanInstancePrefix, "Extension: \"", extension,
			        "\" is not supported(Vulkan SDK installed?)");
		else
			TP_WARN(Log::RendererVulkanInstancePrefix, "Extension: \"", extension, "\" is not supported");

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanInstance::LoadAllInstanceLayers()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	uint32_t layersCount = 0;
	VkCall(vkEnumerateInstanceLayerProperties(&layersCount, nullptr));
	s_availableInstanceLayers.resize(layersCount);
	VkCall(vkEnumerateInstanceLayerProperties(&layersCount, s_availableInstanceLayers.data()));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanInstance::LoadAllInstanceExtensions()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	uint32_t extensionsCount = 0;
	VkCall(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr));
	s_availableInstanceExtensions.resize(extensionsCount);
	VkCall(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, s_availableInstanceExtensions.data()));
}