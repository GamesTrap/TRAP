#ifndef _TRAP_SEMAPHORE_H_
#define _TRAP_SEMAPHORE_H_

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/Objects/VulkanQueue.h"

namespace TRAP::Graphics
{
	class Semaphore
	{
	public:
		static TRAP::Ref<Semaphore> Create();

		virtual ~Semaphore();

		virtual bool IsSignaled() const;

	protected:
		Semaphore();

		bool m_signaled;

	private:
		friend void TRAP::Graphics::API::VulkanQueue::Submit(const RendererAPI::QueueSubmitDesc& desc) const;
		friend TRAP::Graphics::RendererAPI::PresentStatus TRAP::Graphics::API::VulkanQueue::Present(const RendererAPI::QueuePresentDesc& desc) const;
	};
}

#endif /*_TRAP_SEMAPHORE_H_*/