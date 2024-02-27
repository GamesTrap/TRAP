#include "TRAPPCH.h"
#include "VulkanInstance.h"

#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Utils/ErrorCodes/ErrorCodes.h"

namespace
{
	[[nodiscard]] const std::vector<VkLayerProperties>& GetAllInstanceLayers()
	{
		static std::vector<VkLayerProperties> instanceLayers{};

		if(instanceLayers.empty())
		{
			u32 layersCount = 0;
			VkCall(vkEnumerateInstanceLayerProperties(&layersCount, nullptr));
			instanceLayers.resize(layersCount);
			VkCall(vkEnumerateInstanceLayerProperties(&layersCount, instanceLayers.data()));
		}

		return instanceLayers;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] const std::vector<VkExtensionProperties>& GetAllInstanceExtensions()
	{
		static std::vector<VkExtensionProperties> instanceExtensions{};

		if(instanceExtensions.empty())
		{
			u32 extensionsCount = 0;
			VkCall(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr));
			instanceExtensions.resize(extensionsCount);
			VkCall(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, instanceExtensions.data()));
		}


		return instanceExtensions;
	}

	//-------------------------------------------------------------------------------------------------------------------//

#ifdef VERBOSE_GRAPHICS_DEBUG
	void DebugPrintInstanceLayers(const std::span<const std::string> layers)
	{
		if(layers.empty())
			return;

		TP_DEBUG(TRAP::Log::RendererVulkanInstancePrefix, "Loading Instance Layer(s):");
		for (const std::string_view str : layers)
		{
			const auto layerProps = TRAP::Graphics::API::VulkanInstance::GetInstanceLayerProperties(str);
			if(layerProps)
			{
				TP_DEBUG(TRAP::Log::RendererVulkanInstancePrefix, "    ", str, " (", layerProps->description, ") Spec: ",
						VK_API_VERSION_MAJOR(layerProps->specVersion), '.', VK_API_VERSION_MINOR(layerProps->specVersion), '.',
						VK_API_VERSION_PATCH(layerProps->specVersion), '.', VK_API_VERSION_VARIANT(layerProps->specVersion),
						" Rev: ", layerProps->implementationVersion);
			}
			else
				TP_DEBUG(TRAP::Log::RendererVulkanInstancePrefix, "    ", str);
		}
	}
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	//-------------------------------------------------------------------------------------------------------------------//

#ifdef VERBOSE_GRAPHICS_DEBUG
	void DebugPrintInstanceExtensions(const std::span<const std::string> extensions)
	{
		if(extensions.empty())
			return;

		TP_DEBUG(TRAP::Log::RendererVulkanInstancePrefix, "Loading Instance Extension(s):");
		for (const std::string_view str : extensions)
		{
			const auto extensionProps = TRAP::Graphics::API::VulkanInstance::GetInstanceExtensionProperties(str);
			if(extensionProps)
			{
				TP_DEBUG(TRAP::Log::RendererVulkanInstancePrefix, "    ", str, " Spec: ",
						VK_API_VERSION_MAJOR(extensionProps->specVersion), '.', VK_API_VERSION_MINOR(extensionProps->specVersion), '.',
						VK_API_VERSION_PATCH(extensionProps->specVersion), '.', VK_API_VERSION_VARIANT(extensionProps->specVersion));
			}
			else
				TP_DEBUG(TRAP::Log::RendererVulkanInstancePrefix, "    ", str);
		}
	}
#endif /*VERBOSE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanInstance::VulkanInstance(const std::string_view appName,
													std::vector<std::string> instanceLayers,
                                                    std::vector<std::string> instanceExtensions)
	: m_instanceLayers(std::move(instanceLayers)),
	  m_instanceExtensions(std::move(instanceExtensions))
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	std::erase_if(m_instanceLayers, std::not_fn(IsLayerSupported));
	std::erase_if(m_instanceExtensions, std::not_fn(IsExtensionSupported));

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanInstancePrefix, "Creating Instance");

	DebugPrintInstanceLayers(m_instanceLayers);
	DebugPrintInstanceExtensions(m_instanceExtensions);
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	std::vector<const char*> layers(m_instanceLayers.size());
	std::ranges::transform(m_instanceLayers, layers.begin(), std::mem_fn(&std::string::c_str));

	std::vector<const char*> extensions(m_instanceExtensions.size());
	std::ranges::transform(m_instanceExtensions, extensions.begin(), std::mem_fn(&std::string::c_str));

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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanInstancePrefix, "Destroying Instance");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	vkDestroyInstance(m_instance, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::optional<u32> TRAP::Graphics::API::VulkanInstance::GetInstanceVersion()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	return VkGetInstanceVersion();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::optional<VkLayerProperties> TRAP::Graphics::API::VulkanInstance::GetInstanceLayerProperties(const std::string_view instanceLayer)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const std::vector<VkLayerProperties>& instanceLayers = GetAllInstanceLayers();
	auto res = std::ranges::find_if(instanceLayers, [instanceLayer](const VkLayerProperties& layerProps)
	                                                {return instanceLayer == layerProps.layerName;});

	if(res == instanceLayers.end())
		return std::nullopt;

	return *res;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::optional<VkExtensionProperties> TRAP::Graphics::API::VulkanInstance::GetInstanceExtensionProperties(const std::string_view instanceExtension)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const std::vector<VkExtensionProperties>& instanceExtensions = GetAllInstanceExtensions();
	auto res = std::ranges::find_if(instanceExtensions, [instanceExtension](const VkExtensionProperties& extensionProps)
	                                                {return instanceExtension == extensionProps.extensionName;});

	if(res == instanceExtensions.end())
		return std::nullopt;

	return *res;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::API::VulkanInstance::IsLayerSupported(const std::string_view layer)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const std::vector<VkLayerProperties>& instanceLayers = GetAllInstanceLayers();
	const auto result = std::ranges::find_if(instanceLayers,
	                                         [layer](const VkLayerProperties& prop)
		                                     { return layer == prop.layerName; });

	if (result == instanceLayers.end())
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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const std::vector<VkExtensionProperties>& instanceExtensions = GetAllInstanceExtensions();
	const auto result = std::ranges::find_if(instanceExtensions,
	                                         [extension](const VkExtensionProperties& prop)
								             { return extension == prop.extensionName; });

	if (result == instanceExtensions.end())
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
