#ifndef TRAP_FENCE_H
#define TRAP_FENCE_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/Objects/VulkanQueue.h"

namespace TRAP::Graphics
{
	class Fence
	{
	public:
		static TRAP::Ref<Fence> Create();

		virtual ~Fence();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		Fence(const Fence&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Fence& operator=(const Fence&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		Fence(Fence&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		Fence& operator=(Fence&&) = default;

		virtual bool IsSubmitted() const;

		virtual RendererAPI::FenceStatus GetStatus() = 0;
		virtual void Wait() = 0;

		static void WaitForFence(std::vector<Fence>& fences);

	protected:
		Fence();

		bool m_submitted;

	private:
		friend void TRAP::Graphics::API::VulkanQueue::Submit(const RendererAPI::QueueSubmitDesc& desc) const;
		friend TRAP::Graphics::RendererAPI::PresentStatus TRAP::Graphics::API::VulkanQueue::Present(const RendererAPI::QueuePresentDesc& desc) const;
	};
}

#endif /*TRAP_FENCE_H*/