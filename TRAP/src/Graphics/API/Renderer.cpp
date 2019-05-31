#include "TRAPPCH.h"
#include "Renderer.h"

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<TRAP::Graphics::API::Renderer> TRAP::Graphics::API::Renderer::s_Renderer = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Renderer::Init()
{
	switch (API::Context::GetRenderAPI())
	{
	case API::RenderAPI::OPENGL:
		TP_INFO("[Renderer][OpenGL] Initializing Renderer");
		s_Renderer = std::make_unique<OpenGLRenderer>();
		break;

#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		TP_INFO("[Renderer][D3D12] Initializing Renderer");
		s_Renderer = std::make_unique<D3D12Renderer>();
		break;
#else
	case API::RenderAPI::D3D12:
		TP_CRITICAL("[Renderer][D3D12] Unsupported Platform(not Windows)!");
		exit(-1); //TODO User friendly exit(MsgBox?)
#endif

	case API::RenderAPI::VULKAN:
		TP_INFO("[Renderer][Vulkan] Initializing Renderer");
		s_Renderer = std::make_unique<VulkanRenderer>();
		break;

	default:
		return;
	}

	s_Renderer->InitInternal();
}