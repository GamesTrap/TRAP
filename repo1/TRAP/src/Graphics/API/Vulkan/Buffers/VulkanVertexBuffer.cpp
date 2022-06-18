#include "TRAPPCH.h"
#include "VulkanVertexBuffer.h"

#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Graphics/API/Vulkan/Internals/VulkanInitializers.h"
#include "Graphics/API/Vulkan/Internals/Objects/VulkanDevice.h"
#include "Graphics/API/Vulkan/Internals/Objects/VulkanPhysicalDevice.h"
#include "Graphics/API/Vulkan/Internals/Objects/VulkanSwapchain.h"
#include "Graphics/API/Vulkan/Internals/Objects/VulkanCommandPool.h"
#include "Graphics/API/Vulkan/Internals/Objects/VulkanCommandBuffer.h"

TRAP::Graphics::API::VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, const uint32_t size)
	: m_vertexCount(size), m_vertexBuffer(nullptr), m_vertexBufferMemory(nullptr)
{
	TP_PROFILE_FUNCTION();

	AllocateBuffer();
	UploadData(vertices);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanVertexBuffer::VulkanVertexBuffer(const uint32_t size)
	: m_vertexCount(size), m_vertexBuffer(nullptr), m_vertexBufferMemory(nullptr)
{
	TP_PROFILE_FUNCTION();

	AllocateBuffer();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanVertexBuffer::~VulkanVertexBuffer()
{
	TP_PROFILE_FUNCTION();

	DeallocateBuffer();
}

//-------------------------------------------------------------------------------------------------------------------//

VkBuffer& TRAP::Graphics::API::VulkanVertexBuffer::GetHandle()
{
	return m_vertexBuffer;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Graphics::BufferLayout& TRAP::Graphics::API::VulkanVertexBuffer::GetLayout() const
{
	TP_PROFILE_FUNCTION();

	return m_layout;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanVertexBuffer::SetLayout(const BufferLayout& layout)
{
	TP_PROFILE_FUNCTION();

	m_layout = layout;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanVertexBuffer::GetVertexCount() const
{
	TP_PROFILE_FUNCTION();

	return m_vertexCount;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanVertexBuffer::SetData(const void* data, const uint32_t size)
{
	TP_PROFILE_FUNCTION();

	if(size != m_vertexCount)
	{
		m_vertexCount = size;

		DeallocateBuffer();
		AllocateBuffer();
	}

	UploadData(data);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanVertexBuffer::AllocateBuffer()
{
	//Target Buffer
	CreateBuffer(m_vertexCount,
				 VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
	             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
	             m_vertexBuffer,
	             m_vertexBufferMemory);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanVertexBuffer::UploadData(const void* data) const
{
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	
	//Staging Buffer
	CreateBuffer(m_vertexCount,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingBufferMemory);

	void* dataPtr;
	VkCall(vkMapMemory(VulkanRenderer::GetDevice().GetDevice(), stagingBufferMemory, 0, m_vertexCount, 0, &dataPtr));
	memcpy(dataPtr, data, m_vertexCount);
	vkUnmapMemory(VulkanRenderer::GetDevice().GetDevice(), stagingBufferMemory);

	CopyBuffer(stagingBuffer);

	vkDestroyBuffer(VulkanRenderer::GetDevice().GetDevice(), stagingBuffer, nullptr);
	vkFreeMemory(VulkanRenderer::GetDevice().GetDevice(), stagingBufferMemory, nullptr);
}


//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanVertexBuffer::DeallocateBuffer() const
{
	if (m_vertexBuffer)
		vkDestroyBuffer(VulkanRenderer::GetDevice().GetDevice(), m_vertexBuffer, nullptr);

	if (m_vertexBufferMemory)
		vkFreeMemory(VulkanRenderer::GetDevice().GetDevice(), m_vertexBufferMemory, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanVertexBuffer::CopyBuffer(VkBuffer& stagingBuffer) const
{
	Scope<Vulkan::CommandBuffer> commandBuffer = VulkanRenderer::GetCurrentSwapchain().GetTransferCommandPool().AllocateCommandBuffer();

	commandBuffer->StartRecording();

	VkBufferCopy copyRegion{ 0, 0, m_vertexCount };
	vkCmdCopyBuffer(commandBuffer->GetCommandBuffer(), stagingBuffer, m_vertexBuffer, 1, &copyRegion);

	commandBuffer->EndRecording();

	VkSubmitInfo submitInfo = Vulkan::Initializers::SubmitInfo(commandBuffer->GetCommandBuffer());
	VkCall(vkQueueSubmit(VulkanRenderer::GetDevice().GetTransferQueue(), 1, &submitInfo, nullptr));
	VkCall(vkQueueWaitIdle(VulkanRenderer::GetDevice().GetTransferQueue()));
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanVertexBuffer::FindMemoryType(const uint32_t typeFilter, const VkMemoryPropertyFlags props)
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

void TRAP::Graphics::API::VulkanVertexBuffer::CreateBuffer(const VkDeviceSize size,
                                                           const VkBufferUsageFlags usage,
                                                           const VkMemoryPropertyFlags properties,
                                                           VkBuffer& buffer,
                                                           VkDeviceMemory& bufferMemory)
{
	Vulkan::Device& device = VulkanRenderer::GetDevice();
	VkDevice& dev = device.GetDevice();
	
	std::vector<uint32_t> allowedQueueFamilyIndices = {device.GetQueueFamilyIndices().GraphicsIndices, device.GetQueueFamilyIndices().TransferIndices};
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