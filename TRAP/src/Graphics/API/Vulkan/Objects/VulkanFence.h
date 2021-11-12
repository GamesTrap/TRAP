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
		explicit VulkanFence();
		~VulkanFence() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanFence(const VulkanFence&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanFence& operator=(const VulkanFence&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanFence(VulkanFence&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanFence& operator=(VulkanFence&&) = default;

		VkFence GetVkFence() const;

		RendererAPI::FenceStatus GetStatus() override;

		void Wait() override;

		static void WaitForFences(std::vector<VulkanFence>& fences);

	private:
		friend uint32_t TRAP::Graphics::API::VulkanSwapChain::AcquireNextImage(const TRAP::Ref<Semaphore>& signalSemaphore,
		                                                                       const TRAP::Ref<Fence>& fence) const;

		VkFence m_fence;
		TRAP::Ref<VulkanDevice> m_device;
	};
}

#endif /*TRAP_VULKANFENCE_H*/