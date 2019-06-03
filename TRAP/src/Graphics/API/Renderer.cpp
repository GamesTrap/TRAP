#include "TRAPPCH.h"
#include "Renderer.h"

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<TRAP::Graphics::API::Renderer> TRAP::Graphics::API::Renderer::s_Renderer = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Renderer::Shutdown()
{
	s_Renderer.reset();
	s_Renderer = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Renderer::Init()
{
	switch (Context::GetRenderAPI())
	{
	case RenderAPI::OPENGL:
		TP_INFO("[Renderer][OpenGL] Initializing Renderer");
		s_Renderer = std::make_unique<OpenGLRenderer>();
		break;

	case RenderAPI::D3D12:
		TP_INFO("[Renderer][D3D12] Initializing Renderer");
		s_Renderer = std::make_unique<D3D12Renderer>();
		break;

	case RenderAPI::VULKAN:
		TP_INFO("[Renderer][Vulkan] Initializing Renderer");
		s_Renderer = std::make_unique<VulkanRenderer>();
		break;

	default:
		//This should never be reached.
		TP_CRITICAL("[Renderer] Unsupported Device!");
		Show("Device is unsupported!\n No RenderAPI selected!", "Unsupported Device", Utils::MsgBox::Style::Error, Utils::MsgBox::Buttons::Quit);
		exit(-1);
	}

	s_Renderer->InitInternal();
}