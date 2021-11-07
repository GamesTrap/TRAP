#include "TRAPPCH.h"
#include "VulkanPhysicalDevice.h"

#include "Application.h"
#include "VulkanInits.h"
#include "VulkanInstance.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Utils/Dialogs/Dialogs.h"

std::multimap<uint32_t, std::array<uint8_t, 16>> TRAP::Graphics::API::VulkanPhysicalDevice::s_availablePhysicalDeviceUUIDs{};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanPhysicalDevice::VulkanPhysicalDevice(const TRAP::Ref<VulkanInstance>& instance,
																const std::array<uint8_t, 16>& physicalDeviceUUID)
	: m_physicalDevice(VK_NULL_HANDLE),
	  m_physicalDeviceProperties(),
	  m_physicalDeviceSubgroupProperties(),
	  m_physicalDeviceIDProperties(),
	  m_physicalDeviceMemoryProperties(),
	  m_physicalDeviceFeatures(),
	  m_physicalDeviceFragmentShaderInterlockFeatures(),
	  m_physicalDeviceVulkan11Features(),
	  m_physicalDeviceVulkan12Features(),
	  m_physicalDeviceVulkan11Properties(),
	  m_physicalDeviceVulkan12Properties(),
	  m_deviceUUID()
{
	TRAP_ASSERT(instance, "instance is nullptr");

	m_physicalDevice = FindPhysicalDeviceViaUUID(instance, physicalDeviceUUID);

	if(!m_physicalDevice)
	{
		Utils::Dialogs::ShowMsgBox("Vulkan API error", "Vulkan PhysicalDevice creation failed!\n"
								   "Error code: 0x0006", Utils::Dialogs::Style::Error,
								   Utils::Dialogs::Buttons::Quit);
		TP_CRITICAL(Log::RendererVulkanPhysicalDevicePrefix, "PhysicalDevice creation failed!");
		TRAP::Application::Shutdown();
		exit(-1);
	}

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanPhysicalDevicePrefix, "Creating PhysicalDevice");
#endif

	vkGetPhysicalDeviceProperties(m_physicalDevice, &m_physicalDeviceProperties);
	vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_physicalDeviceMemoryProperties);
	vkGetPhysicalDeviceFeatures(m_physicalDevice, &m_physicalDeviceFeatures);

	VkPhysicalDeviceProperties2 props2;
	props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
	props2.pNext = &m_physicalDeviceIDProperties;
	m_physicalDeviceIDProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES;
	m_physicalDeviceIDProperties.pNext = nullptr;
	vkGetPhysicalDeviceProperties2(m_physicalDevice, &props2);

	VkPhysicalDeviceProperties2 props{};
	props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
	props.pNext = &m_physicalDeviceSubgroupProperties;
	m_physicalDeviceSubgroupProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES;
	m_physicalDeviceSubgroupProperties.pNext = nullptr;
	vkGetPhysicalDeviceProperties2(m_physicalDevice, &props);

	m_physicalDeviceVulkan11Properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
	m_physicalDeviceVulkan11Properties.pNext = nullptr;
	props.pNext = &m_physicalDeviceVulkan11Properties;
	vkGetPhysicalDeviceProperties2(m_physicalDevice, &props);

	m_physicalDeviceVulkan12Properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;
	m_physicalDeviceVulkan12Properties.pNext = nullptr;
	props.pNext = &m_physicalDeviceVulkan12Properties;
	vkGetPhysicalDeviceProperties2(m_physicalDevice, &props);

	uint32_t queueFamilyPropertyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyPropertyCount, nullptr);
	m_queueFamilyProperties.resize(queueFamilyPropertyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyPropertyCount,
	                                         m_queueFamilyProperties.data());

	//Copy UUID
	std::memcpy(m_deviceUUID.data(), m_physicalDeviceIDProperties.deviceUUID, m_deviceUUID.size());

	VkPhysicalDeviceFeatures2 features{};
	m_physicalDeviceVulkan11Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
	m_physicalDeviceVulkan11Features.pNext = nullptr;
	features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	features.pNext = &m_physicalDeviceVulkan11Features;
	vkGetPhysicalDeviceFeatures2(m_physicalDevice, &features);

	m_physicalDeviceVulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	m_physicalDeviceVulkan12Features.pNext = nullptr;
	features.pNext = &m_physicalDeviceVulkan12Features;
	vkGetPhysicalDeviceFeatures2(m_physicalDevice, &features);

	VulkanRenderer::s_shaderDrawParameters = m_physicalDeviceVulkan11Features.shaderDrawParameters;
	VulkanRenderer::s_subgroupBroadcastDynamicID = m_physicalDeviceVulkan12Features.subgroupBroadcastDynamicId;

	//Capabilities for VulkanRenderer
	for(uint32_t i = 0; i < static_cast<uint32_t>(TRAP::Graphics::API::ImageFormat::IMAGE_FORMAT_COUNT); ++i)
	{
		VkFormatProperties formatSupport;
		VkFormat fmt = ImageFormatToVkFormat(static_cast<TRAP::Graphics::API::ImageFormat>(i));
		if (fmt == VK_FORMAT_UNDEFINED)
			continue;

		vkGetPhysicalDeviceFormatProperties(m_physicalDevice, fmt, &formatSupport);
		VulkanRenderer::s_GPUCapBits.CanShaderReadFrom[i] = (formatSupport.optimalTilingFeatures &
		                                                     VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) != 0;
		VulkanRenderer::s_GPUCapBits.CanShaderWriteTo[i] = (formatSupport.optimalTilingFeatures &
		                                                    VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT) != 0;
		VulkanRenderer::s_GPUCapBits.CanRenderTargetWriteTo[i] = (formatSupport.optimalTilingFeatures &
			                                                      (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
																   VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)) != 0;
	}

	RendererAPI::GPUSettings.UniformBufferAlignment = m_physicalDeviceProperties.limits.minUniformBufferOffsetAlignment;
	RendererAPI::GPUSettings.MaxUniformBufferRange = m_physicalDeviceProperties.limits.maxUniformBufferRange;
	RendererAPI::GPUSettings.StorageBufferAlignment = m_physicalDeviceProperties.limits.minStorageBufferOffsetAlignment;
	RendererAPI::GPUSettings.MaxStorageBufferRange = m_physicalDeviceProperties.limits.maxStorageBufferRange;
	RendererAPI::GPUSettings.UploadBufferTextureAlignment = m_physicalDeviceProperties.limits.optimalBufferCopyOffsetAlignment;
	RendererAPI::GPUSettings.UploadBufferTextureRowAlignment = m_physicalDeviceProperties.limits.optimalBufferCopyRowPitchAlignment;
	RendererAPI::GPUSettings.MaxVertexInputBindings = m_physicalDeviceProperties.limits.maxVertexInputBindings;
	RendererAPI::GPUSettings.MaxVertexInputAttributes = m_physicalDeviceProperties.limits.maxVertexInputAttributes;
	RendererAPI::GPUSettings.MultiDrawIndirect = m_physicalDeviceProperties.limits.maxDrawIndirectCount > 1u;
	RendererAPI::GPUSettings.MaxAnisotropy = m_physicalDeviceProperties.limits.maxSamplerAnisotropy;
	RendererAPI::GPUSettings.MaxImageDimension2D = m_physicalDeviceProperties.limits.maxImageDimension2D;
	RendererAPI::GPUSettings.MaxImageDimensionCube = m_physicalDeviceProperties.limits.maxImageDimensionCube;
	RendererAPI::GPUSettings.FillModeNonSolid = m_physicalDeviceFeatures.fillModeNonSolid;
	RendererAPI::GPUSettings.MaxPushConstantSize = m_physicalDeviceProperties.limits.maxPushConstantsSize;
	RendererAPI::GPUSettings.MaxSamplerAllocationCount = m_physicalDeviceProperties.limits.maxSamplerAllocationCount;
	RendererAPI::GPUSettings.MaxTessellationControlPoints = m_physicalDeviceProperties.limits.maxTessellationPatchSize;

	//maxBoundDescriptorSets not needed because engine is always limited to 4 descriptor sets

	RendererAPI::GPUSettings.WaveLaneCount = m_physicalDeviceSubgroupProperties.subgroupSize;
	RendererAPI::GPUSettings.WaveOpsSupportFlags = RendererAPI::WaveOpsSupportFlags::None;
	if (m_physicalDeviceSubgroupProperties.supportedOperations & VK_SUBGROUP_FEATURE_BASIC_BIT)
		RendererAPI::GPUSettings.WaveOpsSupportFlags |= RendererAPI::WaveOpsSupportFlags::Basic;
	if (m_physicalDeviceSubgroupProperties.supportedOperations & VK_SUBGROUP_FEATURE_VOTE_BIT)
		RendererAPI::GPUSettings.WaveOpsSupportFlags |= RendererAPI::WaveOpsSupportFlags::Vote;
	if (m_physicalDeviceSubgroupProperties.supportedOperations & VK_SUBGROUP_FEATURE_ARITHMETIC_BIT)
		RendererAPI::GPUSettings.WaveOpsSupportFlags |= RendererAPI::WaveOpsSupportFlags::Arithmetic;
	if (m_physicalDeviceSubgroupProperties.supportedOperations & VK_SUBGROUP_FEATURE_BALLOT_BIT)
		RendererAPI::GPUSettings.WaveOpsSupportFlags |= RendererAPI::WaveOpsSupportFlags::Ballot;
	if (m_physicalDeviceSubgroupProperties.supportedOperations & VK_SUBGROUP_FEATURE_SHUFFLE_BIT)
		RendererAPI::GPUSettings.WaveOpsSupportFlags |= RendererAPI::WaveOpsSupportFlags::Shuffle;
	if (m_physicalDeviceSubgroupProperties.supportedOperations & VK_SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT)
		RendererAPI::GPUSettings.WaveOpsSupportFlags |= RendererAPI::WaveOpsSupportFlags::ShuffleRelative;
	if (m_physicalDeviceSubgroupProperties.supportedOperations & VK_SUBGROUP_FEATURE_CLUSTERED_BIT)
		RendererAPI::GPUSettings.WaveOpsSupportFlags |= RendererAPI::WaveOpsSupportFlags::Clustered;
	if (m_physicalDeviceSubgroupProperties.supportedOperations & VK_SUBGROUP_FEATURE_QUAD_BIT)
		RendererAPI::GPUSettings.WaveOpsSupportFlags |= RendererAPI::WaveOpsSupportFlags::Quad;
	if (m_physicalDeviceSubgroupProperties.supportedOperations & VK_SUBGROUP_FEATURE_PARTITIONED_BIT_NV)
		RendererAPI::GPUSettings.WaveOpsSupportFlags |= RendererAPI::WaveOpsSupportFlags::PartitionedNV;

	RendererAPI::GPUSettings.TessellationSupported = m_physicalDeviceFeatures.tessellationShader;
	RendererAPI::GPUSettings.GeometryShaderSupported = m_physicalDeviceFeatures.geometryShader;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanPhysicalDevice::~VulkanPhysicalDevice()
{
	TRAP_ASSERT(m_physicalDevice);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanPhysicalDevicePrefix, "Destroying PhysicalDevice");
