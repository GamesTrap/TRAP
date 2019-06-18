#include "TRAPPCH.h"
#include "VulkanVertexArray.h"

TRAP::Graphics::API::VulkanVertexArray::VulkanVertexArray() = default;

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

std::vector<std::unique_ptr<TRAP::Graphics::API::VertexBuffer>>& TRAP::Graphics::API::VulkanVertexArray::GetVertexBuffers()
{
	return m_vertexBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::IndexBuffer* TRAP::Graphics::API::VulkanVertexArray::GetIndexBuffer()
{
	return m_indexBuffer.get();
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

void TRAP::Graphics::API::VulkanVertexArray::Draw() const
{
}