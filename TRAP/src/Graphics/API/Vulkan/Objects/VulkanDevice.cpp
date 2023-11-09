#include "TRAPPCH.h"
#include "VulkanDevice.h"

#include "Utils/String/String.h"
#include "VulkanPhysicalDevice.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Graphics/API/Objects/AftermathTracker.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/Objects/VulkanSemaphore.h"

TRAP::Graphics::API::VulkanDevice::VulkanDevice(TRAP::Scope<VulkanPhysicalDevice> physicalDevice,
                                                std::vector<std::string> deviceExtensions)
	: m_physicalDevice(std::move(physicalDevice)),
      m_deviceExtensions(std::move(deviceExtensions))
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(m_physicalDevice, "VulkanDevice(): Vulkan PhysicalDevice is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Creating Device");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	//Remove all extensions unsupported by this device
	std::erase_if(m_deviceExtensions, [this](const std::string_view extension){return !m_physicalDevice->IsExtensionSupported(extension);});

	std::vector<const char*> extensions(m_deviceExtensions.size());
	for(usize i = 0; i < m_deviceExtensions.size(); ++i)
		extensions[i] = m_deviceExtensions[i].c_str();

#ifdef VERBOSE_GRAPHICS_DEBUG
	if (!m_deviceExtensions.empty())
	{
		TP_DEBUG(Log::RendererVulkanDevicePrefix, "Loading Device Extension(s):");
		for (const std::string_view str : m_deviceExtensions)
		{
			const auto extensionProps = m_physicalDevice->GetPhysicalDeviceExtensionProperties(str);
			if(extensionProps)
			{
				TP_DEBUG(Log::RendererVulkanDevicePrefix, "    ", str, " Spec: ",
				         VK_API_VERSION_MAJOR(extensionProps->specVersion), '.', VK_API_VERSION_MINOR(extensionProps->specVersion), '.',
						 VK_API_VERSION_PATCH(extensionProps->specVersion), '.', VK_API_VERSION_VARIANT(extensionProps->specVersion));
			}
			else
				TP_DEBUG(Log::RendererVulkanDevicePrefix, "    ", str);
		}
	}
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	if(std::ranges::contains(m_deviceExtensions, VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME))
		m_physicalDevice->RetrievePhysicalDeviceFragmentShaderInterlockFeatures();

	//Enable features of the device extensions
	VkPhysicalDeviceFeatures2 devFeatures2{};
	devFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	VkBaseOutStructure* base = reinterpret_cast<VkBaseOutStructure*>(&devFeatures2);

	VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT fragmentShaderInterlockFeatures{};
	fragmentShaderInterlockFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT;
	VkPhysicalDeviceDescriptorIndexingFeaturesEXT descriptorIndexingFeatures{};
	descriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
	VkPhysicalDeviceSamplerYcbcrConversionFeatures ycbcrFeatures{};
	ycbcrFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES;
	VkPhysicalDeviceShaderDrawParametersFeatures shaderDrawParametersFeatures{};
	shaderDrawParametersFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;

	if(VulkanRenderer::s_fragmentShaderInterlockExtension)
	{
		base->pNext = reinterpret_cast<VkBaseOutStructure*>(&fragmentShaderInterlockFeatures);
		base = base->pNext;
	}
	if (VulkanRenderer::s_descriptorIndexingExtension)
	{
		base->pNext = reinterpret_cast<VkBaseOutStructure*>(&descriptorIndexingFeatures);
		base = base->pNext;
	}
	if (VulkanRenderer::s_samplerYcbcrConversionExtension)
	{
		base->pNext = reinterpret_cast<VkBaseOutStructure*>(&ycbcrFeatures);
		base = base->pNext;
	}
	base->pNext = reinterpret_cast<VkBaseOutStructure*>(&shaderDrawParametersFeatures);
	base = base->pNext;

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

	if (VulkanRenderer::s_bufferDeviceAddressExtension)
	{
		base->pNext = reinterpret_cast<VkBaseOutStructure*>(&bufferDeviceAddressFeatures);
		base = base->pNext;
	}
	if (VulkanRenderer::s_rayTracingExtension)
	{
		base->pNext = reinterpret_cast<VkBaseOutStructure*>(&rayTracingPipelineFeatures);
		base = base->pNext;
	}
	if (VulkanRenderer::s_rayTracingExtension)
	{
		base->pNext = reinterpret_cast<VkBaseOutStructure*>(&accelerationStructureFeatures);
		base = base->pNext;
	}
	if (VulkanRenderer::s_rayTracingExtension)
	{
		base->pNext = reinterpret_cast<VkBaseOutStructure*>(&rayQueryFeatures);
		base = base->pNext;
	}

	//Shading rate
	if(VulkanRenderer::s_shadingRate)
	{
		base->pNext = reinterpret_cast<VkBaseOutStructure*>(&shadingRateFeatures);
		base = base->pNext;
	}

	//Timeline semaphore
	if(VulkanRenderer::s_timelineSemaphore)
	{
		base->pNext = reinterpret_cast<VkBaseOutStructure*>(&timelineSemaphoreFeatures);
		base = base->pNext;
	}

	vkGetPhysicalDeviceFeatures2(m_physicalDevice->GetVkPhysicalDevice(), &devFeatures2);

	//Need a queuePriority for each queue in the queue family we create
	const std::vector<VkQueueFamilyProperties>& queueFamilyProperties = m_physicalDevice->GetQueueFamilyProperties();
	std::vector<std::vector<f32>> queueFamilyPriorities(queueFamilyProperties.size());
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};

	for(usize i = 0; i < queueFamilyProperties.size(); i++)
	{
		u32 queueCount = queueFamilyProperties[i].queueCount;
		if(queueCount > 0)
		{
			queueFamilyPriorities[i].resize(queueCount);

			VkDeviceQueueCreateInfo info;
			info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			info.pNext = nullptr;
			info.flags = 0;
			info.queueFamilyIndex = NumericCast<u32>(i);
			info.queueCount = queueCount;
			info.pQueuePriorities = queueFamilyPriorities[i].data();
			queueCreateInfos.push_back(info);

			m_availableQueueCount[queueFamilyProperties[i].queueFlags] = queueCount;
		}
	}

