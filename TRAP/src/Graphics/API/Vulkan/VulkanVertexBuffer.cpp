#include "TRAPPCH.h"
#include "VulkanVertexBuffer.h"

TRAP::Graphics::API::VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size, BufferUsage usage)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanVertexBuffer::~VulkanVertexBuffer() = default;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanVertexBuffer::Bind()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanVertexBuffer::Unbind()
{
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Graphics::API::BufferLayout& TRAP::Graphics::API::VulkanVertexBuffer::GetLayout() const
{
	return m_layout;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanVertexBuffer::SetLayout(const BufferLayout& layout)
{
	m_layout = layout;
}