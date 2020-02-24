#include "TRAPPCH.h"
#include "VulkanVertexArray.h"

TRAP::Graphics::API::VulkanVertexArray::VulkanVertexArray()
{
	TP_PROFILE_FUNCTION();
	
	TP_WARN("[VertexArray][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanVertexArray::~VulkanVertexArray()
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanVertexArray::AddVertexBuffer(Scope<VertexBuffer>& buffer)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanVertexArray::SetIndexBuffer(Scope<IndexBuffer>& buffer)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanVertexArray::Bind() const
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanVertexArray::Unbind() const
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Scope<TRAP::Graphics::VertexBuffer>>& TRAP::Graphics::API::VulkanVertexArray::GetVertexBuffers()
{
	TP_PROFILE_FUNCTION();

	return m_vertexBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::IndexBuffer>& TRAP::Graphics::API::VulkanVertexArray::GetIndexBuffer()
{
	TP_PROFILE_FUNCTION();

	return m_indexBuffer;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanVertexArray::GetIndexCount() const
{
	TP_PROFILE_FUNCTION();

	return 0;
}