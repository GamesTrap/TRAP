#include "TRAPPCH.h"
#include "VulkanDevice.h"

#include "Utils/String/String.h"
#include "VulkanPhysicalDevice.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Graphics/API/Objects/AftermathTracker.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/Objects/VulkanSemaphore.h"

namespace
{
#ifdef VERBOSE_GRAPHICS_DEBUG
	void DebugPrintActiveDeviceExtensions(const std::span<const std::string> deviceExtensions,
	                                      const TRAP::Graphics::API::VulkanPhysicalDevice& physicalDevice)
	{
		if (deviceExtensions.empty())
			return;

		TP_DEBUG(TRAP::Log::RendererVulkanDevicePrefix, "Loading Device Extension(s):");
		for (const std::string_view str : deviceExtensions)
		{
			const auto extensionProps = physicalDevice.GetPhysicalDeviceExtensionProperties(str);
			if(extensionProps)
			{
				TP_DEBUG(TRAP::Log::RendererVulkanDevicePrefix, "    ", str, " Spec: ",
						VK_API_VERSION_MAJOR(extensionProps->specVersion), '.', VK_API_VERSION_MINOR(extensionProps->specVersion), '.',
						VK_API_VERSION_PATCH(extensionProps->specVersion), '.', VK_API_VERSION_VARIANT(extensionProps->specVersion));
			}
			else
				TP_DEBUG(TRAP::Log::RendererVulkanDevicePrefix, "    ", str);
		}
	}
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	void EnableNsightAftermath(const TRAP::Graphics::API::VulkanPhysicalDevice& physicaldevice,
	                           VkBaseOutStructure*& base,
							   VkDeviceDiagnosticsConfigCreateInfoNV& diagnosticsCreateInfoNV)
	{
		if(!physicaldevice.IsFeatureEnabled(TRAP::Graphics::API::VulkanPhysicalDeviceFeature::NsightAftermath))
			return;

		TRAP::Graphics::API::LinkVulkanStruct(base, diagnosticsCreateInfoNV);
		//Enable Nsight Aftermath GPU crash dump creation.
		//This needs to be done before the Vulkan device is created.
		TRAP::Graphics::AftermathTracker::Initialize();
	}

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
	void InitNVIDIAReflex(VkSemaphore& reflexSemaphore, const TRAP::Graphics::RendererAPI::GPUVendor gpuVendor, VkDevice device)
	{
		if (gpuVendor == TRAP::Graphics::RendererAPI::GPUVendor::NVIDIA &&
			TRAP::Graphics::API::VulkanPhysicalDevice::m_deviceExtensions.TimelineSemaphore &&
			TRAP::Utils::IsWindows10BuildOrGreaterWin32(10240))
		{
			const NvLL_VK_Status status = NvLL_VK_InitLowLatencyDevice(device, reinterpret_cast<HANDLE*>(&reflexSemaphore));
			VkReflexCall(status);
			if(status == NVLL_VK_OK)
				TRAP::Graphics::RendererAPI::GPUSettings.ReflexSupported = true;
		}
	}
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanDevice::VulkanDevice(TRAP::Scope<VulkanPhysicalDevice> physicalDevice,
                                                std::vector<std::string> deviceExtensions)
	: m_physicalDevice(std::move(physicalDevice)), m_deviceExtensions(std::move(deviceExtensions))
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_physicalDevice, "VulkanDevice(): Vulkan PhysicalDevice is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Creating Device");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	//Remove all extensions that are unsupported by this device
	std::erase_if(m_deviceExtensions, [this](const std::string_view extension){return !m_physicalDevice->IsExtensionSupported(extension);});

