#include "TRAPPCH.h"
#include "VertexBuffer.h"

std::unique_ptr<TRAP::Graphics::API::VertexBuffer> TRAP::Graphics::API::VertexBuffer::Create(float* vertices, uint32_t size, BufferUsage usage)
{
	switch(Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case RenderAPI::D3D12:
		TP_WARN("[VertexBuffer][D3D12] WIP");
		return std::make_unique<D3D12VertexBuffer>(vertices, size, usage);
#endif

	case RenderAPI::VULKAN:
		TP_WARN("[VertexBuffer][Vulkan] WIP");
		return std::make_unique<VulkanVertexBuffer>(vertices, size, usage);

	case RenderAPI::OPENGL:
		return std::make_unique<OpenGLVertexBuffer>(vertices, size, usage);

	default:
		//Shouldn't be reached
		return nullptr;
	}
}