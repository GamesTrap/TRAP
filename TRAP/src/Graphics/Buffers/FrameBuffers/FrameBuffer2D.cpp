#include "TRAPPCH.h"

#include "FrameBuffer2D.h"
#include "Graphics/API/Context.h"

#include "Graphics/API/OpenGL/Buffers/FrameBuffers/OpenGLFrameBuffer2D.h"
#include "Graphics/API/Vulkan/Buffers/FrameBuffers/VulkanFrameBuffer2D.h"

TRAP::Scope<TRAP::Graphics::FrameBuffer2D> TRAP::Graphics::FrameBuffer2D::Create(uint32_t width, uint32_t height)
{
	TP_PROFILE_FUNCTION();

	switch(API::Context::GetRenderAPI())
	{
	case API::RenderAPI::Vulkan:
		return MakeScope<API::VulkanFrameBuffer2D>(width, height);
		
	case API::RenderAPI::OpenGL:
		return MakeScope<API::OpenGLFrameBuffer2D>(width, height);
		
	default:
		return nullptr;
	}
}