#ifdef VERBOSE_GRAPHICS_DEBUG
	DebugPrintActiveDeviceExtensions(m_deviceExtensions, *m_physicalDevice);
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	//Enable features of the device extensions
	VkPhysicalDeviceFeatures2 devFeatures2{};
	devFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

	VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT fragmentShaderInterlockFeatures{};
	fragmentShaderInterlockFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT;
	VkPhysicalDeviceDescriptorIndexingFeaturesEXT descriptorIndexingFeatures{};
	descriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
	VkPhysicalDeviceSamplerYcbcrConversionFeatures ycbcrFeatures{};
	ycbcrFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES;
	VkPhysicalDeviceShaderDrawParametersFeatures shaderDrawParametersFeatures{};
	shaderDrawParametersFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;
	VkPhysicalDeviceBufferDeviceAddressFeaturesKHR bufferDeviceAddressFeatures{};
	bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR;
	VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingPipelineFeatures{};
	rayTracingPipelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
	VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures{};
	accelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
	VkPhysicalDeviceRayQueryFeaturesKHR rayQueryFeatures{};
	rayQueryFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR;
	VkPhysicalDeviceFragmentShadingRateFeaturesKHR shadingRateFeatures{};
	shadingRateFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;
	VkPhysicalDeviceTimelineSemaphoreFeaturesKHR timelineSemaphoreFeatures{};
	timelineSemaphoreFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR;
	VkDeviceDiagnosticsConfigCreateInfoNV diagnosticsCreateInfoNV{};
	diagnosticsCreateInfoNV.sType = VK_STRUCTURE_TYPE_DEVICE_DIAGNOSTICS_CONFIG_CREATE_INFO_NV;
	diagnosticsCreateInfoNV.flags = VK_DEVICE_DIAGNOSTICS_CONFIG_ENABLE_SHADER_DEBUG_INFO_BIT_NV |
			                        VK_DEVICE_DIAGNOSTICS_CONFIG_ENABLE_RESOURCE_TRACKING_BIT_NV |
								    VK_DEVICE_DIAGNOSTICS_CONFIG_ENABLE_AUTOMATIC_CHECKPOINTS_BIT_NV;

	VkBaseOutStructure* base = reinterpret_cast<VkBaseOutStructure*>(&devFeatures2);

	if(m_physicalDevice->IsExtensionSupported(VulkanPhysicalDeviceExtension::FragmentShaderInterlock))
		TRAP::Graphics::API::LinkVulkanStruct(base, fragmentShaderInterlockFeatures);
	if (m_physicalDevice->IsExtensionSupported(VulkanPhysicalDeviceExtension::DescriptorIndexing))
		TRAP::Graphics::API::LinkVulkanStruct(base, descriptorIndexingFeatures);
	TRAP::Graphics::API::LinkVulkanStruct(base, ycbcrFeatures);
	TRAP::Graphics::API::LinkVulkanStruct(base, shaderDrawParametersFeatures);
	//RayTracing
	if (m_physicalDevice->IsExtensionSupported(VulkanPhysicalDeviceExtension::BufferDeviceAddress))
		TRAP::Graphics::API::LinkVulkanStruct(base, bufferDeviceAddressFeatures);
	if (m_physicalDevice->IsExtensionSupported(VulkanPhysicalDeviceExtension::RayTracing))
	{
		TRAP::Graphics::API::LinkVulkanStruct(base, rayTracingPipelineFeatures);
		TRAP::Graphics::API::LinkVulkanStruct(base, accelerationStructureFeatures);
		TRAP::Graphics::API::LinkVulkanStruct(base, rayQueryFeatures);
	}
	//Shading rate
	if(m_physicalDevice->IsExtensionSupported(VulkanPhysicalDeviceExtension::ShadingRate))
		TRAP::Graphics::API::LinkVulkanStruct(base, shadingRateFeatures);
	//Timeline semaphore
	if(m_physicalDevice->IsExtensionSupported(VulkanPhysicalDeviceExtension::TimelineSemaphore))
		TRAP::Graphics::API::LinkVulkanStruct(base, timelineSemaphoreFeatures);

	vkGetPhysicalDeviceFeatures2(m_physicalDevice->GetVkPhysicalDevice(), &devFeatures2);

	//Need a queuePriority for each queue in the queue family we create
	const std::vector<VkQueueFamilyProperties>& queueFamilyProperties = m_physicalDevice->GetQueueFamilyProperties();
	std::vector<std::vector<f32>> queueFamilyPriorities(queueFamilyProperties.size());
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};

	for(usize i = 0; i < queueFamilyProperties.size(); ++i)
	{
		const auto& queueFamilyProps = queueFamilyProperties[i];

		queueFamilyPriorities[i].resize(queueFamilyProps.queueCount);

		const VkDeviceQueueCreateInfo info
		{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.queueFamilyIndex = NumericCast<u32>(i),
			.queueCount = queueFamilyProps.queueCount,
			.pQueuePriorities = queueFamilyPriorities[i].data()
		};
		queueCreateInfos.push_back(info);

		m_availableQueueCount[queueFamilyProps.queueFlags] = queueFamilyProps.queueCount;
	}

	EnableNsightAftermath(*m_physicalDevice, base, diagnosticsCreateInfoNV);

	std::vector<const char*> extensions(m_deviceExtensions.size());
	std::ranges::transform(m_deviceExtensions, extensions.begin(), [](const std::string_view ext){return ext.data();});

	const VkDeviceCreateInfo deviceCreateInfo = VulkanInits::DeviceCreateInfo(queueCreateInfos, extensions,
	                                                                          &devFeatures2);

	VkCall(vkCreateDevice(m_physicalDevice->GetVkPhysicalDevice(), &deviceCreateInfo, nullptr, &m_device));
	TRAP_ASSERT(m_device, "VulkanDevice(): No Vulkan device!");

	VkLoadDevice(m_device);

	FindQueueFamilyIndices();

