#include "TRAPPCH.h"
#include "UniformBuffer.h"

std::unique_ptr<TRAP::Graphics::UniformBuffer> TRAP::Graphics::UniformBuffer::Create(const char* name, uint32_t size, BufferUsage usage)
{
	switch (API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		TP_WARN("[VertexArray][D3D12] WIP");
		return std::make_unique<API::D3D12UniformBuffer>(name, size, usage);
#endif

	case API::RenderAPI::VULKAN:
		TP_WARN("[VertexArray][Vulkan] WIP");
		return std::make_unique<API::VulkanUniformBuffer>(name, size, usage);

	case API::RenderAPI::OPENGL:
		return std::make_unique<API::OpenGLUniformBuffer>(name, size, usage);

	default:
		//Shouldn't be reached
		return nullptr;
	}
}
