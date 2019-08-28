#include "TRAPPCH.h"

#include "FrameBuffer2D.h"
#include "Graphics/API/Context.h"

#include "Graphics/API/OpenGL/Buffers/FrameBuffers/OpenGLFrameBuffer2D.h"
#include "Graphics/API/Vulkan/Buffers/FrameBuffers/VulkanFrameBuffer2D.h"
#include "Graphics/API/D3D12/Buffers/FrameBuffers/D3D12FrameBuffer2D.h"

std::unique_ptr<TRAP::Graphics::FrameBuffer2D> TRAP::Graphics::FrameBuffer2D::Create(uint32_t width, uint32_t height)
{
	switch(API::Context::GetRenderAPI())
	{
	case API::RenderAPI::Vulkan:
		return std::make_unique<API::VulkanFrameBuffer2D>(width, height);
		
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		return std::make_unique<API::D3D12FrameBuffer2D>(width, height);
#endif
		
	case API::RenderAPI::OpenGL:
		return std::make_unique<API::OpenGLFrameBuffer2D>(width, height);
		
	default:
		return nullptr;
	}
}
