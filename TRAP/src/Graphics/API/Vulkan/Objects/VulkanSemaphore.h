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
		explicit VulkanSemaphore();
		~VulkanSemaphore() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanSemaphore(const VulkanSemaphore&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanSemaphore& operator=(const VulkanSemaphore&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanSemaphore(VulkanSemaphore&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanSemaphore& operator=(VulkanSemaphore&&) = default;

		VkSemaphore GetVkSemaphore() const;

	private:
		friend uint32_t TRAP::Graphics::API::VulkanSwapChain::AcquireNextImage(const TRAP::Ref<Semaphore>& signalSemaphore,
		                                                                       const TRAP::Ref<Fence>& fence) const;

		VkSemaphore m_semaphore;

		TRAP::Ref<VulkanDevice> m_device;
	};
}

#endif /*TRAP_VULKANSEMAPHORE_H*/