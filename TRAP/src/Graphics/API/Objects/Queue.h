#ifndef TRAP_QUEUE_H
#define TRAP_QUEUE_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class Queue
	{
	public:
		static TRAP::Ref<Queue> Create(const RendererAPI::QueueDesc& desc);

		virtual ~Queue();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		Queue(const Queue&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Queue& operator=(const Queue&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		Queue(Queue&&) = default;
		/// <summary>
		/// Move assignment operator.
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

#endif /*TRAP_QUEUE_H*/