#ifndef TRAP_VULKANPHYSICALDEVICE_H
#define TRAP_VULKANPHYSICALDEVICE_H
#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics::API
{
	class VulkanInstance;

	class VulkanPhysicalDevice
	{
	public:
		VulkanPhysicalDevice(const TRAP::Ref<VulkanInstance>& instance,
			                 const std::array<uint8_t, 16>& physicalDeviceUUID);
		~VulkanPhysicalDevice();
		VulkanPhysicalDevice(const VulkanPhysicalDevice&) = default;
		VulkanPhysicalDevice& operator=(const VulkanPhysicalDevice&) = default;
		VulkanPhysicalDevice(VulkanPhysicalDevice&&) = default;
		VulkanPhysicalDevice& operator=(VulkanPhysicalDevice&&) = default;

		VkPhysicalDevice GetVkPhysicalDevice() const;
		const VkPhysicalDeviceProperties& GetVkPhysicalDeviceProperties() const;
		const VkPhysicalDeviceSubgroupProperties& GetVkPhysicalDeviceSubgroupProperties() const;
		const VkPhysicalDeviceIDProperties& GetVkPhysicalDeviceIDProperties() const;
		const VkPhysicalDeviceMemoryProperties& GetVkPhysicalDeviceMemoryProperties() const;
		const VkPhysicalDeviceFeatures& GetVkPhysicalDeviceFeatures() const;
		const VkPhysicalDeviceVulkan11Features& GetVkPhysicalDeviceVulkan11Features() const;
		const VkPhysicalDeviceVulkan12Features& GetVkPhysicalDeviceVulkan12Features() const;
		const VkPhysicalDeviceVulkan11Properties& GetVkPhysicalDeviceVulkan11Properties() const;
		const VkPhysicalDeviceVulkan12Properties& GetVkPhysicalDeviceVulkan12Properties() const;
		const VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT& GetVkPhysicalDeviceFragmentShaderInterlockFeatures() const;
		const std::vector<VkQueueFamilyProperties>& GetQueueFamilyProperties() const;
		bool IsExtensionSupported(const std::string& extension);
		uint32_t GetMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties) const;

		const std::vector<VkExtensionProperties>& GetAvailablePhysicalDeviceExtensions();

		const std::array<uint8_t, 16>& GetPhysicalDeviceUUID() const;

		void RetrievePhysicalDeviceFragmentShaderInterlockFeatures();

		static const std::multimap<uint32_t, std::array<uint8_t, 16>>& GetAllRatedPhysicalDevices(const TRAP::Ref<VulkanInstance>& instance);

		static VkPhysicalDevice FindPhysicalDeviceViaUUID(const TRAP::Ref<VulkanInstance>& instance,
			                                              const std::array<uint8_t, 16>& physicalDeviceUUID);


	private:
		friend bool TRAP::Graphics::RendererAPI::IsVulkanCapable();
		static const std::multimap<uint32_t, std::array<uint8_t, 16>>& GetAllRatedPhysicalDevices(const VkInstance& instance);

		static std::vector<VkPhysicalDevice> GetAllVkPhysicalDevices(const VkInstance& instance);

		static void RatePhysicalDevices(const std::vector<VkPhysicalDevice>& physicalDevices);

		void LoadAllPhysicalDeviceExtensions();

		VkPhysicalDevice m_physicalDevice;
		VkPhysicalDeviceProperties m_physicalDeviceProperties;
		VkPhysicalDeviceSubgroupProperties m_physicalDeviceSubgroupProperties;
		VkPhysicalDeviceIDProperties m_physicalDeviceIDProperties;
		VkPhysicalDeviceMemoryProperties m_physicalDeviceMemoryProperties;
		VkPhysicalDeviceFeatures m_physicalDeviceFeatures;
		VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT m_physicalDeviceFragmentShaderInterlockFeatures;
		VkPhysicalDeviceVulkan11Features m_physicalDeviceVulkan11Features;
		VkPhysicalDeviceVulkan12Features m_physicalDeviceVulkan12Features;
		VkPhysicalDeviceVulkan11Properties m_physicalDeviceVulkan11Properties;
		VkPhysicalDeviceVulkan12Properties m_physicalDeviceVulkan12Properties;
		std::vector<VkQueueFamilyProperties> m_queueFamilyProperties;

		std::array<uint8_t, 16> m_deviceUUID;

		std::vector<VkExtensionProperties> m_availablePhysicalDeviceExtensions;

		static std::multimap<uint32_t, std::array<uint8_t, 16>> s_availablePhysicalDeviceUUIDs;
	};
}

#endif /*TRAP_VULKANPHYSICALDEVICE_H*/