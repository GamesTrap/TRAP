#include "TRAPPCH.h"
#include "VulkanVertexBuffer.h"

TRAP::Graphics::API::VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size)
{
	TP_PROFILE_FUNCTION();
	
	TP_WARN("[VertexBuffer][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanVertexBuffer::~VulkanVertexBuffer()
{
	TP_PROFILE_FUNCTION();
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

	return 0;
}