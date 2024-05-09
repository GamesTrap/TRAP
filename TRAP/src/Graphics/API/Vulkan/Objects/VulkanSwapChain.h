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

	/// @remark @headless This class is not available in headless mode.
	class VulkanSwapChain final : public SwapChain
	{
	public:
		/// @brief Constructor.
		/// @param desc Swap chain description.
		explicit VulkanSwapChain(RendererAPI::SwapChainDesc& desc);
		/// @brief Destructor.
		~VulkanSwapChain() override;

		/// @brief Copy constructor.
		consteval VulkanSwapChain(const VulkanSwapChain&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval VulkanSwapChain& operator=(const VulkanSwapChain&) noexcept = delete;
		/// @brief Move constructor.
		VulkanSwapChain(VulkanSwapChain&&) noexcept = default;
		/// @brief Move assignment operator.
		VulkanSwapChain& operator=(VulkanSwapChain&&) noexcept = default;

		/// @brief Acquire the next presentable image from the swapchain to render to.
		/// @param signalSemaphore Semaphore to signal when the image is ready to be presented.
		/// @param fence Fence to wait for the image to be ready to be presented.
		/// @return Acuired image index.
		[[nodiscard]] std::optional<u32> AcquireNextImage(const TRAP::Ref<Semaphore>& signalSemaphore,
		                                                       const TRAP::Ref<Fence>& fence) const override;

		/// @brief Toggle VSync on and off.
		void ToggleVSync() override;

		/// @brief Updates the framebuffer size and recreates the swap chain.
		void UpdateFramebufferSize() override;

		/// @brief Retrieve the Vulkan swap chain handle.
		/// @return Vulkan swap chain handle.
		[[nodiscard]] constexpr VkSwapchainKHR GetVkSwapChain() const noexcept;
		/// @brief Retrieve the Vulkan queue used for presentation.
		/// @return Vulkan queue used for presentation.
		[[nodiscard]] constexpr VkQueue GetPresentVkQueue() const noexcept;

	private:
		/// @brief Initialize the swap chain.
		/// @param desc Swap chain description.
		/// @param oldSwapChain Optional: Old Vulkan swapchain, potentially speeds up creation of new swapchain.
		void InitSwapchain(RendererAPI::SwapChainDesc& desc, VkSwapchainKHR* oldSwapChain = nullptr);
		/// @brief Uninitialize the swap chain.
		/// @param allowSwapChainReuse Don't destroy swap chain and surface to allow for reuse on new swap chain creation.
		void DeInitSwapchain(bool allowSwapChainReuse = false);
		/// @brief Reinitialize the swap chain while reusing the old one.
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