#include "TRAPPCH.h"
#include "VulkanIndexBuffer.h"

TRAP::Graphics::API::VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, const uint32_t size)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanIndexBuffer::~VulkanIndexBuffer()
{
}

//-------------------------------------------------------------------------------------------------------------------//

VkBuffer& TRAP::Graphics::API::VulkanIndexBuffer::GetHandle()
{
	VkBuffer x = nullptr;
	return x;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanIndexBuffer::GetCount() const
{
	return 0;
}