#include "TRAPPCH.h"
#include "VulkanPhysicalDevice.h"

#include "VulkanInstance.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Graphics/API/Vulkan/Utils/VulkanLoader.h"
#include "Utils/ErrorCodes/ErrorCodes.h"
#include "Maths/Math.h"

namespace
{
	[[nodiscard]] std::vector<VkExtensionProperties> LoadAllPhysicalDeviceExtensions(VkPhysicalDevice physicalDevice)
	{
		std::vector<VkExtensionProperties> deviceExtensions{};

		u32 extensionsCount = 0;
		VkCall(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, nullptr));
		deviceExtensions.resize(extensionsCount);
		VkCall(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, deviceExtensions.data()));

		return deviceExtensions;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr u32 GetMaxUsableMSAASampleCount(const VkPhysicalDeviceProperties& devProps)
	{
		VkSampleCountFlags sampleCounts = TRAP::Math::Min(devProps.limits.framebufferColorSampleCounts,
														  devProps.limits.framebufferDepthSampleCounts);
		sampleCounts = TRAP::Math::Min(sampleCounts, devProps.limits.framebufferStencilSampleCounts);

		if((sampleCounts & VK_SAMPLE_COUNT_64_BIT) != 0u)
			return VK_SAMPLE_COUNT_64_BIT;
		if((sampleCounts & VK_SAMPLE_COUNT_32_BIT) != 0u)
			return VK_SAMPLE_COUNT_32_BIT;
		if((sampleCounts & VK_SAMPLE_COUNT_16_BIT) != 0u)
			return VK_SAMPLE_COUNT_16_BIT;
		if((sampleCounts & VK_SAMPLE_COUNT_8_BIT) != 0u)
			return VK_SAMPLE_COUNT_8_BIT;
		if((sampleCounts & VK_SAMPLE_COUNT_4_BIT) != 0u)
			return VK_SAMPLE_COUNT_4_BIT;
		if((sampleCounts & VK_SAMPLE_COUNT_2_BIT) != 0u)
			return VK_SAMPLE_COUNT_2_BIT;

		return VK_SAMPLE_COUNT_1_BIT;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] std::vector<VkPhysicalDevice> GetAllVkPhysicalDevices(VkInstance instance)
	{
		u32 physicalDeviceCount = 0;
		VkCall(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr));

		std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
		VkCall(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data()));

		return physicalDevices;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void SetupRendererAPIGPUSettings(const TRAP::Graphics::API::VulkanPhysicalDevice& physicalDevice)
	{
		// Capabilities for VulkanRenderer
		for (u32 i = 0; i < std::to_underlying(TRAP::Graphics::API::ImageFormat::IMAGE_FORMAT_COUNT); ++i)
		{
			const VkFormat fmt = ImageFormatToVkFormat(static_cast<TRAP::Graphics::API::ImageFormat>(i));
			if (fmt == VK_FORMAT_UNDEFINED)
				continue;

			VkFormatProperties formatSupport{};
			vkGetPhysicalDeviceFormatProperties(physicalDevice.GetVkPhysicalDevice(), fmt, &formatSupport);
			TRAP::Graphics::API::VulkanRenderer::s_GPUCapBits.CanShaderReadFrom[i] = (formatSupport.optimalTilingFeatures &
																                      VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) != 0;
			TRAP::Graphics::API::VulkanRenderer::s_GPUCapBits.CanShaderWriteTo[i] = (formatSupport.optimalTilingFeatures &
																                     VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT) != 0;
			TRAP::Graphics::API::VulkanRenderer::s_GPUCapBits.CanRenderTargetWriteTo[i] = (formatSupport.optimalTilingFeatures &
																	                       (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
																	                        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)) != 0;
		}

		const auto& devProps = physicalDevice.GetVkPhysicalDeviceProperties();
		const auto& devFeatures = physicalDevice.GetVkPhysicalDeviceFeatures();
		const auto& devSubGroupProps = physicalDevice.GetVkPhysicalDeviceSubgroupProperties();

		TRAP::Graphics::RendererAPI::GPUSettings.UniformBufferAlignment = devProps.limits.minUniformBufferOffsetAlignment;
		TRAP::Graphics::RendererAPI::GPUSettings.MaxUniformBufferRange = devProps.limits.maxUniformBufferRange;
		TRAP::Graphics::RendererAPI::GPUSettings.StorageBufferAlignment = devProps.limits.minStorageBufferOffsetAlignment;
		TRAP::Graphics::RendererAPI::GPUSettings.MaxStorageBufferRange = devProps.limits.maxStorageBufferRange;
		TRAP::Graphics::RendererAPI::GPUSettings.UploadBufferTextureAlignment = NumericCast<u32>(devProps.limits.optimalBufferCopyOffsetAlignment);
		TRAP::Graphics::RendererAPI::GPUSettings.UploadBufferTextureRowAlignment = NumericCast<u32>(devProps.limits.optimalBufferCopyRowPitchAlignment);
		TRAP::Graphics::RendererAPI::GPUSettings.MaxVertexInputBindings = devProps.limits.maxVertexInputBindings;
		TRAP::Graphics::RendererAPI::GPUSettings.MaxVertexInputAttributes = devProps.limits.maxVertexInputAttributes;
		TRAP::Graphics::RendererAPI::GPUSettings.MultiDrawIndirectSupported = devProps.limits.maxDrawIndirectCount > 1u;
		TRAP::Graphics::RendererAPI::GPUSettings.MaxAnisotropy = devProps.limits.maxSamplerAnisotropy;
		TRAP::Graphics::RendererAPI::GPUSettings.MaxImageDimension2D = devProps.limits.maxImageDimension2D;
		TRAP::Graphics::RendererAPI::GPUSettings.MaxImageDimensionCube = devProps.limits.maxImageDimensionCube;
		TRAP::Graphics::RendererAPI::GPUSettings.FillModeNonSolid = (devFeatures.fillModeNonSolid != 0u);
		TRAP::Graphics::RendererAPI::GPUSettings.MaxPushConstantSize = devProps.limits.maxPushConstantsSize;
		TRAP::Graphics::RendererAPI::GPUSettings.MaxSamplerAllocationCount = devProps.limits.maxSamplerAllocationCount;
		TRAP::Graphics::RendererAPI::GPUSettings.MaxTessellationControlPoints = devProps.limits.maxTessellationPatchSize;
		TRAP::Graphics::RendererAPI::GPUSettings.MaxMSAASampleCount = static_cast<TRAP::Graphics::RendererAPI::SampleCount>(TRAP::Math::Min(GetMaxUsableMSAASampleCount(devProps), static_cast<u32>(VK_SAMPLE_COUNT_16_BIT)));
		TRAP::Graphics::RendererAPI::GPUSettings.MaxColorRenderTargets = devProps.limits.maxColorAttachments;

		// maxBoundDescriptorSets not needed because engine is always limited to 4 descriptor sets

		TRAP::Graphics::RendererAPI::GPUSettings.WaveLaneCount = devSubGroupProps.subgroupSize;
		TRAP::Graphics::RendererAPI::GPUSettings.WaveOpsSupportFlags = TRAP::Graphics::RendererAPI::WaveOpsSupportFlags::None;
		if ((devSubGroupProps.supportedOperations & VK_SUBGROUP_FEATURE_BASIC_BIT) != 0u)
			TRAP::Graphics::RendererAPI::GPUSettings.WaveOpsSupportFlags |= TRAP::Graphics::RendererAPI::WaveOpsSupportFlags::Basic;
		if ((devSubGroupProps.supportedOperations & VK_SUBGROUP_FEATURE_VOTE_BIT) != 0u)
			TRAP::Graphics::RendererAPI::GPUSettings.WaveOpsSupportFlags |= TRAP::Graphics::RendererAPI::WaveOpsSupportFlags::Vote;
		if ((devSubGroupProps.supportedOperations & VK_SUBGROUP_FEATURE_ARITHMETIC_BIT) != 0u)
			TRAP::Graphics::RendererAPI::GPUSettings.WaveOpsSupportFlags |= TRAP::Graphics::RendererAPI::WaveOpsSupportFlags::Arithmetic;
		if ((devSubGroupProps.supportedOperations & VK_SUBGROUP_FEATURE_BALLOT_BIT) != 0u)
			TRAP::Graphics::RendererAPI::GPUSettings.WaveOpsSupportFlags |= TRAP::Graphics::RendererAPI::WaveOpsSupportFlags::Ballot;
		if ((devSubGroupProps.supportedOperations & VK_SUBGROUP_FEATURE_SHUFFLE_BIT) != 0u)
			TRAP::Graphics::RendererAPI::GPUSettings.WaveOpsSupportFlags |= TRAP::Graphics::RendererAPI::WaveOpsSupportFlags::Shuffle;
		if ((devSubGroupProps.supportedOperations & VK_SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT) != 0u)
			TRAP::Graphics::RendererAPI::GPUSettings.WaveOpsSupportFlags |= TRAP::Graphics::RendererAPI::WaveOpsSupportFlags::ShuffleRelative;
		if ((devSubGroupProps.supportedOperations & VK_SUBGROUP_FEATURE_CLUSTERED_BIT) != 0u)
			TRAP::Graphics::RendererAPI::GPUSettings.WaveOpsSupportFlags |= TRAP::Graphics::RendererAPI::WaveOpsSupportFlags::Clustered;
		if ((devSubGroupProps.supportedOperations & VK_SUBGROUP_FEATURE_QUAD_BIT) != 0u)
			TRAP::Graphics::RendererAPI::GPUSettings.WaveOpsSupportFlags |= TRAP::Graphics::RendererAPI::WaveOpsSupportFlags::Quad;
		if ((devSubGroupProps.supportedOperations & VK_SUBGROUP_FEATURE_PARTITIONED_BIT_NV) != 0u)
			TRAP::Graphics::RendererAPI::GPUSettings.WaveOpsSupportFlags |= TRAP::Graphics::RendererAPI::WaveOpsSupportFlags::PartitionedNV;

		TRAP::Graphics::RendererAPI::GPUSettings.TessellationSupported = (devFeatures.tessellationShader != 0u);
		TRAP::Graphics::RendererAPI::GPUSettings.GeometryShaderSupported = (devFeatures.geometryShader != 0u);
		TRAP::Graphics::RendererAPI::GPUSettings.SampleRateShadingSupported = (devFeatures.sampleRateShading != 0u);

#ifndef TRAP_HEADLESS_MODE
		//Surface and present test is a hard requirement to pass the Physical device rating.
		TRAP::Graphics::RendererAPI::GPUSettings.SurfaceSupported = true;
		TRAP::Graphics::RendererAPI::GPUSettings.PresentSupported = true;
#endif /*TRAP_HEADLESS_MODE*/
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] VkPhysicalDevice FindPhysicalDeviceViaUUID(const TRAP::Graphics::API::VulkanInstance& instance,
	                                                         const TRAP::Utils::UUID& physicalDeviceUUID)
	{
		ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

		const auto physicalDevices = GetAllVkPhysicalDevices(instance.GetVkInstance());

		for (VkPhysicalDevice device : physicalDevices)
		{
			VkPhysicalDeviceIDPropertiesKHR physicalDeviceIDProperties{};
			physicalDeviceIDProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES;
			VkPhysicalDeviceProperties2 props2{};
			props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
			props2.pNext = &physicalDeviceIDProperties;
			vkGetPhysicalDeviceProperties2(device, &props2);

			const TRAP::Utils::UUID testUUID = std::to_array(physicalDeviceIDProperties.deviceUUID);

			if(testUUID == physicalDeviceUUID)
				return device;
		}

		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanPhysicalDevice::VulkanPhysicalDevice(TRAP::Ref<VulkanInstance> instance,
																const TRAP::Utils::UUID& physicalDeviceUUID)
	: m_instance(std::move(instance))
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_instance, "VulkanPhysicalDevice(): Vulkan Instance is nullptr");

	m_physicalDevice = FindPhysicalDeviceViaUUID(*m_instance, physicalDeviceUUID);
	TRAP_ASSERT(m_physicalDevice, "VulkanPhysicalDevice(): Vulkan Physical Device is nullptr!");

	if (m_physicalDevice == nullptr)
		Utils::DisplayError(Utils::ErrorCode::VulkanPhysicalDeviceCreationFailed);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanPhysicalDevicePrefix, "Creating PhysicalDevice");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	LoadDevicePropertiesAndFeatures();

	SetupRendererAPIGPUSettings(*this);

	m_availablePhysicalDeviceExtensions = LoadAllPhysicalDeviceExtensions(m_physicalDevice);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanPhysicalDevice::~VulkanPhysicalDevice()
{
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanPhysicalDevicePrefix, "Destroying PhysicalDevice");
#endif /*VERBOSE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanPhysicalDevice::LoadDevicePropertiesAndFeatures()
{
	vkGetPhysicalDeviceProperties(m_physicalDevice, &m_physicalDeviceProperties);
	vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_physicalDeviceMemoryProperties);
	vkGetPhysicalDeviceFeatures(m_physicalDevice, &m_physicalDeviceFeatures);

	VkPhysicalDeviceProperties2 props2{};
	props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

	m_physicalDeviceIDProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES;
	m_physicalDeviceSubgroupProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES;
	m_physicalDeviceDriverProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;

	VkBaseOutStructure* base = reinterpret_cast<VkBaseOutStructure*>(&props2);
	TRAP::Graphics::API::LinkVulkanStruct(base, m_physicalDeviceIDProperties);
	TRAP::Graphics::API::LinkVulkanStruct(base, m_physicalDeviceSubgroupProperties);
	TRAP::Graphics::API::LinkVulkanStruct(base, m_physicalDeviceDriverProperties);

	vkGetPhysicalDeviceProperties2(m_physicalDevice, &props2);

	u32 queueFamilyPropertyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyPropertyCount, nullptr);
	m_queueFamilyProperties.resize(queueFamilyPropertyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyPropertyCount,
											 m_queueFamilyProperties.data());

	for (u32 i = 0; i < std::to_underlying(TRAP::Graphics::API::ImageFormat::IMAGE_FORMAT_COUNT); ++i)
	{
		const VkFormat fmt = ImageFormatToVkFormat(static_cast<TRAP::Graphics::API::ImageFormat>(i));
		if (fmt == VK_FORMAT_UNDEFINED)
			continue;

		vkGetPhysicalDeviceFormatProperties(m_physicalDevice, fmt, &m_physicalDeviceFormatProperties[i]);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanPhysicalDevice::LoadPhysicalDeviceFragmentShaderInterlockFeatures()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	if(VulkanRenderer::s_fragmentShaderInterlockExtension)
	{
		VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT fragShaderInterlock{};
		fragShaderInterlock.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT;

		VkPhysicalDeviceFeatures2 features2{};
		features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		features2.pNext = &fragShaderInterlock;

		vkGetPhysicalDeviceFeatures2(m_physicalDevice, &features2);

		RendererAPI::GPUSettings.ROVsSupported = fragShaderInterlock.fragmentShaderPixelInterlock;
	}
	else
		RendererAPI::GPUSettings.ROVsSupported = 0u;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const std::multimap<u32, TRAP::Graphics::API::RatedVulkanPhysicalDevice>& TRAP::Graphics::API::VulkanPhysicalDevice::GetAllRatedPhysicalDevices(const VulkanInstance& instance)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	return GetAllRatedPhysicalDevices(instance.GetVkInstance());
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const std::multimap<u32, TRAP::Graphics::API::RatedVulkanPhysicalDevice>& TRAP::Graphics::API::VulkanPhysicalDevice::GetAllRatedPhysicalDevices(VkInstance instance)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	if (!s_ratedPhysicalDevices.empty())
		return s_ratedPhysicalDevices;

	const std::vector<VkPhysicalDevice> physicalDevices = GetAllVkPhysicalDevices(instance);

	if (!physicalDevices.empty())
		RatePhysicalDevices(physicalDevices, instance);
	else
		Utils::DisplayError(Utils::ErrorCode::VulkanNoPhysicalDeviceFound);

	return s_ratedPhysicalDevices;
}

