#include "TRAPPCH.h"
#include "VulkanIndexBuffer.h"

TRAP::Graphics::API::VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, const uint32_t size, BufferUsage usage)
	: m_count(size)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanIndexBuffer::~VulkanIndexBuffer() = default;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanIndexBuffer::Bind() const
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanIndexBuffer::Unbind() const
{
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanIndexBuffer::GetCount() const
{
	return m_count;
}