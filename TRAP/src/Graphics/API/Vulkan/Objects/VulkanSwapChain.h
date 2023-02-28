#ifndef TRAP_VULKANSWAPCHAIN_H
#define TRAP_VULKANSWAPCHAIN_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/SwapChain.h"

namespace TRAP::Graphics::API
{
	class VulkanMemoryAllocator;
	class VulkanInstance;
	class VulkanDevice;
	class VulkanSurface;

	class VulkanSwapChain final : public SwapChain
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="desc">Swap chain description.</param>
		explicit VulkanSwapChain(RendererAPI::SwapChainDesc& desc);
		/// <summary>
		/// Destructor.
		/// </summary>
		~VulkanSwapChain() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanSwapChain(const VulkanSwapChain&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanSwapChain& operator=(const VulkanSwapChain&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanSwapChain(VulkanSwapChain&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanSwapChain& operator=(VulkanSwapChain&&) noexcept = default;

		/// <summary>
		/// Acquire the next presentable image from the swapchain to render to.
		/// </summary>
		/// <param name="signalSemaphore">Semaphore to signal when the image is ready to be presented.</param>
		/// <param name="fence">Fence to wait for the image to be ready to be presented.</param>
		/// <returns>Acuired image index.</returns>
		[[nodiscard]] std::optional<uint32_t> AcquireNextImage(const TRAP::Ref<Semaphore>& signalSemaphore,
		                                                       const TRAP::Ref<Fence>& fence) const override;

		/// <summary>
		/// Toggle Vsync on and off.
		/// </summary>
		void ToggleVSync() override;

		/// <summary>
		/// Retrieve the Vulkan swap chain handle.
		/// </summary>
		/// <returns>Vulkan swap chain handle.</returns>
		[[nodiscard]] VkSwapchainKHR GetVkSwapChain() const noexcept;
		/// <summary>
		/// Retrieve the Vulkan queue used for presentation.
		/// </summary>
		/// <returns>Vulkan queue used for presentation.</returns>
		[[nodiscard]] VkQueue GetPresentVkQueue() const noexcept;

	private:
		/// <summary>
		/// Initialize the swap chain.
		/// </summary>
		/// <param name="desc">Swap chain description.</param>
		void InitSwapchain(RendererAPI::SwapChainDesc& desc);
		/// <summary>
		/// Uninitialize the swap chain.
		/// </summary>
		void DeInitSwapchain();

		TRAP::Ref<VulkanMemoryAllocator> m_vma;
		TRAP::Ref<VulkanInstance> m_instance;
		TRAP::Ref<VulkanDevice> m_device;

		//Present queue if one exists (queuePresent will use this queue if the hardware has a dedicated
		//present queue)
		VkQueue m_presentQueue;
		VkSwapchainKHR m_swapChain;
		TRAP::Ref<VulkanSurface> m_surface;
		uint32_t m_presentQueueFamilyIndex;
		uint32_t m_imageCount;
		bool m_enableVSync;

		RendererAPI::SwapChainDesc m_desc;
	};
}

#endif /*TRAP_VULKANSWAPCHAIN_H*/