#ifdef ENABLE_NSIGHT_AFTERMATH
	if(RendererAPI::s_diagnosticCheckPointsSupport && RendererAPI::s_diagnosticsConfigSupport)
		RendererAPI::s_aftermathSupport = true;

	VkDeviceDiagnosticsConfigCreateInfoNV diagnosticsCreateInfo{};
	if(RendererAPI::s_aftermathSupport)
	{
		diagnosticsCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_DIAGNOSTICS_CONFIG_CREATE_INFO_NV;
		diagnosticsCreateInfo.flags = VK_DEVICE_DIAGNOSTICS_CONFIG_ENABLE_SHADER_DEBUG_INFO_BIT_NV |
			                          VK_DEVICE_DIAGNOSTICS_CONFIG_ENABLE_RESOURCE_TRACKING_BIT_NV |
									  VK_DEVICE_DIAGNOSTICS_CONFIG_ENABLE_AUTOMATIC_CHECKPOINTS_BIT_NV;
		diagnosticsCreateInfo.pNext = devFeatures2.pNext;
		devFeatures2.pNext = &diagnosticsCreateInfo;
		//Enable Nsight Aftermath GPU crash dump creation.
		//This needs to be done before the Vulkan device is created.
		TRAP::Graphics::AftermathTracker::Initialize();
	}
#endif /*ENABLE_NSIGHT_AFTERMATH*/

	const VkDeviceCreateInfo deviceCreateInfo = VulkanInits::DeviceCreateInfo(&devFeatures2, queueCreateInfos,
	                                                                          extensions);

	VkCall(vkCreateDevice(m_physicalDevice->GetVkPhysicalDevice(), &deviceCreateInfo, nullptr, &m_device));
	TRAP_ASSERT(m_device, "VulkanDevice(): No Vulkan device!");

	VkLoadDevice(m_device);

	VulkanRenderer::s_debugMarkerSupport = ((vkCmdBeginDebugUtilsLabelEXT) != nullptr) && ((vkCmdEndDebugUtilsLabelEXT) != nullptr) &&
		                                   ((vkCmdInsertDebugUtilsLabelEXT) != nullptr) && ((vkSetDebugUtilsObjectNameEXT) != nullptr);
	VulkanRenderer::s_bufferDeviceAddressExtension = (bufferDeviceAddressFeatures.bufferDeviceAddress != 0u);
	VulkanRenderer::s_samplerYcbcrConversionExtension = (ycbcrFeatures.samplerYcbcrConversion != 0u);
	VulkanRenderer::s_shaderDrawParameters = (shaderDrawParametersFeatures.shaderDrawParameters != 0u);
	VulkanRenderer::s_timelineSemaphore = (timelineSemaphoreFeatures.timelineSemaphore != 0u);
	LoadShadingRateCaps(shadingRateFeatures);

