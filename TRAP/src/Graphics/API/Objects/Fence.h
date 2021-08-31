#ifndef _TRAP_FENCE_H_
#define _TRAP_FENCE_H_

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/Objects/VulkanQueue.h"

namespace TRAP::Graphics
{
	class Fence
	{
	public:
		static TRAP::Ref<Fence> Create();

		virtual ~Fence();

		virtual bool IsSubmitted() const;

		virtual RendererAPI::FenceStatus GetStatus() = 0;
		virtual void Wait() = 0;

		static void WaitForFence(std::vector<Fence>& fences);

	protected:
		Fence();

		bool m_submitted;

	private:
		friend void TRAP::Graphics::API::VulkanQueue::Submit(const RendererAPI::QueueSubmitDesc& desc) const;
		friend TRAP::Graphics::RendererAPI::PresentStatus TRAP::Graphics::API::VulkanQueue::Present(const RendererAPI::QueuePresentDesc& desc) const;
	};
}

#endif /*_TRAP_FENCE_H_*/