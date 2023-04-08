#include "TRAPPCH.h"
#include "VulkanPhysicalDevice.h"

#include "Application.h"
#include "VulkanInits.h"
#include "VulkanInstance.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Utils/Dialogs/Dialogs.h"
#include <vulkan/vulkan_core.h>

std::multimap<uint32_t, std::array<uint8_t, 16>> TRAP::Graphics::API::VulkanPhysicalDevice::s_availablePhysicalDeviceUUIDs{};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanPhysicalDevice::VulkanPhysicalDevice(const TRAP::Ref<VulkanInstance> &instance,
																const std::array<uint8_t, 16> &physicalDeviceUUID)
	: m_physicalDevice(VK_NULL_HANDLE),
	  m_physicalDeviceProperties(),
	  m_physicalDeviceSubgroupProperties(),
	  m_physicalDeviceIDProperties(),
	  m_physicalDeviceMemoryProperties(),
	  m_physicalDeviceFeatures(),
	  m_physicalDeviceFragmentShaderInterlockFeatures(),
	  m_deviceUUID()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(instance, "VulkanPhysicalDevice(): Vulkan Instance is nullptr");

	m_physicalDevice = FindPhysicalDeviceViaUUID(instance, physicalDeviceUUID);
	TRAP_ASSERT(m_physicalDevice, "VulkanPhysicalDevice(): Vulkan Physical Device is nullptr!");

	if (m_physicalDevice == nullptr)
	{
		Utils::Dialogs::ShowMsgBox("Vulkan API error", "Vulkan: Physical device creation failed!\n"
													   "Error code: 0x0006",
								   Utils::Dialogs::Style::Error,
								   Utils::Dialogs::Buttons::Quit);
		TP_CRITICAL(Log::RendererVulkanPhysicalDevicePrefix, "Vulkan: Physical device creation failed! (0x0006)");
		exit(0x0006);
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

	uint32_t queueFamilyPropertyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyPropertyCount, nullptr);
	m_queueFamilyProperties.resize(queueFamilyPropertyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyPropertyCount,
											 m_queueFamilyProperties.data());

	// Copy UUID
	std::copy_n(m_physicalDeviceIDProperties.deviceUUID, m_deviceUUID.size(), m_deviceUUID.begin());

	// Capabilities for VulkanRenderer
	for (uint32_t i = 0; i < static_cast<uint32_t>(TRAP::Graphics::API::ImageFormat::IMAGE_FORMAT_COUNT); ++i)
	{
		const VkFormat fmt = ImageFormatToVkFormat(static_cast<TRAP::Graphics::API::ImageFormat>(i));
		if (fmt == VK_FORMAT_UNDEFINED)
			continue;

		VkFormatProperties formatSupport{};
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
	RendererAPI::GPUSettings.MultiDrawIndirectSupported = m_physicalDeviceProperties.limits.maxDrawIndirectCount > 1u;
	RendererAPI::GPUSettings.MaxAnisotropy = m_physicalDeviceProperties.limits.maxSamplerAnisotropy;
	RendererAPI::GPUSettings.MaxImageDimension2D = m_physicalDeviceProperties.limits.maxImageDimension2D;
	RendererAPI::GPUSettings.MaxImageDimensionCube = m_physicalDeviceProperties.limits.maxImageDimensionCube;
	RendererAPI::GPUSettings.FillModeNonSolid = (m_physicalDeviceFeatures.fillModeNonSolid != 0u);
	RendererAPI::GPUSettings.MaxPushConstantSize = m_physicalDeviceProperties.limits.maxPushConstantsSize;
	RendererAPI::GPUSettings.MaxSamplerAllocationCount = m_physicalDeviceProperties.limits.maxSamplerAllocationCount;
	RendererAPI::GPUSettings.MaxTessellationControlPoints = m_physicalDeviceProperties.limits.maxTessellationPatchSize;
	RendererAPI::GPUSettings.MaxMSAASampleCount = static_cast<RendererAPI::SampleCount>(TRAP::Math::Min(GetMaxUsableMSAASampleCount(), static_cast<uint32_t>(VK_SAMPLE_COUNT_16_BIT)));

	// maxBoundDescriptorSets not needed because engine is always limited to 4 descriptor sets

	RendererAPI::GPUSettings.WaveLaneCount = m_physicalDeviceSubgroupProperties.subgroupSize;
	RendererAPI::GPUSettings.WaveOpsSupportFlags = RendererAPI::WaveOpsSupportFlags::None;
	if ((m_physicalDeviceSubgroupProperties.supportedOperations & VK_SUBGROUP_FEATURE_BASIC_BIT) != 0u)
		RendererAPI::GPUSettings.WaveOpsSupportFlags |= RendererAPI::WaveOpsSupportFlags::Basic;
	if ((m_physicalDeviceSubgroupProperties.supportedOperations & VK_SUBGROUP_FEATURE_VOTE_BIT) != 0u)
		RendererAPI::GPUSettings.WaveOpsSupportFlags |= RendererAPI::WaveOpsSupportFlags::Vote;
	if ((m_physicalDeviceSubgroupProperties.supportedOperations & VK_SUBGROUP_FEATURE_ARITHMETIC_BIT) != 0u)
		RendererAPI::GPUSettings.WaveOpsSupportFlags |= RendererAPI::WaveOpsSupportFlags::Arithmetic;
	if ((m_physicalDeviceSubgroupProperties.supportedOperations & VK_SUBGROUP_FEATURE_BALLOT_BIT) != 0u)
		RendererAPI::GPUSettings.WaveOpsSupportFlags |= RendererAPI::WaveOpsSupportFlags::Ballot;
	if ((m_physicalDeviceSubgroupProperties.supportedOperations & VK_SUBGROUP_FEATURE_SHUFFLE_BIT) != 0u)
		RendererAPI::GPUSettings.WaveOpsSupportFlags |= RendererAPI::WaveOpsSupportFlags::Shuffle;
	if ((m_physicalDeviceSubgroupProperties.supportedOperations & VK_SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT) != 0u)
		RendererAPI::GPUSettings.WaveOpsSupportFlags |= RendererAPI::WaveOpsSupportFlags::ShuffleRelative;
	if ((m_physicalDeviceSubgroupProperties.supportedOperations & VK_SUBGROUP_FEATURE_CLUSTERED_BIT) != 0u)
		RendererAPI::GPUSettings.WaveOpsSupportFlags |= RendererAPI::WaveOpsSupportFlags::Clustered;
	if ((m_physicalDeviceSubgroupProperties.supportedOperations & VK_SUBGROUP_FEATURE_QUAD_BIT) != 0u)
		RendererAPI::GPUSettings.WaveOpsSupportFlags |= RendererAPI::WaveOpsSupportFlags::Quad;
	if ((m_physicalDeviceSubgroupProperties.supportedOperations & VK_SUBGROUP_FEATURE_PARTITIONED_BIT_NV) != 0u)
		RendererAPI::GPUSettings.WaveOpsSupportFlags |= RendererAPI::WaveOpsSupportFlags::PartitionedNV;

	RendererAPI::GPUSettings.TessellationSupported = (m_physicalDeviceFeatures.tessellationShader != 0u);
	RendererAPI::GPUSettings.GeometryShaderSupported = (m_physicalDeviceFeatures.geometryShader != 0u);
	RendererAPI::GPUSettings.SampleRateShadingSupported = (m_physicalDeviceFeatures.sampleRateShading != 0u);

	// Surface & Present test
#ifndef TRAP_HEADLESS_MODE
	{
		INTERNAL::WindowingAPI::WindowHint(INTERNAL::WindowingAPI::Hint::Visible, false);
		INTERNAL::WindowingAPI::WindowHint(INTERNAL::WindowingAPI::Hint::Focused, false);
		INTERNAL::WindowingAPI::InternalWindow* win = INTERNAL::WindowingAPI::CreateWindow(2, 2, "Vulkan Surface Tester", nullptr);
		INTERNAL::WindowingAPI::DefaultWindowHints();
		if (win != nullptr)
		{
			VkSurfaceKHR surface = VK_NULL_HANDLE;
			VkResult res = TRAP::INTERNAL::WindowingAPI::CreateWindowSurface(instance->GetVkInstance(), *win, nullptr, surface);

			if (surface != VK_NULL_HANDLE && res == VK_SUCCESS)
			{
				// Passed Surface creation
				// Check if Surface contains present modes
				uint32_t surfPresModeCount = 0;
				VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, surface, &surfPresModeCount, nullptr));
				std::vector<VkPresentModeKHR> presModes(surfPresModeCount);
				VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, surface, &surfPresModeCount, presModes.data()));
				if (!presModes.empty())
				{
					// Passed present mode check
					// Check if Surface contains formats
					uint32_t surfFormatCount = 0;
					VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, surface, &surfFormatCount, nullptr));
					std::vector<VkSurfaceFormatKHR> surfFormats(surfFormatCount);
					VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, surface, &surfFormatCount, surfFormats.data()));
					RendererAPI::GPUSettings.SurfaceSupported = !surfFormats.empty(); // Finished Surface format check
				}
				else
					RendererAPI::GPUSettings.SurfaceSupported = false;

				// Present test
				VkBool32 presentSupport = VK_FALSE;
				const auto &queueFam = GetQueueFamilyProperties();
				for (std::size_t i = 0; i < queueFam.size(); ++i)
				{
					VkCall(vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice, static_cast<uint32_t>(i), surface, &presentSupport));
					if (presentSupport != 0u)
					{
						RendererAPI::GPUSettings.PresentSupported = true;
						break;
					}
				}

				// Cleanup
				vkDestroySurfaceKHR(instance->GetVkInstance(), surface, nullptr);
				TRAP::INTERNAL::WindowingAPI::DestroyWindow(win);
			}
			else
			{
				RendererAPI::GPUSettings.SurfaceSupported = false;
				// Cleanup
				TRAP::INTERNAL::WindowingAPI::DestroyWindow(win);
			}
		}
	}
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanPhysicalDevice::~VulkanPhysicalDevice()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanPhysicalDevicePrefix, "Destroying PhysicalDevice");
#endif
	m_physicalDevice = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkPhysicalDevice TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDevice() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_physicalDevice;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkFormatProperties TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceFormatProperties(const VkFormat format) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkFormatProperties formatProps{};

	vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &formatProps);

	return formatProps;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const VkPhysicalDeviceProperties &TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceProperties() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_physicalDeviceProperties;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const VkPhysicalDeviceSubgroupProperties &TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceSubgroupProperties() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_physicalDeviceSubgroupProperties;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const VkPhysicalDeviceIDProperties &TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceIDProperties() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_physicalDeviceIDProperties;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const VkPhysicalDeviceMemoryProperties &TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceMemoryProperties() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_physicalDeviceMemoryProperties;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const VkPhysicalDeviceFeatures &TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceFeatures() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_physicalDeviceFeatures;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT &TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceFragmentShaderInterlockFeatures() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_physicalDeviceFragmentShaderInterlockFeatures;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const std::vector<VkQueueFamilyProperties> &TRAP::Graphics::API::VulkanPhysicalDevice::GetQueueFamilyProperties() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_queueFamilyProperties;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::API::VulkanPhysicalDevice::IsExtensionSupported(const std::string_view extension)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	if (m_availablePhysicalDeviceExtensions.empty())
		LoadAllPhysicalDeviceExtensions();

	const auto result = std::find_if(m_availablePhysicalDeviceExtensions.begin(),
									 m_availablePhysicalDeviceExtensions.end(),
									 [extension](const VkExtensionProperties &props)
									 {
										 return std::strcmp(extension.data(), props.extensionName) == 0;
									 });

	if (result == m_availablePhysicalDeviceExtensions.end())
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

