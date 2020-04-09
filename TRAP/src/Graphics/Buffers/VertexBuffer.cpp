#include "TRAPPCH.h"
#include "VertexBuffer.h"

#include "Graphics/API/Context.h"
#include "Graphics/API/Vulkan/Buffers/VulkanVertexBuffer.h"
#include "Graphics/API/OpenGL/Buffers/OpenGLVertexBuffer.h"

TRAP::Scope<TRAP::Graphics::VertexBuffer> TRAP::Graphics::VertexBuffer::Create(float* vertices, uint32_t size)
{
	TP_PROFILE_FUNCTION();

	switch(API::Context::GetRenderAPI())
	{
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

	case API::RenderAPI::Vulkan:
		return MakeScope<API::VulkanVertexBuffer>(size);

	case API::RenderAPI::OpenGL:
		return MakeScope<API::OpenGLVertexBuffer>(size);

	default:
		//Shouldn't be reached
		return nullptr;
	}
}
