#include "TRAPPCH.h"
#include "VulkanVertexArray.h"

#include "VulkanVertexBuffer.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Graphics/API/Vulkan/Buffers/VulkanIndexBuffer.h"

TRAP::Graphics::API::VulkanVertexArray::VulkanVertexArray()
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanVertexArray::~VulkanVertexArray()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanVertexArray::SetVertexBuffer(Scope<VertexBuffer>& buffer)
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

const TRAP::Scope<TRAP::Graphics::VertexBuffer>& TRAP::Graphics::API::VulkanVertexArray::GetVertexBuffer()
{
	return m_vertexBuffer;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::IndexBuffer>& TRAP::Graphics::API::VulkanVertexArray::GetIndexBuffer()
{
	return m_indexBuffer;
}