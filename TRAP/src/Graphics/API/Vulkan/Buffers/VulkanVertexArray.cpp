#include "TRAPPCH.h"
#include "VulkanVertexArray.h"

TRAP::Graphics::API::VulkanVertexArray::VulkanVertexArray()
{
	TRAP_WARN("[VertexArray][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanVertexArray::~VulkanVertexArray() = default;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanVertexArray::AddVertexBuffer(Scope<VertexBuffer>& buffer)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanVertexArray::SetIndexBuffer(Scope<IndexBuffer>& buffer)
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

std::vector<TRAP::Scope<TRAP::Graphics::VertexBuffer>>& TRAP::Graphics::API::VulkanVertexArray::GetVertexBuffers()
{
	return m_vertexBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::IndexBuffer>& TRAP::Graphics::API::VulkanVertexArray::GetIndexBuffer()
{
	return m_indexBuffer;
}