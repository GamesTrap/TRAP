#include "TRAPPCH.h"
#include "VulkanCommandBuffer.h"

#include "VulkanRootSignature.h"
#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "VulkanQueue.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

TRAP::Graphics::API::VulkanCommandBuffer::~VulkanCommandBuffer()
{
	if(m_vkCommandBuffer)
	{
#ifdef ENABLE_GRAPHICS_DEBUG
		TP_DEBUG(Log::RendererVulkanCommandBufferPrefix, "Destroying CommandBuffer");
#endif
		
		vkFreeCommandBuffers(m_device->GetVkDevice(), m_vkCommandPool, 1, &m_vkCommandBuffer);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanCommandBuffer::VulkanCommandBuffer(TRAP::Ref<VulkanDevice> device,
                                                              TRAP::Ref<VulkanQueue> queue,
                                                              VkCommandPool& commandPool,
                                                              const bool secondary)
	: m_vkCommandBuffer(VK_NULL_HANDLE),
	  m_device(std::move(device)),
	  m_queue(std::move(queue)),
	  m_vkCommandPool(commandPool),
	  m_secondary(secondary)
{
	TRAP_ASSERT(m_device, "device is nullptr");

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanCommandBufferPrefix, "Creating CommandBuffer");
#endif

	VkCommandBufferAllocateInfo info = VulkanInits::CommandBufferAllocateInfo(commandPool, secondary);

	VkCall(vkAllocateCommandBuffers(m_device->GetVkDevice(), &info, &m_vkCommandBuffer));
}

//-------------------------------------------------------------------------------------------------------------------//

VkCommandBuffer& TRAP::Graphics::API::VulkanCommandBuffer::GetVkCommandBuffer()
{
	return m_vkCommandBuffer;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::QueueType TRAP::Graphics::API::VulkanCommandBuffer::GetQueueType() const
{
	return m_queue->GetQueueType();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::VulkanCommandBuffer::IsSecondary() const
{
	return m_secondary;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BindPushConstants(const TRAP::Ref<VulkanRootSignature>& rootSignature,
                                                                 const char* name,
                                                                 const void* constants) const
{
	TRAP_ASSERT(constants);
	TRAP_ASSERT(rootSignature);
	TRAP_ASSERT(name);

	const RendererAPI::DescriptorInfo* desc = rootSignature->GetDescriptor(name);
	TRAP_ASSERT(desc);
	TRAP_ASSERT(desc->Type == RendererAPI::DescriptorType::RootConstant);

	vkCmdPushConstants(m_vkCommandBuffer, rootSignature->GetVkPipelineLayout(), desc->VkStages, 0, desc->Size, constants);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BindPushConstantsByIndex(const TRAP::Ref<VulkanRootSignature>& rootSignature,
	const uint32_t paramIndex,
	const void* constants) const
{
	TRAP_ASSERT(constants);
	TRAP_ASSERT(rootSignature);
	TRAP_ASSERT(paramIndex >= 0 && paramIndex < rootSignature->GetDescriptorCount());

	const RendererAPI::DescriptorInfo* desc = &rootSignature->GetDescriptors()[paramIndex];
	TRAP_ASSERT(desc);
	TRAP_ASSERT(desc->Type == RendererAPI::DescriptorType::RootConstant);

	vkCmdPushConstants(m_vkCommandBuffer, rootSignature->GetVkPipelineLayout(), desc->VkStages, 0, desc->Size, constants);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::AddDebugMarker(const float r, const float g, const float b, const char* name) const
{
	VkDebugUtilsLabelEXT markerInfo = VulkanInits::DebugUtilsLabelExt(r, g, b, name);

	vkCmdInsertDebugUtilsLabelEXT(m_vkCommandBuffer, &markerInfo);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BeginDebugMarker(const float r, const float g, const float b, const char* name) const
{
	VkDebugUtilsLabelEXT markerInfo = VulkanInits::DebugUtilsLabelExt(r, g, b, name);

	vkCmdBeginDebugUtilsLabelEXT(m_vkCommandBuffer, &markerInfo);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::EndDebugMarker() const
{
	vkCmdEndDebugUtilsLabelEXT(m_vkCommandBuffer);
}