#ifdef ENABLE_GRAPHICS_DEBUG
	if (m_physicalDevice->GetVkPhysicalDeviceProperties().deviceName[0] != '\0')
		TRAP::Graphics::API::VkSetObjectName(*this, std::bit_cast<u64>(m_device), VK_OBJECT_TYPE_DEVICE, m_physicalDevice->GetVkPhysicalDeviceProperties().deviceName);
#endif /*ENABLE_GRAPHICS_DEBUG*/

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
	InitNVIDIAReflex(m_reflexSemaphore, m_physicalDevice->GetVendor(), m_device);
#endif /*defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)*/
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanDevice::~VulkanDevice()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Destroying Device");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
	if(RendererAPI::GPUSettings.ReflexSupported)
		vkDestroySemaphore(m_device, m_reflexSemaphore, nullptr);
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/

	if(m_device != VK_NULL_HANDLE)
		vkDestroyDevice(m_device, nullptr);

#ifdef ENABLE_NSIGHT_AFTERMATH
	if(m_physicalDevice->IsFeatureEnabled(VulkanPhysicalDeviceFeature::NsightAftermath))
		TRAP::Graphics::AftermathTracker::Shutdown();
#endif /*ENABLE_NSIGHT_AFTERMATH*/
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const TRAP::Graphics::API::VulkanPhysicalDevice& TRAP::Graphics::API::VulkanDevice::GetPhysicalDevice() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return *m_physicalDevice;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanDevice::FindQueueFamilyIndices()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	[[maybe_unused]] VkQueueFamilyProperties qfp{};

	FindQueueFamilyIndex(RendererAPI::QueueType::Graphics, qfp, m_graphicsQueueFamilyIndex, m_graphicsQueueIndex);
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Using Graphics Queue Family Index ",
	         NumericCast<u32>(m_graphicsQueueFamilyIndex));
#endif /*VERBOSE_GRAPHICS_DEBUG*/
	FindQueueFamilyIndex(RendererAPI::QueueType::Compute, qfp, m_computeQueueFamilyIndex, m_computeQueueIndex);
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Using Compute Queue Family Index ",
	         NumericCast<u32>(m_computeQueueFamilyIndex));
