#include "TRAPPCH.h"
#include "VulkanDevice.h"

#include "VulkanPhysicalDevice.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

TRAP::Graphics::API::VulkanDevice::VulkanDevice(TRAP::Scope<VulkanPhysicalDevice> physicalDevice,
                                                std::vector<std::string> deviceExtensions,
                                                bool requestAllAvailableQueues)
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
	TRAP_ASSERT(m_physicalDevice, "physicalDevice is nullptr");
	
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Creating Device");
#endif

	std::vector<const char*> extensions(m_deviceExtensions.size());
	for (uint32_t i = 0; i < m_deviceExtensions.size(); i++)
	{
		if (m_physicalDevice->IsExtensionSupported(m_deviceExtensions[i]))
			extensions[i] = m_deviceExtensions[i].c_str();
		else
			m_deviceExtensions.erase(m_deviceExtensions.begin() + i);
	}

#ifdef VERBOSE_GRAPHICS_DEBUG
	if (!m_deviceExtensions.empty())
	{
		TP_DEBUG(Log::RendererVulkanDevicePrefix, "Loading Device Extension(s):");
		for (const std::string& str : m_deviceExtensions)
			TP_DEBUG(Log::RendererVulkanDevicePrefix, "    ", str);
	}
#endif

	if (std::find_if(extensions.begin(),
			extensions.end(),
			[](const char* ext)
			{
				return std::strcmp(ext, VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME) == 0;
			}) != extensions.end())
	{
		m_physicalDevice->RetrievePhysicalDeviceFragmentShaderInterlockFeatures();
	}
	
	VkPhysicalDeviceFeatures2 deviceFeatures2{};
	VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures{};
	VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT fragmentShaderInterlockFeatures{};
	VkPhysicalDeviceSamplerYcbcrConversionFeatures ycbcrFeatures{};
	if (VulkanRenderer::s_descriptorIndexingExtension)
	{
		if (VulkanRenderer::s_fragmentShaderInterlockExtension)
		{
			fragmentShaderInterlockFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT;
			fragmentShaderInterlockFeatures.pNext = nullptr;
			descriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
			descriptorIndexingFeatures.pNext = &fragmentShaderInterlockFeatures;
		}
		else
			descriptorIndexingFeatures.pNext = nullptr;

		deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		deviceFeatures2.pNext = &descriptorIndexingFeatures;

		ycbcrFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES;
		if(VulkanRenderer::s_samplerYcbcrConversionExtension)
		{
			deviceFeatures2.pNext = &ycbcrFeatures;
			ycbcrFeatures.pNext = &descriptorIndexingFeatures;
		}
		
		vkGetPhysicalDeviceFeatures2(m_physicalDevice->GetVkPhysicalDevice(), &deviceFeatures2);
	}

	//Need a queuePriority for each queue in the queue family we create
	const std::vector<VkQueueFamilyProperties>& queueFamilyProperties = m_physicalDevice->GetQueueFamilyProperties();
	std::vector<std::vector<float>> queueFamilyPriorities(queueFamilyProperties.size());
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};

	const uint32_t maxQueueFlag = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT |
			                      VK_QUEUE_SPARSE_BINDING_BIT | VK_QUEUE_PROTECTED_BIT;
	m_availableQueueCount.resize(maxQueueFlag * sizeof(uint32_t));
	m_usedQueueCount.resize(maxQueueFlag * sizeof(uint32_t));

	for(uint32_t i = 0; i < queueFamilyProperties.size(); i++)
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
			info.queueFamilyIndex = i;
			info.queueCount = queueCount;
			info.pQueuePriorities = queueFamilyPriorities[i].data();
			queueCreateInfos.push_back(info);

			m_availableQueueCount[queueFamilyProperties[i].queueFlags] = queueCount;
		}
	}

	VkDeviceCreateInfo deviceCreateInfo = VulkanInits::DeviceCreateInfo(&deviceFeatures2, queueCreateInfos, extensions);

	VkCall(vkCreateDevice(m_physicalDevice->GetVkPhysicalDevice(), &deviceCreateInfo, nullptr, &m_device));

	VkLoadDevice(m_device);

	VulkanRenderer::s_debugMarkerSupport = (&vkCmdBeginDebugUtilsLabelEXT) && (&vkCmdEndDebugUtilsLabelEXT) &&
		(&vkCmdInsertDebugUtilsLabelEXT) && (&vkSetDebugUtilsObjectNameEXT);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanDevice::~VulkanDevice()
{
	if(m_device)
	{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Destroying Device");
#endif
		vkDestroyDevice(m_device, nullptr);
		m_device = nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VkDevice& TRAP::Graphics::API::VulkanDevice::GetVkDevice()
{
	return m_device;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::API::VulkanPhysicalDevice>& TRAP::Graphics::API::VulkanDevice::GetPhysicalDevice() const
{
	return m_physicalDevice;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<std::string>& TRAP::Graphics::API::VulkanDevice::GetUsedPhysicalDeviceExtensions() const
{
	return m_deviceExtensions;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanDevice::FindQueueFamilyIndices()
{
	FindQueueFamilyIndex(RendererAPI::QueueType::Graphics, m_graphicsQueueFamilyIndex, m_graphicsQueueIndex);
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Using Graphics Queue Family Index ", static_cast<uint32_t>(m_graphicsQueueFamilyIndex));
#endif
	FindQueueFamilyIndex(RendererAPI::QueueType::Compute, m_computeQueueFamilyIndex, m_computeQueueIndex);
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Using Compute Queue Family Index ", static_cast<uint32_t>(m_computeQueueFamilyIndex));
#endif
	FindQueueFamilyIndex(RendererAPI::QueueType::Transfer, m_transferQueueFamilyIndex, m_transferQueueIndex);
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Using Transfer Queue Family Index ", static_cast<uint32_t>(m_transferQueueFamilyIndex));
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanDevice::WaitIdle() const
{
	VkCall(vkDeviceWaitIdle(m_device));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanDevice::FindQueueFamilyIndex(const RendererAPI::QueueType queueType, uint8_t& queueFamilyIndex, uint8_t& queueIndex)
{
	uint32_t qfi = std::numeric_limits<uint32_t>::max();
	uint32_t qi = std::numeric_limits<uint32_t>::max();
	const VkQueueFlags requiredFlags = QueueTypeToVkQueueFlags(queueType);
	bool found = false;

	const std::vector<VkQueueFamilyProperties>& props = m_physicalDevice->GetQueueFamilyProperties();

	uint32_t minQueueFlag = std::numeric_limits<uint32_t>::max();

	//Try to find a dedicated queue of this type
	for(uint32_t index = 0; index < props.size(); ++index)
	{
		const VkQueueFlags queueFlags = props[index].queueFlags;
		const bool graphicsQueue = (queueFlags & VK_QUEUE_GRAPHICS_BIT) ? true : false;
		const uint32_t flagAnd = (queueFlags & requiredFlags);
		if(queueType == RendererAPI::QueueType::Graphics && graphicsQueue)
		{
			found = true;
			qfi = index;
			qi = 0;
			break;
		}
		if((queueFlags & requiredFlags) && ((queueFlags & ~requiredFlags) == 0) &&
			m_usedQueueCount[queueFlags] < m_availableQueueCount[queueFlags])
		{
			found = true;
			qfi = index;
			qi = m_usedQueueCount[queueFlags];
			break;
		}
		if(flagAnd && ((queueFlags - flagAnd) < minQueueFlag) && !graphicsQueue &&
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
	if(!found)
	{
		for(uint32_t index = 0; index < props.size(); ++index)
		{
			const VkQueueFlags queueFlags = props[index].queueFlags;
			if((queueFlags & requiredFlags) && m_usedQueueCount[queueFlags] < m_availableQueueCount[queueFlags])
			{
				found = true;
				qfi = index;
				qi = m_usedQueueCount[queueFlags];
				break;
			}
		}
	}

	if(!found)
	{
		qfi = 0;
		qi = 0;

		TP_WARN(Log::RendererVulkanDevicePrefix, "Could not find queue of type ", static_cast<uint32_t>(queueType), ". Using default queue");
	}

	queueFamilyIndex = static_cast<uint8_t>(qfi);
	queueIndex = static_cast<uint8_t>(qi);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanDevice::FindQueueFamilyIndex(const RendererAPI::QueueType queueType, VkQueueFamilyProperties& queueFamilyProperties, uint8_t& queueFamilyIndex, uint8_t& queueIndex)
{
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
		const bool graphicsQueue = (queueFlags & VK_QUEUE_GRAPHICS_BIT) ? true : false;
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

		TP_WARN(Log::RendererVulkanDevicePrefix, "Could not find queue of type ", static_cast<uint32_t>(queueType), ". Using default queue");
	}

	queueFamilyProperties = props[qfi];
	queueFamilyIndex = static_cast<uint8_t>(qfi);
	queueIndex = static_cast<uint8_t>(qi);
}

//-------------------------------------------------------------------------------------------------------------------//

uint8_t TRAP::Graphics::API::VulkanDevice::GetGraphicsQueueFamilyIndex() const
{
	return m_graphicsQueueFamilyIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

uint8_t TRAP::Graphics::API::VulkanDevice::GetTransferQueueFamilyIndex() const
{
	return m_transferQueueFamilyIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

uint8_t TRAP::Graphics::API::VulkanDevice::GetComputeQueueFamilyIndex() const
{
	return m_computeQueueFamilyIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

std::array<uint8_t, 3> TRAP::Graphics::API::VulkanDevice::GetQueueFamilyIndices() const
{
	return { m_graphicsQueueFamilyIndex, m_transferQueueFamilyIndex, m_computeQueueFamilyIndex };
}

//-------------------------------------------------------------------------------------------------------------------//

uint8_t TRAP::Graphics::API::VulkanDevice::GetGraphicsQueueIndex() const
{
	return m_graphicsQueueIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

uint8_t TRAP::Graphics::API::VulkanDevice::GetTransferQueueIndex() const
{
	return m_transferQueueIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

uint8_t TRAP::Graphics::API::VulkanDevice::GetComputeQueueIndex() const
{
	return m_computeQueueIndex;
}
