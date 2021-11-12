#ifndef TRAP_SEMAPHORE_H
#define TRAP_SEMAPHORE_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/Objects/VulkanQueue.h"

namespace TRAP::Graphics
{
	class Semaphore
	{
	public:
		static TRAP::Ref<Semaphore> Create();

		virtual ~Semaphore();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		Semaphore(const Semaphore&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Semaphore& operator=(const Semaphore&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		Semaphore(Semaphore&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		Semaphore& operator=(Semaphore&&) = default;

		virtual bool IsSignaled() const;

	protected:
		Semaphore();

		bool m_signaled;

	private:
		friend void TRAP::Graphics::API::VulkanQueue::Submit(const RendererAPI::QueueSubmitDesc& desc) const;
		friend TRAP::Graphics::RendererAPI::PresentStatus TRAP::Graphics::API::VulkanQueue::Present(const RendererAPI::QueuePresentDesc& desc) const;
	};
}

#endif /*TRAP_SEMAPHORE_H*/