//-------------------------------------------------------------------------------------------------------------------//

namespace
{
	//TODO Split RatePhysicalDevices() into smaller chunks
}

void TRAP::Graphics::API::VulkanPhysicalDevice::RatePhysicalDevices(const std::vector<VkPhysicalDevice> &physicalDevices, VkInstance instance)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	// Score each Physical Device and insert into multimap
	u32 score = 0;
	for (VkPhysicalDevice dev : physicalDevices)
	{
		// Get PhysicalDeviceProperties
		VkPhysicalDeviceProperties devProps;
		vkGetPhysicalDeviceProperties(dev, &devProps);

		// Required: Vulkan 1.1 is minimum
		if (devProps.apiVersion < VK_API_VERSION_1_1)
		{
			TP_ERROR(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
					 "\" Failed Vulkan Version Test!");
			continue;
		}

		// Required (dGPU or iGPU): Discrete GPUs have a significant performance advantage
		// Headless mode: Allows all types of physical devices
		if (devProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			score += 5000;
		else if (devProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
			score += 2500;
#ifndef TRAP_HEADLESS_MODE
		else
		{
			TP_ERROR(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
					 "\" Failed Dedicated/Internal GPU Test!");
			continue;
		}
#endif /*TRAP_HEADLESS_MODE*/

		// Required: Minimum 4 simultaneously bound descriptor sets support
		if (devProps.limits.maxBoundDescriptorSets < 4)
		{
			TP_ERROR(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
					 "\" Failed Max bound descriptor set Test!");
			continue;
		}

		// Get all PhysicalDevice Extensions
		u32 supportedExtensionsCount = 0;
		std::vector<VkExtensionProperties> supportedExtensions;
		VkCall(vkEnumerateDeviceExtensionProperties(dev, nullptr, &supportedExtensionsCount, nullptr));
		supportedExtensions.resize(supportedExtensionsCount);
		VkCall(vkEnumerateDeviceExtensionProperties(dev, nullptr, &supportedExtensionsCount, supportedExtensions.data()));
		score += supportedExtensionsCount * 50;

		// Required: Check if PhysicalDevice supports SPIRV 1.4
		const auto spirv1_4Result = std::ranges::find_if(supportedExtensions, [](const VkExtensionProperties& props)
		{
			return std::string_view(VK_KHR_SPIRV_1_4_EXTENSION_NAME) == props.extensionName;
		});
		const auto shaderFloatControlsResult = std::ranges::find_if(supportedExtensions, [](const VkExtensionProperties& props)
		{
			return std::string_view(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME) == props.extensionName;
		});
		if(spirv1_4Result == supportedExtensions.end() || shaderFloatControlsResult == supportedExtensions.end())
		{
			TP_ERROR(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
					 "\" Failed Required PhysicalDevice SPIRV 1.4 Extensions Test!");
			continue;
		}

		// Required: Check if PhysicalDevice supports swapchains
		// Disabled in Headless mode.
#ifndef TRAP_HEADLESS_MODE
		const auto swapChainResult = std::ranges::find_if(supportedExtensions, [](const VkExtensionProperties& props)
        {
			return std::string_view(VK_KHR_SWAPCHAIN_EXTENSION_NAME) == props.extensionName;
		});

		if (swapChainResult == supportedExtensions.end())
		{
			TP_ERROR(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
					 "\" Failed Required PhysicalDevice Extensions Test!");
			continue;
		}

		// Required: Create Vulkan Instance

		// Init WindowingAPI needed here for instance extensions
		// Disabled in Headless mode.

		if (!INTERNAL::WindowingAPI::Init())
			Utils::DisplayError(Utils::ErrorCode::WindowingAPIFailedInitialization);

		// Required: Create Vulkan Surface Test Window
		// Disabled in Headless mode.
		INTERNAL::WindowingAPI::WindowHint(INTERNAL::WindowingAPI::Hint::Visible, false);
		INTERNAL::WindowingAPI::WindowHint(INTERNAL::WindowingAPI::Hint::Focused, false);
		INTERNAL::WindowingAPI::InternalWindow* vulkanTestWindow = INTERNAL::WindowingAPI::CreateWindow(400,
																									    400,
																									    "TRAP Vulkan Surface Tester",
																									    nullptr);
		INTERNAL::WindowingAPI::DefaultWindowHints();
		if (vulkanTestWindow == nullptr)
		{
			vkDestroyInstance(instance, nullptr);
			TP_ERROR(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
					 "\" Failed Vulkan Surface Test Window creation!");
			continue;
		}

		// Required: Check if Surface can be created
		// Disabled in Headless mode

		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkResult res{};
		VkCall(res = TRAP::INTERNAL::WindowingAPI::CreateWindowSurface(instance, *vulkanTestWindow, nullptr,
																	   surface));
		if ((surface == nullptr) || res != VK_SUCCESS)
		{
			TRAP::INTERNAL::WindowingAPI::DestroyWindow(vulkanTestWindow);
			vkDestroyInstance(instance, nullptr);
			TP_ERROR(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" Failed Surface creation!");
			continue;
		}
#endif /*TRAP_HEADLESS_MODE*/

		// Required: Get Queue Families
		u32 queueFamilyPropertyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueFamilyPropertyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueFamilyPropertyCount, queueFamilyProperties.data());
		if (queueFamilyProperties.empty())
		{
#ifndef TRAP_HEADLESS_MODE
			vkDestroySurfaceKHR(instance, surface, nullptr);
			TRAP::INTERNAL::WindowingAPI::DestroyWindow(vulkanTestWindow);
#endif /*TRAP_HEADLESS_MODE*/
			vkDestroyInstance(instance, nullptr);
			TP_ERROR(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
					 "\" Failed Querying Queue Family Properties!");
			continue;
		}

		// Required: Check if PhysicalDevice supports Graphics queue
		bool foundGraphicsQueue = false;
		for (const VkQueueFamilyProperties &props : queueFamilyProperties)
		{
			if ((props.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0u)
			{
				foundGraphicsQueue = true;
				break;
			}
		}
		if (!foundGraphicsQueue)
		{
#ifndef TRAP_HEADLESS_MODE
			vkDestroySurfaceKHR(instance, surface, nullptr);
			TRAP::INTERNAL::WindowingAPI::DestroyWindow(vulkanTestWindow);
#endif /*TRAP_HEADLESS_MODE*/
			vkDestroyInstance(instance, nullptr);
			TP_ERROR(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" Failed Graphics Queue Test!");
			continue;
		}

		// Required: Check if PhysicalDevice supports Presenting
		// Disabled in Headless mode.
#ifndef TRAP_HEADLESS_MODE
		VkBool32 foundPresentSupport = VK_FALSE;
		for (usize i = 0; i < queueFamilyProperties.size(); i++)
		{
			VkCall(vkGetPhysicalDeviceSurfaceSupportKHR(dev, NumericCast<u32>(i), surface, &foundPresentSupport));
			if (foundPresentSupport != 0u)
				break;
		}
		if (foundPresentSupport == 0u)
		{
			vkDestroySurfaceKHR(instance, surface, nullptr);
			TRAP::INTERNAL::WindowingAPI::DestroyWindow(vulkanTestWindow);
			vkDestroyInstance(instance, nullptr);
			TP_ERROR(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
					 "\" Failed Present Queue Test!");
			continue;
		}

		// Required: Check if Surface contains present modes
		// Disabled in Headless mode.

		u32 surfacePresentModeCount = 0;
		VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surface, &surfacePresentModeCount, nullptr));
		std::vector<VkPresentModeKHR> presentModes(surfacePresentModeCount);
		VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surface, &surfacePresentModeCount,
														 presentModes.data()));
		if (presentModes.empty())
		{
			vkDestroySurfaceKHR(instance, surface, nullptr);
			TRAP::INTERNAL::WindowingAPI::DestroyWindow(vulkanTestWindow);
			vkDestroyInstance(instance, nullptr);
			TP_ERROR(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" Failed Present Mode Test!");
			continue;
		}

		// Required: Check if Surface contains formats
		// Disabled in Headless mode.

		u32 surfaceFormatCount = 0;
		VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surface, &surfaceFormatCount, nullptr));
		std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
		VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surface, &surfaceFormatCount, surfaceFormats.data()));
		if (surfaceFormats.empty())
		{
			vkDestroySurfaceKHR(instance, surface, nullptr);
			TRAP::INTERNAL::WindowingAPI::DestroyWindow(vulkanTestWindow);
			vkDestroyInstance(instance, nullptr);
			TP_ERROR(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
					 "\" Failed Surface Format Test!");
			continue;
		}
