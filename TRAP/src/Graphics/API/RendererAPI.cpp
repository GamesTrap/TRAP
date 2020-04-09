#include "TRAPPCH.h"
#include "RendererAPI.h"

#include "OpenGL/OpenGLRenderer.h"
#include "Vulkan/VulkanRenderer.h"
#include "Utils/MsgBox/MsgBox.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::API::RendererAPI> TRAP::Graphics::API::RendererAPI::s_Renderer = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::RendererAPI::Init()
{
	TP_PROFILE_FUNCTION();
	
	switch (Context::GetRenderAPI())
	{
	case RenderAPI::OpenGL:
		TP_INFO("[Renderer][OpenGL] Initializing Renderer");
		s_Renderer = MakeScope<OpenGLRenderer>();
		break;

	case RenderAPI::Vulkan:
		TP_INFO("[Renderer][Vulkan] Initializing Renderer");
		s_Renderer = MakeScope<VulkanRenderer>();
		break;

	default:
		TP_CRITICAL("[Renderer] Unsupported Device!");
		Show("Device is unsupported!\n No RenderAPI selected!", "Unsupported Device", Utils::MsgBox::Style::Error, Utils::MsgBox::Buttons::Quit);
		exit(-1);
	}

	s_Renderer->InitInternal();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::RendererAPI::Shutdown()
{
	TP_PROFILE_FUNCTION();

	s_Renderer.reset();
	s_Renderer = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::RendererAPI* TRAP::Graphics::API::RendererAPI::GetRenderer()
{
	TP_PROFILE_FUNCTION();

	return s_Renderer.get();
}