#ifdef ENABLE_GRAPHICS_DEBUG
	if (m_physicalDevice->GetVkPhysicalDeviceProperties().deviceName[0] != '\0')
		SetDeviceName(m_physicalDevice->GetVkPhysicalDeviceProperties().deviceName);
#endif /*ENABLE_GRAPHICS_DEBUG*/
#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
	m_reflexSemaphore = {};
	if (m_physicalDevice->GetVendor() == RendererAPI::GPUVendor::NVIDIA &&
	    VulkanRenderer::s_timelineSemaphore &&
		TRAP::Utils::IsWindows10BuildOrGreaterWin32(10240))
	{
		TP_WARN(Log::RendererVulkanDevicePrefix, "The following VkSemaphore error comes from NVIDIA Reflex and can be ignored");
		const NvLL_VK_Status status = NvLL_VK_InitLowLatencyDevice(m_device, reinterpret_cast<HANDLE*>(&m_reflexSemaphore));
		VkReflexCall(status);
		if(status == NVLL_VK_OK)
			RendererAPI::GPUSettings.ReflexSupported = true;
	}
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanDevice::~VulkanDevice()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Destroying Device");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
	if(RendererAPI::GPUSettings.ReflexSupported)
		vkDestroySemaphore(m_device, m_reflexSemaphore, nullptr);
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/

	vkDestroyDevice(m_device, nullptr);

#ifdef ENABLE_NSIGHT_AFTERMATH
	if(RendererAPI::s_aftermathSupport)
		TRAP::Graphics::AftermathTracker::Shutdown();
#endif /*ENABLE_NSIGHT_AFTERMATH*/
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::API::VulkanPhysicalDevice* TRAP::Graphics::API::VulkanDevice::GetPhysicalDevice() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_physicalDevice.get();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanDevice::FindQueueFamilyIndices()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	FindQueueFamilyIndex(RendererAPI::QueueType::Graphics, m_graphicsQueueFamilyIndex, m_graphicsQueueIndex);
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Using Graphics Queue Family Index ",
	         NumericCast<u32>(m_graphicsQueueFamilyIndex));
#endif /*VERBOSE_GRAPHICS_DEBUG*/
	FindQueueFamilyIndex(RendererAPI::QueueType::Compute, m_computeQueueFamilyIndex, m_computeQueueIndex);
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Using Compute Queue Family Index ",
	         NumericCast<u32>(m_computeQueueFamilyIndex));
#endif /*VERBOSE_GRAPHICS_DEBUG*/
	FindQueueFamilyIndex(RendererAPI::QueueType::Transfer, m_transferQueueFamilyIndex, m_transferQueueIndex);
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Using Transfer Queue Family Index ",
	         NumericCast<u32>(m_transferQueueFamilyIndex));
