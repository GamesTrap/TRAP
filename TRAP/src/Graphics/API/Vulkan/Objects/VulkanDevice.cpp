#include "TRAPPCH.h"
#include "VulkanDevice.h"

#include "VulkanPhysicalDevice.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Graphics/API/Objects/AftermathTracker.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/Objects/VulkanSemaphore.h"
#include "Graphics/API/Objects/Semaphore.h"
#include "Application.h"

TRAP::Graphics::API::VulkanDevice::VulkanDevice(TRAP::Scope<VulkanPhysicalDevice> physicalDevice,
                                                std::vector<std::string> deviceExtensions,
                                                const bool requestAllAvailableQueues)
	: m_physicalDevice(std::move(physicalDevice)),
      m_deviceExtensions(std::move(deviceExtensions)),
      m_graphicsQueueFamilyIndex(0),
	  m_transferQueueFamilyIndex(0),
	  m_computeQueueFamilyIndex(0),
	  m_graphicsQueueIndex(),
	  m_transferQueueIndex(),
	  m_computeQueueIndex(),
	  m_device()
{
	ZoneScoped;

	TRAP_ASSERT(m_physicalDevice, "physicalDevice is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Creating Device");
#endif

	std::vector<const char*> extensions{};
	for (uint32_t i = 0; i < m_deviceExtensions.size(); i++)
	{
		if (m_physicalDevice->IsExtensionSupported(m_deviceExtensions[i]))
			extensions.push_back(m_deviceExtensions[i].c_str());
		else
			m_deviceExtensions.erase(m_deviceExtensions.begin() + i);
	}

#ifdef VERBOSE_GRAPHICS_DEBUG
	if (!m_deviceExtensions.empty())
	{
		TP_DEBUG(Log::RendererVulkanDevicePrefix, "Loading Device Extension(s):");
		for (const std::string_view str : m_deviceExtensions)
			TP_DEBUG(Log::RendererVulkanDevicePrefix, "    ", str);
	}
#endif

	if (std::find_if(extensions.begin(), extensions.end(), [](const char* const ext)
		{
			return std::strcmp(ext, VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME) == 0;
		}) != extensions.end())
	{
		m_physicalDevice->RetrievePhysicalDeviceFragmentShaderInterlockFeatures();
	}

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
	std::vector<std::vector<float>> queueFamilyPriorities(queueFamilyProperties.size());
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};

	for(std::size_t i = 0; i < queueFamilyProperties.size(); i++)
	{
		uint32_t queueCount = queueFamilyProperties[i].queueCount;
		if(queueCount > 0)
		{
			queueFamilyPriorities[i].resize(queueCount);

			//Request only one queue of each type if RequestAllAvailableQueues is not set to true
			if (queueCount > 1 && !requestAllAvailableQueues)
				queueCount = 1;

			VkDeviceQueueCreateInfo info;
			info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			info.pNext = nullptr;
			info.flags = 0;
			info.queueFamilyIndex = static_cast<uint32_t>(i);
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
#endif

	const VkDeviceCreateInfo deviceCreateInfo = VulkanInits::DeviceCreateInfo(&devFeatures2, queueCreateInfos,
	                                                                          extensions);

	VkCall(vkCreateDevice(m_physicalDevice->GetVkPhysicalDevice(), &deviceCreateInfo, nullptr, &m_device));

	VkLoadDevice(m_device);

	VulkanRenderer::s_debugMarkerSupport = (vkCmdBeginDebugUtilsLabelEXT) && (vkCmdEndDebugUtilsLabelEXT) &&
		                                   (vkCmdInsertDebugUtilsLabelEXT) && (vkSetDebugUtilsObjectNameEXT);
	VulkanRenderer::s_bufferDeviceAddressExtension = bufferDeviceAddressFeatures.bufferDeviceAddress;
	VulkanRenderer::s_samplerYcbcrConversionExtension = ycbcrFeatures.samplerYcbcrConversion;
	VulkanRenderer::s_shaderDrawParameters = shaderDrawParametersFeatures.shaderDrawParameters;
	VulkanRenderer::s_timelineSemaphore = timelineSemaphoreFeatures.timelineSemaphore;
	LoadShadingRateCaps(shadingRateFeatures);

#ifdef ENABLE_GRAPHICS_DEBUG
	if (m_physicalDevice->GetVkPhysicalDeviceProperties().deviceName[0] != '\0')
		SetDeviceName(m_physicalDevice->GetVkPhysicalDeviceProperties().deviceName);
#endif /*ENABLE_GRAPHICS_DEBUG*/
#ifdef NVIDIA_REFLEX_AVAILABLE
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
#endif /*NVIDIA_REFLEX_AVAILABLE*/
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanDevice::~VulkanDevice()
{
	ZoneScoped;

	TRAP_ASSERT(m_device);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Destroying Device");
#endif

#ifdef NVIDIA_REFLEX_AVAILABLE
	if(RendererAPI::GPUSettings.ReflexSupported)
		vkDestroySemaphore(m_device, m_reflexSemaphore, nullptr);
#endif /*NVIDIA_REFLEX_AVAILABLE*/

	vkDestroyDevice(m_device, nullptr);
	m_device = nullptr;

#ifdef ENABLE_NSIGHT_AFTERMATH
	if(RendererAPI::s_aftermathSupport)
		TRAP::Graphics::AftermathTracker::Shutdown();
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

VkDevice TRAP::Graphics::API::VulkanDevice::GetVkDevice() const
{
	ZoneScoped;

	return m_device;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanPhysicalDevice* TRAP::Graphics::API::VulkanDevice::GetPhysicalDevice() const
{
	ZoneScoped;

	return m_physicalDevice.get();
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<std::string>& TRAP::Graphics::API::VulkanDevice::GetUsedPhysicalDeviceExtensions() const
{
	ZoneScoped;

	return m_deviceExtensions;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanDevice::FindQueueFamilyIndices()
{
	ZoneScoped;

	FindQueueFamilyIndex(RendererAPI::QueueType::Graphics, m_graphicsQueueFamilyIndex, m_graphicsQueueIndex);
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Using Graphics Queue Family Index ",
	         static_cast<uint32_t>(m_graphicsQueueFamilyIndex));
#endif
	FindQueueFamilyIndex(RendererAPI::QueueType::Compute, m_computeQueueFamilyIndex, m_computeQueueIndex);
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Using Compute Queue Family Index ",
	         static_cast<uint32_t>(m_computeQueueFamilyIndex));
#endif
	FindQueueFamilyIndex(RendererAPI::QueueType::Transfer, m_transferQueueFamilyIndex, m_transferQueueIndex);
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Using Transfer Queue Family Index ",
	         static_cast<uint32_t>(m_transferQueueFamilyIndex));
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanDevice::WaitIdle() const
{
	ZoneScoped;

	VkCall(vkDeviceWaitIdle(m_device));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanDevice::FindQueueFamilyIndex(const RendererAPI::QueueType queueType,
                                                             uint8_t& queueFamilyIndex, uint8_t& queueIndex)
{
	ZoneScoped;

	uint32_t qfi = std::numeric_limits<uint32_t>::max();
	uint32_t qi = std::numeric_limits<uint32_t>::max();
	const VkQueueFlags requiredFlags = QueueTypeToVkQueueFlags(queueType);
	bool found = false;

	const std::vector<VkQueueFamilyProperties>& props = m_physicalDevice->GetQueueFamilyProperties();

	uint32_t minQueueFlag = std::numeric_limits<uint32_t>::max();

	//Try to find a dedicated queue of this type
	for(std::size_t index = 0; index < props.size(); ++index)
	{
		const VkQueueFlags queueFlags = props[index].queueFlags;
		const bool graphicsQueue = (queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0;
		const uint32_t flagAnd = (queueFlags & requiredFlags);
		if(queueType == RendererAPI::QueueType::Graphics && graphicsQueue)
		{
			found = true;
			qfi = static_cast<uint32_t>(index);
			qi = 0;
			break;
		}
		if((queueFlags & requiredFlags) && ((queueFlags & ~requiredFlags) == 0) &&
			m_usedQueueCount[queueFlags] < m_availableQueueCount[queueFlags])
		{
			found = true;
			qfi = static_cast<uint32_t>(index);
			qi = m_usedQueueCount[queueFlags];
			break;
		}
		if(flagAnd && ((queueFlags - flagAnd) < minQueueFlag) && !graphicsQueue &&
			m_usedQueueCount[queueFlags] < m_availableQueueCount[queueFlags])
		{
			found = true;
			minQueueFlag = (queueFlags - flagAnd);
			qfi = static_cast<uint32_t>(index);
			qi = m_usedQueueCount[queueFlags];
			break;
		}
	}

	//If hardware doesn't provide a dedicated queue try to find a non-dedicated one
	if(!found)
	{
		for(std::size_t index = 0; index < props.size(); ++index)
		{
			const VkQueueFlags queueFlags = props[index].queueFlags;
			if((queueFlags & requiredFlags) && m_usedQueueCount[queueFlags] < m_availableQueueCount[queueFlags])
			{
				found = true;
				qfi = static_cast<uint32_t>(index);
				qi = m_usedQueueCount[queueFlags];
				break;
			}
		}
	}

	if(!found)
	{
		qfi = 0;
		qi = 0;

		TP_WARN(Log::RendererVulkanDevicePrefix, "Could not find queue of type ",
		        static_cast<uint32_t>(queueType), ". Using default queue");
	}

	queueFamilyIndex = static_cast<uint8_t>(qfi);
	queueIndex = static_cast<uint8_t>(qi);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanDevice::FindQueueFamilyIndex(const RendererAPI::QueueType queueType,
                                                             VkQueueFamilyProperties& queueFamilyProperties,
															 uint8_t& queueFamilyIndex, uint8_t& queueIndex)
{
	ZoneScoped;

	uint32_t qfi = std::numeric_limits<uint32_t>::max();
	uint32_t qi = std::numeric_limits<uint32_t>::max();
	const VkQueueFlags requiredFlags = QueueTypeToVkQueueFlags(queueType);
	bool found = false;

	const std::vector<VkQueueFamilyProperties>& props = m_physicalDevice->GetQueueFamilyProperties();

	uint32_t minQueueFlag = std::numeric_limits<uint32_t>::max();

	//Try to find a dedicated queue of this type
	for (uint32_t index = 0; index < props.size(); ++index)
	{
		const VkQueueFlags queueFlags = props[index].queueFlags;
		const bool graphicsQueue = (queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0;
		const uint32_t flagAnd = (queueFlags & requiredFlags);
		if (queueType == RendererAPI::QueueType::Graphics && graphicsQueue)
		{
			found = true;
			qfi = index;
			qi = 0;
			break;
		}
		if ((queueFlags & requiredFlags) && ((queueFlags & ~requiredFlags) == 0) &&
			m_usedQueueCount[queueFlags] < m_availableQueueCount[queueFlags])
		{
			found = true;
			qfi = index;
			qi = m_usedQueueCount[queueFlags];
			break;
		}
		if (flagAnd && ((queueFlags - flagAnd) < minQueueFlag) && !graphicsQueue &&
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
		for (uint32_t index = 0; index < props.size(); ++index)
		{
			const VkQueueFlags queueFlags = props[index].queueFlags;
			if ((queueFlags & requiredFlags) && m_usedQueueCount[queueFlags] < m_availableQueueCount[queueFlags])
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

		TP_WARN(Log::RendererVulkanDevicePrefix, "Could not find queue of type ",
		        static_cast<uint32_t>(queueType), ". Using default queue");
	}

	queueFamilyProperties = props[qfi];
	queueFamilyIndex = static_cast<uint8_t>(qfi);
	queueIndex = static_cast<uint8_t>(qi);
}

//-------------------------------------------------------------------------------------------------------------------//

uint8_t TRAP::Graphics::API::VulkanDevice::GetGraphicsQueueFamilyIndex() const
{
	ZoneScoped;

	return m_graphicsQueueFamilyIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

uint8_t TRAP::Graphics::API::VulkanDevice::GetTransferQueueFamilyIndex() const
{
	ZoneScoped;

	return m_transferQueueFamilyIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

uint8_t TRAP::Graphics::API::VulkanDevice::GetComputeQueueFamilyIndex() const
{
	ZoneScoped;

	return m_computeQueueFamilyIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

std::array<uint8_t, 3> TRAP::Graphics::API::VulkanDevice::GetQueueFamilyIndices() const
{
	ZoneScoped;

	return { m_graphicsQueueFamilyIndex, m_transferQueueFamilyIndex, m_computeQueueFamilyIndex };
}

//-------------------------------------------------------------------------------------------------------------------//

uint8_t TRAP::Graphics::API::VulkanDevice::GetGraphicsQueueIndex() const
{
	ZoneScoped;

	return m_graphicsQueueIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

uint8_t TRAP::Graphics::API::VulkanDevice::GetTransferQueueIndex() const
{
	ZoneScoped;

	return m_transferQueueIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

uint8_t TRAP::Graphics::API::VulkanDevice::GetComputeQueueIndex() const
{
	ZoneScoped;

	return m_computeQueueIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef NVIDIA_REFLEX_AVAILABLE
VkSemaphore& TRAP::Graphics::API::VulkanDevice::GetReflexSemaphore()
{
	ZoneScoped;

	return m_reflexSemaphore;
}
#endif /*NVIDIA_REFLEX_AVAILABLE*/

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanDevice::SetDeviceName(const std::string_view name) const
{
	ZoneScoped;

	if(!VulkanRenderer::s_debugMarkerSupport)
		return;

#ifdef ENABLE_DEBUG_UTILS_EXTENSION
	VkSetObjectName(m_device, Utils::BitCast<VkDevice, uint64_t>(m_device), VK_OBJECT_TYPE_DEVICE, name);
#else
	VkSetObjectName(m_device, Utils::BitCast<VkDevice, uint64_t>(m_device), VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT, name);
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanDevice::LoadShadingRateCaps(const VkPhysicalDeviceFragmentShadingRateFeaturesKHR& shadingRateFeatures) const
{
	ZoneScoped;

	if(!VulkanRenderer::s_shadingRate)
		return;

	if(shadingRateFeatures.pipelineFragmentShadingRate)
		RendererAPI::GPUSettings.ShadingRateCaps |= RendererAPI::ShadingRateCaps::PerDraw;
	if(shadingRateFeatures.primitiveFragmentShadingRate)
		RendererAPI::GPUSettings.ShadingRateCaps |= RendererAPI::ShadingRateCaps::PerPrimitive;
	if(shadingRateFeatures.attachmentFragmentShadingRate)
		RendererAPI::GPUSettings.ShadingRateCaps |= RendererAPI::ShadingRateCaps::PerTile;

	if(static_cast<uint32_t>(RendererAPI::GPUSettings.ShadingRateCaps))
	{
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
		if(fragmentShadingRateProperties.fragmentShadingRateNonTrivialCombinerOps)
		{
			RendererAPI::GPUSettings.ShadingRateCombiner |= RendererAPI::ShadingRateCombiner::Min;
			RendererAPI::GPUSettings.ShadingRateCombiner |= RendererAPI::ShadingRateCombiner::Max;
			RendererAPI::GPUSettings.ShadingRateCombiner |= RendererAPI::ShadingRateCombiner::Sum;
		}

		uint32_t fragmentShadingRatesCount = 0;
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
}