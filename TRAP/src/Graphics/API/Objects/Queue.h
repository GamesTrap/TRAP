#ifndef _TRAP_QUEUE_H_
#define _TRAP_QUEUE_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class Queue
	{
	public:
		static TRAP::Ref<Queue> Create(const RendererAPI::QueueDesc& desc);

		Queue() = default;
		virtual ~Queue() = default;

		virtual void WaitQueueIdle() const = 0;
		
		virtual void Submit(const RendererAPI::QueueSubmitDesc& desc) const = 0;

		virtual RendererAPI::PresentStatus Present(const RendererAPI::QueuePresentDesc& desc) const = 0;
	
	protected:
		//No Graphic API independent data
	};
}

#endif /*_TRAP_QUEUE_H_*/