#ifndef _TRAP_VULKANSWAPCHAIN_H_
#define _TRAP_VULKANSWAPCHAIN_H_

#include "Window/WindowingAPI.h"

namespace TRAP::Graphics::API::Vulkan
{
	class Fence;
	class CommandBuffer;
	class RenderPass;
	class CommandPool;
	class Semaphore;
	class Pipeline;
	class Device;
	class Surface;

	class Swapchain
	{
	public:
		Swapchain(INTERNAL::WindowingAPI::InternalWindow* window, Scope<Surface>& surface, const Scope<Device>& device);
		~Swapchain();
		Swapchain(const Swapchain&) = delete;
		Swapchain& operator=(const Swapchain&) = delete;
		Swapchain(Swapchain&&) = default;
		Swapchain& operator=(Swapchain&&) = default;

		void SetVsync(bool vsync);

		INTERNAL::WindowingAPI::InternalWindow* GetBoundWindow() const;
		VkSwapchainKHR& GetSwapchain();
		Surface& GetSurface() const;
		RenderPass& GetRenderPass() const;
		Pipeline& GetPipeline() const;
		std::vector<VkFramebuffer>& GetFrameBuffers();
		VkExtent2D GetExtent() const;
		CommandPool& GetGraphicsCommandPool() const;
		CommandPool& GetComputeCommandPool() const;
		CommandPool& GetTransferCommandPool() const;
		CommandBuffer& GetGraphicsCommandBuffer() const;

		VkResult AcquireNextImage(const Scope<Semaphore>& presentCompleteSemaphore);
		VkResult QueuePresent(VkQueue& presentQueue, const Scope<Semaphore>& waitSemaphore);
		uint32_t& GetCurrentFrameBufferIndex();
		
		void CreateSwapchain();
		void RecreateSwapchain();

		void PrepareNextFrame();
		void StartGraphicCommandBuffersAndRenderPass() const;
		void EndGraphicsCommandBufferAndRenderPass() const;

		const Scope<Fence>& GetWaitFence() const;
		const Scope<Semaphore>& GetRenderSemaphore() const;
		const Scope<Semaphore>& GetPresentSemaphore() const;
		
	private:		
		INTERNAL::WindowingAPI::InternalWindow* m_boundWindow;
		VkSwapchainKHR m_swapchain;
		VkPresentModeKHR m_presentMode;
		Scope<Surface> m_surface;
		VkExtent2D m_extent;
		Device* m_device;
		uint32_t m_imageCount;
		std::vector<VkImage> m_swapchainImages;
		std::vector<VkImageView> m_swapchainImageViews;
		std::vector<VkFramebuffer> m_swapchainFrameBuffers;
		Scope<RenderPass> m_renderPass;
		Scope<Pipeline> m_pipeline;
		VkSwapchainKHR m_oldSwapchain;
		Scope<CommandPool> m_graphicsCommandPool;
		Scope<CommandPool> m_computeCommandPool;
		Scope<CommandPool> m_transferCommandPool;
		Scope<CommandBuffer> m_graphicsCommandBuffer;

		Scope<Fence> m_waitFence;
		Scope<Semaphore> m_renderCompleteSemaphore;
		Scope<Semaphore> m_presentCompleteSemaphore;
		
		uint32_t m_currentFrameBuffer;
	};
}

#endif /*_TRAP_VULKANSWAPCHAIN_H_*/