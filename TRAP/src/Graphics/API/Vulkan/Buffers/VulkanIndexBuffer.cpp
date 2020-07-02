#include "TRAPPCH.h"
#include "VulkanIndexBuffer.h"

#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Graphics/API/Vulkan/Internals/VulkanInitializers.h"
#include "Graphics/API/Vulkan/Internals/Objects/VulkanDevice.h"
#include "Graphics/API/Vulkan/Internals/Objects/VulkanPhysicalDevice.h"
#include "Graphics/API/Vulkan/Internals/Objects/VulkanSwapchain.h"
#include "Graphics/API/Vulkan/Internals/Objects/VulkanCommandPool.h"
#include "Graphics/API/Vulkan/Internals/Objects/VulkanCommandBuffer.h"

TRAP::Graphics::API::VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, const uint32_t size)
	: m_count(size), m_indexBuffer(), m_indexBufferMemory()
{
	TP_PROFILE_FUNCTION();
	
	//Target Buffer Creation
	CreateBuffer(m_count,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_indexBuffer,
		m_indexBufferMemory);
	////////////////////////
	
	//Staging Buffer Creation
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CreateBuffer(m_count,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingBufferMemory);
	/////////////////////////

	//Upload Data
	void* dataPtr;
	VkCall(vkMapMemory(VulkanRenderer::GetDevice().GetDevice(), stagingBufferMemory, 0, m_count, 0, &dataPtr));
	memcpy(dataPtr, indices, m_count);
	vkUnmapMemory(VulkanRenderer::GetDevice().GetDevice(), stagingBufferMemory);
	/////////////
	
	//Copy Buffer
	Scope<Vulkan::CommandBuffer> commandBuffer = VulkanRenderer::GetCurrentSwapchain().GetTransferCommandPool().AllocateCommandBuffer();

	commandBuffer->StartRecording();

	VkBufferCopy copyRegion{ 0, 0, m_count };
	vkCmdCopyBuffer(commandBuffer->GetCommandBuffer(), stagingBuffer, m_indexBuffer, 1, &copyRegion);

	commandBuffer->EndRecording();

	VkSubmitInfo submitInfo = Vulkan::Initializers::SubmitInfo(commandBuffer->GetCommandBuffer());
	VkCall(vkQueueSubmit(VulkanRenderer::GetDevice().GetTransferQueue(), 1, &submitInfo, nullptr));
	VkCall(vkQueueWaitIdle(VulkanRenderer::GetDevice().GetTransferQueue()));
	/////////////
	
	//Destroy Staging Buffer
	vkDestroyBuffer(VulkanRenderer::GetDevice().GetDevice(), stagingBuffer, nullptr);
	vkFreeMemory(VulkanRenderer::GetDevice().GetDevice(), stagingBufferMemory, nullptr);
	////////////////////////
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanIndexBuffer::~VulkanIndexBuffer()
{
	TP_PROFILE_FUNCTION();

	if (m_indexBuffer)
		vkDestroyBuffer(VulkanRenderer::GetDevice().GetDevice(), m_indexBuffer, nullptr);

	if (m_indexBufferMemory)
		vkFreeMemory(VulkanRenderer::GetDevice().GetDevice(), m_indexBufferMemory, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

VkBuffer& TRAP::Graphics::API::VulkanIndexBuffer::GetHandle()
{
	return m_indexBuffer;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanIndexBuffer::GetCount() const
{
	TP_PROFILE_FUNCTION();

	return m_count;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanIndexBuffer::FindMemoryType(const uint32_t typeFilter, const VkMemoryPropertyFlags props)
{
	VkPhysicalDeviceMemoryProperties& memoryProperties = VulkanRenderer::GetPhysicalDevice().GetPhysicalDeviceMemoryProperties();

	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & props) == props)
			return i;
	}

	TRAP_CORE_ASSERT(false, "No Available Memory Properties");
	return -1;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanIndexBuffer::CreateBuffer(const VkDeviceSize size,
                                                          const VkBufferUsageFlags usage,
                                                          const VkMemoryPropertyFlags properties,
                                                          VkBuffer& buffer,
                                                          VkDeviceMemory& bufferMemory)
{
	Vulkan::Device& device = VulkanRenderer::GetDevice();
	VkDevice& dev = device.GetDevice();

	std::vector<uint32_t> allowedQueueFamilyIndices = { device.GetQueueFamilyIndices().GraphicsIndices, device.GetQueueFamilyIndices().TransferIndices };
	VkBufferCreateInfo bufferCreateInfo = Vulkan::Initializers::BufferCreateInfo(size, usage, allowedQueueFamilyIndices);

	VkCall(vkCreateBuffer(dev, &bufferCreateInfo, nullptr, &buffer));

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(dev, buffer, &memoryRequirements);

	VkMemoryAllocateInfo memoryAllocateInfo = Vulkan::Initializers::MemoryAllocateInfo(memoryRequirements.size,
		FindMemoryType(memoryRequirements.memoryTypeBits,
			properties));

	VkCall(vkAllocateMemory(dev, &memoryAllocateInfo, nullptr, &bufferMemory));

	VkCall(vkBindBufferMemory(dev, buffer, bufferMemory, 0));
}