#endif /*VERBOSE_GRAPHICS_DEBUG*/
	FindQueueFamilyIndex(RendererAPI::QueueType::Transfer, qfp, m_transferQueueFamilyIndex, m_transferQueueIndex);
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Using Transfer Queue Family Index ",
	         NumericCast<u32>(m_transferQueueFamilyIndex));
#endif /*VERBOSE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanDevice::WaitIdle() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	VkCall(vkDeviceWaitIdle(m_device));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanDevice::FindQueueFamilyIndex(const RendererAPI::QueueType queueType,
                                                             VkQueueFamilyProperties& queueFamilyProperties,
															 u8& queueFamilyIndex, u8& queueIndex)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	u32 qfi = std::numeric_limits<u32>::max();
	u32 qi = std::numeric_limits<u32>::max();
	const VkQueueFlags requiredFlags = QueueTypeToVkQueueFlags(queueType);
	bool found = false;

	const std::vector<VkQueueFamilyProperties>& props = m_physicalDevice->GetQueueFamilyProperties();

	u32 minQueueFlag = std::numeric_limits<u32>::max();

	//Try to find a dedicated queue of this type
	for (u32 index = 0; index < props.size(); ++index)
	{
		const VkQueueFlags queueFlags = props[index].queueFlags;
		const bool graphicsQueue = (queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0;
		const u32 flagAnd = (queueFlags & requiredFlags);
		//Usually only one graphics queue so choose the first one with graphics bit
		if (queueType == RendererAPI::QueueType::Graphics && graphicsQueue)
		{
			found = true;
			qfi = index;
			qi = 0;
			break;
		}
		//The only flag set on this queue family is the one required - Most optimal choice
		//Example: Required flag is VK_QUEUE_COMPUTE_BIT and the queue family has only VK_QUEUE_COMPUTE_BIT set.
		if (((queueFlags & requiredFlags) != 0u) && ((queueFlags & ~requiredFlags) == 0) &&
			m_usedQueueCount[queueFlags] < m_availableQueueCount[queueFlags])
		{
			found = true;
			qfi = index;
			qi = m_usedQueueCount[queueFlags];
			break;
		}
		//Queue family has flags other than the required flag(s) - Choose the family with the least number of other flags.
		//Example: Required flag is VK_QUEUE_GRAPHICS_BIT
		//Two queue families available:
		// - Queue family 1 has VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT
		// - Queue family 2 has VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT
		// Since queue family 1 has less flags, we choose queue family 1.
		if ((flagAnd != 0u) && ((queueFlags - flagAnd) < minQueueFlag) && !graphicsQueue &&
			m_usedQueueCount[queueFlags] < m_availableQueueCount[queueFlags])
		{
			found = true;
			minQueueFlag = (queueFlags - flagAnd);
			qfi = index;
			qi = m_usedQueueCount[queueFlags];
		}
	}

	//If hardware doesn't provide a dedicated queue try to find a non-dedicated one
	if (!found)
	{
		for (u32 index = 0; index < props.size(); ++index)
		{
			const VkQueueFlags queueFlags = props[index].queueFlags;
			if (((queueFlags & requiredFlags) != 0u) && m_usedQueueCount[queueFlags] < m_availableQueueCount[queueFlags])
			{
				found = true;
				qfi = index;
				qi = m_usedQueueCount[queueFlags];
				break;
			}
		}
	}

	if (!found)
	{
		qfi = 0;
		qi = 0;

		TP_WARN(Log::RendererVulkanDevicePrefix, "Could not find queue of type ", queueType, '(',
		        std::to_underlying(queueType), "). Using default queue");
	}

	queueFamilyProperties = props[qfi];
	queueFamilyIndex = NumericCast<u8>(qfi);
	queueIndex = NumericCast<u8>(qi);
}
