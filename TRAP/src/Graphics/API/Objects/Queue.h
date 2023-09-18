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
		[[nodiscard]] static TRAP::Ref<Queue> Create(const RendererAPI::QueueDesc& desc);

		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~Queue();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr Queue(const Queue&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Queue& operator=(const Queue&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr Queue(Queue&&) noexcept = default;
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
		/// Retrieve the queue type.
		/// </summary>
		/// <returns>Queue type.</returns>
		[[nodiscard]] constexpr RendererAPI::QueueType GetQueueType() const noexcept;

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Queue an image for presentation.
		/// </summary>
		/// <param name="desc">Queue presentation description.</param>
		/// <returns>Presentation status.</returns>
		[[nodiscard]] virtual RendererAPI::PresentStatus Present(const RendererAPI::QueuePresentDesc& desc) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		explicit Queue(RendererAPI::QueueType queueType);

		RendererAPI::QueueType m_type = RendererAPI::QueueType::Graphics;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::QueueType TRAP::Graphics::Queue::GetQueueType() const noexcept
{
	return m_type;
}

#endif /*TRAP_QUEUE_H*/