[[nodiscard]] const std::vector<VkExtensionProperties> &TRAP::Graphics::API::VulkanPhysicalDevice::GetAvailablePhysicalDeviceExtensions()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	if (m_availablePhysicalDeviceExtensions.empty())
		LoadAllPhysicalDeviceExtensions();

	return m_availablePhysicalDeviceExtensions;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const std::array<uint8_t, 16> &TRAP::Graphics::API::VulkanPhysicalDevice::GetPhysicalDeviceUUID() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_deviceUUID;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanPhysicalDevice::RetrievePhysicalDeviceFragmentShaderInterlockFeatures()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	if(VulkanRenderer::s_fragmentShaderInterlockExtension)
	{
		VkPhysicalDeviceFeatures2 features2;
		features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		m_physicalDeviceFragmentShaderInterlockFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT;
		m_physicalDeviceFragmentShaderInterlockFeatures.pNext = nullptr;
		features2.pNext = &m_physicalDeviceFragmentShaderInterlockFeatures;
		vkGetPhysicalDeviceFeatures2(m_physicalDevice, &features2);

		RendererAPI::GPUSettings.ROVsSupported = static_cast<uint32_t>(static_cast<bool>(m_physicalDeviceFragmentShaderInterlockFeatures.fragmentShaderPixelInterlock));
	}
	RendererAPI::GPUSettings.ROVsSupported = 0u;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::RendererAPI::GPUVendor TRAP::Graphics::API::VulkanPhysicalDevice::GetVendor() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	switch(m_physicalDeviceProperties.vendorID)
	{
	case 0x1002:
		[[fallthrough]];
	case 0x1010:
		[[fallthrough]];
	case 0x10DE:
		[[fallthrough]];
	case 0x13B5:
		[[fallthrough]];
	case 0x14E4:
		[[fallthrough]];
	case 0x5143:
		[[fallthrough]];
	case 0x8086:
		[[fallthrough]];
	case 0x106B:
		[[fallthrough]];
	case 0x7A05:
		[[fallthrough]];
	case 0x1EB1:
		[[fallthrough]];
	case 0x10003:
		[[fallthrough]];
	case 0x10004:
		[[fallthrough]];
	case 0x10005:
		return static_cast<RendererAPI::GPUVendor>(m_physicalDeviceProperties.vendorID);

	default:
		return RendererAPI::GPUVendor::Unknown;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const std::multimap<uint32_t, std::array<uint8_t, 16>> &TRAP::Graphics::API::VulkanPhysicalDevice::GetAllRatedPhysicalDevices(const TRAP::Ref<VulkanInstance> &instance)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(instance, "VulkanPhysicalDevice::GetAllRatedPhysicalDevices(): Vulkan Instance is nullptr");

	if (!s_availablePhysicalDeviceUUIDs.empty())
		return s_availablePhysicalDeviceUUIDs;

	const std::vector<VkPhysicalDevice> physicalDevices = GetAllVkPhysicalDevices(instance->GetVkInstance());

	if (!physicalDevices.empty())
		RatePhysicalDevices(physicalDevices, instance->GetVkInstance());
	else
	{
		Utils::Dialogs::ShowMsgBox("Vulkan API error", "Vulkan: No capable physical device was found!\n"
													   "Error code: 0x0007",
								   Utils::Dialogs::Style::Error,
								   Utils::Dialogs::Buttons::Quit);
		TP_CRITICAL(Log::RendererVulkanPrefix, "Vulkan: No capable physical device was found! (0x0007)");
		exit(0x0007);
	}

	return s_availablePhysicalDeviceUUIDs;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkPhysicalDevice TRAP::Graphics::API::VulkanPhysicalDevice::FindPhysicalDeviceViaUUID(const TRAP::Ref<VulkanInstance> &instance,
																					  const std::array<uint8_t, 16> &physicalDeviceUUID)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(instance, "VulkanPhysicalDevice::FindPhysicalDeviceViaUUID(): Vulkan Instance is nullptr");

	const auto physicalDevices = GetAllVkPhysicalDevices(instance->GetVkInstance());

	for (const auto &device : physicalDevices)
	{
		std::array<uint8_t, 16> testUUID{};

		VkPhysicalDeviceIDPropertiesKHR physicalDeviceIDProperties;
		VkPhysicalDeviceProperties2 props2;
		props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		props2.pNext = &physicalDeviceIDProperties;
		physicalDeviceIDProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES;
		physicalDeviceIDProperties.pNext = nullptr;
		vkGetPhysicalDeviceProperties2(device, &props2);

		// Copy UUID
		std::copy_n(physicalDeviceIDProperties.deviceUUID, testUUID.size(), testUUID.begin());

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

[[nodiscard]] const std::multimap<uint32_t, std::array<uint8_t, 16>> &TRAP::Graphics::API::VulkanPhysicalDevice::GetAllRatedPhysicalDevices(const VkInstance &instance)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	if (!s_availablePhysicalDeviceUUIDs.empty())
		return s_availablePhysicalDeviceUUIDs;

	const std::vector<VkPhysicalDevice> physicalDevices = GetAllVkPhysicalDevices(instance);

	if (!physicalDevices.empty())
		RatePhysicalDevices(physicalDevices, instance);
	else
	{
		Utils::Dialogs::ShowMsgBox("Vulkan API error", "Vulkan: No capable physical device was found!\n"
													   "Error code: 0x0007",
								   Utils::Dialogs::Style::Error,
								   Utils::Dialogs::Buttons::Quit);
		TP_CRITICAL(Log::RendererVulkanPrefix, "Vulkan: No capable physical device was found! (0x0007)");
		exit(0x0007);
	}

	return s_availablePhysicalDeviceUUIDs;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<VkPhysicalDevice> TRAP::Graphics::API::VulkanPhysicalDevice::GetAllVkPhysicalDevices(const VkInstance &instance)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	uint32_t physicalDeviceCount = 0;
	VkCall(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr));

	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	VkCall(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data()));

	return physicalDevices;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanPhysicalDevice::RatePhysicalDevices(const std::vector<VkPhysicalDevice> &physicalDevices, VkInstance instance)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	// Score each Physical Device and insert into multimap
	uint32_t score = 0;
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
		uint32_t extensionsCount = 0;
		std::vector<VkExtensionProperties> extensions;
		VkCall(vkEnumerateDeviceExtensionProperties(dev, nullptr, &extensionsCount, nullptr));
		extensions.resize(extensionsCount);
		VkCall(vkEnumerateDeviceExtensionProperties(dev, nullptr, &extensionsCount, extensions.data()));
		score += extensionsCount * 50;

		// Required: Check if PhysicalDevice supports swapchains
		// Disabled in Headless mode.
