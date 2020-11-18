#include "TRAPPCH.h"
#include "RendererAPI.h"

#include "Context.h"
#include "Vulkan/VulkanRenderer.h"
#include "Utils/Dialogs/MsgBox.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::API::RendererAPI> TRAP::Graphics::API::RendererAPI::s_Renderer = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::RendererAPI::Init()
{
	TP_PROFILE_FUNCTION();
	
	switch (Context::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		TP_INFO(Log::RendererVulkanPrefix, "Initializing Renderer");
		s_Renderer = MakeScope<VulkanRenderer>();
		break;

	default:
		TP_CRITICAL(Log::RendererPrefix, "Unsupported Device!");
		Utils::Dialogs::MsgBox::Show("Device is unsupported!\n No RenderAPI selected!", "Unsupported Device", Utils::Dialogs::MsgBox::Style::Error, Utils::Dialogs::MsgBox::Buttons::Quit);
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