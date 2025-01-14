#ifndef TRAP_SEMAPHORE_H
#define TRAP_SEMAPHORE_H

#include "Core/Base.h"
#include "Graphics/API/Vulkan/Objects/VulkanQueue.h"

namespace TRAP::Graphics
{
	enum class SemaphoreType : u8
	{
		Binary,
		Timeline
	};

	class Semaphore
	{
	public:
		/// @brief Create a new semaphore.
		/// @param semaphoreType Type of semaphore to create. Default: Binary.
		/// @param name Optional: Debug name used in GPU-profile
		/// @return Created semaphore.
		[[nodiscard]] static TRAP::Ref<Semaphore> Create(SemaphoreType semaphoreType = SemaphoreType::Binary, const std::string& name = "");

		/// @brief Destructor.
		constexpr virtual ~Semaphore();

		/// @brief Copy constructor.
		consteval Semaphore(const Semaphore&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval Semaphore& operator=(const Semaphore&) noexcept = delete;
		/// @brief Move constructor.
		constexpr Semaphore(Semaphore&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr Semaphore& operator=(Semaphore&&) noexcept = default;

		/// @brief Is the semaphore signaled?
		/// @return True if the semaphore is signaled, false otherwise.
		[[nodiscard]] constexpr bool IsSignaled() const noexcept;

	protected:
		/// @brief Constructor.
		constexpr Semaphore();

		bool m_signaled = false;

	private:
		friend void TRAP::Graphics::API::VulkanQueue::Submit(const QueueSubmitDesc& desc) const;

#ifndef TRAP_HEADLESS_MODE
		friend TRAP::Graphics::PresentStatus TRAP::Graphics::API::VulkanQueue::Present(const QueuePresentDesc& desc) const;
#endif /*TRAP_HEADLESS_MODE*/
	};
}

constexpr TRAP::Graphics::Semaphore::~Semaphore()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererSemaphorePrefix, "Destroying Semaphore");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::Semaphore::Semaphore()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererSemaphorePrefix, "Creating Semaphore");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Graphics::Semaphore::IsSignaled() const noexcept
{
	return m_signaled;
}

#endif /*TRAP_SEMAPHORE_H*/
