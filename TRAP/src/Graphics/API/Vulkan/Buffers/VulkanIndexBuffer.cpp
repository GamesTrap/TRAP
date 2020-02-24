#include "TRAPPCH.h"
#include "VulkanIndexBuffer.h"

TRAP::Graphics::API::VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, const uint32_t size)
	: m_count(size)
{
	TP_PROFILE_FUNCTION();
	
	TP_WARN("[IndexBuffer][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanIndexBuffer::~VulkanIndexBuffer()
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanIndexBuffer::GetCount() const
{
	TP_PROFILE_FUNCTION();

	return m_count;
}