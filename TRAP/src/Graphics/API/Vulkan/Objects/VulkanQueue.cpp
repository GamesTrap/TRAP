#include "TRAPPCH.h"
#include "VulkanQueue.h"

#include "VulkanFence.h"
#include "VulkanSemaphore.h"
#include "VulkanCommandBuffer.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

TRAP::Graphics::API::VulkanQueue::VulkanQueue(TRAP::Ref<VulkanDevice> device, const RendererAPI::QueueDesc& desc)
	: m_device(std::move(device)),
	  m_vkQueue(VK_NULL_HANDLE),
	  m_submitMutex(VulkanRenderer::s_NullDescriptors->SubmitMutex),
	  m_vkQueueFamilyIndex(std::numeric_limits<uint8_t>::max()),
	  m_vkQueueIndex(std::numeric_limits<uint8_t>::max()),
	  m_type(desc.Type),
	  m_flags(),
	  m_timestampPeriod(m_device->GetPhysicalDevice()->GetVkPhysicalDeviceProperties().limits.timestampPeriod)
{
	TRAP_ASSERT(m_device, "device is nullptr");
	
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanQueuePrefix, "Creating Queue");
#endif

	VkQueueFamilyProperties queueProps = {};

	m_device->FindQueueFamilyIndex(m_type, queueProps, m_vkQueueFamilyIndex, m_vkQueueIndex);
	++m_device->m_usedQueueCount[queueProps.queueFlags];

	m_flags = queueProps.queueFlags;

	//Get queue handle
	vkGetDeviceQueue(m_device->GetVkDevice(), m_vkQueueFamilyIndex, m_vkQueueIndex, &m_vkQueue);
	TRAP_ASSERT(m_vkQueue != VK_NULL_HANDLE);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanQueue::~VulkanQueue()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanQueuePrefix, "Destroying Queue");
#endif

	if(m_vkQueue)
		--m_device->m_usedQueueCount[m_flags];
}

//-------------------------------------------------------------------------------------------------------------------//

VkQueue& TRAP::Graphics::API::VulkanQueue::GetVkQueue()
{
	return m_vkQueue;
}

//-------------------------------------------------------------------------------------------------------------------//

uint8_t TRAP::Graphics::API::VulkanQueue::GetQueueFamilyIndex() const
{
	return m_vkQueueFamilyIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

uint8_t TRAP::Graphics::API::VulkanQueue::GetQueueIndex() const
{
	return m_vkQueueIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::QueueType TRAP::Graphics::API::VulkanQueue::GetQueueType() const
{
	return m_type;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanQueue::GetFlags() const
{
	return m_flags;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::API::VulkanQueue::GetTimestampPeriod() const
{
	return m_timestampPeriod;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanQueue::WaitQueueIdle() const
{
	VkCall(vkQueueWaitIdle(m_vkQueue));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanQueue::Submit(const RendererAPI::QueueSubmitDesc& desc) const
{
	TRAP_ASSERT(!desc.Cmds.empty());
	TRAP_ASSERT(m_vkQueue != VK_NULL_HANDLE);

	std::vector<VkCommandBuffer> cmds(desc.Cmds.size());
	for (uint32_t i = 0; i < desc.Cmds.size(); ++i)
		cmds[i] = desc.Cmds[i]->GetVkCommandBuffer();

	std::vector<VkSemaphore> waitSemaphores(desc.WaitSemaphores.size());
	std::vector<VkPipelineStageFlags> waitMasks(desc.WaitSemaphores.size());
	uint32_t waitCount = 0;
	for (const auto& WaitSemaphore : desc.WaitSemaphores)
	{
		if(WaitSemaphore->IsSignaled())
		{
			waitSemaphores[waitCount] = WaitSemaphore->GetVkSemaphore();
			waitMasks[waitCount] = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
			++waitCount;

			WaitSemaphore->m_signaled = false;
		}
	}

	std::vector<VkSemaphore> signalSemaphores(desc.SignalSemaphores.size());
	uint32_t signalCount = 0;
	for(uint32_t i = 0; i < desc.SignalSemaphores.size(); ++i)
	{
		if(!desc.SignalSemaphores[i]->IsSignaled())
		{
			signalSemaphores[signalCount] = desc.SignalSemaphores[i]->GetVkSemaphore();
			desc.SignalSemaphores[signalCount]->m_signaled = true;
			++signalCount;
		}
	}

	VkSubmitInfo submitInfo = VulkanInits::SubmitInfo(waitSemaphores, waitMasks, cmds, signalSemaphores);

	//Lightweight lock to make sure multiple threads dont use the same queue simultaneously
	//Many setups have just one queue family and one queue.
	//In this case, async compute, async transfer doesn't exist and we end up using the same queue for all three operations
	std::lock_guard<std::mutex> lock(m_submitMutex);
	VkCall(vkQueueSubmit(m_vkQueue, 1, &submitInfo, desc.SignalFence ? desc.SignalFence->GetVkFence() : VK_NULL_HANDLE));

	if (desc.SignalFence)
		desc.SignalFence->m_submitted = true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanQueue::Present(const RendererAPI::QueuePresentDesc& desc) const
{
	const std::vector<TRAP::Ref<VulkanSemaphore>>& waitSemaphores = desc.WaitSemaphores;

	if(desc.SwapChain)
	{
		TRAP_ASSERT(m_vkQueue != VK_NULL_HANDLE);

		std::vector<VkSemaphore> wSemaphores;
		if (!waitSemaphores.empty())
			wSemaphores.reserve(waitSemaphores.size());
		for (const auto& waitSemaphore : waitSemaphores)
		{
			if(waitSemaphore->IsSignaled())
			{
				wSemaphores.push_back(waitSemaphore->GetVkSemaphore());
				waitSemaphore->m_signaled = false;
			}
		}

		uint32_t presentIndex = desc.Index;

		VkPresentInfoKHR presentInfo = VulkanInits::PresentInfo(wSemaphores, desc.SwapChain->GetVkSwapChain(), presentIndex);

		//Lightweigt lock to make sure multiple threads dont use the same queue simultaneously
		std::lock_guard<std::mutex> lock(m_submitMutex);
		const VkResult res = vkQueuePresentKHR(desc.SwapChain->GetPresentVkQueue() ? desc.SwapChain->GetPresentVkQueue() : m_vkQueue, &presentInfo);
		if(res == VK_ERROR_OUT_OF_DATE_KHR)
		{
			//TODO
		}
		else
		{
			VkCall(res);
		}
	}
}
