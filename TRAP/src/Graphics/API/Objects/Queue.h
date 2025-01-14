#ifndef TRAP_QUEUE_H
#define TRAP_QUEUE_H

#include "Core/Base.h"
#include "Core/Types.h"
#include "Utils/SmartPtr.h"

namespace TRAP::Graphics
{
	struct QueueDesc;
	struct QueueSubmitDesc;
	struct QueuePresentDesc;
    enum class QueueType : u8;
    enum class PresentStatus : u8;

	class Queue
	{
	public:
		/// @brief Create a new queue from the given description.
		/// @param desc Queue description.
		/// @return Created queue.
		[[nodiscard]] static TRAP::Ref<Queue> Create(const QueueDesc& desc);

		/// @brief Destructor.
		constexpr virtual ~Queue();

		/// @brief Copy constructor.
		consteval Queue(const Queue&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval Queue& operator=(const Queue&) noexcept = delete;
		/// @brief Move constructor.
		constexpr Queue(Queue&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr Queue& operator=(Queue&&) noexcept = default;

		/// @brief Wait for the queue to finish all submitted commands.
		virtual void WaitQueueIdle() const = 0;

		/// @brief Submit work to the queue.
		/// @param desc Queue submit description.
		virtual void Submit(const QueueSubmitDesc& desc) const = 0;

		/// @brief Retrieve the queue type.
		/// @return Queue type.
		[[nodiscard]] constexpr QueueType GetQueueType() const noexcept;

#ifndef TRAP_HEADLESS_MODE
		/// @brief Queue an image for presentation.
		/// @param desc Queue presentation description.
		/// @return Presentation status.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] virtual PresentStatus Present(const QueuePresentDesc& desc) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

	protected:
		/// @brief Constructor.
		constexpr explicit Queue(QueueType queueType);

		QueueType m_type;

	private:
		static void DebugLog(std::string_view msg);
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::Queue::~Queue()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	if(!std::is_constant_evaluated())
	{
		DebugLog("Destroying Queue");
	}
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::Queue::Queue(const QueueType queueType)
	: m_type(queueType)
{
#ifdef ENABLE_GRAPHICS_DEBUG
	if(!std::is_constant_evaluated())
	{
		DebugLog("Creating Queue");
	}
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::QueueType TRAP::Graphics::Queue::GetQueueType() const noexcept
{
	return m_type;
}

#endif /*TRAP_QUEUE_H*/
