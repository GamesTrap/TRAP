#include "TRAPPCH.h"
#include "VertexArray.h"

const TRAP::Graphics::API::VertexArray* TRAP::Graphics::API::VertexArray::s_CurrentlyBound = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<TRAP::Graphics::API::VertexArray> TRAP::Graphics::API::VertexArray::Create()
{
	switch(Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case RenderAPI::D3D12:
		TP_WARN("[VertexArray][D3D12] WIP");
		return std::make_unique<D3D12VertexArray>();
#endif

	case RenderAPI::VULKAN:
		TP_WARN("[VertexArray][Vulkan] WIP");
		return std::make_unique<VulkanVertexArray>();

	case RenderAPI::OPENGL:
		return std::make_unique<OpenGLVertexArray>();

	default:
		//Shouldn't be reached
		return nullptr;
	}
}