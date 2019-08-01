#include "TRAPPCH.h"
#include "UniformBuffer.h"

#include "Graphics/API/Context.h"
#include "Graphics/API/D3D12/Buffers/D3D12UniformBuffer.h"
#include "Graphics/API/Vulkan/Buffers/VulkanUniformBuffer.h"
#include "Graphics/API/OpenGL/Buffers/OpenGLUniformBuffer.h"

std::unique_ptr<TRAP::Graphics::UniformBuffer> TRAP::Graphics::UniformBuffer::Create(const char* name, uint32_t size, BufferUsage usage)
{
	switch (API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		TP_WARN("[VertexArray][D3D12] WIP");
		return std::make_unique<API::D3D12UniformBuffer>(name, size, usage);
#endif

	case API::RenderAPI::Vulkan:
		TP_WARN("[VertexArray][Vulkan] WIP");
		return std::make_unique<API::VulkanUniformBuffer>(name, size, usage);

	case API::RenderAPI::OpenGL:
		return std::make_unique<API::OpenGLUniformBuffer>(name, size, usage);

	default:
		//Shouldn't be reached
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<TRAP::Graphics::UniformBuffer> TRAP::Graphics::UniformBuffer::Create(const char* name, void* data, uint32_t size, BufferUsage usage)
{
	switch (API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		TP_WARN("[VertexArray][D3D12] WIP");
		return std::make_unique<API::D3D12UniformBuffer>(name, data, size, usage);
#endif

	case API::RenderAPI::Vulkan:
		TP_WARN("[VertexArray][Vulkan] WIP");
		return std::make_unique<API::VulkanUniformBuffer>(name, data, size, usage);

	case API::RenderAPI::OpenGL:
		return std::make_unique<API::OpenGLUniformBuffer>(name, data, size, usage);

	default:
		//Shouldn't be reached
		return nullptr;
	}
}