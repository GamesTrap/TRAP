#include "TRAPPCH.h"
#include "VertexBuffer.h"

#include "Graphics/API/Context.h"
#include "Graphics/API/D3D12/Buffers/D3D12VertexBuffer.h"
#include "Graphics/API/Vulkan/Buffers/VulkanVertexBuffer.h"
#include "Graphics/API/OpenGL/Buffers/OpenGLVertexBuffer.h"

std::unique_ptr<TRAP::Graphics::VertexBuffer> TRAP::Graphics::VertexBuffer::Create(float* vertices, uint32_t size, BufferUsage usage)
{
	switch(API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		TP_WARN("[VertexBuffer][D3D12] WIP");
		return std::make_unique<API::D3D12VertexBuffer>(vertices, size, usage);
#endif

	case API::RenderAPI::VULKAN:
		TP_WARN("[VertexBuffer][Vulkan] WIP");
		return std::make_unique<API::VulkanVertexBuffer>(vertices, size, usage);

	case API::RenderAPI::OPENGL:
		return std::make_unique<API::OpenGLVertexBuffer>(vertices, size, usage);

	default:
		//Shouldn't be reached
		return nullptr;
	}
}
