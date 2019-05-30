#include "TRAPPCH.h"
#include "Context.h"

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<TRAP::Graphics::API::Context> TRAP::Graphics::API::Context::s_Context = nullptr;
TRAP::Graphics::API::RenderAPI TRAP::Graphics::API::Context::s_RenderAPI = RenderAPI::NONE;

unsigned int TRAP::Graphics::API::Context::m_vsyncInterval = 0;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Context::Create(Window* window)
{
	switch (GetRenderAPI())
	{
	case RenderAPI::OPENGL:
		TP_INFO("[Context][OpenGL] Initializing Context");
		s_Context = std::make_unique<OpenGLContext>(window);
		break;

#ifdef TRAP_PLATFORM_WINDOWS
	case RenderAPI::D3D12:
		TP_INFO("[Context][D3D12] Initializing Context");
		s_Context = std::make_unique<D3D12Context>(window);
		break;
#endif

	case RenderAPI::VULKAN:
		TP_INFO("[Context][Vulkan] Initializing Context");
		s_Context = std::make_unique<VulkanContext>(window);
		break;

	default:
		break;
	}
}