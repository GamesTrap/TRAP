#include "TRAPPCH.h"
#include "IndexBuffer.h"

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/Buffers/VulkanIndexBuffer.h"

TRAP::Scope<TRAP::Graphics::IndexBuffer> TRAP::Graphics::IndexBuffer::Create(uint32_t* indices, uint32_t count)
{
	TP_PROFILE_FUNCTION();

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return MakeScope<API::VulkanIndexBuffer>(indices, count);

	default:
		//Shouldn't be reached
		return nullptr;
	}
}