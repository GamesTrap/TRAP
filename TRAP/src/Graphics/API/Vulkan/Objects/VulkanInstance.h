#ifndef _TRAP_VULKANINSTANCE_H_
#define _TRAP_VULKANINSTANCE_H_

namespace TRAP::Graphics::API
{
	class VulkanInstance
	{
	public:
		VulkanInstance(const std::string& appName, std::vector<std::string> instanceLayers,
		               std::vector<std::string> instanceExtensions);
		~VulkanInstance();
		VulkanInstance(const VulkanInstance&) = default;
		VulkanInstance& operator=(const VulkanInstance&) = default;
		VulkanInstance(VulkanInstance&&) = default;
		VulkanInstance& operator=(VulkanInstance&&) = default;

		VkInstance& GetVkInstance();
		const std::vector<std::string>& GetUsedInstanceLayers() const;
		const std::vector<std::string>& GetUsedInstanceExtensions() const;

		static uint32_t GetInstanceVersion();
		static const std::vector<VkLayerProperties>& GetAvailableInstanceLayers();
		static const std::vector<VkExtensionProperties>& GetAvailableInstanceExtensions();
		static bool IsLayerSupported(const std::string& layer);
		static bool IsExtensionSupported(const std::string& extension);

	private:
		static void LoadAllInstanceLayers();
		static void LoadAllInstanceExtensions();

		static uint32_t s_instanceVersion;

		VkInstance m_instance;

		std::vector<std::string> m_instanceLayers;
		std::vector<std::string> m_instanceExtensions;

		static std::vector<VkLayerProperties> s_availableInstanceLayers;
		static std::vector<VkExtensionProperties> s_availableInstanceExtensions;
	};
}

#endif /*_TRAP_VULKANINSTANCE_H_*/