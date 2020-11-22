#ifndef _TRAP_VULKANDEVICE_H_
#define _TRAP_VULKANDEVICE_H_

namespace TRAP::Graphics::API
{
	class VulkanPhysicalDevice;

	class VulkanDevice
	{
	public:
		explicit VulkanDevice(TRAP::Scope<VulkanPhysicalDevice> physicalDevice,
			std::vector<std::string> deviceExtensions);
		~VulkanDevice();

		VulkanDevice(const VulkanDevice&) = delete;
		VulkanDevice& operator=(const VulkanDevice&) = delete;
		VulkanDevice(VulkanDevice&&) = default;
		VulkanDevice& operator=(VulkanDevice&&) = default;

		const TRAP::Scope<VulkanPhysicalDevice>& GetPhysicalDevice() const;

		const std::vector<std::string>& GetUsedPhysicalDeviceExtensions() const;

	private:
		TRAP::Scope<VulkanPhysicalDevice> m_physicalDevice;

		std::vector<std::string> m_deviceExtensions;
	};
}

#endif /*_TRAP_VULKANDEVICE_H_*/