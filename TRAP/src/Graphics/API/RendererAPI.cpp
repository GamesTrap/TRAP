#include "TRAPPCH.h"
#include "RendererAPI.h"

#include "OpenGL/OpenGLRenderer.h"
#include "D3D12/D3D12Renderer.h"
#include "Vulkan/VulkanRenderer.h"
#include "Utils/MsgBox/MsgBox.h"

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<TRAP::Graphics::API::RendererAPI> TRAP::Graphics::API::RendererAPI::s_Renderer = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::RendererAPI::Shutdown()
{
	s_Renderer.reset();
	s_Renderer = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::RendererAPI::Init()
{
	switch (Context::GetRenderAPI())
	{
	case RenderAPI::OPENGL:
		TP_INFO("[Renderer][OpenGL] Initializing Renderer");
		s_Renderer = std::make_unique<OpenGLRenderer>();
		break;

#ifdef TRAP_PLATFORM_WINDOWS
	case RenderAPI::D3D12:
		TP_INFO("[Renderer][D3D12] Initializing Renderer");
		s_Renderer = std::make_unique<D3D12Renderer>();
		break;
#endif

	case RenderAPI::VULKAN:
		TP_INFO("[Renderer][Vulkan] Initializing Renderer");
		s_Renderer = std::make_unique<VulkanRenderer>();
		break;

	default:
		//This should never be reached.
		TP_CRITICAL("[Renderer] Unsupported Device!");
#ifndef TRAP_PLATFORM_LINUX
		Show("Device is unsupported!\n No RenderAPI selected!", "Unsupported Device", Utils::MsgBox::Style::Error, Utils::MsgBox::Buttons::Quit);
#endif
		exit(-1);
	}

	s_Renderer->InitInternal();
}