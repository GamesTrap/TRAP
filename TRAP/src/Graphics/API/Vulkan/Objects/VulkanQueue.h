#ifndef _TRAP_VULKANQUEUE_H_
#define _TRAP_VULKANQUEUE_H_

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/Queue.h"

namespace TRAP::Graphics::API
{
	class VulkanDevice;

	class VulkanQueue final : public Queue
	{
	public:
		explicit VulkanQueue(const RendererAPI::QueueDesc& desc);
		~VulkanQueue();

		VkQueue& GetVkQueue();
		uint8_t GetQueueFamilyIndex() const;
		uint8_t GetQueueIndex() const;
		RendererAPI::QueueType GetQueueType() const;
		uint32_t GetFlags() const;
		float GetTimestampPeriod() const;
		double GetTimestampFrequency() const;

		void WaitQueueIdle() const override;

		void Submit(const RendererAPI::QueueSubmitDesc& desc) const override;

		RendererAPI::PresentStatus Present(const RendererAPI::QueuePresentDesc& desc) const override;

	private:
		TRAP::Ref<VulkanDevice> m_device;

		VkQueue m_vkQueue;
		std::mutex& m_submitMutex;
		uint8_t m_vkQueueFamilyIndex;
		uint8_t m_vkQueueIndex;
		RendererAPI::QueueType m_type;
		uint32_t m_flags;
		float m_timestampPeriod;
	};
}

#endif /*_TRAP_VULKANQUEUE_H_*/