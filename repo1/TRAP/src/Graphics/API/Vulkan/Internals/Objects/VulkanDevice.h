#ifndef _TRAP_VULKANDEVICE_H_
#define _TRAP_VULKANDEVICE_H_

#include "Graphics/API/Vulkan/Internals/VulkanQueueFamilyIndices.h"

namespace TRAP::Graphics::API::Vulkan
{
	class PhysicalDevice;
	class Instance;

	class Device
	{
	public:
		explicit Device(const Scope<PhysicalDevice>& physicalDevice);
		Device(const Scope<PhysicalDevice>& physicalDevice, const std::vector<std::string>& physicalDeviceExtensions);
		~Device();
		Device(const Device&) = default;
		Device& operator=(const Device&) = default;
		Device(Device&&) = default;
		Device& operator=(Device&&) = default;

		VkDevice& GetDevice();
		
		const std::vector<std::string>& GetUsedExtensions() const;
		VkQueue& GetGraphicsQueue();
		VkQueue& GetPresentQueue();
		VkQueue& GetComputeQueue();
		VkQueue& GetTransferQueue();
		const QueueFamilyIndices& GetQueueFamilyIndices() const;

		void WaitIdle() const;

	private:
		VkDevice m_device;
		std::vector<std::string> m_deviceExtensions;
		VkQueue m_graphicsQueue;
		VkQueue m_presentQueue;
		VkQueue m_computeQueue;
		VkQueue m_transferQueue;
		QueueFamilyIndices m_queueFamilyIndices;
	};
}

#endif /*_TRAP_VULKANDEVICE_H_*/