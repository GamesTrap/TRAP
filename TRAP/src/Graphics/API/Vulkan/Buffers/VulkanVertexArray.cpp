#include "TRAPPCH.h"
#include "VulkanVertexArray.h"

TRAP::Graphics::API::VulkanVertexArray::VulkanVertexArray()
{
	TP_WARN("[VertexArray][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanVertexArray::~VulkanVertexArray() = default;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanVertexArray::AddVertexBuffer(std::unique_ptr<VertexBuffer>& buffer)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanVertexArray::SetIndexBuffer(std::unique_ptr<IndexBuffer>& buffer)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanVertexArray::Bind() const
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanVertexArray::Unbind() const
{	
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::unique_ptr<TRAP::Graphics::VertexBuffer>>& TRAP::Graphics::API::VulkanVertexArray::GetVertexBuffers()
{
	return m_vertexBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::IndexBuffer* TRAP::Graphics::API::VulkanVertexArray::GetIndexBuffer()
{
	return m_indexBuffer.get();
}