#ifndef TRAP_HEADLESS_MODE
		const auto result = std::find_if(extensions.begin(), extensions.end(), [](const VkExtensionProperties &props)
										 { return std::strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, props.extensionName) == 0; });

		if (result == extensions.end())
		{
			TP_ERROR(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
					 "\" Failed Required PhysicalDevice Extensions Test!");
			continue;
		}
#endif

		// Required: Create Vulkan Instance

		// Init WindowingAPI needed here for instance extensions
		// Disabled in Headless mode.
#ifndef TRAP_HEADLESS_MODE
		if (!INTERNAL::WindowingAPI::Init())
		{
			Utils::Dialogs::ShowMsgBox("Failed to initialize WindowingAPI", "The WindowingAPI couldn't be initialized!\n"
								       "Error code: 0x0011", Utils::Dialogs::Style::Error,
								       Utils::Dialogs::Buttons::Quit);
			TP_CRITICAL(Log::RendererVulkanPrefix, "The WindowingAPI couldn't be initialized! (0x0011)");
			exit(0x0011);
		}
#endif

		// Required: Create Vulkan Surface Test Window
		// Disabled in Headless mode.
#ifndef TRAP_HEADLESS_MODE
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
#endif

		// Required: Check if Surface can be created
		// Disabled in Headless mode
