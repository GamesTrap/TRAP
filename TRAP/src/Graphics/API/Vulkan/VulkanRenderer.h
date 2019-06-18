#ifndef _TRAP_VULKANRENDERER_H_
#define _TRAP_VULKANRENDERER_H_

namespace TRAP::Graphics::API
{
	class VulkanRenderer final : public Renderer
	{
	public:
		VulkanRenderer();
		~VulkanRenderer();
		VulkanRenderer(const VulkanRenderer&) = default;
		VulkanRenderer& operator=(const VulkanRenderer&) = default;
		VulkanRenderer(VulkanRenderer&&) = default;
		VulkanRenderer& operator=(VulkanRenderer&&) = default;

		void InitInternal() override;

		void ClearInternal(unsigned int buffer) override;
		void PresentInternal(Window* window) override;

		void SetDepthTestingInternal(bool enabled) override;
		void SetBlendInternal(bool enabled) override;
		void SetCullInternal(bool enabled) override;
		void SetFrontFaceInternal(RendererFrontFace frontFace) override;
		void SetWireFrameInternal(bool enabled) override;
		void SetViewportInternal(unsigned int x, unsigned int y, unsigned int width, unsigned int height) override;

		void SetBlendFunctionInternal(RendererBlendFunction source, RendererBlendFunction destination) override;
		void SetBlendEquationInternal(RendererBlendEquation blendEquation) override;

		void SetCullModeInternal(RendererCullMode cullMode) override;

		std::string_view GetTitleInternal() const override;

		static VulkanRenderer* Get();
		VkInstance& GetInstance();
		VkPhysicalDevice& GetPhysicalDevice();
		std::optional<uint32_t>& GetGraphicsQueueFamilyIndex();
		std::optional<uint32_t>& GetPresentQueueFamilyIndex();
		VkDevice& GetDevice();

	private:
		void SetupInstanceLayersAndExtensions();
		void SetupDeviceLayersAndExtensions();
		void SetupPhysicalDevice();
		void SetupQueues();

		void InitInstance();
		void DeInitInstance();

		void InitDebug();
		void DeInitDebug();

		void InitDevice();
		void DeInitDevice();

		void PickPhysicalDevice(std::vector<VkPhysicalDevice>& availablePhysicalDevices);
		int RateDeviceSuitability(VkPhysicalDevice physicalDevice) const;

		static bool IsLayerSupported(const std::vector<VkLayerProperties>& availableLayers, const char* layer);
		static bool IsExtensionSupported(const std::vector<VkExtensionProperties>& availableExtensions, const char* extension);

		std::vector<VkPhysicalDevice> GetAvailablePhysicalDevices() const;
		static std::vector<VkLayerProperties> GetAvailableInstanceLayers();
		static std::vector<VkExtensionProperties> GetAvailableInstanceExtensions();
		static std::vector<VkLayerProperties> GetAvailableDeviceLayers();
		std::vector<VkExtensionProperties> GetAvailableDeviceExtensions() const;
		static std::vector<VkExtensionProperties> GetAvailableDeviceExtensions(VkPhysicalDevice physicalDevice);
		std::vector<VkQueueFamilyProperties> GetAvailableQueueFamilies() const;
		static std::vector<VkQueueFamilyProperties> GetAvailableQueueFamilies(VkPhysicalDevice physicalDevice);

		void AddInstanceLayer(const std::vector<VkLayerProperties>& availableInstanceLayers, const char* layer);
		void AddInstanceExtension(const std::vector<VkExtensionProperties>& availableInstanceExtensions, const char* extension);
		void AddDeviceLayer(const std::vector<VkLayerProperties>& availableDeviceLayers, const char* layer);
		void AddDeviceExtension(const std::vector<VkExtensionProperties>& availableDeviceExtensions, const char* extension);

		std::vector<const char*> m_instanceExtensions;
		std::vector<const char*> m_instanceLayers;
		VkInstance m_instance;

		VkPhysicalDevice m_physicalDevice;
		VkPhysicalDeviceProperties m_physicalDeviceProperties;
		VkPhysicalDeviceMemoryProperties m_physicalDeviceMemoryProperties;
		VkDevice m_device;
		VkQueue m_graphicsQueue;
		VkQueue m_presentQueue;

		std::optional<uint32_t> m_graphicsFamilyIndex;
		std::optional<uint32_t> m_presentFamilyIndex;
		
		std::vector<const char*> m_deviceExtensions;
		std::vector<const char*> m_deviceLayers; //Only for compatibility(Deprecated See Vulkan Specification)

		bool m_debugCallbackSupported;
		VkDebugUtilsMessengerEXT m_debugReport;		

		VulkanContext* m_context;
		std::string m_rendererTitle;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string_view TRAP::Graphics::API::VulkanRenderer::GetTitleInternal() const
{
	return m_rendererTitle;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::VulkanRenderer* TRAP::Graphics::API::VulkanRenderer::Get()
{
	return reinterpret_cast<VulkanRenderer*>(s_Renderer.get());
}

//-------------------------------------------------------------------------------------------------------------------//

inline VkInstance& TRAP::Graphics::API::VulkanRenderer::GetInstance()
{
	return m_instance;
}

//-------------------------------------------------------------------------------------------------------------------//

inline VkPhysicalDevice& TRAP::Graphics::API::VulkanRenderer::GetPhysicalDevice()
{
	return m_physicalDevice;
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::optional<uint32_t>& TRAP::Graphics::API::VulkanRenderer::GetGraphicsQueueFamilyIndex()
{
	return m_graphicsFamilyIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::optional<uint32_t>& TRAP::Graphics::API::VulkanRenderer::GetPresentQueueFamilyIndex()
{
	return m_presentFamilyIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

inline VkDevice& TRAP::Graphics::API::VulkanRenderer::GetDevice()
{
	return m_device;
}

#endif /*_TRAP_VULKANRENDERER_H_*/