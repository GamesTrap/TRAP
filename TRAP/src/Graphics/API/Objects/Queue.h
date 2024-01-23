#ifndef TRAP_QUEUE_H
#define TRAP_QUEUE_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class Queue
	{
	public:
		/// @brief Create a new queue from the given description.
		/// @param desc Queue description.
		/// @return Created queue.
		[[nodiscard]] static TRAP::Ref<Queue> Create(const RendererAPI::QueueDesc& desc);

		/// @brief Destructor.
		virtual ~Queue();

		/// @brief Copy constructor.
		consteval Queue(const Queue&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval Queue& operator=(const Queue&) noexcept = delete;
		/// @brief Move constructor.
		constexpr Queue(Queue&&) noexcept = default;
		/// @brief Move assignment operator.
		Queue& operator=(Queue&&) noexcept = default;

		/// @brief Wait for the queue to finish all submitted commands.
		virtual void WaitQueueIdle() const = 0;

		/// @brief Submit work to the queue.
		/// @param desc Queue submit description.
		virtual void Submit(const RendererAPI::QueueSubmitDesc& desc) const = 0;

		/// @brief Retrieve the queue type.
		/// @return Queue type.
		[[nodiscard]] constexpr RendererAPI::QueueType GetQueueType() const noexcept;

#ifndef TRAP_HEADLESS_MODE
		/// @brief Queue an image for presentation.
		/// @param desc Queue presentation description.
		/// @return Presentation status.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] virtual RendererAPI::PresentStatus Present(const RendererAPI::QueuePresentDesc& desc) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

	protected:
		/// @brief Constructor.
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
