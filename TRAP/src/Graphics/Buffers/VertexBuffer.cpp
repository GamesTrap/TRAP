#include "TRAPPCH.h"
#include "VertexBuffer.h"

#include "Graphics/API/Context.h"
#include "Graphics/API/D3D12/Buffers/D3D12VertexBuffer.h"
#include "Graphics/API/Vulkan/Buffers/VulkanVertexBuffer.h"
#include "Graphics/API/OpenGL/Buffers/OpenGLVertexBuffer.h"

TRAP::Scope<TRAP::Graphics::VertexBuffer> TRAP::Graphics::VertexBuffer::Create(float* vertices, uint32_t size)
{
	TP_PROFILE_FUNCTION();

	switch(API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		return MakeScope<API::D3D12VertexBuffer>(vertices, size);
#endif

	case API::RenderAPI::Vulkan:
		return MakeScope<API::VulkanVertexBuffer>(vertices, size);

	case API::RenderAPI::OpenGL:
		return MakeScope<API::OpenGLVertexBuffer>(vertices, size);

	default:
		//Shouldn't be reached
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::VertexBuffer> TRAP::Graphics::VertexBuffer::Create(uint32_t size)
{
	TP_PROFILE_FUNCTION();

	switch (API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		return MakeScope<API::D3D12VertexBuffer>(size);
#endif

	case API::RenderAPI::Vulkan:
		return MakeScope<API::VulkanVertexBuffer>(size);

	case API::RenderAPI::OpenGL:
		return MakeScope<API::OpenGLVertexBuffer>(size);

	default:
		//Shouldn't be reached
		return nullptr;
	}
}
