#ifndef TRAP_VULKANSWAPCHAIN_H
#define TRAP_VULKANSWAPCHAIN_H

#include "Graphics/API/Vulkan/VulkanRenderer.h"
#ifndef TRAP_HEADLESS_MODE

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
		[[nodiscard]] std::optional<u32> AcquireNextImage(const TRAP::Ref<Semaphore>& signalSemaphore,
		                                                       const TRAP::Ref<Fence>& fence) const override;

		/// <summary>
		/// Toggle VSync on and off.
		/// </summary>
		void ToggleVSync() override;

		/// <summary>
		/// Updates the framebuffer size and recreates the swap chain.
		/// </summary>
		void UpdateFramebufferSize() override;

		/// <summary>
		/// Retrieve the Vulkan swap chain handle.
		/// </summary>
		/// <returns>Vulkan swap chain handle.</returns>
		[[nodiscard]] constexpr VkSwapchainKHR GetVkSwapChain() const noexcept;
		/// <summary>
		/// Retrieve the Vulkan queue used for presentation.
		/// </summary>
		/// <returns>Vulkan queue used for presentation.</returns>
		[[nodiscard]] constexpr VkQueue GetPresentVkQueue() const noexcept;

	private:
		/// <summary>
		/// Initialize the swap chain.
		/// </summary>
		/// <param name="desc">Swap chain description.</param>
		void InitSwapchain(RendererAPI::SwapChainDesc& desc, VkSwapchainKHR oldSwapChain = VK_NULL_HANDLE);
		/// <summary>
		/// Uninitialize the swap chain.
		/// </summary>
		/// <param name="allowSwapChainReuse">Don't destroy swap chain and surface to allow for reuse on new swap chain creation.</param>
		void DeInitSwapchain(bool allowSwapChainReuse = false);
		/// <summary>
		/// Reinitialize the swap chain while reusing the old one.
		/// </summary>
		void ReInitSwapChain();

		TRAP::Ref<VulkanMemoryAllocator> m_vma = dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetVMA();
		TRAP::Ref<VulkanInstance> m_instance = dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetInstance();
		TRAP::Ref<VulkanDevice> m_device = dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice();

		//Present queue if one exists (queuePresent will use this queue if the hardware has a dedicated
		//present queue)
		VkQueue m_presentQueue = VK_NULL_HANDLE;
		VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
		TRAP::Ref<VulkanSurface> m_surface = nullptr;
		u32 m_presentQueueFamilyIndex = 0;
		u32 m_imageCount = 0;
		bool m_enableVSync = false;

		RendererAPI::SwapChainDesc m_desc{};
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkSwapchainKHR TRAP::Graphics::API::VulkanSwapChain::GetVkSwapChain() const noexcept
{
	return m_swapChain;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkQueue TRAP::Graphics::API::VulkanSwapChain::GetPresentVkQueue() const noexcept
{
	return m_presentQueue;
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_VULKANSWAPCHAIN_H*/