#endif /*TRAP_HEADLESS_MODE*/

		// Big Optionally: Check if PhysicalDevice supports Compute queue
		bool foundComputeQueue = false;
		for (const VkQueueFamilyProperties &props : queueFamilyProperties)
		{
			if ((props.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0u)
			{
				foundComputeQueue = true;
				score += 1000;
				break;
			}
		}
		if (!foundComputeQueue)
			TP_WARN(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" Failed Compute Queue Test!");

		// Big Optionally: Check if PhysicalDevice supports Transfer queue
		bool foundTransferQueue = false;
		for (const VkQueueFamilyProperties &props : queueFamilyProperties)
		{
			if ((props.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0u)
			{
				foundTransferQueue = true;
				score += 1000;
				break;
			}
		}
		if (!foundTransferQueue)
			TP_WARN(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" Failed Transfer Queue Test!");

		// Big Optionally: Check if Raytracing extensions are supported
		bool raytracing = true;
		static constexpr std::array<std::string_view, 8> raytracingExt =
		{
			VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
			VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME,
			VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
			VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
			VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
			VK_KHR_SPIRV_1_4_EXTENSION_NAME,
			VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
			VK_KHR_RAY_QUERY_EXTENSION_NAME
		};

		for (const std::string_view str : raytracingExt)
		{
			const auto extRes = std::ranges::find_if(supportedExtensions,
											 [str](const VkExtensionProperties &props)
											 {
												 return str == props.extensionName;
											 });

			if (extRes == supportedExtensions.end())
			{
				raytracing = false;
				break;
			}
		}

		if (raytracing)
			score += 2000;
		else
			TP_WARN(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" Failed Raytracing Test!");

		// Get PhysicalDevice Features
		VkPhysicalDeviceFeatures devFeatures;
		vkGetPhysicalDeviceFeatures(dev, &devFeatures);

		// Big Optionally: Check if Geometry Shaders are supported
		if (devFeatures.geometryShader != 0u)
			score += 1000;
		else
			TP_WARN(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" Failed Geometry Shader Test!");

		// Big Optionally: Check if Tessellation Shaders are supported
		if (devFeatures.tessellationShader != 0u)
			score += 1000;
		else
			TP_WARN(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
					"\" Failed Tessellation Shader Test!");

		// Big Optionally: Check if Variable Rate Shading is supported and Tier 1/Tier 2
		bool VRS = true;
		static constexpr std::array<std::string_view, 2> VRSExt =
		{
			VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
			VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME
		};
		for (const std::string_view str : VRSExt)
		{
			const auto extRes = std::ranges::find_if(supportedExtensions,
											 [str](const VkExtensionProperties &props)
											 {
												 return str == props.extensionName;
											 });

			if (extRes == supportedExtensions.end())
			{
				VRS = false;
				break;
			}
		}

		if (VRS)
		{
			//VRS Tier 1/Tier 2
			VkPhysicalDeviceFragmentShadingRateFeaturesKHR fsrFeatures{};
			fsrFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;
			fsrFeatures.pNext = nullptr;
			VkPhysicalDeviceFeatures2KHR devFeatures2{};
			devFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR;
			devFeatures2.pNext = &fsrFeatures;
			vkGetPhysicalDeviceFeatures2KHR(dev, &devFeatures2);

			if((fsrFeatures.pipelineFragmentShadingRate == 0u) && (fsrFeatures.attachmentFragmentShadingRate == 0u))
				TP_WARN(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" Failed Variable Rate Shading Test!");
			else
				score += 1000;

			if(fsrFeatures.pipelineFragmentShadingRate != 0u) //Tier 1
				score += 100;
			if(fsrFeatures.attachmentFragmentShadingRate != 0u) //Tier 2
				score += 200;
		}
		else
			TP_WARN(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" Failed Variable Rate Shading Test!");

		// Optionally: Check if device support fill mode non solid
		if (devFeatures.fillModeNonSolid != 0u)
			score += 250;
		else
			TP_WARN(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
					"\" Failed fillModeNonSolid Test!");

		// Optionally: Check if Surface has optimal surface format
		// Disabled in Headless mode.
#ifndef TRAP_HEADLESS_MODE
		bool optimalFormat = false;
		for (auto &format : surfaceFormats)
		{
			if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			// if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				optimalFormat = true;
				break;
			}
		}
		if (optimalFormat)
			score += 250;
		else
			TP_WARN(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
					"\" Failed Optimal Surface Format Test!");

		vkDestroySurfaceKHR(instance, surface, nullptr);
		TRAP::INTERNAL::WindowingAPI::DestroyWindow(vulkanTestWindow);
#endif /*TRAP_HEADLESS_MODE*/

		// Optionally: Check VRAM size (1e+9 == Bytes to Gigabytes)
		// Get PhysicalDevice Memory Properties
		VkPhysicalDeviceMemoryProperties memProps;
		vkGetPhysicalDeviceMemoryProperties(dev, &memProps);
		for (u32 i = 0; i < memProps.memoryHeapCount; i++)
		{
			if ((VK_MEMORY_HEAP_DEVICE_LOCAL_BIT & memProps.memoryHeaps[i].flags) != 0u)
			{
				static constexpr u64 ByteToGigabyte = 1e+9;
				score += NumericCast<u32>(memProps.memoryHeaps[i].size / ByteToGigabyte * 100u);
			}
		}

		// Optionally: Check 2D & Cube Image Max Size
		score += devProps.limits.maxImageDimension2D / 32;
		score += devProps.limits.maxImageDimensionCube / 32;

		// Optionally: Check max supported MSAA sample count
		VkSampleCountFlags sampleCounts = TRAP::Math::Min(devProps.limits.framebufferColorSampleCounts,
														  devProps.limits.framebufferDepthSampleCounts);
		sampleCounts = TRAP::Math::Min(sampleCounts, devProps.limits.framebufferStencilSampleCounts);
		score += static_cast<u32>(sampleCounts) * 10;

		// Optionally: Check if Anisotropic Filtering is supported
		if (devFeatures.samplerAnisotropy != 0u)
			score += 500;
		else
			TP_WARN(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
					"\" Failed Anisotropic Filtering Test!");

		// Get PhysicalDevice UUID
		TRAP::Utils::UUID physicalDeviceUUID{};

		VkPhysicalDeviceIDPropertiesKHR physicalDeviceIDProperties;
		VkPhysicalDeviceProperties2 props2;
		props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		props2.pNext = &physicalDeviceIDProperties;
		physicalDeviceIDProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES;
		physicalDeviceIDProperties.pNext = nullptr;
		vkGetPhysicalDeviceProperties2(dev, &props2);

		// Copy UUID
		std::ranges::copy(physicalDeviceIDProperties.deviceUUID, physicalDeviceUUID.begin());

		TP_INFO(Log::RendererVulkanPrefix, "Found GPU: \"", devProps.deviceName, "\"(", TRAP::Utils::UUIDToString(physicalDeviceUUID), ')', " Score: ", score);

		const RatedVulkanPhysicalDevice ratedDevice
		{
			.PhysicalDeviceUUID = physicalDeviceUUID,
			.PhysicalDeviceName = devProps.deviceName,
		};
		s_ratedPhysicalDevices.emplace(score, ratedDevice);
		score = 0;
	}
}
