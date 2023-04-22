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
#include <memory>

TRAP::Graphics::API::VulkanQueue::VulkanQueue(const RendererAPI::QueueDesc& desc)
	: m_device(dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice()),
	  m_vkQueue(VK_NULL_HANDLE),
	  m_submitMutex(VulkanRenderer::s_NullDescriptors->SubmitMutex),
	  m_vkQueueFamilyIndex(std::numeric_limits<uint8_t>::max()),
	  m_vkQueueIndex(std::numeric_limits<uint8_t>::max()),
	  m_type(desc.Type),
	  m_flags(),
	  m_timestampPeriod(m_device->GetPhysicalDevice()->GetVkPhysicalDeviceProperties().limits.timestampPeriod)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(m_device, "VulkanQueue(): Vulkan Device is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanQueuePrefix, "Creating Queue");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	VkQueueFamilyProperties queueProps = {};

	m_device->FindQueueFamilyIndex(m_type, queueProps, m_vkQueueFamilyIndex, m_vkQueueIndex);
	++m_device->m_usedQueueCount[queueProps.queueFlags];

	m_flags = queueProps.queueFlags;

	//Get queue handle
	vkGetDeviceQueue(m_device->GetVkDevice(), m_vkQueueFamilyIndex, m_vkQueueIndex, &m_vkQueue);
	TRAP_ASSERT(m_vkQueue != VK_NULL_HANDLE, "VulkanQueue(): Vulkan Queue is nullptr");

#ifdef ENABLE_GRAPHICS_DEBUG
	switch(m_type)
	{
	case RendererAPI::QueueType::Graphics:
		SetQueueName("Queue_Graphics");
		break;
	case RendererAPI::QueueType::Compute:
		SetQueueName("Queue_Compute");
		break;
	case RendererAPI::QueueType::Transfer:
		SetQueueName("Queue_Transfer");
		break;
	default:
		break;
	}
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanQueue::~VulkanQueue()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanQueuePrefix, "Destroying Queue");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	--m_device->m_usedQueueCount[m_flags];
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkQueue TRAP::Graphics::API::VulkanQueue::GetVkQueue() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_vkQueue;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint8_t TRAP::Graphics::API::VulkanQueue::GetQueueFamilyIndex() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_vkQueueFamilyIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint8_t TRAP::Graphics::API::VulkanQueue::GetQueueIndex() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_vkQueueIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::RendererAPI::QueueType TRAP::Graphics::API::VulkanQueue::GetQueueType() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_type;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::API::VulkanQueue::GetFlags() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_flags;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] float TRAP::Graphics::API::VulkanQueue::GetTimestampPeriod() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_timestampPeriod;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] double TRAP::Graphics::API::VulkanQueue::GetTimestampFrequency() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	//The engine is using ticks per sec as frequency.
	//Vulkan is nano sec per tick.
	//Handle the conversion logic here.

	return 1.0 / (NumericCast<double>(m_timestampPeriod) * 1e-9);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanQueue::WaitQueueIdle() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkCall(vkQueueWaitIdle(m_vkQueue));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanQueue::Submit(const RendererAPI::QueueSubmitDesc& desc) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(!desc.Cmds.empty(), "VulkanQueue::Submit(): No CommandBuffers to submit!");

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

		waitSemaphores[waitCount] = std::dynamic_pointer_cast<VulkanSemaphore>(waitSemaphore)->GetVkSemaphore();
		waitMasks[waitCount] = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		++waitCount;

		waitSemaphore->m_signaled = false;
	}

	std::vector<VkSemaphore> signalSemaphores(desc.SignalSemaphores.size());
	uint32_t signalCount = 0;

	for(const auto& signalSemaphore : desc.SignalSemaphores)
	{
		if(signalSemaphore->IsSignaled())
			continue;

		signalSemaphores[signalCount] = std::dynamic_pointer_cast<VulkanSemaphore>(signalSemaphore)->GetVkSemaphore();
		signalSemaphore->m_signaled = true;
		++signalCount;
	}

	const VkSubmitInfo submitInfo = VulkanInits::SubmitInfo(waitSemaphores, waitCount, waitMasks,
	                                                        cmds, signalSemaphores, signalCount);

	//Lightweight lock to make sure multiple threads dont use the same queue simultaneously
	//Many setups have just one queue family and one queue.
	//In this case, async compute, async transfer doesn't exist and we end up using the same queue for all
	//three operations
	std::lock_guard lock(m_submitMutex);
	LockMark(m_submitMutex);
	VkCall(vkQueueSubmit(m_vkQueue, 1, &submitInfo, desc.SignalFence ?
	                                                std::dynamic_pointer_cast<VulkanFence>(desc.SignalFence)->GetVkFence() :
													VK_NULL_HANDLE));

	if (desc.SignalFence)
		desc.SignalFence->m_submitted = true;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] TRAP::Graphics::RendererAPI::PresentStatus TRAP::Graphics::API::VulkanQueue::Present(const RendererAPI::QueuePresentDesc& desc) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	const std::vector<TRAP::Ref<Semaphore>>& waitSemaphores = desc.WaitSemaphores;
	RendererAPI::PresentStatus presentStatus = RendererAPI::PresentStatus::Failed;

	if(!desc.SwapChain)
		return presentStatus;

	std::vector<VkSemaphore> wSemaphores;
	if (!waitSemaphores.empty())
		wSemaphores.reserve(waitSemaphores.size());
	for (const auto& waitSemaphore : waitSemaphores)
	{
		if(waitSemaphore->IsSignaled())
		{
			wSemaphores.push_back(std::dynamic_pointer_cast<VulkanSemaphore>(waitSemaphore)->GetVkSemaphore());
			waitSemaphore->m_signaled = false;
		}
	}

	uint32_t presentIndex = desc.Index;

	const Ref<VulkanSwapChain> sChain = std::dynamic_pointer_cast<VulkanSwapChain>(desc.SwapChain);
	VkSwapchainKHR sc = sChain->GetVkSwapChain();
	const VkPresentInfoKHR presentInfo = VulkanInits::PresentInfo(wSemaphores, sc, presentIndex);

	//Lightweigt lock to make sure multiple threads dont use the same queue simultaneously
	std::lock_guard lock(m_submitMutex);
	LockMark(m_submitMutex);
	const VkResult res = vkQueuePresentKHR(sChain->GetPresentVkQueue() != nullptr ? sChain->GetPresentVkQueue() : m_vkQueue,
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
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanQueue::SetQueueName(const std::string_view name) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	if(!VulkanRenderer::s_debugMarkerSupport)
		return;

#ifdef ENABLE_DEBUG_UTILS_EXTENSION
	VkSetObjectName(m_device->GetVkDevice(), Utils::BitCast<uint64_t>(m_vkQueue), VK_OBJECT_TYPE_QUEUE, name);
#else
	VkSetObjectName(m_device->GetVkDevice(), Utils::BitCast<uint64_t>(m_vkQueue), VK_DEBUG_REPORT_OBJECT_TYPE_QUEUE_EXT, name);
#endif
}