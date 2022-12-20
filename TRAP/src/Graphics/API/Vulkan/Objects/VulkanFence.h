#ifndef TRAP_VULKANFENCE_H
#define TRAP_VULKANFENCE_H

#include "VulkanSwapChain.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/Fence.h"

namespace TRAP::Graphics::API
{
	class VulkanQueue;
	class VulkanDevice;

	class VulkanFence final : public Fence
	{
	public:
		/// <summary>
		/// Constructor.
		/// Fence is not signaled.
		/// </summary>
		explicit VulkanFence();
		/// <summary>
		/// Destructor.
		/// </summary>
		~VulkanFence() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanFence(const VulkanFence&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanFence& operator=(const VulkanFence&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanFence(VulkanFence&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanFence& operator=(VulkanFence&&) noexcept = default;

		/// <summary>
		/// Retrieve the VkFence handle.
		/// </summary>
		/// <returns>VkFence handle.</returns>
		VkFence GetVkFence() const noexcept;

		/// <summary>
		/// Retrieve the current status of the fence.
		/// </summary>
		/// <returns>Fence status.</returns>
		RendererAPI::FenceStatus GetStatus() override;

		/// <summary>
		/// Wait for the fence to be signaled.
		///
		/// 1. Waits for the fence to be signaled.
		/// 2. Resets the fence.
		/// </summary>
		void Wait() override;

	private:
		friend uint32_t TRAP::Graphics::API::VulkanSwapChain::AcquireNextImage(const TRAP::Ref<Semaphore>& signalSemaphore,
		                                                                       const TRAP::Ref<Fence>& fence) const;

		VkFence m_fence;
		TRAP::Ref<VulkanDevice> m_device;
	};
}

#endif /*TRAP_VULKANFENCE_H*/