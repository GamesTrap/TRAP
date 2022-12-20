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
		VkSemaphore GetVkSemaphore() const noexcept;

	private:
		friend uint32_t TRAP::Graphics::API::VulkanSwapChain::AcquireNextImage(const TRAP::Ref<Semaphore>& signalSemaphore,
		                                                                       const TRAP::Ref<Fence>& fence) const;

		VkSemaphore m_semaphore;

		TRAP::Ref<VulkanDevice> m_device;
	};
}

#endif /*TRAP_VULKANSEMAPHORE_H*/