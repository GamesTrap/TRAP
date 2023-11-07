#ifndef TRAP_VULKANSEMAPHORE_H
#define TRAP_VULKANSEMAPHORE_H

#include "VulkanSwapChain.h"
#include "Graphics/API/Objects/Semaphore.h"

namespace TRAP::Graphics::API
{
	class VulkanQueue;
	class VulkanDevice;

	class VulkanSemaphore final : public Semaphore
	{
	public:
		/// @brief Constructor.
		explicit VulkanSemaphore();
		/// @brief Destructor.
		~VulkanSemaphore() override;

		/// @brief Copy constructor.
		VulkanSemaphore(const VulkanSemaphore&) noexcept = default;
		/// @brief Copy assignment operator.
		VulkanSemaphore& operator=(const VulkanSemaphore&) noexcept = default;
		/// @brief Move constructor.
		VulkanSemaphore(VulkanSemaphore&&) noexcept = default;
		/// @brief Move assignment operator.
		VulkanSemaphore& operator=(VulkanSemaphore&&) noexcept = default;

		/// @brief Retrieve the Vulkan semaphore handle.
		/// @return Vulkan semaphore handle.
		[[nodiscard]] constexpr VkSemaphore GetVkSemaphore() const noexcept;

	private:
#ifndef TRAP_HEADLESS_MODE
		friend std::optional<u32> TRAP::Graphics::API::VulkanSwapChain::AcquireNextImage(const TRAP::Ref<Semaphore>& signalSemaphore,
		                                                                                      const TRAP::Ref<Fence>& fence) const;
#endif /*TRAP_HEADLESS_MODE*/

		VkSemaphore m_semaphore = VK_NULL_HANDLE;

		TRAP::Ref<VulkanDevice> m_device = dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice();
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkSemaphore TRAP::Graphics::API::VulkanSemaphore::GetVkSemaphore() const noexcept
{
	return m_semaphore;
}

#endif /*TRAP_VULKANSEMAPHORE_H*/