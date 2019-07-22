#include "TRAPPCH.h"
#include "IndexBuffer.h"

#include "Graphics/API/Context.h"
#include "Graphics/API/D3D12/Buffers/D3D12IndexBuffer.h"
#include "Graphics/API/Vulkan/Buffers/VulkanIndexBuffer.h"
#include "Graphics/API/OpenGL/Buffers/OpenGLIndexBuffer.h"

std::unique_ptr<TRAP::Graphics::IndexBuffer> TRAP::Graphics::IndexBuffer::Create(uint32_t* indices, uint32_t size, BufferUsage usage)
{
	switch(API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		TP_WARN("[IndexBuffer][D3D12] WIP");
		return std::make_unique<API::D3D12IndexBuffer>(indices, size, usage);
#endif

	case API::RenderAPI::VULKAN:
		TP_WARN("[IndexBuffer][Vulkan] WIP");
		return std::make_unique<API::VulkanIndexBuffer>(indices, size, usage);

	case API::RenderAPI::OPENGL:
		return std::make_unique<API::OpenGLIndexBuffer>(indices, size, usage);

	default:
		//Shouldn't be reached
		return nullptr;
	}
}