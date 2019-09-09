#include "TRAPPCH.h"
#include "UniformBuffer.h"

#include "Graphics/API/Context.h"
#include "Graphics/API/D3D12/Buffers/D3D12UniformBuffer.h"
#include "Graphics/API/Vulkan/Buffers/VulkanUniformBuffer.h"
#include "Graphics/API/OpenGL/Buffers/OpenGLUniformBuffer.h"

TRAP::Scope<TRAP::Graphics::UniformBuffer> TRAP::Graphics::UniformBuffer::Create(const char* name, uint32_t size)
{
	switch (API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		return MakeScope<API::D3D12UniformBuffer>(name, size);
#endif

	case API::RenderAPI::Vulkan:
		return MakeScope<API::VulkanUniformBuffer>(name, size);

	case API::RenderAPI::OpenGL:
		return MakeScope<API::OpenGLUniformBuffer>(name, size);

	default:
		//Shouldn't be reached
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::UniformBuffer> TRAP::Graphics::UniformBuffer::Create(const char* name, void* data, uint32_t size)
{
	switch (API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		return MakeScope<API::D3D12UniformBuffer>(name, data, size);
#endif

	case API::RenderAPI::Vulkan:
		return MakeScope<API::VulkanUniformBuffer>(name, data, size);

	case API::RenderAPI::OpenGL:
		return MakeScope<API::OpenGLUniformBuffer>(name, data, size);

	default:
		//Shouldn't be reached
		return nullptr;
	}
}