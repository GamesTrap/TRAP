#ifndef _TRAP_VULKANRENDERER_H_
#define _TRAP_VULKANRENDERER_H_

#include "Graphics/API/RendererAPI.h"
#include "Maths/Math.h"
#include "Window/WindowingAPI.h"

namespace TRAP::Graphics::API
{
	class VulkanContext;

	namespace Vulkan
	{
		class CommandBuffer;
		class Semaphore;
		class Swapchain;
		class CommandPool;
		class Debug;
		class PhysicalDevice;
		class Instance;
		class Surface;
		class Device;
		class RenderPass;
		class Fence;
	}

	class VulkanRenderer final : public RendererAPI
	{
	public:
		explicit VulkanRenderer();
		~VulkanRenderer();
		
		VulkanRenderer(const VulkanRenderer&) = delete;
		VulkanRenderer& operator=(const VulkanRenderer&) = delete;
		VulkanRenderer(VulkanRenderer&&) = delete;
		VulkanRenderer& operator=(VulkanRenderer&&) = delete;

		void InitInternal() override;

		void Clear(RendererBufferType buffer) override;
		void Present(const Scope<Window>& window) override;

		void SetClearColor(const Math::Vec4& color = { 0.1f, 0.1f, 0.1f, 1.0f }) override;
		void SetDepthTesting(bool enabled) override;
		void SetDepthMasking(bool enabled) override;
		void SetDepthFunction(RendererFunction function) override;
		void SetStencilTesting(bool enabled) override;
		void SetStencilMasking(uint32_t mask) override;
		void SetStencilMaskingSeparate(RendererFaceMode face, uint32_t mask) override;
		void SetStencilFunction(RendererFunction function, int32_t reference, uint32_t mask) override;
		void SetStencilFunctionSeparate(RendererFaceMode face, RendererFunction function, int32_t reference, uint32_t mask) override;
		void SetStencilOperation(RendererOperation stencilFail, RendererOperation depthFail, RendererOperation pass) override;
		void SetStencilOperationSeparate(RendererFaceMode face, RendererOperation stencilFail, RendererOperation depthFail, RendererOperation pass) override;
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

		void SetCullMode(RendererFaceMode cullMode) override;

		void DrawIndexed(const Scope<VertexArray>& vertexArray, uint32_t indexCount, RendererPrimitive primitive) override;
		void Draw(const Scope<VertexArray>& vertexArray, RendererPrimitive primitive) override;

		const std::string& GetTitle() const override;

		std::array<uint8_t, 16> GetCurrentGPUUUID() override;
		std::string GetCurrentGPUName() override;
		std::vector<std::pair<std::string, std::array<uint8_t, 16>>> GetAllGPUs() override;

		//VulkanRenderer specific
		static void SetVSyncIntervalInternal(uint32_t interval);
		static const std::vector<Scope<Vulkan::Swapchain>>& GetAllSwapchains();
		static void SetCurrentSwapchain(Vulkan::Swapchain* swapchain);
		static Vulkan::Swapchain* CreateWindowSwapchain(INTERNAL::WindowingAPI::InternalWindow* window);
		static void DeleteWindowSwapchain(INTERNAL::WindowingAPI::InternalWindow* window);
		static Vulkan::Device& GetDevice();
		static Vulkan::Swapchain& GetCurrentSwapchain();
		static Vulkan::PhysicalDevice& GetPhysicalDevice();
		static void BindGraphicsPipeline();
		
	private:
		static VulkanRenderer* Get();
		
		std::array<std::vector<std::string>, 2> SetupInstanceLayersAndExtensions();
		static std::vector<std::string> SetupDeviceExtensions(const Scope<Vulkan::PhysicalDevice>& physicalDevice);
		
		std::string m_rendererTitle;

		//Vulkan Data
		bool m_debugCallbackSupported;
		Scope<Vulkan::Instance> m_instance;
		Scope<Vulkan::PhysicalDevice> m_physicalDevice;
		Scope<Vulkan::Debug> m_debug;
		Scope<Vulkan::Device> m_device;
		std::vector<Scope<Vulkan::Swapchain>> m_swapchains;

		std::vector<std::pair<std::string, std::array<uint8_t, 16>>> m_deviceNamesAndUUIDs;

		static Vulkan::Swapchain* s_currentSwapchain;
		static VulkanRenderer* s_renderer;
	};
}

#endif /*_TRAP_VULKANRENDERER_H_*/