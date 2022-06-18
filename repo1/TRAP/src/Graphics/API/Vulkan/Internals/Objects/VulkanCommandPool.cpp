#include "TRAPPCH.h"

#include "VulkanCommandPool.h"
#include "VulkanDevice.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/Internals/VulkanInitializers.h"
#include "VulkanCommandBuffer.h"

TRAP::Graphics::API::Vulkan::CommandPool::CommandPool(const Scope<Device>& device, const uint32_t queueFamilyIndex)
	: m_commandPool(), m_device(device.get())
{
	VkCommandPoolCreateInfo graphicsPoolInfo = Initializers::CommandPoolCreateInfo(queueFamilyIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	VkCall(vkCreateCommandPool(m_device->GetDevice(), &graphicsPoolInfo, nullptr, &m_commandPool));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::Vulkan::CommandPool::~CommandPool()
{
	//Command Buffers are implicitly deleted
	
	if (m_commandPool)
	{
		vkDestroyCommandPool(m_device->GetDevice(), m_commandPool, nullptr);
		m_commandPool = nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VkCommandPool& TRAP::Graphics::API::Vulkan::CommandPool::GetCommandPool()
{
	return m_commandPool;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Vulkan::CommandPool::ResetCommandPool() const
{	
	VkCall(vkResetCommandPool(m_device->GetDevice(), m_commandPool, 0));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::API::Vulkan::CommandBuffer> TRAP::Graphics::API::Vulkan::CommandPool::AllocateCommandBuffer() const
{
	VkCommandBuffer buffer = nullptr;
	
	VkCommandBufferAllocateInfo info = Initializers::CommandBufferAllocateInfo(m_commandPool, 1);
	vkAllocateCommandBuffers(m_device->GetDevice(), &info, &buffer);

	Scope<CommandBuffer> commandBuffer = MakeScope<CommandBuffer>(buffer);
	
	return commandBuffer;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Vulkan::CommandPool::FreeCommandBuffer(Scope<CommandBuffer> commandBuffer) const
{
	if(commandBuffer && commandBuffer->GetCommandBuffer())
	{
		vkFreeCommandBuffers(m_device->GetDevice(), m_commandPool, 1, &commandBuffer->GetCommandBuffer());
		commandBuffer->GetCommandBuffer() = nullptr;
		commandBuffer.reset();
	}
}
