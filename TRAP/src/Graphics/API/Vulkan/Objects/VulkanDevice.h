#ifndef _TRAP_VULKANDEVICE_H_
#define _TRAP_VULKANDEVICE_H_

#include "VulkanQueue.h"
#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics::API
{
	class VulkanInstance;
	class VulkanPhysicalDevice;

	class VulkanDevice
	{
	public:
		explicit VulkanDevice(TRAP::Scope<VulkanPhysicalDevice> physicalDevice,
		                      std::vector<std::string> deviceExtensions,
		                      bool requestAllAvailableQueues = false);
		~VulkanDevice();

		VulkanDevice(const VulkanDevice&) = delete;
		VulkanDevice& operator=(const VulkanDevice&) = delete;
		VulkanDevice(VulkanDevice&&) = default;
		VulkanDevice& operator=(VulkanDevice&&) = default;

		VkDevice& GetVkDevice();
		
		const TRAP::Scope<VulkanPhysicalDevice>& GetPhysicalDevice() const;

		const std::vector<std::string>& GetUsedPhysicalDeviceExtensions() const;

		void FindQueueFamilyIndices();

		void WaitIdle() const;
		
		uint8_t GetGraphicsQueueFamilyIndex() const;
		uint8_t GetTransferQueueFamilyIndex() const;
		uint8_t GetComputeQueueFamilyIndex() const;
		std::array<uint8_t, 3> GetQueueFamilyIndices() const;
		
		uint8_t GetGraphicsQueueIndex() const;
		uint8_t GetTransferQueueIndex() const;
		uint8_t GetComputeQueueIndex() const;
		
	private:
		friend VulkanQueue;
		
		void FindQueueFamilyIndex(RendererAPI::QueueType queueType, uint8_t& queueFamilyIndex, uint8_t& queueIndex);
		void FindQueueFamilyIndex(RendererAPI::QueueType type, VkQueueFamilyProperties& queueFamilyProperties, uint8_t& queueFamilyIndex, uint8_t& queueIndex);
		
		TRAP::Scope<VulkanPhysicalDevice> m_physicalDevice;

		std::vector<std::string> m_deviceExtensions;

		std::unordered_map<VkQueueFlags, uint32_t> m_availableQueueCount;
		std::unordered_map<VkQueueFlags, uint32_t> m_usedQueueCount;

		uint8_t m_graphicsQueueFamilyIndex;
		uint8_t m_transferQueueFamilyIndex;
		uint8_t m_computeQueueFamilyIndex;

		uint8_t m_graphicsQueueIndex;
		uint8_t m_transferQueueIndex;
		uint8_t m_computeQueueIndex;

		VkDevice m_device;
	};
}

#endif /*_TRAP_VULKANDEVICE_H_*/