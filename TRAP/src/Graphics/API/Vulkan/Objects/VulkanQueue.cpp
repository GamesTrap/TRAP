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

TRAP::Graphics::API::VulkanQueue::VulkanQueue(const RendererAPI::QueueDesc& desc)
	: m_device(dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer().get())->GetDevice()),
	  m_vkQueue(VK_NULL_HANDLE),
	  m_submitMutex(VulkanRenderer::s_NullDescriptors->SubmitMutex),
	  m_vkQueueFamilyIndex(std::numeric_limits<uint8_t>::max()),
	  m_vkQueueIndex(std::numeric_limits<uint8_t>::max()),
	  m_type(desc.Type),
	  m_flags(),
	  m_timestampPeriod(m_device->GetPhysicalDevice()->GetVkPhysicalDeviceProperties().limits.timestampPeriod)
{
	TRAP_ASSERT(m_device, "device is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
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
	TRAP_ASSERT(m_vkQueue);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanQueuePrefix, "Destroying Queue");
#endif

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

double TRAP::Graphics::API::VulkanQueue::GetTimestampFrequency() const
{
	//The engine is using ticks per sec as frequency.
	//Vulkan is nano sec per tick.
	//Handle the conversion logic here.

	return 1.0 / (static_cast<double>(m_timestampPeriod) * 1e-9);
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
		cmds[i] = dynamic_cast<VulkanCommandBuffer*>(desc.Cmds[i])->GetVkCommandBuffer();

	std::vector<VkSemaphore> waitSemaphores(desc.WaitSemaphores.size());
	std::vector<VkPipelineStageFlags> waitMasks(desc.WaitSemaphores.size());
	uint32_t waitCount = 0;
	for (const auto& waitSemaphore : desc.WaitSemaphores)
	{
		if(!waitSemaphore->IsSignaled())
			continue;

		waitSemaphores[waitCount] = dynamic_cast<VulkanSemaphore*>(waitSemaphore.get())->GetVkSemaphore();
		waitMasks[waitCount] = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		++waitCount;

		waitSemaphore->m_signaled = false;
	}

	std::vector<VkSemaphore> signalSemaphores(desc.SignalSemaphores.size());
	std::size_t signalCount = 0;
	for(std::size_t i = 0; i < desc.SignalSemaphores.size(); ++i)
	{
		if(desc.SignalSemaphores[i]->IsSignaled())
			continue;

		signalSemaphores[signalCount] = dynamic_cast<VulkanSemaphore*>
		(
			desc.SignalSemaphores[i].get()
		)->GetVkSemaphore();
		desc.SignalSemaphores[i]->m_signaled = true;
		++signalCount;
	}

	VkSubmitInfo submitInfo = VulkanInits::SubmitInfo(waitSemaphores, waitMasks, cmds, signalSemaphores);

	//Lightweight lock to make sure multiple threads dont use the same queue simultaneously
	//Many setups have just one queue family and one queue.
	//In this case, async compute, async transfer doesn't exist and we end up using the same queue for all
	//three operations
	std::lock_guard<std::mutex> lock(m_submitMutex);
	VkCall(vkQueueSubmit(m_vkQueue, 1, &submitInfo, desc.SignalFence ?
	                                                dynamic_cast<VulkanFence*>(desc.SignalFence.get())->GetVkFence() :
													VK_NULL_HANDLE));

	if (desc.SignalFence)
		desc.SignalFence->m_submitted = true;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::PresentStatus TRAP::Graphics::API::VulkanQueue::Present(const RendererAPI::QueuePresentDesc& desc) const
{
	const std::vector<TRAP::Ref<Semaphore>>& waitSemaphores = desc.WaitSemaphores;
	RendererAPI::PresentStatus presentStatus = RendererAPI::PresentStatus::Failed;

	if(!desc.SwapChain)
		return presentStatus;

	TRAP_ASSERT(m_vkQueue != VK_NULL_HANDLE);

	std::vector<VkSemaphore> wSemaphores;
	if (!waitSemaphores.empty())
		wSemaphores.reserve(waitSemaphores.size());
	for (const auto& waitSemaphore : waitSemaphores)
	{
		if(waitSemaphore->IsSignaled())
		{
			wSemaphores.push_back(dynamic_cast<VulkanSemaphore*>(waitSemaphore.get())->GetVkSemaphore());
			waitSemaphore->m_signaled = false;
		}
	}

	uint32_t presentIndex = desc.Index;

	VulkanSwapChain* sChain = dynamic_cast<VulkanSwapChain*>(desc.SwapChain.get());
	VkPresentInfoKHR presentInfo = VulkanInits::PresentInfo(wSemaphores, sChain->GetVkSwapChain(), presentIndex);

	//Lightweigt lock to make sure multiple threads dont use the same queue simultaneously
	std::lock_guard<std::mutex> lock(m_submitMutex);
	const VkResult res = vkQueuePresentKHR(sChain->GetPresentVkQueue() ? sChain->GetPresentVkQueue() : m_vkQueue,
	                                       &presentInfo);
	if (res == VK_SUCCESS)
		presentStatus = RendererAPI::PresentStatus::Success;
	else if (res == VK_ERROR_DEVICE_LOST)
		presentStatus = RendererAPI::PresentStatus::DeviceReset;
	else if(res == VK_ERROR_OUT_OF_DATE_KHR)
		presentStatus = RendererAPI::PresentStatus::OutOfDate;
	else
	{
		VkCall(res);
	}

	return presentStatus;
}