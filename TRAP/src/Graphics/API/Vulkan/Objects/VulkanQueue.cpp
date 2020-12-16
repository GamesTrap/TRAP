#include "TRAPPCH.h"
#include "VulkanQueue.h"

#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
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
	TRAP_ASSERT(device, "device is nullptr");
	
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