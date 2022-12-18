#ifndef TRAP_QUEUE_H
#define TRAP_QUEUE_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class Queue
	{
	public:
		/// <summary>
		/// Create a new queue from the given description.
		/// </summary>
		/// <param name="desc">Queue description.</param>
		/// <returns>Created queue.</returns>
		static TRAP::Ref<Queue> Create(const RendererAPI::QueueDesc& desc);

		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~Queue();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		Queue(const Queue&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Queue& operator=(const Queue&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		Queue(Queue&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		Queue& operator=(Queue&&) noexcept = default;

		/// <summary>
		/// Wait for the queue to finish all submitted commands.
		/// </summary>
		virtual void WaitQueueIdle() const = 0;

		/// <summary>
		/// Submit work to the queue.
		/// </summary>
		/// <param name="desc">Queue submit description.</param>
		virtual void Submit(const RendererAPI::QueueSubmitDesc& desc) const = 0;

		/// <summary>
		/// Queue an image for presentation.
		/// </summary>
		/// <param name="desc">Queue presentation description.</param>
		/// <returns>Presentation status.</returns>
		virtual RendererAPI::PresentStatus Present(const RendererAPI::QueuePresentDesc& desc) const = 0;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		Queue();

		//No Graphic API independent data
	};
}

#endif /*TRAP_QUEUE_H*/