#endif
	m_physicalDevice = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

VkPhysicalDevice TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDevice() const
{
	return m_physicalDevice;
}

//-------------------------------------------------------------------------------------------------------------------//

const VkPhysicalDeviceProperties& TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceProperties() const
{
	return m_physicalDeviceProperties;
}

//-------------------------------------------------------------------------------------------------------------------//

const VkPhysicalDeviceSubgroupProperties& TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceSubgroupProperties() const
{
	return m_physicalDeviceSubgroupProperties;
}

//-------------------------------------------------------------------------------------------------------------------//

const VkPhysicalDeviceIDProperties& TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceIDProperties() const
{
	return m_physicalDeviceIDProperties;
}

//-------------------------------------------------------------------------------------------------------------------//

const VkPhysicalDeviceMemoryProperties& TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceMemoryProperties() const
{
	return m_physicalDeviceMemoryProperties;
}

//-------------------------------------------------------------------------------------------------------------------//

const VkPhysicalDeviceFeatures& TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceFeatures() const
{
	return m_physicalDeviceFeatures;
}

//-------------------------------------------------------------------------------------------------------------------//

const VkPhysicalDeviceVulkan11Features& TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceVulkan11Features() const
{
	return m_physicalDeviceVulkan11Features;
}

//-------------------------------------------------------------------------------------------------------------------//

