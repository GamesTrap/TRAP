#include "TRAPPCH.h"
#include "VulkanIndexBuffer.h"

TRAP::Graphics::API::VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, const uint32_t size)
	: m_count(size)
{
	TRAP_WARN("[IndexBuffer][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanIndexBuffer::~VulkanIndexBuffer() = default;

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanIndexBuffer::GetCount() const
{
	return m_count;
}