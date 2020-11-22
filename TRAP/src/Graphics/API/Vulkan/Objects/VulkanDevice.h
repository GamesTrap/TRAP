#ifndef _TRAP_VULKANDEVICE_H_
#define _TRAP_VULKANDEVICE_H_

namespace TRAP::Graphics::API
{
	class VulkanInstance;
	class VulkanPhysicalDevice;

	class VulkanDevice
	{
	public:
		explicit VulkanDevice(const TRAP::Ref<VulkanInstance>& instance,
		                      TRAP::Scope<VulkanPhysicalDevice> physicalDevice,
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

	private:
		TRAP::Scope<VulkanPhysicalDevice> m_physicalDevice;

		std::vector<std::string> m_deviceExtensions;

		std::vector<std::vector<uint32_t>> m_availableQueueCount;
		std::vector<std::vector<uint32_t>> m_usedQueueCount;

		VkDevice m_device;
	};
}

#endif /*_TRAP_VULKANDEVICE_H_*/