#endif /*VERBOSE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanDevice::WaitIdle() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkCall(vkDeviceWaitIdle(m_device));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanDevice::FindQueueFamilyIndex(const RendererAPI::QueueType queueType,
                                                             u8& queueFamilyIndex, u8& queueIndex)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	u32 qfi = std::numeric_limits<u32>::max();
	u32 qi = std::numeric_limits<u32>::max();
	const VkQueueFlags requiredFlags = QueueTypeToVkQueueFlags(queueType);
	bool found = false;

	const std::vector<VkQueueFamilyProperties>& props = m_physicalDevice->GetQueueFamilyProperties();

	u32 minQueueFlag = std::numeric_limits<u32>::max();

	//Try to find a dedicated queue of this type
	for(usize index = 0; index < props.size(); ++index)
	{
		const VkQueueFlags queueFlags = props[index].queueFlags;
		const bool graphicsQueue = (queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0;
		const u32 flagAnd = (queueFlags & requiredFlags);
		if(queueType == RendererAPI::QueueType::Graphics && graphicsQueue)
		{
			found = true;
			qfi = NumericCast<u32>(index);
			qi = 0u;
			break;
		}
		if(((queueFlags & requiredFlags) != 0u) && ((queueFlags & ~requiredFlags) == 0) &&
			m_usedQueueCount[queueFlags] < m_availableQueueCount[queueFlags])
		{
			found = true;
			qfi = NumericCast<u32>(index);
			qi = m_usedQueueCount[queueFlags];
			break;
		}
		if((flagAnd != 0u) && ((queueFlags - flagAnd) < minQueueFlag) && !graphicsQueue &&
			m_usedQueueCount[queueFlags] < m_availableQueueCount[queueFlags])
		{
			found = true;
			minQueueFlag = (queueFlags - flagAnd);
			qfi = NumericCast<u32>(index);
			qi = m_usedQueueCount[queueFlags];
			break;
		}
	}

	//If hardware doesn't provide a dedicated queue try to find a non-dedicated one
	if(!found)
	{
		for(usize index = 0; index < props.size(); ++index)
		{
			const VkQueueFlags queueFlags = props[index].queueFlags;
			if(((queueFlags & requiredFlags) != 0u) && m_usedQueueCount[queueFlags] < m_availableQueueCount[queueFlags])
			{
				found = true;
				qfi = NumericCast<u32>(index);
				qi = m_usedQueueCount[queueFlags];
				break;
			}
		}
	}

	if(!found)
	{
		qfi = 0;
		qi = 0;

		TP_WARN(Log::RendererVulkanDevicePrefix, "Could not find queue of type ", queueType, '(',
		        std::to_underlying(queueType), "). Using default queue");
	}

	queueFamilyIndex = NumericCast<u8>(qfi);
	queueIndex = NumericCast<u8>(qi);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanDevice::FindQueueFamilyIndex(const RendererAPI::QueueType queueType,
                                                             VkQueueFamilyProperties& queueFamilyProperties,
															 u8& queueFamilyIndex, u8& queueIndex)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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
		if (queueType == RendererAPI::QueueType::Graphics && graphicsQueue)
		{
			found = true;
			qfi = index;
			qi = 0;
			break;
		}
		if (((queueFlags & requiredFlags) != 0u) && ((queueFlags & ~requiredFlags) == 0) &&
			m_usedQueueCount[queueFlags] < m_availableQueueCount[queueFlags])
		{
			found = true;
			qfi = index;
			qi = m_usedQueueCount[queueFlags];
			break;
		}
		if ((flagAnd != 0u) && ((queueFlags - flagAnd) < minQueueFlag) && !graphicsQueue &&
			m_usedQueueCount[queueFlags] < m_availableQueueCount[queueFlags])
		{
			found = true;
			minQueueFlag = (queueFlags - flagAnd);
			qfi = index;
			qi = m_usedQueueCount[queueFlags];
			break;
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

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanDevice::SetDeviceName(const std::string_view name) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	if(!VulkanRenderer::s_debugMarkerSupport)
		return;

#ifdef ENABLE_DEBUG_UTILS_EXTENSION
	VkSetObjectName(m_device, std::bit_cast<u64>(m_device), VK_OBJECT_TYPE_DEVICE, name);
#else
	VkSetObjectName(m_device, std::bit_cast<u64>(m_device), VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT, name);
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanDevice::LoadShadingRateCaps(const VkPhysicalDeviceFragmentShadingRateFeaturesKHR& shadingRateFeatures) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	if(!VulkanRenderer::s_shadingRate)
		return;

	if(shadingRateFeatures.pipelineFragmentShadingRate != 0u)
		RendererAPI::GPUSettings.ShadingRateCaps |= RendererAPI::ShadingRateCaps::PerDraw;
	if(shadingRateFeatures.primitiveFragmentShadingRate != 0u)
		RendererAPI::GPUSettings.ShadingRateCaps |= RendererAPI::ShadingRateCaps::PerPrimitive;
	if(shadingRateFeatures.attachmentFragmentShadingRate != 0u)
		RendererAPI::GPUSettings.ShadingRateCaps |= RendererAPI::ShadingRateCaps::PerTile;

	if(RendererAPI::GPUSettings.ShadingRateCaps == RendererAPI::ShadingRateCaps::NotSupported)
		return;

	VkPhysicalDeviceFragmentShadingRatePropertiesKHR fragmentShadingRateProperties{};
	fragmentShadingRateProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR;
	VkPhysicalDeviceProperties2 deviceProperties2{};
	deviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
	deviceProperties2.pNext = &fragmentShadingRateProperties;
	vkGetPhysicalDeviceProperties2(m_physicalDevice->GetVkPhysicalDevice(), &deviceProperties2);

	RendererAPI::GPUSettings.ShadingRateTexelWidth = fragmentShadingRateProperties.maxFragmentShadingRateAttachmentTexelSize.width;
	RendererAPI::GPUSettings.ShadingRateTexelHeight = fragmentShadingRateProperties.maxFragmentShadingRateAttachmentTexelSize.height;

	RendererAPI::GPUSettings.ShadingRateCombiner |= RendererAPI::ShadingRateCombiner::Passthrough;
	RendererAPI::GPUSettings.ShadingRateCombiner |= RendererAPI::ShadingRateCombiner::Override;
	if(fragmentShadingRateProperties.fragmentShadingRateNonTrivialCombinerOps != 0u)
	{
		RendererAPI::GPUSettings.ShadingRateCombiner |= RendererAPI::ShadingRateCombiner::Min;
		RendererAPI::GPUSettings.ShadingRateCombiner |= RendererAPI::ShadingRateCombiner::Max;
		RendererAPI::GPUSettings.ShadingRateCombiner |= RendererAPI::ShadingRateCombiner::Sum;
	}

	u32 fragmentShadingRatesCount = 0;
	vkGetPhysicalDeviceFragmentShadingRatesKHR(m_physicalDevice->GetVkPhysicalDevice(), &fragmentShadingRatesCount, nullptr);
	std::vector<VkPhysicalDeviceFragmentShadingRateKHR> fragmentShadingRates(fragmentShadingRatesCount);
	for(auto& rate : fragmentShadingRates)
	{
		rate.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_KHR;
		rate.pNext = nullptr;
	}
	vkGetPhysicalDeviceFragmentShadingRatesKHR(m_physicalDevice->GetVkPhysicalDevice(), &fragmentShadingRatesCount, fragmentShadingRates.data());
	for(const auto& rate : fragmentShadingRates)
	{
		if(rate.fragmentSize.width == 1 && rate.fragmentSize.height == 2)
			RendererAPI::GPUSettings.ShadingRates |= RendererAPI::ShadingRate::OneXTwo;
		if(rate.fragmentSize.width == 2 && rate.fragmentSize.height == 1)
			RendererAPI::GPUSettings.ShadingRates |= RendererAPI::ShadingRate::TwoXOne;
		if(rate.fragmentSize.width == 2 && rate.fragmentSize.height == 4)
			RendererAPI::GPUSettings.ShadingRates |= RendererAPI::ShadingRate::TwoXFour;
		if(rate.fragmentSize.width == 4 && rate.fragmentSize.height == 2)
			RendererAPI::GPUSettings.ShadingRates |= RendererAPI::ShadingRate::FourXTwo;
		if(rate.fragmentSize.width == 4 && rate.fragmentSize.height == 4)
			RendererAPI::GPUSettings.ShadingRates |= RendererAPI::ShadingRate::Quarter;
		if(rate.fragmentSize.width == 8 && rate.fragmentSize.height == 8)
			RendererAPI::GPUSettings.ShadingRates |= RendererAPI::ShadingRate::Eighth;
		if(rate.fragmentSize.width == 2 && rate.fragmentSize.height == 2)
			RendererAPI::GPUSettings.ShadingRates |= RendererAPI::ShadingRate::Half;
		if(rate.fragmentSize.width == 1 && rate.fragmentSize.height == 1)
			RendererAPI::GPUSettings.ShadingRates |= RendererAPI::ShadingRate::Full;
	}
}