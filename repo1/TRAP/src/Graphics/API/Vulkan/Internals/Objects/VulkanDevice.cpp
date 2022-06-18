#include "TRAPPCH.h"

#include "VulkanDevice.h"

#include "VulkanPhysicalDevice.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/Internals/VulkanInitializers.h"

TRAP::Graphics::API::Vulkan::Device::Device(const Scope<PhysicalDevice>& physicalDevice)
	: m_device(), m_graphicsQueue(), m_presentQueue(), m_queueFamilyIndices(physicalDevice->GetQueueFamilyIndices())
{	
	float priority = 1.0f;
	std::set<uint32_t> uniqueQueueFamilies = { m_queueFamilyIndices.GraphicsIndices, m_queueFamilyIndices.PresentIndices, m_queueFamilyIndices.ComputeIndices, m_queueFamilyIndices.TransferIndices };
	std::vector<VkDeviceQueueCreateInfo> queueInfo;
	for (uint32_t queueFamily : uniqueQueueFamilies)
		queueInfo.emplace_back(Initializers::DeviceQueueCreateInfo(queueFamily, &priority));
	
	VkDeviceCreateInfo info = Initializers::DeviceCreateInfo(queueInfo, physicalDevice->GetPhysicalDeviceFeatures());

	VkCall(vkCreateDevice(physicalDevice->GetPhysicalDevice(), &info, nullptr, &m_device));

	vkGetDeviceQueue(m_device, m_queueFamilyIndices.GraphicsIndices, 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_device, m_queueFamilyIndices.PresentIndices, 0, &m_presentQueue);
	vkGetDeviceQueue(m_device, m_queueFamilyIndices.ComputeIndices, 0, &m_computeQueue);
	vkGetDeviceQueue(m_device, m_queueFamilyIndices.TransferIndices, 0, &m_transferQueue);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::Vulkan::Device::Device(const Scope<PhysicalDevice>& physicalDevice, const std::vector<std::string>& physicalDeviceExtensions)
	: m_device(), m_deviceExtensions(physicalDeviceExtensions), m_graphicsQueue(), m_presentQueue(), m_queueFamilyIndices(physicalDevice->GetQueueFamilyIndices())
{	
	float priority = 1.0f;
	std::set<uint32_t> uniqueQueueFamilies = { m_queueFamilyIndices.GraphicsIndices, m_queueFamilyIndices.PresentIndices, m_queueFamilyIndices.ComputeIndices, m_queueFamilyIndices.TransferIndices };
	std::vector<VkDeviceQueueCreateInfo> queueInfo;
	for (uint32_t queueFamily : uniqueQueueFamilies)
		queueInfo.emplace_back(Initializers::DeviceQueueCreateInfo(queueFamily, &priority));

	std::vector<const char*> extensions(physicalDeviceExtensions.size());
	std::transform(physicalDeviceExtensions.begin(), physicalDeviceExtensions.end(), extensions.begin(), [](const std::string_view s) {return s.data(); });

	VkDeviceCreateInfo info = Initializers::DeviceCreateInfo(queueInfo, physicalDevice->GetPhysicalDeviceFeatures(), extensions);

	VkCall(vkCreateDevice(physicalDevice->GetPhysicalDevice(), &info, nullptr, &m_device));

	vkGetDeviceQueue(m_device, m_queueFamilyIndices.GraphicsIndices, 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_device, m_queueFamilyIndices.PresentIndices, 0, &m_presentQueue);
	vkGetDeviceQueue(m_device, m_queueFamilyIndices.ComputeIndices, 0, &m_computeQueue);
	vkGetDeviceQueue(m_device, m_queueFamilyIndices.TransferIndices, 0, &m_transferQueue);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::Vulkan::Device::~Device()
{
	if (m_device)
	{
		vkDestroyDevice(m_device, nullptr);
		m_device = nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VkDevice& TRAP::Graphics::API::Vulkan::Device::GetDevice()
{
	return m_device;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<std::string>& TRAP::Graphics::API::Vulkan::Device::GetUsedExtensions() const
{
	return m_deviceExtensions;
}

//-------------------------------------------------------------------------------------------------------------------//

VkQueue& TRAP::Graphics::API::Vulkan::Device::GetGraphicsQueue()
{
	return m_graphicsQueue;
}

//-------------------------------------------------------------------------------------------------------------------//

VkQueue& TRAP::Graphics::API::Vulkan::Device::GetPresentQueue()
{
	return m_presentQueue;
}

//-------------------------------------------------------------------------------------------------------------------//

VkQueue& TRAP::Graphics::API::Vulkan::Device::GetComputeQueue()
{
	return m_computeQueue;
}

//-------------------------------------------------------------------------------------------------------------------//

VkQueue& TRAP::Graphics::API::Vulkan::Device::GetTransferQueue()
{
	return m_transferQueue;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Graphics::API::Vulkan::QueueFamilyIndices& TRAP::Graphics::API::Vulkan::Device::GetQueueFamilyIndices() const
{
	return m_queueFamilyIndices;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Vulkan::Device::WaitIdle() const
{
	VkCall(vkDeviceWaitIdle(m_device));
}
