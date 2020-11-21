#include "TRAPPCH.h"
#include "RendererAPI.h"

#include "Application.h"
#include "Vulkan/VulkanRenderer.h"
#include "Utils/Dialogs/MsgBox.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::RendererAPI> TRAP::Graphics::RendererAPI::s_Renderer = nullptr;
TRAP::Graphics::RenderAPI TRAP::Graphics::RendererAPI::s_RenderAPI = TRAP::Graphics::RenderAPI::NONE;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::Init()
{	
	switch (s_RenderAPI)
	{
	case RenderAPI::Vulkan:
		TP_INFO(Log::RendererVulkanPrefix, "Initializing Renderer");
		s_Renderer = MakeScope<API::VulkanRenderer>();
		break;

	default:
		TP_CRITICAL(Log::RendererPrefix, "Unsupported Device!");
		Utils::Dialogs::MsgBox::Show("Device is unsupported!\n No RenderAPI selected!", "Unsupported Device", Utils::Dialogs::MsgBox::Style::Error, Utils::Dialogs::MsgBox::Buttons::Quit);
		exit(-1);
	}

	s_Renderer->InitInternal();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::Shutdown()
{
	s_Renderer.reset();
	s_Renderer = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::RendererAPI>& TRAP::Graphics::RendererAPI::GetRenderer()
{
	return s_Renderer;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::AutoSelectRenderAPI()
{
	TP_INFO(Log::RendererPrefix, "Auto selecting RenderAPI");

	//Check if Vulkan capable
	if (IsVulkanCapable())
	{
		s_RenderAPI = RenderAPI::Vulkan;
		return;
	}
	TP_DEBUG(Log::RendererVulkanPrefix, "Device isn't Vulkan 1.2 capable!");

	s_RenderAPI = RenderAPI::NONE;
	TRAP::Utils::Dialogs::MsgBox::Show("TRAP was unable to detect a compatible RenderAPI!\nPlease check your GPU driver!",
		"No compatible RenderAPI found",
		Utils::Dialogs::MsgBox::Style::Error,
		Utils::Dialogs::MsgBox::Buttons::Quit);
	TRAP::Application::Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::SwitchRenderAPI(const RenderAPI api)
{
	if (api != s_RenderAPI)
	{
		if (api == RenderAPI::Vulkan)
		{
			if (IsVulkanCapable())
			{
				TP_WARN(Log::RendererPrefix, "Switching RenderAPI to Vulkan 1.2");
				s_RenderAPI = RenderAPI::Vulkan;

				return;
			}

			TP_ERROR(Log::RendererVulkanPrefix, "This device doesn't support Vulkan 1.2!");
			
			TRAP::Utils::Dialogs::MsgBox::Show("TRAP was unable to detect a compatible RenderAPI!\nPlease check your GPU driver!",
				"No compatible RenderAPI found",
				Utils::Dialogs::MsgBox::Style::Error,
				Utils::Dialogs::MsgBox::Buttons::Quit);
			TRAP::Application::Shutdown();
		}
		else
		{
			TRAP::Utils::Dialogs::MsgBox::Show("TRAP was unable to detect a compatible RenderAPI!\nPlease check your GPU driver!",
				"No compatible RenderAPI found",
				Utils::Dialogs::MsgBox::Style::Error,
				Utils::Dialogs::MsgBox::Buttons::Quit);
			TRAP::Application::Shutdown();
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::RendererAPI::IsVulkanCapable()
{
	return TRAP::INTERNAL::WindowingAPI::VulkanSupported(); //TODO More tests
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::RendererAPI::IsSupported(const RenderAPI api)
{
	if (api == RenderAPI::Vulkan)
		return IsVulkanCapable();

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RenderAPI TRAP::Graphics::RendererAPI::GetRenderAPI()
{
	return s_RenderAPI;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::SetVSync(bool enabled)
{
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::RendererAPI::GetVSync()
{
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::Use(Window* window)
{
}