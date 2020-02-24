#include "TRAPPCH.h"
#include "VertexArray.h"

#include "Graphics/API/Context.h"
#include "Graphics/API/D3D12/Buffers/D3D12VertexArray.h"
#include "Graphics/API/Vulkan/Buffers/VulkanVertexArray.h"
#include "Graphics/API/OpenGL/Buffers/OpenGLVertexArray.h"

const TRAP::Graphics::VertexArray* TRAP::Graphics::VertexArray::s_CurrentlyBound = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::VertexArray> TRAP::Graphics::VertexArray::Create()
{
	TP_PROFILE_FUNCTION();

	switch(API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		return TRAP::MakeScope<API::D3D12VertexArray>();
#endif

	case API::RenderAPI::Vulkan:
		return TRAP::MakeScope<API::VulkanVertexArray>();

	case API::RenderAPI::OpenGL:
		return TRAP::MakeScope<API::OpenGLVertexArray>();

	default:
		//Shouldn't be reached
		return nullptr;
	}
}