#ifndef _TRAP_VULKANINSTANCE_H_
#define _TRAP_VULKANINSTANCE_H_

namespace TRAP::Graphics::API::Vulkan
{
	class Instance
	{
	public:
		Instance(const char* gameName, uint32_t gameVersion);
		Instance(const char* gameName, uint32_t gameVersion, const std::vector<std::string>& instanceLayers, const std::vector<std::string>& instanceExtensions);
		~Instance();
		Instance(const Instance&) = default;
		Instance& operator=(const Instance&) = default;
		Instance(Instance&&) = default;
		Instance& operator=(Instance&&) = default;

		VkInstance& GetInstance();
		const std::vector<std::string>& GetUsedLayers() const;
		const std::vector<std::string>& GetUsedExtensions() const;
		static const std::vector<VkLayerProperties>& GetAvailableLayers();
		static const std::vector<VkExtensionProperties>& GetAvailableExtensions();
		static bool IsLayerSupported(const char* layer);
		static bool IsExtensionSupported(const char* extension);
		
	private:
		VkInstance m_instance;
		std::vector<std::string> m_layers;
		std::vector<std::string> m_extensions;

		static std::vector<VkLayerProperties> s_availableLayers;
		static std::vector<VkExtensionProperties> s_availableExtensions;

		static bool s_metaLoaderInitialized;
	};
}

#endif /*_TRAP_VULKANINSTANCE_H_*/