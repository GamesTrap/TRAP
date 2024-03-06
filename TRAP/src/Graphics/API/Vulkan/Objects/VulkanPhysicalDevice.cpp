#include "TRAPPCH.h"
#include "VulkanPhysicalDevice.h"

#include "Utils/UniqueResource.h"
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

[[nodiscard]] TRAP::Optional<VkImageFormatProperties> TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceImageFormatProperties(const TRAP::Graphics::API::ImageFormat format,
                                                                                                                                          const VkImageType imageType,
																														                  const VkImageUsageFlags imageUsageFlags) const
{
	VkImageFormatProperties props{};
	VkResult res = vkGetPhysicalDeviceImageFormatProperties(m_physicalDevice, ImageFormatToVkFormat(format),
	                                                        imageType, VK_IMAGE_TILING_OPTIMAL, imageUsageFlags, 0,
															&props);
	VkCall(res);

	if(res != VK_SUCCESS)
		return TRAP::NullOpt;

	return props;
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
	[[nodiscard]] constexpr bool RatePhysicalDevice_CheckVulkanVersion(const VkPhysicalDeviceProperties& devProps)
	{
		// Required: Vulkan 1.1 or higher
		if (devProps.apiVersion < VK_API_VERSION_1_1)
		{
			TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed Vulkan version test!");
			return false;
		}

		return true;
	}

	[[nodiscard]] constexpr TRAP::Optional<u32> RatePhysicalDevice_CheckDeviceType(const VkPhysicalDeviceProperties& devProps)
	{
		// Required (dGPU or iGPU): Discrete GPUs have a significant performance advantage.
		// Headless mode: Any device type is allowed (even software implementations).
		if(devProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			return 5000;
		if(devProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
			return 2500;

#ifndef TRAP_HEADLESS_MODE
		TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed dedicated/integrated GPU test!");
		return TRAP::NullOpt;
#else
		return 0;
#endif /*TRAP_HEADLESS_MODE*/
	}

	[[nodiscard]] constexpr bool RatePhysicalDevice_CheckMaxBoundDescriptorSet(const VkPhysicalDeviceProperties& devProps)
	{
		// Required: Support for at least 4 simultaneously bound descriptor sets
		if(devProps.limits.maxBoundDescriptorSets < 4)
		{
			TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed max bound descriptor set test!");
			return false;
		}

		return true;
	}

	[[nodiscard]] std::pair<u32, std::vector<VkExtensionProperties>> RatePhysicalDevice_RateDeviceExtensions(VkPhysicalDevice physicalDevice)
	{
		std::pair<u32, std::vector<VkExtensionProperties>> result{};
		auto& [score, extensions] = result;

		//Get all physical device extensions
		u32 supportedExtensionsCount = 0;
		VkCall(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &supportedExtensionsCount, nullptr));
		extensions.resize(supportedExtensionsCount);
		VkCall(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &supportedExtensionsCount, extensions.data()));

		score = supportedExtensionsCount * 50;

		return result;
	}

	[[nodiscard]] constexpr bool RatePhysicalDevice_CheckSPIRV1_4Support(const std::span<const VkExtensionProperties> extensions, const VkPhysicalDeviceProperties& devProps)
	{
		// Required: Check if physical device supports SPIRV 1.4 extensions
		const bool supportsSPIRV1_4Ext = std::ranges::find_if(extensions, [](const auto& props){return std::string_view(VK_KHR_SPIRV_1_4_EXTENSION_NAME) == props.extensionName;}) != extensions.end();
		const bool supportsShaderFloatControlsExt = std::ranges::find_if(extensions, [](const auto& props){return std::string_view(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME) == props.extensionName;}) != extensions.end();

		if(!supportsSPIRV1_4Ext || !supportsShaderFloatControlsExt)
		{
			TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed required SPIRV 1.4 extensions test!");
			return false;
		}

		return true;
	}

#ifndef TRAP_HEADLESS_MODE
	[[nodiscard]] constexpr bool RatePhysicalDevice_CheckSwapChainSupport(const std::span<const VkExtensionProperties> extensions, const VkPhysicalDeviceProperties& devProps)
	{
		// Required: Check if physical device supports swapchains

		const bool swapChainSupported = std::ranges::find_if(extensions, [](const auto& props){return std::string_view(VK_KHR_SWAPCHAIN_EXTENSION_NAME) == props.extensionName;}) != extensions.end();
		if(!swapChainSupported)
		{
			TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed required swapchain extension test!");
			return false;
		}

		return true;
	}
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
	using TestWindowHandle = TRAP::UniqueResource<TRAP::INTERNAL::WindowingAPI::InternalWindow*, decltype(&TRAP::INTERNAL::WindowingAPI::DestroyWindow)>;
	[[nodiscard]] TestWindowHandle RatePhysicalDevice_CreateSurfaceTestWindow(const VkPhysicalDeviceProperties& devProps)
	{
		// Required: Create Vulkan surface test window
		// Headless mode: Dont use this function

		if(!TRAP::INTERNAL::WindowingAPI::Init())
			TRAP::Utils::DisplayError(TRAP::Utils::ErrorCode::WindowingAPIFailedInitialization);

		TRAP::INTERNAL::WindowingAPI::WindowHint(TRAP::INTERNAL::WindowingAPI::Hint::Visible, false);
		TRAP::INTERNAL::WindowingAPI::WindowHint(TRAP::INTERNAL::WindowingAPI::Hint::Focused, false);
		auto* const vulkanTestWindow = TRAP::INTERNAL::WindowingAPI::CreateWindow(400, 400, "TRAP Vulkan Surface Tester", nullptr);

		TRAP::INTERNAL::WindowingAPI::DefaultWindowHints();
		if(vulkanTestWindow == nullptr)
			TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed Vulkan test window creation!");

		return TRAP::MakeUniqueResourceChecked(vulkanTestWindow, nullptr, &TRAP::INTERNAL::WindowingAPI::DestroyWindow);
	}
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
	[[nodiscard]] auto RatePhysicalDevice_CheckSurfaceSupport(VkInstance instance, const TRAP::INTERNAL::WindowingAPI::InternalWindow& window,
                                                              const VkPhysicalDeviceProperties& devProps)
	{
		// Required: Check if surface can be created

		VkSurfaceKHR surface = VK_NULL_HANDLE;
		const VkResult res = TRAP::INTERNAL::WindowingAPI::CreateWindowSurface(instance, window, nullptr, surface);
		VkCall(res);

		if((surface == VK_NULL_HANDLE) || res != VK_SUCCESS)
			TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed surface creation test!");

		return TRAP::MakeUniqueResourceChecked(surface, VK_NULL_HANDLE, [instance](VkSurfaceKHR s){vkDestroySurfaceKHR(instance, s, nullptr);});
	}
#endif /*TRAP_HEADLESS_MODE*/

	[[nodiscard]] std::vector<VkQueueFamilyProperties> RatePhysicalDevice_GetQueueFamilies(VkPhysicalDevice physicalDevice,
	                                                                                       const VkPhysicalDeviceProperties& devProps)
	{
		// Required: Get queue families
		u32 queueFamilyPropertyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, queueFamilyProperties.data());
		if (queueFamilyProperties.empty())
		{
			TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
					 "\" failed querying queue family properties!");
			return {};
		}

		return queueFamilyProperties;
	}

	[[nodiscard]] constexpr bool RatePhysicalDevice_CheckGraphicsQueue(const std::span<const VkQueueFamilyProperties> queueFamilyProps,
	                                                                   const VkPhysicalDeviceProperties& devProps)
	{
		// Required: Check if ühysical device supports graphics queue

		for (const VkQueueFamilyProperties& props : queueFamilyProps)
		{
			if ((props.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0u)
				return true;
		}

		TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed graphics queue test!");
		return false;
	}

#ifndef TRAP_HEADLESS_MODE
	[[nodiscard]] bool RatePhysicalDevice_CheckPresentSupport(const std::span<const VkQueueFamilyProperties> queueFamilyProps, VkPhysicalDevice physicalDevice,
	                                                          VkSurfaceKHR surface, const VkPhysicalDeviceProperties& devProps)
	{
		// Required: Check if physical device supports presenting
		// Disabled in Headless mode.

		VkBool32 foundPresentSupport = VK_FALSE;
		for (usize i = 0; i < queueFamilyProps.size(); i++)
		{
			VkCall(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, NumericCast<u32>(i), surface, &foundPresentSupport));
			if (foundPresentSupport != 0u)
				return true;
		}

		TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed present queue test!");
		return false;
	}
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
	[[nodiscard]] bool RatePhysicalDevice_CheckSurfacePresentModes(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
	                                                               const VkPhysicalDeviceProperties& devProps)
	{
		// Required: Check if surface contains present modes
		// Disabled in Headless mode.

		u32 surfacePresentModeCount = 0;
		VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &surfacePresentModeCount, nullptr));
		std::vector<VkPresentModeKHR> presentModes(surfacePresentModeCount);
		VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &surfacePresentModeCount,
														 presentModes.data()));
		if (presentModes.empty())
		{
			TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed present mode test!");
			return false;
		}

		return true;
	}
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
	[[nodiscard]] std::vector<VkSurfaceFormatKHR> RatePhysicalDevice_CheckSurfaceFormats(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
	                                                                                     const VkPhysicalDeviceProperties& devProps)
	{
		// Required: Check if surface contains formats
		// Disabled in Headless mode.

		u32 surfaceFormatCount = 0;
		VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr));
		std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
		VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats.data()));
		if (surfaceFormats.empty())
			TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed surface format test!");

		return surfaceFormats;
	}