const VkPhysicalDeviceVulkan12Features& TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceVulkan12Features() const
{
	return m_physicalDeviceVulkan12Features;
}

//-------------------------------------------------------------------------------------------------------------------//

const VkPhysicalDeviceVulkan11Properties& TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceVulkan11Properties() const
{
	return m_physicalDeviceVulkan11Properties;
}

//-------------------------------------------------------------------------------------------------------------------//

const VkPhysicalDeviceVulkan12Properties& TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceVulkan12Properties() const
{
	return m_physicalDeviceVulkan12Properties;
}

//-------------------------------------------------------------------------------------------------------------------//

const VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT& TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceFragmentShaderInterlockFeatures() const
{
	return m_physicalDeviceFragmentShaderInterlockFeatures;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<VkQueueFamilyProperties>& TRAP::Graphics::API::VulkanPhysicalDevice::GetQueueFamilyProperties() const
{
	return m_queueFamilyProperties;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::VulkanPhysicalDevice::IsExtensionSupported(const std::string& extension)
{
	if(m_availablePhysicalDeviceExtensions.empty())
		LoadAllPhysicalDeviceExtensions();

	const auto result = std::find_if(m_availablePhysicalDeviceExtensions.begin(),
		                             m_availablePhysicalDeviceExtensions.end(),
		                             [extension](const VkExtensionProperties& props)
		{
			return std::strcmp(extension.c_str(), props.extensionName) == 0;
		});

	if(result == m_availablePhysicalDeviceExtensions.end())
	{
		if (extension == VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
			TP_WARN(Log::RendererVulkanPhysicalDevicePrefix, "Extension: \"", extension,
			        "\" is not supported(Vulkan SDK installed?)");
		else
			TP_WARN(Log::RendererVulkanPhysicalDevicePrefix, "Extension: \"", extension, "\" is not supported");

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanPhysicalDevice::GetMemoryType(uint32_t typeBits,
                                                                  const VkMemoryPropertyFlags properties) const
{
	for(uint32_t i = 0; i < m_physicalDeviceMemoryProperties.memoryTypeCount; i++)
	{
		if((typeBits & 1) == 1)
		{
			if((m_physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;
		}
		typeBits >>= 1;
	}

	TP_ERROR(Log::RendererVulkanPhysicalDevicePrefix, "Could not find a matching memory type!");
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<VkExtensionProperties>& TRAP::Graphics::API::VulkanPhysicalDevice::GetAvailablePhysicalDeviceExtensions()
{
	if (m_availablePhysicalDeviceExtensions.empty())
		LoadAllPhysicalDeviceExtensions();

	return m_availablePhysicalDeviceExtensions;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::array<uint8_t, 16>& TRAP::Graphics::API::VulkanPhysicalDevice::GetPhysicalDeviceUUID() const
{
	return m_deviceUUID;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanPhysicalDevice::RetrievePhysicalDeviceFragmentShaderInterlockFeatures()
{
	VkPhysicalDeviceFeatures2 features2;
	features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	m_physicalDeviceFragmentShaderInterlockFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT;
	m_physicalDeviceFragmentShaderInterlockFeatures.pNext = nullptr;
	features2.pNext = &m_physicalDeviceFragmentShaderInterlockFeatures;
	vkGetPhysicalDeviceFeatures2(m_physicalDevice, &features2);

	RendererAPI::GPUSettings.ROVsSupported = static_cast<bool>(m_physicalDeviceFragmentShaderInterlockFeatures.fragmentShaderPixelInterlock);
}

//-------------------------------------------------------------------------------------------------------------------//

const std::multimap<uint32_t, std::array<uint8_t, 16>>& TRAP::Graphics::API::VulkanPhysicalDevice::GetAllRatedPhysicalDevices(const TRAP::Ref<VulkanInstance>& instance)
{
	TRAP_ASSERT(instance, "instance is nullptr");

	if(!s_availablePhysicalDeviceUUIDs.empty())
		return s_availablePhysicalDeviceUUIDs;

	const std::vector<VkPhysicalDevice> physicalDevices = GetAllVkPhysicalDevices(instance->GetVkInstance());

	if (!physicalDevices.empty())
		RatePhysicalDevices(physicalDevices);
	else
	{
		Utils::Dialogs::ShowMsgBox("Vulkan API error", "No Vulkan capable PhysicalDevice was found!\n"
								   "Error code: 0x0007", Utils::Dialogs::Style::Error,
								   Utils::Dialogs::Buttons::Quit);
		TP_CRITICAL(Log::RendererVulkanPrefix, "No Vulkan capable physical device was found!!");
		TRAP::Application::Shutdown();
		exit(-1);
	}

	return s_availablePhysicalDeviceUUIDs;
}

//-------------------------------------------------------------------------------------------------------------------//

VkPhysicalDevice TRAP::Graphics::API::VulkanPhysicalDevice::FindPhysicalDeviceViaUUID(const TRAP::Ref<VulkanInstance>& instance,
	                                                                                  const std::array<uint8_t, 16>& physicalDeviceUUID)
{
	TRAP_ASSERT(instance, "instance is nullptr");

	const auto physicalDevices = GetAllVkPhysicalDevices(instance->GetVkInstance());

	for (const auto& device : physicalDevices)
	{
		std::array<uint8_t, 16> testUUID{};

		VkPhysicalDeviceIDPropertiesKHR physicalDeviceIDProperties;
		VkPhysicalDeviceProperties2 props2;
		props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		props2.pNext = &physicalDeviceIDProperties;
		physicalDeviceIDProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES;
		physicalDeviceIDProperties.pNext = nullptr;
		vkGetPhysicalDeviceProperties2(device, &props2);

		//Copy UUID
		std::memcpy(testUUID.data(), physicalDeviceIDProperties.deviceUUID, testUUID.size());

		bool same = true;
		for (uint32_t i = 0; i < physicalDeviceUUID.size(); i++)
		{
			if (physicalDeviceUUID[i] != testUUID[i])
			{
				same = false;
				break;
			}
		}

		if (same)
			return device;
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::multimap<uint32_t, std::array<uint8_t, 16>>& TRAP::Graphics::API::VulkanPhysicalDevice::GetAllRatedPhysicalDevices(const VkInstance& instance)
{
	if(!s_availablePhysicalDeviceUUIDs.empty())
		return s_availablePhysicalDeviceUUIDs;

	const std::vector<VkPhysicalDevice> physicalDevices = GetAllVkPhysicalDevices(instance);

	if (!physicalDevices.empty())
		RatePhysicalDevices(physicalDevices);
	else
	{
		Utils::Dialogs::ShowMsgBox("Vulkan API error", "No Vulkan capable PhysicalDevice was found!\n"
								   "Error code: 0x0007", Utils::Dialogs::Style::Error,
								   Utils::Dialogs::Buttons::Quit);
		TP_CRITICAL(Log::RendererVulkanPrefix, "No Vulkan capable physical device was found!");
		TRAP::Application::Shutdown();
		exit(-1);
	}

	return s_availablePhysicalDeviceUUIDs;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<VkPhysicalDevice> TRAP::Graphics::API::VulkanPhysicalDevice::GetAllVkPhysicalDevices(const VkInstance& instance)
{
	uint32_t physicalDeviceCount = 0;
	VkCall(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr));

	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	VkCall(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data()));

	return physicalDevices;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanPhysicalDevice::RatePhysicalDevices(const std::vector<VkPhysicalDevice>& physicalDevices)
{
	//Score each Physical Device and insert into multimap
	uint32_t score = 0;
	for (VkPhysicalDevice dev : physicalDevices)
	{
		//Get PhysicalDeviceProperties
		VkPhysicalDeviceProperties devProps;
		vkGetPhysicalDeviceProperties(dev, &devProps);

		//Required: Vulkan 1.2 is minimum
		if(devProps.apiVersion < VK_API_VERSION_1_2)
		{
			TP_ERROR(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
			         "\" Failed Vulkan Version Test!");
			continue;
		}

		//Required: Discrete GPUs have a significant performance advantage
		if (devProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			score += 5000;
		else if (devProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
			score += 2500;
		else
		{
			TP_ERROR(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
			         "\" Failed Dedicated/Internal GPU Test!");
			continue;
		}

		//Required: Minimum 4 simultaneously bound descriptor sets support
		if(devProps.limits.maxBoundDescriptorSets < 4)
		{
			TP_ERROR(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
			         "\" Failed Max bound descriptor set Test!");
			continue;
		}

		//Get all PhysicalDevice Extensions
		uint32_t extensionsCount = 0;
		std::vector<VkExtensionProperties> extensions;
		VkCall(vkEnumerateDeviceExtensionProperties(dev, nullptr, &extensionsCount, nullptr));
		extensions.resize(extensionsCount);
		VkCall(vkEnumerateDeviceExtensionProperties(dev, nullptr, &extensionsCount, extensions.data()));
		score += extensionsCount * 50;

		//Required: Check if PhysicalDevice supports swapchains
		const auto result = std::find_if(extensions.begin(), extensions.end(), [](const VkExtensionProperties& props)
			{
				return std::strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, props.extensionName) == 0;
			});

		if (result == extensions.end())
		{
			TP_ERROR(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
			         "\" Failed Required PhysicalDevice Extensions Test!");
			continue;
		}

		//Create Instance

		//Init WindowingAPI needed here for instance extensions
		if(!INTERNAL::WindowingAPI::Init())
		{
			TP_ERROR(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
			         "\" Failed to initialize WindowingAPI!");
			TRAP::Application::Shutdown();
		}

		std::vector<std::string> instanceExtensions{};
		const auto reqExt = INTERNAL::WindowingAPI::GetRequiredInstanceExtensions();
		instanceExtensions.push_back(reqExt[0]);
		instanceExtensions.push_back(reqExt[1]);
		VkInstance instance;
		std::vector<const char*> instExtensions(instanceExtensions.size());
		for (uint32_t i = 0; i < instExtensions.size(); i++)
			instExtensions[i] = instanceExtensions[i].c_str();
		const VkApplicationInfo appInfo = API::VulkanInits::ApplicationInfo("Vulkan Surface Tester");
		VkInstanceCreateInfo instanceCreateInfo = API::VulkanInits::InstanceCreateInfo(appInfo, {}, instExtensions);
		VkCall(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));
		if(!instance)
		{
			TP_ERROR(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
			         "\" Failed Instance Creation Test!");
			continue;
		}

		//Create Vulkan Surface Test Window
		INTERNAL::WindowingAPI::WindowHint(INTERNAL::WindowingAPI::Hint::Visible, false);
		INTERNAL::WindowingAPI::WindowHint(INTERNAL::WindowingAPI::Hint::Focused, false);
		Scope<INTERNAL::WindowingAPI::InternalWindow> vulkanTestWindow = INTERNAL::WindowingAPI::CreateWindow(400,
			                                                                                                  400,
			                                                                                                  "TRAP Vulkan Surface Tester",
			                                                                                                  nullptr);
		INTERNAL::WindowingAPI::DefaultWindowHints();
		if (!vulkanTestWindow)
		{
			vkDestroyInstance(instance, nullptr);
			TP_ERROR(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
			         "\" Failed Vulkan Surface Test Window creation!");
			continue;
		}

		//Required: Check if Surface can be created
		VkSurfaceKHR surface;
		VkResult res;
		VkCall(res = TRAP::INTERNAL::WindowingAPI::CreateWindowSurface(instance, vulkanTestWindow.get(), nullptr,
		                                                               surface));
		if(!surface || res != VK_SUCCESS)
		{
			TRAP::INTERNAL::WindowingAPI::DestroyWindow(std::move(vulkanTestWindow));
			vkDestroyInstance(instance, nullptr);
			TP_ERROR(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" Failed Surface creation!");
			continue;
		}

		//Get Queue Families
		uint32_t queueFamilyPropertyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueFamilyPropertyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueFamilyPropertyCount, queueFamilyProperties.data());
		if(queueFamilyProperties.empty())
		{
			vkDestroySurfaceKHR(instance, surface, nullptr);
			TRAP::INTERNAL::WindowingAPI::DestroyWindow(std::move(vulkanTestWindow));
			vkDestroyInstance(instance, nullptr);
			TP_ERROR(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
			         "\" Failed Querying Queue Family Properties!");
			continue;
		}

		//Required: Check if PhysicalDevice supports Graphics queue
		bool foundGraphicsQueue = false;
		for(const VkQueueFamilyProperties& props : queueFamilyProperties)
		{
			if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				foundGraphicsQueue = true;
				break;
			}
		}
		if(!foundGraphicsQueue)
		{
			vkDestroySurfaceKHR(instance, surface, nullptr);
			TRAP::INTERNAL::WindowingAPI::DestroyWindow(std::move(vulkanTestWindow));
			vkDestroyInstance(instance, nullptr);
			TP_ERROR(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" Failed Graphics Queue Test!");
			continue;
		}

		//Required: Check if PhysicalDevice supports Presenting
		VkBool32 foundPresentSupport = false;
		for(std::size_t i = 0; i < queueFamilyProperties.size(); i++)
		{
			VkCall(vkGetPhysicalDeviceSurfaceSupportKHR(dev, static_cast<uint32_t>(i), surface, &foundPresentSupport));
			if (foundPresentSupport)
				break;
		}
		if (!foundPresentSupport)
		{
			vkDestroySurfaceKHR(instance, surface, nullptr);
			TRAP::INTERNAL::WindowingAPI::DestroyWindow(std::move(vulkanTestWindow));
			vkDestroyInstance(instance, nullptr);
			TP_ERROR(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
			         "\" Failed Present Queue Test!");
			continue;
		}

		//Required: Check if Surface contains present modes
		uint32_t surfacePresentModeCount = 0;
		VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surface, &surfacePresentModeCount, nullptr));
		std::vector<VkPresentModeKHR> presentModes(surfacePresentModeCount);
		VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surface, &surfacePresentModeCount,
		                                                 presentModes.data()));
		if(presentModes.empty())
		{
			vkDestroySurfaceKHR(instance, surface, nullptr);
			TRAP::INTERNAL::WindowingAPI::DestroyWindow(std::move(vulkanTestWindow));
			vkDestroyInstance(instance, nullptr);
			TP_ERROR(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" Failed Present Mode Test!");
			continue;
		}

		//Required: Check if Surface contains formats
		uint32_t surfaceFormatCount = 0;
		VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surface, &surfaceFormatCount, nullptr));
		std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
		VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surface, &surfaceFormatCount, surfaceFormats.data()));
		if (surfaceFormats.empty())
		{
			vkDestroySurfaceKHR(instance, surface, nullptr);
			TRAP::INTERNAL::WindowingAPI::DestroyWindow(std::move(vulkanTestWindow));
			vkDestroyInstance(instance, nullptr);
			TP_ERROR(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
			         "\" Failed Surface Format Test!");
			continue;
		}

		//Big Optionally: Check if PhysicalDevice supports Compute queue
		bool foundComputeQueue = false;
		for (const VkQueueFamilyProperties& props : queueFamilyProperties)
		{
			if (props.queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				foundComputeQueue = true;
				score += 1000;
				break;
			}
		}
		if (!foundComputeQueue)
			TP_WARN(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" Failed Compute Queue Test!");

		//Big Optionally: Check if PhysicalDevice supports Transfer queue
		bool foundTransferQueue = false;
		for (const VkQueueFamilyProperties& props : queueFamilyProperties)
		{
			if (props.queueFlags & VK_QUEUE_TRANSFER_BIT)
			{
				foundTransferQueue = true;
				score += 1000;
				break;
			}
		}
		if (!foundTransferQueue)
			TP_WARN(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" Failed Transfer Queue Test!");

		//Big Optionally: Check if Raytracing extensions are supported
		bool raytracing = true;
		const std::vector<std::string> raytracingExt =
		{
			VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
			VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
			VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
			VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
			VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
			VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
			VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME
		};
		for (const std::string& str : raytracingExt)
		{
			const auto extRes = std::find_if(extensions.begin(), extensions.end(),
			                                 [str](const VkExtensionProperties& props)
				{
					return std::strcmp(str.c_str(), props.extensionName) == 0;
				});

			if (extRes == extensions.end())
			{
				raytracing = false;
				break;
			}
		}

		if (raytracing)
			score += 2000;
		else
			TP_WARN(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" Failed Raytracing Test!");

		//Get PhysicalDevice Features
		VkPhysicalDeviceFeatures devFeatures;
		vkGetPhysicalDeviceFeatures(dev, &devFeatures);

		//Big Optionally: Check if Geometry Shaders are supported
		if (devFeatures.geometryShader)
			score += 1000;
		else
			TP_WARN(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" Failed Geometry Shader Test!");

		//Big Optionally: Check if Tessellation Shaders are supported
		if (devFeatures.tessellationShader)
			score += 1000;
		else
			TP_WARN(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
			        "\" Failed Tessellation Shader Test!");

		//Optionally: Check if device support fill mode non solid
		if (devFeatures.fillModeNonSolid)
			score += 250;
		else
			TP_WARN(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
			        "\" Failed fillModeNonSolid Test!");

		//Optionally: Check if Surface has optimal surface format
		bool optimalFormat = false;
		for (auto& format : surfaceFormats)
		{
			if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				optimalFormat = true;
				break;
			}
		}
		if(optimalFormat)
			score += 250;
		else
			TP_WARN(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
			        "\" Failed Optimal Surface Format Test!");

		vkDestroySurfaceKHR(instance, surface, nullptr);
		TRAP::INTERNAL::WindowingAPI::DestroyWindow(std::move(vulkanTestWindow));
		vkDestroyInstance(instance, nullptr);

		//Optionally: Check VRAM size (1e+9 == Bytes to Gigabytes)
		//Get PhysicalDevice Memory Properties
		VkPhysicalDeviceMemoryProperties memProps;
		vkGetPhysicalDeviceMemoryProperties(dev, &memProps);
		for(uint32_t i = 0; i < memProps.memoryHeapCount; i++)
		{
			if (VK_MEMORY_HEAP_DEVICE_LOCAL_BIT & memProps.memoryHeaps[i].flags)
				score += static_cast<uint32_t>(memProps.memoryHeaps[i].size) / static_cast<uint32_t>(1e+9) * 100u;
		}

		//Optionally: Check 2D & Cube Image Max Size
		score += devProps.limits.maxImageDimension2D;
		score += devProps.limits.maxImageDimensionCube;

		//Optionally: Check if Anisotropic Filtering is supported
		if (devFeatures.samplerAnisotropy)
			score += 500;
		else
			TP_WARN(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
			        "\" Failed Anisotropic Filtering Test!");

		//Get PhysicalDevice UUID
		std::array<uint8_t, 16> physicalDeviceUUID{};

		VkPhysicalDeviceIDPropertiesKHR physicalDeviceIDProperties;
		VkPhysicalDeviceProperties2 props2;
		props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		props2.pNext = &physicalDeviceIDProperties;
		physicalDeviceIDProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES;
		physicalDeviceIDProperties.pNext = nullptr;
		vkGetPhysicalDeviceProperties2(dev, &props2);

		//Copy UUID
		std::memcpy(physicalDeviceUUID.data(), physicalDeviceIDProperties.deviceUUID, physicalDeviceUUID.size());

		TP_INFO(Log::RendererVulkanPrefix, "Found GPU: \"", devProps.deviceName, "\" Score: ", score);
		s_availablePhysicalDeviceUUIDs.emplace(score, physicalDeviceUUID);
		score = 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanPhysicalDevice::LoadAllPhysicalDeviceExtensions()
{
	uint32_t extensionsCount = 0;
	VkCall(vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &extensionsCount, nullptr));
	m_availablePhysicalDeviceExtensions.resize(extensionsCount);
	VkCall(vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &extensionsCount,
	                                            m_availablePhysicalDeviceExtensions.data()));
}