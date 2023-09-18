#include "TRAPPCH.h"
#include "VulkanInstance.h"

#include "Application.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Utils/Dialogs/Dialogs.h"
#include "Utils/ErrorCodes/ErrorCodes.h"

TRAP::Graphics::API::VulkanInstance::VulkanInstance(const std::string_view appName,
													std::vector<std::string> instanceLayers,
                                                    std::vector<std::string> instanceExtensions)
	: m_instanceLayers(std::move(instanceLayers)),
	  m_instanceExtensions(std::move(instanceExtensions))
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	std::erase_if(m_instanceLayers, [](const std::string_view layer){return !IsLayerSupported(layer);});
	std::erase_if(m_instanceExtensions, [](const std::string_view extension){return !IsExtensionSupported(extension);});

	std::vector<const char*> layers(m_instanceLayers.size());
	for (std::size_t i = 0; i < m_instanceLayers.size(); i++)
		layers[i] = m_instanceLayers[i].c_str();

	std::vector<const char*> extensions(m_instanceExtensions.size());
	for (std::size_t i = 0; i < m_instanceExtensions.size(); i++)
		extensions[i] = m_instanceExtensions[i].c_str();

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanInstancePrefix, "Creating Instance");

	if (!m_instanceLayers.empty())
	{
		TP_DEBUG(Log::RendererVulkanInstancePrefix, "Loading Instance Layer(s):");
		for (const std::string_view str : m_instanceLayers)
		{
			const auto layerProps = GetInstanceLayerProperties(str);
			if(layerProps)
			{
				TP_DEBUG(Log::RendererVulkanInstancePrefix, "    ", str, " (", layerProps->description, ") Spec: ",
				         VK_API_VERSION_MAJOR(layerProps->specVersion), '.', VK_API_VERSION_MINOR(layerProps->specVersion), '.',
						 VK_API_VERSION_PATCH(layerProps->specVersion), '.', VK_API_VERSION_VARIANT(layerProps->specVersion),
						 " Rev: ", layerProps->implementationVersion);
			}
			else
				TP_DEBUG(Log::RendererVulkanInstancePrefix, "    ", str);
		}
	}
	if (!m_instanceExtensions.empty())
	{
		TP_DEBUG(Log::RendererVulkanInstancePrefix, "Loading Instance Extension(s):");
		for (const std::string_view str : m_instanceExtensions)
		{
			const auto extensionProps = GetInstanceExtensionProperties(str);
			if(extensionProps)
			{
				TP_DEBUG(Log::RendererVulkanInstancePrefix, "    ", str, " Spec: ",
				         VK_API_VERSION_MAJOR(extensionProps->specVersion), '.', VK_API_VERSION_MINOR(extensionProps->specVersion), '.',
						 VK_API_VERSION_PATCH(extensionProps->specVersion), '.', VK_API_VERSION_VARIANT(extensionProps->specVersion));
			}
			else
				TP_DEBUG(Log::RendererVulkanInstancePrefix, "    ", str);
		}
	}
#endif /*VERBOSE_GRAPHICS_DEBUG*/

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
#endif /*ENABLE_GRAPHICS_DEBUG && ENABLE_GPU_BASED_VALIDATION*/

	VkCall(vkCreateInstance(&info, nullptr, &m_instance));
	TRAP_ASSERT(m_instance, "VulkanInstance(): Vulkan Instance is nullptr!");

	if (m_instance != nullptr)
		VkLoadInstance(m_instance);
	else
		Utils::DisplayError(Utils::ErrorCode::VulkanInstanceCreationFailed);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanInstance::~VulkanInstance()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanInstancePrefix, "Destroying Instance");
#endif /*VERBOSE_GRAPHICS_DEBUG*/
	vkDestroyInstance(m_instance, nullptr);
	m_instance = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::API::VulkanInstance::GetInstanceVersion()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if(s_instanceVersion == 0u)
		s_instanceVersion = VkGetInstanceVersion();

	return s_instanceVersion;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const std::vector<VkLayerProperties>& TRAP::Graphics::API::VulkanInstance::GetAvailableInstanceLayers()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if (s_availableInstanceLayers.empty())
		LoadAllInstanceLayers();

	return s_availableInstanceLayers;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const std::vector<VkExtensionProperties>& TRAP::Graphics::API::VulkanInstance::GetAvailableInstanceExtensions()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if (s_availableInstanceExtensions.empty())
		LoadAllInstanceExtensions();

	return s_availableInstanceExtensions;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::optional<VkLayerProperties> TRAP::Graphics::API::VulkanInstance::GetInstanceLayerProperties(const std::string_view instanceLayer)
{
	const std::vector<VkLayerProperties>& instanceLayers = GetAvailableInstanceLayers();
	auto res = std::ranges::find_if(instanceLayers, [instanceLayer](const VkLayerProperties& layerProps)
	                                                {return instanceLayer == layerProps.layerName;});

	if(res == std::ranges::end(instanceLayers))
		return std::nullopt;

	return *res;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::optional<VkExtensionProperties> TRAP::Graphics::API::VulkanInstance::GetInstanceExtensionProperties(const std::string_view instanceExtension)
{
	const std::vector<VkExtensionProperties>& instanceExtensions = GetAvailableInstanceExtensions();
	auto res = std::ranges::find_if(instanceExtensions, [instanceExtension](const VkExtensionProperties& extensionProps)
	                                                {return instanceExtension == extensionProps.extensionName;});

	if(res == std::ranges::end(instanceExtensions))
		return std::nullopt;

	return *res;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::API::VulkanInstance::IsLayerSupported(const std::string_view layer)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	if (s_availableInstanceLayers.empty())
		LoadAllInstanceLayers();

	const auto result = std::ranges::find_if(s_availableInstanceLayers,
	                                         [layer](const VkLayerProperties& prop)
		                                     { return layer == prop.layerName; });

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

[[nodiscard]] bool TRAP::Graphics::API::VulkanInstance::IsExtensionSupported(const std::string_view extension)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	if (s_availableInstanceExtensions.empty())
		LoadAllInstanceExtensions();

	const auto result = std::ranges::find_if(s_availableInstanceExtensions,
	                                         [extension](const VkExtensionProperties& prop)
								             { return extension == prop.extensionName; });

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