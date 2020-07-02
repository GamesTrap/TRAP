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

void TRAP::Graphics::API::VulkanVertexArray::AddVertexBuffer(Scope<VertexBuffer>& buffer)
{
	TP_PROFILE_FUNCTION();

	TRAP_CORE_ASSERT(buffer->GetLayout().GetElements().size(), "[VBO][OpenGL] VertexBuffer has no layout!");

	m_vertexBuffers.emplace_back(std::move(buffer));
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
	std::vector<VkBuffer> vertexBuffers{};
	for (const auto& vertexBuffer : m_vertexBuffers)
		vertexBuffers.emplace_back(dynamic_cast<VulkanVertexBuffer*>(vertexBuffer.get())->GetHandle());
	vkCmdBindVertexBuffers(VulkanRenderer::GetCurrentSwapchain().GetGraphicsCommandBuffer().GetCommandBuffer(), 0, 1, vertexBuffers.data(), &offset);
	
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