#include "TRAPPCH.h"
#include "VulkanVertexBuffer.h"

TRAP::Graphics::API::VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size)
{
	TRAP_WARN("[VertexBuffer][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanVertexBuffer::~VulkanVertexBuffer() = default;

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Graphics::BufferLayout& TRAP::Graphics::API::VulkanVertexBuffer::GetLayout() const
{
	return m_layout;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanVertexBuffer::SetLayout(const BufferLayout& layout)
{
	m_layout = layout;
}