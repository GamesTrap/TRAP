#ifndef _TRAP_VULKANRENDERER_H_
#define _TRAP_VULKANRENDERER_H_

#include "Graphics/API/RendererAPI.h"
#include "Maths/Math.h"
#include "VulkanContext.h"

namespace TRAP::Graphics::API
{
	class VulkanRenderer final : public RendererAPI
	{
	public:
		VulkanRenderer();
		~VulkanRenderer();
		VulkanRenderer(const VulkanRenderer&) = default;
		VulkanRenderer& operator=(const VulkanRenderer&) = default;
		VulkanRenderer(VulkanRenderer&&) = default;
		VulkanRenderer& operator=(VulkanRenderer&&) = default;

		void InitInternal() override;

		void Clear(RendererBufferType buffer) override;
		void Present(const Scope<Window>& window) override;

		void SetClearColor(const Math::Vec4& color = { 0.1f, 0.1f, 0.1f, 1.0f }) override;
		void SetDepthTesting(bool enabled) override;
		void SetDepthMasking(bool enabled) override;
		void SetDepthFunction(RendererDepthFunction function) override;
		void SetBlend(bool enabled) override;
		void SetCull(bool enabled) override;
		void SetFrontFace(RendererFrontFace frontFace) override;
		void SetWireFrame(bool enabled) override;
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		void SetBlendFunction(RendererBlendFunction source, RendererBlendFunction destination) override;
		void SetBlendFunctionSeparate(RendererBlendFunction sourceRGB,
		                              RendererBlendFunction sourceAlpha,
		                              RendererBlendFunction destinationRGB,
		                              RendererBlendFunction destinationAlpha) override;
		void SetBlendEquation(RendererBlendEquation blendEquation) override;
		void SetBlendEquationSeparate(RendererBlendEquation blendEquationRGB, RendererBlendEquation blendEquationAlpha) override;

		void SetCullMode(RendererCullMode cullMode) override;

		void DrawIndexed(const Scope<VertexArray>& vertexArray, RendererPrimitive primitive) override;
		void Draw(const Scope<VertexArray>& vertexArray, RendererPrimitive primitive) override;

		std::string_view GetTitle() const override;

		static VulkanRenderer* Get();
		VkInstance& GetInstance();
		VkPhysicalDevice& GetPhysicalDevice();
		std::optional<uint32_t>& GetGraphicsQueueFamilyIndex();
		std::optional<uint32_t>& GetPresentQueueFamilyIndex();
		VkDevice& GetDevice();
		std::vector<VkCommandBuffer>& GetCommandBuffers();
		VkQueue GetGraphicsQueue() const;
		VkQueue GetPresentQueue() const;

		void InitGraphicsPipeline(const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages);
		void DeInitGraphicsPipeline();
		void BindGraphicsPipeline();

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
		
		void InitRenderPass();
		void DeInitRenderPass();
		
		void InitPipelineLayout();
		void DeInitPipelineLayout();

		void InitCommandPool();
		void DeInitCommandPool();

		void InitCommandBuffers();
		void DeInitCommandBuffers();
		
		void StartRecording();
		void StartRenderPass();
		void StopRenderPass();
		void StopRecording();

		void PickPhysicalDevice(std::vector<VkPhysicalDevice>& availablePhysicalDevices);
		int32_t RateDeviceSuitability(VkPhysicalDevice physicalDevice) const;

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

		void AddInstanceLayer(const std::vector<VkLayerProperties>& availableInstanceLayers, const std::string& layer);
		void AddInstanceExtension(const std::vector<VkExtensionProperties>& availableInstanceExtensions, const std::string& extension);
		void AddDeviceLayer(const std::vector<VkLayerProperties>& availableDeviceLayers, const std::string& layer);
		void AddDeviceExtension(const std::vector<VkExtensionProperties>& availableDeviceExtensions, const std::string& extension);

		std::vector<std::string> m_instanceExtensions;
		std::vector<std::string> m_instanceLayers;
		VkInstance m_instance;

		VkPhysicalDevice m_physicalDevice;
		VkPhysicalDeviceProperties m_physicalDeviceProperties;
		VkPhysicalDeviceMemoryProperties m_physicalDeviceMemoryProperties;
		VkDevice m_device;
		VkQueue m_graphicsQueue;
		VkQueue m_presentQueue;

		std::optional<uint32_t> m_graphicsFamilyIndex;
		std::optional<uint32_t> m_presentFamilyIndex;
		
		std::vector<std::string> m_deviceExtensions;
		std::vector<std::string> m_deviceLayers; //Only for compatibility(Deprecated See Vulkan Specification)
		
		bool m_debugCallbackSupported;
		VkDebugUtilsMessengerEXT m_debugReport;

		VkRenderPass m_renderPass;
		VkPipelineLayout m_pipelineLayout;
		VkPipeline m_graphicsPipeline;

		VkCommandPool m_commandPool;
		std::vector<VkCommandBuffer> m_commandBuffers;

		VulkanContext* m_context;
		std::string m_rendererTitle;

		VkClearValue m_clearColor;
	};
}

#endif /*_TRAP_VULKANRENDERER_H_*/