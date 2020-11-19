#include "TRAPPCH.h"
#include "VertexArray.h"

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/Buffers/VulkanVertexArray.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::VertexArray> TRAP::Graphics::VertexArray::Create()
{
	TP_PROFILE_FUNCTION();

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeScope<API::VulkanVertexArray>();

	default:
		//Shouldn't be reached
		return nullptr;
	}
}