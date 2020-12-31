#ifndef _TRAP_VULKANFENCE_H_
#define _TRAP_VULKANFENCE_H_

#include "VulkanSwapChain.h"
#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics::API
{
	class VulkanQueue;
	class VulkanDevice;

	class VulkanFence
	{
	public:
		explicit VulkanFence(TRAP::Ref<VulkanDevice> device);
		~VulkanFence();

		VkFence& GetVkFence();

		bool IsSubmitted() const;

		RendererAPI::FenceStatus GetStatus();

		void Wait();

		static void WaitForFences(const TRAP::Scope<VulkanDevice>& device, std::vector<VulkanFence>& fences);
		
	private:
		friend VulkanQueue;
		friend uint32_t TRAP::Graphics::API::VulkanSwapChain::AcquireNextImage(const TRAP::Ref<VulkanSemaphore>& signalSemaphore, const TRAP::Ref<VulkanFence>& fence) const;
		
		VkFence m_fence;
		bool m_submitted;
		TRAP::Ref<VulkanDevice> m_device;
	};
}

#endif /*_TRAP_VULKANFENCE_H_*/