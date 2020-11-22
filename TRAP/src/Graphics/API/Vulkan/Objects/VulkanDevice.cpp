#include "TRAPPCH.h"
#include "VulkanDevice.h"

#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

TRAP::Graphics::API::VulkanDevice::VulkanDevice(const TRAP::Ref<VulkanInstance>& instance, 
                                                TRAP::Scope<VulkanPhysicalDevice> physicalDevice,
                                                std::vector<std::string> deviceExtensions,
                                                bool requestAllAvailableQueues)
	: m_physicalDevice(std::move(physicalDevice)),
      m_deviceExtensions(std::move(deviceExtensions)),
      m_device()
{
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

	VkDeviceGroupDeviceCreateInfo deviceGroupInfo;
	deviceGroupInfo.sType = VK_STRUCTURE_TYPE_DEVICE_GROUP_DEVICE_CREATE_INFO;
	std::array<VkPhysicalDeviceGroupProperties, VulkanRenderer::MAX_LINKED_GPUS> physicalDeviceGroupProperties{};

	VulkanRenderer::Renderer.LinkedNodeCount = 1;
	if (VulkanRenderer::Renderer.GPUMode == VulkanRenderer::GPUMode::Linked)
	{
		uint32_t deviceGroupCount = 0;

		VkCall(vkEnumeratePhysicalDeviceGroups(instance->GetVkInstance(), &deviceGroupCount, nullptr));

		for (uint32_t i = 0; i < deviceGroupCount; i++)
		{
			physicalDeviceGroupProperties[i].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES;
			physicalDeviceGroupProperties[i].pNext = nullptr;
		}
		VkCall(vkEnumeratePhysicalDeviceGroups(instance->GetVkInstance(), &deviceGroupCount, &physicalDeviceGroupProperties[0]));

		for (uint32_t i = 0; i < deviceGroupCount; i++)
		{
			if (physicalDeviceGroupProperties[i].physicalDeviceCount > 1)
			{
				deviceGroupInfo.physicalDeviceCount = physicalDeviceGroupProperties[i].physicalDeviceCount;
				deviceGroupInfo.pPhysicalDevices = physicalDeviceGroupProperties[i].physicalDevices;
				VulkanRenderer::Renderer.LinkedNodeCount = deviceGroupInfo.physicalDeviceCount;
				break;
			}
		}
	}

	if (VulkanRenderer::Renderer.LinkedNodeCount < 2)
		VulkanRenderer::Renderer.GPUMode = VulkanRenderer::GPUMode::Single;
	
	VkPhysicalDeviceFeatures2 deviceFeatures2;
	VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures;
	VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT fragmentShaderInterlockFeatures;
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
		vkGetPhysicalDeviceFeatures2(m_physicalDevice->GetVkPhysicalDevice(), &deviceFeatures2);
	}

	//Need a queuePriority for each queue in the queue family we create
	const std::vector<VkQueueFamilyProperties>& queueFamilyProperties = m_physicalDevice->GetQueueFamilyProperties();
	std::vector<std::vector<float>> queueFamilyPriorities(queueFamilyProperties.size());
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};

	const uint32_t maxQueueFlag = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT |
			                      VK_QUEUE_SPARSE_BINDING_BIT | VK_QUEUE_PROTECTED_BIT;
	m_availableQueueCount.resize(VulkanRenderer::Renderer.LinkedNodeCount);
	m_usedQueueCount.resize(VulkanRenderer::Renderer.LinkedNodeCount);
	for(uint32_t i = 0; i < VulkanRenderer::Renderer.LinkedNodeCount; i++)
	{
		m_availableQueueCount[i].resize(maxQueueFlag);
		m_usedQueueCount[i].resize(maxQueueFlag);
	}

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

			for (uint32_t n = 0; n < VulkanRenderer::Renderer.LinkedNodeCount; n++)
				m_availableQueueCount[n][queueFamilyProperties[i].queueFlags] = queueCount;
		}
	}

	VkDeviceCreateInfo deviceCreateInfo;
	
	if(VulkanRenderer::Renderer.GPUMode == RendererAPI::GPUMode::Linked)
		deviceCreateInfo = VulkanInits::DeviceCreateInfo(&deviceGroupInfo, queueCreateInfos, extensions);
	else
		deviceCreateInfo = VulkanInits::DeviceCreateInfo(&deviceFeatures2, queueCreateInfos, extensions);

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