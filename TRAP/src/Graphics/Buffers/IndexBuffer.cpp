#include "TRAPPCH.h"
#include "IndexBuffer.h"

#include "Graphics/API/Context.h"
#include "Graphics/API/D3D12/Buffers/D3D12IndexBuffer.h"
#include "Graphics/API/Vulkan/Buffers/VulkanIndexBuffer.h"
#include "Graphics/API/OpenGL/Buffers/OpenGLIndexBuffer.h"

TRAP::Scope<TRAP::Graphics::IndexBuffer> TRAP::Graphics::IndexBuffer::Create(uint32_t* indices, uint32_t size)
{
	TP_PROFILE_FUNCTION();

	switch(API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		return MakeScope<API::D3D12IndexBuffer>(indices, size);
#endif

	case API::RenderAPI::Vulkan:
		return MakeScope<API::VulkanIndexBuffer>(indices, size);

	case API::RenderAPI::OpenGL:
		return MakeScope<API::OpenGLIndexBuffer>(indices, size);

	default:
		//Shouldn't be reached
		return nullptr;
	}
}