#include "TRAPPCH.h"
#include "VulkanVertexBuffer.h"

TRAP::Graphics::API::VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, const uint32_t size)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanVertexBuffer::VulkanVertexBuffer(const uint32_t size)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanVertexBuffer::~VulkanVertexBuffer()
{
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Graphics::BufferLayout& TRAP::Graphics::API::VulkanVertexBuffer::GetLayout() const
{
	return {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanVertexBuffer::SetLayout(const BufferLayout& layout)
{
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanVertexBuffer::GetVertexCount() const
{
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanVertexBuffer::SetData(const void* data, const uint32_t size)
{
}