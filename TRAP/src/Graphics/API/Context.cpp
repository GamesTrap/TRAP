#include "TRAPPCH.h"
#include "Context.h"

#include "OpenGL/OpenGLContext.h"
#include "D3D12/D3D12Context.h"
#include "Vulkan/VulkanContext.h"
#include "Utils/MsgBox/MsgBox.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::API::Context> TRAP::Graphics::API::Context::s_Context = nullptr;
TRAP::Graphics::API::RenderAPI TRAP::Graphics::API::Context::s_RenderAPI = RenderAPI::NONE;
TRAP::Graphics::API::RenderAPI TRAP::Graphics::API::Context::s_newRenderAPI = RenderAPI::NONE;

bool TRAP::Graphics::API::Context::s_isD3D12Capable = false;
bool TRAP::Graphics::API::Context::s_isVulkanCapable = false;
bool TRAP::Graphics::API::Context::s_isOpenGLCapable = false;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Context::Shutdown()
{
	s_Context.reset();
	s_Context = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Context::Create(Window* window)
{
	switch (GetRenderAPI())
	{
	case RenderAPI::OpenGL:
		TP_INFO("[Context][OpenGL] Initializing Context");
		s_Context = MakeScope<OpenGLContext>(window);
		break;

#ifdef TRAP_PLATFORM_WINDOWS
	case RenderAPI::D3D12:
		TP_INFO("[Context][D3D12] Initializing Context");
		s_Context = MakeScope<D3D12Context>(window);
		break;
#endif

	case RenderAPI::Vulkan:
		TP_INFO("[Context][Vulkan] Initializing Context");
		s_Context = MakeScope<VulkanContext>(window);
		break;

	default:
		//This should never be reached.
		TP_CRITICAL("[Engine] Unsupported Device!");
		Show("Device is unsupported!\nNo RenderAPI selected!", "Unsupported Device", Utils::MsgBox::Style::Error, Utils::MsgBox::Buttons::Quit);
		exit(-1);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Context::AutoSelectRenderAPI()
{
	TP_INFO("[Context] Auto selecting RenderAPI");
#ifdef TRAP_PLATFORM_WINDOWS
	if (s_isD3D12Capable)
	{
		SetRenderAPI(RenderAPI::D3D12);
		return;
	}

	TP_DEBUG("[Context][D3D12] Device isn't D3D12 capable!");
#endif

	//Check if Vulkan capable
	if (s_isVulkanCapable)
	{
		SetRenderAPI(RenderAPI::Vulkan);
		return;
	}
	TP_DEBUG("[Context][Vulkan] Device isn't Vulkan 1.1 capable!");


	if (s_isOpenGLCapable)
	{
		SetRenderAPI(RenderAPI::OpenGL);
		return;
	}
	TP_DEBUG("[Context][OpenGL] Device isn't OpenGL 4.6 capable!");

	SetRenderAPI(RenderAPI::NONE);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Context::CheckAllRenderAPIs()
{
#ifdef TRAP_PLATFORM_WINDOWS
	//Check if D3D12 capable
	s_isD3D12Capable = D3D12Context::IsD3D12Capable();
#else
	s_isD3D12Capable = false;
#endif

	//Check if Vulkan 1.1 capable
	s_isVulkanCapable = VulkanContext::IsVulkanCapable();

	//Check if OpenGL 4.6 capable
	s_isOpenGLCapable = OpenGLContext::IsOpenGLCapable();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Context::SetRenderAPI(const RenderAPI api)
{
	s_RenderAPI = api;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Context::SwitchRenderAPI(const RenderAPI api)
{
	if (api != s_RenderAPI)
	{
		if (api == RenderAPI::D3D12)
		{
			if (s_isD3D12Capable)
			{
				TP_WARN("[Context] Switching RenderAPI to D3D12");
				s_newRenderAPI = RenderAPI::D3D12;

				return;
			}

			TP_ERROR("[Context][D3D12] This device doesn't support D3D12!");
			if (s_isVulkanCapable)
			{
				SwitchRenderAPI(RenderAPI::Vulkan);
				return;
			}

			if (s_isOpenGLCapable)
			{
				SwitchRenderAPI(RenderAPI::OpenGL);
				return;
			}

			return;
		}

		if (api == RenderAPI::Vulkan)
		{
			if (s_isVulkanCapable)
			{
				TP_WARN("[Context] Switching RenderAPI to Vulkan 1.1");
				s_newRenderAPI = RenderAPI::Vulkan;

				return;
			}

			TP_ERROR("[Context][Vulkan] This device doesn't support Vulkan 1.1!");
			if (s_isD3D12Capable)
			{
				SwitchRenderAPI(RenderAPI::D3D12);
				return;
			}

			if (s_isOpenGLCapable)
			{
				SwitchRenderAPI(RenderAPI::OpenGL);
				return;
			}

			return;
		}

		if (api == RenderAPI::OpenGL)
		{
			if (s_isOpenGLCapable)
			{
				TP_WARN("[Context] Switching RenderAPI to OpenGL 4.6");
				s_newRenderAPI = RenderAPI::OpenGL;

				return;
			}

			TP_ERROR("[Context][OpenGL] This device doesn't support OpenGL 4.6!");
			if (s_isD3D12Capable)
			{
				SwitchRenderAPI(RenderAPI::D3D12);
				return;
			}

			if (s_isVulkanCapable)
				SwitchRenderAPI(RenderAPI::Vulkan);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::Context::IsSupported(const RenderAPI api)
{
	if (api == RenderAPI::D3D12)
		if (s_isD3D12Capable)
			return true;

	if (api == RenderAPI::Vulkan)
		if (s_isVulkanCapable)
			return true;

	if (api == RenderAPI::OpenGL)
		if (s_isOpenGLCapable)
			return true;

	return false;
}