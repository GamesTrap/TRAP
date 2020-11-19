#include "TRAPPCH.h"
#include "VertexBuffer.h"

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/Buffers/VulkanVertexBuffer.h"

TRAP::Scope<TRAP::Graphics::VertexBuffer> TRAP::Graphics::VertexBuffer::Create(float* vertices, uint32_t size)
{
	TP_PROFILE_FUNCTION();

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return MakeScope<API::VulkanVertexBuffer>(vertices, size);

	default:
		//Shouldn't be reached
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::VertexBuffer> TRAP::Graphics::VertexBuffer::Create(uint32_t size)
{
	TP_PROFILE_FUNCTION();

	switch (RendererAPI::GetRenderAPI())
	{

	case RenderAPI::Vulkan:
		return MakeScope<API::VulkanVertexBuffer>(size);

	default:
		//Shouldn't be reached
		return nullptr;
	}
}