#endif /*TRAP_HEADLESS_MODE*/

	[[nodiscard]] constexpr u32 RatePhysicalDevice_CheckComputeQueue(const std::span<const VkQueueFamilyProperties> queueFamilyProps,
                                                                     const VkPhysicalDeviceProperties& devProps)
	{
		// Big Optionally: Check if physical device supports compute queue

		for (const VkQueueFamilyProperties &props : queueFamilyProps)
		{
			if ((props.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0u)
				return 1000;
		}

		TP_WARN(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed compute queue test!");
		return 0;
	}

	[[nodiscard]] constexpr u32 RatePhysicalDevice_CheckTransferQueue(const std::span<const VkQueueFamilyProperties> queueFamilyProps,
	                                                                  const VkPhysicalDeviceProperties& devProps)
	{
		// Big Optionally: Check if physical device supports transfer queue

		for (const VkQueueFamilyProperties &props : queueFamilyProps)
		{
			if ((props.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0u)
				return 1000;
		}

		TP_WARN(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed transfer queue test!");
		return 0;
	}

	[[nodiscard]] constexpr u32 RatePhysicalDevice_CheckRayTracingSupport(const std::span<const VkExtensionProperties> extensions,
	                                                                      const VkPhysicalDeviceProperties& devProps)
	{
		// Big Optionally: Check if ray tracing extensions are supported

		constexpr std::array<std::string_view, 8> raytracingExt =
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

		bool raytracing = true;
		for (const std::string_view str : raytracingExt)
		{
			const auto extRes = std::ranges::find_if(extensions,
											 [str](const VkExtensionProperties &props)
											 {
												 return str == props.extensionName;
											 }) != extensions.end();

			if (!extRes)
			{
				raytracing = false;
				break;
			}
		}

		if (raytracing)
			return 2000;

		TP_WARN(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed ray tracing test!");
		return 0;
	}

	[[nodiscard]] constexpr u32 RatePhysicalDevice_CheckGeometryShaderSupport(const VkPhysicalDeviceFeatures& devFeatures,
	                                                                          const VkPhysicalDeviceProperties& devProps)
	{
		// Big Optionally: Check if geometry shaders are supported

		if (devFeatures.geometryShader != 0u)
			return 1000;

		TP_WARN(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed geometry shader test!");
		return 0;
	}

	[[nodiscard]] constexpr u32 RatePhysicalDevice_CheckTessellationShaderSupport(const VkPhysicalDeviceFeatures& devFeatures,
	                                                                              const VkPhysicalDeviceProperties& devProps)
	{
		// Big Optionally: Check if tessellation shaders are supported

		if (devFeatures.tessellationShader != 0u)
			return 1000;

		TP_WARN(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed tessellation shader test!");
		return 0;
	}

	[[nodiscard]] u32 RatePhysicalDevice_CheckVariableRateShadingSupport(const std::span<const VkExtensionProperties> extensions,
	                                                                     VkPhysicalDevice physicalDevice,
																		 const VkPhysicalDeviceProperties& devProps)
	{
		// Big Optionally: Check if variable rate shading is supported and Tier 1/Tier 2

		u32 score = 0;
		bool VRS = true;
		static constexpr std::array<std::string_view, 2> VRSExt =
		{
			VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
			VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME
		};
		for (const std::string_view str : VRSExt)
		{
			const auto extRes = std::ranges::find_if(extensions,
											 [str](const VkExtensionProperties &props)
											 {
												 return str == props.extensionName;
											 }) != extensions.end();

			if (!extRes)
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
			VkPhysicalDeviceFeatures2KHR devFeatures2{};
			devFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR;
			devFeatures2.pNext = &fsrFeatures;
			vkGetPhysicalDeviceFeatures2KHR(physicalDevice, &devFeatures2);

			if((fsrFeatures.pipelineFragmentShadingRate == 0u) && (fsrFeatures.attachmentFragmentShadingRate == 0u))
			{
				TP_WARN(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed variable rate shading test!");
				return 0;
			}

			score += 1000;

			if(fsrFeatures.pipelineFragmentShadingRate != 0u) //Tier 1
				score += 100;
			if(fsrFeatures.attachmentFragmentShadingRate != 0u) //Tier 2
				score += 200;
		}
		else
			TP_WARN(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed variable rate shading test!");

		return score;
	}

	[[nodiscard]] constexpr u32 RatePhysicalDevice_CheckFillModeNonSolidSupport(const VkPhysicalDeviceFeatures& devFeatures,
	                                                                            const VkPhysicalDeviceProperties& devProps)
	{
		// Optionally: Check if physical device support fill mode non solid
		if (devFeatures.fillModeNonSolid != 0u)
			return 250;

		TP_WARN(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed fillModeNonSolid test!");
		return 0;
	}

#ifndef TRAP_HEADLESS_MODE
	[[nodiscard]] constexpr u32 RatePhysicalDevice_CheckOptimalSurfaceFormat(const std::span<const VkSurfaceFormatKHR> surfaceFormats,
	                                                                         const VkPhysicalDeviceProperties& devProps)
	{
		// Optionally: Check if surface has optimal surface format

		for (const auto& format : surfaceFormats)
		{
			if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return 250;
		}

		TP_WARN(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed optimal surface format test!");
		return 0;
	}
#endif /*TRAP_HEADLESS_MODE*/

	[[nodiscard]] u32 RatePhysicalDevice_CheckVRAMSize(VkPhysicalDevice physicalDevice)
	{
		// Optionally: Check VRAM size

		VkPhysicalDeviceMemoryProperties memProps{};
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProps);

		u32 score = 0;
		for (u32 i = 0; i < memProps.memoryHeapCount; i++)
		{
			if ((VK_MEMORY_HEAP_DEVICE_LOCAL_BIT & memProps.memoryHeaps[i].flags) != 0u)
			{
				static constexpr u64 ByteToGigabyte = 1e+9;
				score += NumericCast<u32>(memProps.memoryHeaps[i].size / ByteToGigabyte * 100u);
			}
		}

		return score;
	}

	[[nodiscard]] constexpr u32 RatePhysicalDevice_CheckMaxImageDimensions(const VkPhysicalDeviceProperties& devProps)
	{
		// Optionally: Check 2D & cube image max size

		u32 score = 0;

		score += devProps.limits.maxImageDimension2D / 32;
		score += devProps.limits.maxImageDimensionCube / 32;

		return score;
	}

	[[nodiscard]] constexpr u32 RatePhysicalDevice_CheckMaxMSAASampleCount(const VkPhysicalDeviceProperties& devProps)
	{
		// Optionally: Check max supported MSAA sample count

		VkSampleCountFlags sampleCounts = TRAP::Math::Min(devProps.limits.framebufferColorSampleCounts,
														  devProps.limits.framebufferDepthSampleCounts);
		sampleCounts = TRAP::Math::Min(sampleCounts, devProps.limits.framebufferStencilSampleCounts);

		return sampleCounts * 10;
	}

	[[nodiscard]] constexpr u32 RatePhysicalDevice_CheckAnisotropicFiltering(const VkPhysicalDeviceFeatures& devFeatures,
	                                                                         const VkPhysicalDeviceProperties& devProps)
	{
		// Optionally: Check if anisotropic filtering is supported

		if (devFeatures.samplerAnisotropy != 0u)
			return 500;

		TP_WARN(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed anisotropic filtering test!");
		return 0;
	}

	[[nodiscard]] TRAP::Utils::UUID GetPhysicalDeviceUUID(VkPhysicalDevice physicalDevice)
	{
		VkPhysicalDeviceIDPropertiesKHR physicalDeviceIDProperties{};
		physicalDeviceIDProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES;

		VkPhysicalDeviceProperties2 props2{};
		props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		props2.pNext = &physicalDeviceIDProperties;

		vkGetPhysicalDeviceProperties2(physicalDevice, &props2);

		return std::to_array(physicalDeviceIDProperties.deviceUUID);
	}

	[[nodiscard]] TRAP::Optional<std::pair<u32, TRAP::Graphics::API::RatedVulkanPhysicalDevice>> RatePhysicalDevice(VkPhysicalDevice physicalDevice, [[maybe_unused]] VkInstance instance)
	{
		std::pair<u32, TRAP::Graphics::API::RatedVulkanPhysicalDevice> rating{};
		auto& [score, devRating] = rating;

		// Get PhysicalDeviceProperties
		VkPhysicalDeviceProperties devProps{};
		vkGetPhysicalDeviceProperties(physicalDevice, &devProps);

		if(!RatePhysicalDevice_CheckVulkanVersion(devProps))
			return TRAP::NullOpt;

		if(const auto devTypeScore = RatePhysicalDevice_CheckDeviceType(devProps))
			score += *devTypeScore;
		else
			return TRAP::NullOpt;

		if(!RatePhysicalDevice_CheckMaxBoundDescriptorSet(devProps))
			return TRAP::NullOpt;

		const auto [extensionScore, supportedExtensions] = RatePhysicalDevice_RateDeviceExtensions(physicalDevice);
		score += extensionScore;

		if(!RatePhysicalDevice_CheckSPIRV1_4Support(supportedExtensions, devProps))
			return TRAP::NullOpt;

#ifndef TRAP_HEADLESS_MODE
		if(!RatePhysicalDevice_CheckSwapChainSupport(supportedExtensions, devProps))
			return TRAP::NullOpt;

		const auto vulkanTestWindow = RatePhysicalDevice_CreateSurfaceTestWindow(devProps);
		if(vulkanTestWindow.Get() == nullptr)
			return TRAP::NullOpt;

		const auto surface = RatePhysicalDevice_CheckSurfaceSupport(instance, *vulkanTestWindow, devProps);
		if(surface.Get() == VK_NULL_HANDLE)
			return TRAP::NullOpt;
#endif /*TRAP_HEADLESS_MODE*/

		const auto queueFamilyProperties = RatePhysicalDevice_GetQueueFamilies(physicalDevice, devProps);
		if(queueFamilyProperties.empty())
			return TRAP::NullOpt;

		if(!RatePhysicalDevice_CheckGraphicsQueue(queueFamilyProperties, devProps))
			return TRAP::NullOpt;

#ifndef TRAP_HEADLESS_MODE
		if(!RatePhysicalDevice_CheckPresentSupport(queueFamilyProperties, physicalDevice, surface.Get(), devProps))
			return TRAP::NullOpt;

		if(!RatePhysicalDevice_CheckSurfacePresentModes(physicalDevice, surface.Get(), devProps))
			return TRAP::NullOpt;

		const std::vector<VkSurfaceFormatKHR> surfaceFormats = RatePhysicalDevice_CheckSurfaceFormats(physicalDevice, surface.Get(), devProps);
		if(surfaceFormats.empty())
			return TRAP::NullOpt;
#endif /*TRAP_HEADLESS_MODE*/

		score += RatePhysicalDevice_CheckComputeQueue(queueFamilyProperties, devProps);
		score += RatePhysicalDevice_CheckTransferQueue(queueFamilyProperties, devProps);
		score += RatePhysicalDevice_CheckRayTracingSupport(supportedExtensions, devProps);

		// Get PhysicalDevice Features
		VkPhysicalDeviceFeatures devFeatures;
		vkGetPhysicalDeviceFeatures(physicalDevice, &devFeatures);

		score += RatePhysicalDevice_CheckGeometryShaderSupport(devFeatures, devProps);
		score += RatePhysicalDevice_CheckTessellationShaderSupport(devFeatures, devProps);
		score += RatePhysicalDevice_CheckVariableRateShadingSupport(supportedExtensions, physicalDevice, devProps);
		score += RatePhysicalDevice_CheckFillModeNonSolidSupport(devFeatures, devProps);
#ifndef TRAP_HEADLESS_MODE
		score += RatePhysicalDevice_CheckOptimalSurfaceFormat(surfaceFormats, devProps);
#endif /*TRAP_HEADLESS_MODE*/
		score += RatePhysicalDevice_CheckVRAMSize(physicalDevice);
		score += RatePhysicalDevice_CheckMaxImageDimensions(devProps);
		score += RatePhysicalDevice_CheckMaxMSAASampleCount(devProps);
		score += RatePhysicalDevice_CheckAnisotropicFiltering(devFeatures, devProps);

		TRAP::Utils::UUID physicalDeviceUUID = GetPhysicalDeviceUUID(physicalDevice);

		TP_INFO(TRAP::Log::RendererVulkanPrefix, "Found GPU: \"", devProps.deviceName, "\"(", TRAP::Utils::UUIDToString(physicalDeviceUUID), ')', " Score: ", score);

		devRating = TRAP::Graphics::API::RatedVulkanPhysicalDevice
		{
			.PhysicalDeviceUUID = physicalDeviceUUID,
			.PhysicalDeviceName = devProps.deviceName,
		};

		return rating;
	}
}

void TRAP::Graphics::API::VulkanPhysicalDevice::RatePhysicalDevices(const std::span<const VkPhysicalDevice> physicalDevices, VkInstance instance)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	// Score each Physical Device and insert into multimap
	for (VkPhysicalDevice physicalDevice : physicalDevices)
	{
		const auto deviceRating = RatePhysicalDevice(physicalDevice, instance);
		if(deviceRating)
			s_ratedPhysicalDevices.emplace(*deviceRating);
	}
}
