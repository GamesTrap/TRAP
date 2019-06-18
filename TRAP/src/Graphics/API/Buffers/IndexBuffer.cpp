#include "TRAPPCH.h"
#include "IndexBuffer.h"

std::unique_ptr<TRAP::Graphics::API::IndexBuffer> TRAP::Graphics::API::IndexBuffer::Create(uint32_t* indices, uint32_t size, BufferUsage usage)
{
	switch(Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case RenderAPI::D3D12:
		TP_WARN("[IndexBuffer][D3D12] WIP");
		return std::make_unique<D3D12IndexBuffer>(indices, size, usage);
#endif

	case RenderAPI::VULKAN:
		TP_WARN("[IndexBuffer][Vulkan] WIP");
		return std::make_unique<VulkanIndexBuffer>(indices, size, usage);

	case RenderAPI::OPENGL:
		return std::make_unique<OpenGLIndexBuffer>(indices, size, usage);

	default:
		//Shouldn't be reached
		return nullptr;
	}
}