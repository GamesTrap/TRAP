#include "TRAPPCH.h"
#include "FrameBuffer.h"

#include "Graphics/API/Context.h"
#include "Graphics/API/Vulkan/Buffers/VulkanFrameBuffer.h"

TRAP::Scope<TRAP::Graphics::FrameBuffer> TRAP::Graphics::FrameBuffer::Create(const FrameBufferProps& props)
{
	TP_PROFILE_FUNCTION();

	switch (API::Context::GetRenderAPI())
	{
	case API::RenderAPI::Vulkan:
		return MakeScope<API::VulkanFrameBuffer>(props);

	default:
		return nullptr;
	}
}
