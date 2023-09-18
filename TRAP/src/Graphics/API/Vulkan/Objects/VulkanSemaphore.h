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
		/// <summary>
		/// Constructor.
		/// </summary>
		explicit VulkanSemaphore();
		/// <summary>
		/// Destructor.
		/// </summary>
		~VulkanSemaphore() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanSemaphore(const VulkanSemaphore&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanSemaphore& operator=(const VulkanSemaphore&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanSemaphore(VulkanSemaphore&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanSemaphore& operator=(VulkanSemaphore&&) noexcept = default;

		/// <summary>
		/// Retrieve the Vulkan semaphore handle.
		/// </summary>
		/// <returns>Vulkan semaphore handle.</returns>
		[[nodiscard]] constexpr VkSemaphore GetVkSemaphore() const noexcept;

	private:
#ifndef TRAP_HEADLESS_MODE
		friend std::optional<uint32_t> TRAP::Graphics::API::VulkanSwapChain::AcquireNextImage(const TRAP::Ref<Semaphore>& signalSemaphore,
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