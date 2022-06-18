#include "TRAPPCH.h"
#include "VulkanVertexArray.h"

#include "VulkanVertexBuffer.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Graphics/API/Vulkan/Internals/Objects/VulkanSwapchain.h"
#include "Graphics/API/Vulkan/Internals/Objects/VulkanCommandBuffer.h"
#include "Graphics/API/Vulkan/Buffers/VulkanIndexBuffer.h"
#include "Graphics/API/Vulkan/Internals/Objects/VulkanPipeline.h"

TRAP::Graphics::API::VulkanVertexArray::VulkanVertexArray()
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanVertexArray::~VulkanVertexArray()
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanVertexArray::SetVertexBuffer(Scope<VertexBuffer>& buffer)
{
	TP_PROFILE_FUNCTION();

	TRAP_CORE_ASSERT(buffer->GetLayout().GetElements().size(), "[VBO][OpenGL] VertexBuffer has no layout!");

	m_vertexBuffer = std::move(buffer);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanVertexArray::SetIndexBuffer(Scope<IndexBuffer>& buffer)
{
	TP_PROFILE_FUNCTION();

	m_indexBuffer = std::move(buffer);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanVertexArray::Bind() const
{
	TP_PROFILE_FUNCTION();

	//Bind VertexBuffers
	VkDeviceSize offset{ 0 };
	vkCmdBindVertexBuffers(VulkanRenderer::GetCurrentSwapchain().GetGraphicsCommandBuffer().GetCommandBuffer(), 0, 1, &dynamic_cast<VulkanVertexBuffer*>(m_vertexBuffer.get())->GetHandle(), &offset);
	
	//Bind IndexBuffers
	if (m_indexBuffer)
	{
		vkCmdBindIndexBuffer(VulkanRenderer::GetCurrentSwapchain().GetGraphicsCommandBuffer().GetCommandBuffer(),
			dynamic_cast<VulkanIndexBuffer*>(m_indexBuffer.get())->GetHandle(),
			VkDeviceSize(0),
			VK_INDEX_TYPE_UINT32);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanVertexArray::Unbind() const
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::VertexBuffer>& TRAP::Graphics::API::VulkanVertexArray::GetVertexBuffer()
{
	TP_PROFILE_FUNCTION();

	return m_vertexBuffer;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::IndexBuffer>& TRAP::Graphics::API::VulkanVertexArray::GetIndexBuffer()
{
	TP_PROFILE_FUNCTION();

	return m_indexBuffer;
}