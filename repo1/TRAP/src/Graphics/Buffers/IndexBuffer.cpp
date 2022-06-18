#include "TRAPPCH.h"
#include "IndexBuffer.h"

#include "Graphics/API/Context.h"
#include "Graphics/API/Vulkan/Buffers/VulkanIndexBuffer.h"
#include "Graphics/API/OpenGL/Buffers/OpenGLIndexBuffer.h"

TRAP::Scope<TRAP::Graphics::IndexBuffer> TRAP::Graphics::IndexBuffer::Create(uint32_t* indices, uint32_t count)
{
	TP_PROFILE_FUNCTION();

	switch(API::Context::GetRenderAPI())
	{
	case API::RenderAPI::Vulkan:
		return MakeScope<API::VulkanIndexBuffer>(indices, count);

	case API::RenderAPI::OpenGL:
		return MakeScope<API::OpenGLIndexBuffer>(indices, count);

	default:
		//Shouldn't be reached
		return nullptr;
	}
}