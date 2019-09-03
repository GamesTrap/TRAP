#include "TRAPPCH.h"
#include "VertexBuffer.h"

#include "Graphics/API/Context.h"
#include "Graphics/API/D3D12/Buffers/D3D12VertexBuffer.h"
#include "Graphics/API/Vulkan/Buffers/VulkanVertexBuffer.h"
#include "Graphics/API/OpenGL/Buffers/OpenGLVertexBuffer.h"

std::unique_ptr<TRAP::Graphics::VertexBuffer> TRAP::Graphics::VertexBuffer::Create(float* vertices, uint32_t size)
{
	switch(API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		return std::make_unique<API::D3D12VertexBuffer>(vertices, size);
#endif

	case API::RenderAPI::Vulkan:
		return std::make_unique<API::VulkanVertexBuffer>(vertices, size);

	case API::RenderAPI::OpenGL:
		return std::make_unique<API::OpenGLVertexBuffer>(vertices, size);

	default:
		//Shouldn't be reached
		return nullptr;
	}
}
