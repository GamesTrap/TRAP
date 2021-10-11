#ifndef _TRAP_QUEUE_H_
#define _TRAP_QUEUE_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class Queue
	{
	public:
		static TRAP::Ref<Queue> Create(const RendererAPI::QueueDesc& desc);

		virtual ~Queue();

		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		Queue(const Queue&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		Queue& operator=(const Queue&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		Queue(Queue&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		Queue& operator=(Queue&&) = default;

		virtual void WaitQueueIdle() const = 0;

		virtual void Submit(const RendererAPI::QueueSubmitDesc& desc) const = 0;

		virtual RendererAPI::PresentStatus Present(const RendererAPI::QueuePresentDesc& desc) const = 0;

	protected:
		Queue();

		//No Graphic API independent data
	};
}

#endif /*_TRAP_QUEUE_H_*/