#ifndef TRAP_HEADLESS_MODE
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
#endif

		// Required: Get Queue Families
		uint32_t queueFamilyPropertyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueFamilyPropertyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueFamilyPropertyCount, queueFamilyProperties.data());
		if (queueFamilyProperties.empty())
		{
#ifndef TRAP_HEADLESS_MODE
			vkDestroySurfaceKHR(instance, surface, nullptr);
			TRAP::INTERNAL::WindowingAPI::DestroyWindow(vulkanTestWindow);
#endif
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
#endif
			vkDestroyInstance(instance, nullptr);
			TP_ERROR(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" Failed Graphics Queue Test!");
			continue;
		}

		// Required: Check if PhysicalDevice supports Presenting
		// Disabled in Headless mode.
#ifndef TRAP_HEADLESS_MODE
		VkBool32 foundPresentSupport = VK_FALSE;
		for (std::size_t i = 0; i < queueFamilyProperties.size(); i++)
		{
			VkCall(vkGetPhysicalDeviceSurfaceSupportKHR(dev, static_cast<uint32_t>(i), surface, &foundPresentSupport));
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
#endif

		// Required: Check if Surface contains present modes
		// Disabled in Headless mode.
#ifndef TRAP_HEADLESS_MODE
		uint32_t surfacePresentModeCount = 0;
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
#endif

		// Required: Check if Surface contains formats
		// Disabled in Headless mode.
#ifndef TRAP_HEADLESS_MODE
		uint32_t surfaceFormatCount = 0;
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
#endif

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
		for (const std::string_view str : raytracingExt)
		{
			const auto extRes = std::find_if(extensions.begin(), extensions.end(),
											 [str](const VkExtensionProperties &props)
											 {
												 return str.compare(props.extensionName) == 0;
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
		const std::array<std::string_view, 2> VRSExt =
		{
			VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
			VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME
		};
		for (const std::string_view str : VRSExt)
		{
			const auto extRes = std::find_if(extensions.begin(), extensions.end(),
											 [str](const VkExtensionProperties &props)
											 {
												 return str.compare(props.extensionName) == 0;
											 });

			if (extRes == extensions.end())
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
#endif

#ifndef TRAP_HEADLESS_MODE
		vkDestroySurfaceKHR(instance, surface, nullptr);
		TRAP::INTERNAL::WindowingAPI::DestroyWindow(vulkanTestWindow);
#endif

		// Optionally: Check VRAM size (1e+9 == Bytes to Gigabytes)
		// Get PhysicalDevice Memory Properties
		VkPhysicalDeviceMemoryProperties memProps;
		vkGetPhysicalDeviceMemoryProperties(dev, &memProps);
		for (uint32_t i = 0; i < memProps.memoryHeapCount; i++)
		{
			if ((VK_MEMORY_HEAP_DEVICE_LOCAL_BIT & memProps.memoryHeaps[i].flags) != 0u)
				score += static_cast<uint32_t>(memProps.memoryHeaps[i].size) / static_cast<uint32_t>(1e+9) * 100u;
		}

		// Optionally: Check 2D & Cube Image Max Size
		score += devProps.limits.maxImageDimension2D;
		score += devProps.limits.maxImageDimensionCube;

		// Optionally: Check max supported MSAA sample count
		VkSampleCountFlags sampleCounts = TRAP::Math::Min(devProps.limits.framebufferColorSampleCounts,
														  devProps.limits.framebufferDepthSampleCounts);
		sampleCounts = TRAP::Math::Min(sampleCounts, devProps.limits.framebufferStencilSampleCounts);
		score += static_cast<uint32_t>(sampleCounts) * 10;

		// Optionally: Check if Anisotropic Filtering is supported
		if (devFeatures.samplerAnisotropy != 0u)
			score += 500;
		else
			TP_WARN(Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
					"\" Failed Anisotropic Filtering Test!");

		// Get PhysicalDevice UUID
		std::array<uint8_t, 16> physicalDeviceUUID{};

		VkPhysicalDeviceIDPropertiesKHR physicalDeviceIDProperties;
		VkPhysicalDeviceProperties2 props2;
		props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		props2.pNext = &physicalDeviceIDProperties;
		physicalDeviceIDProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES;
		physicalDeviceIDProperties.pNext = nullptr;
		vkGetPhysicalDeviceProperties2(dev, &props2);

		// Copy UUID
		std::copy_n(physicalDeviceIDProperties.deviceUUID, physicalDeviceUUID.size(), physicalDeviceUUID.begin());

		TP_INFO(Log::RendererVulkanPrefix, "Found GPU: \"", devProps.deviceName, '(', TRAP::Utils::UUIDToString(physicalDeviceUUID), ')', "\" Score: ", score);
		s_availablePhysicalDeviceUUIDs.emplace(score, physicalDeviceUUID);
		score = 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanPhysicalDevice::LoadAllPhysicalDeviceExtensions()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	uint32_t extensionsCount = 0;
	VkCall(vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &extensionsCount, nullptr));
	m_availablePhysicalDeviceExtensions.resize(extensionsCount);
	VkCall(vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &extensionsCount,
												m_availablePhysicalDeviceExtensions.data()));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::API::VulkanPhysicalDevice::GetMaxUsableMSAASampleCount() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	VkSampleCountFlags sampleCounts = TRAP::Math::Min(m_physicalDeviceProperties.limits.framebufferColorSampleCounts,
	                                                  m_physicalDeviceProperties.limits.framebufferDepthSampleCounts);
	sampleCounts = TRAP::Math::Min(sampleCounts, m_physicalDeviceProperties.limits.framebufferStencilSampleCounts);

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
