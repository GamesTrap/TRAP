#include "TRAPPCH.h"
#include "Renderer.h"

namespace TRAP::Graphics
{
	std::unique_ptr<Renderer> Renderer::s_Instance = nullptr;

	void Renderer::Init()
	{
		switch(API::Context::GetRenderAPI())
		{
		case API::RenderAPI::OPENGL:
			TP_INFO("[Renderer][OpenGL] Initializing Renderer");
			s_Instance = std::make_unique<OpenGLRenderer>();
			break;

#ifdef TRAP_PLATFORM_WINDOWS
		case API::RenderAPI::D3D12:
			TP_INFO("[Renderer][D3D12] Initializing Renderer");
			s_Instance = std::make_unique<D3D12Renderer>();
			break;
#endif

		case API::RenderAPI::VULKAN:
			TP_INFO("[Renderer][Vulkan] Initializing Renderer");
			s_Instance = std::make_unique<VulkanRenderer>();
			break;

		default:
			return;
		}

		s_Instance->InitInternal();
	}
}
