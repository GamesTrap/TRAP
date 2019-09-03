#include "TRAPPCH.h"
#include "UniformBuffer.h"

#include "Graphics/API/Context.h"
#include "Graphics/API/D3D12/Buffers/D3D12UniformBuffer.h"
#include "Graphics/API/Vulkan/Buffers/VulkanUniformBuffer.h"
#include "Graphics/API/OpenGL/Buffers/OpenGLUniformBuffer.h"

std::unique_ptr<TRAP::Graphics::UniformBuffer> TRAP::Graphics::UniformBuffer::Create(const char* name, uint32_t size)
{
	switch (API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		return std::make_unique<API::D3D12UniformBuffer>(name, size);
#endif

	case API::RenderAPI::Vulkan:
		return std::make_unique<API::VulkanUniformBuffer>(name, size);

	case API::RenderAPI::OpenGL:
		return std::make_unique<API::OpenGLUniformBuffer>(name, size);

	default:
		//Shouldn't be reached
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<TRAP::Graphics::UniformBuffer> TRAP::Graphics::UniformBuffer::Create(const char* name, void* data, uint32_t size)
{
	switch (API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		return std::make_unique<API::D3D12UniformBuffer>(name, data, size);
#endif

	case API::RenderAPI::Vulkan:
		return std::make_unique<API::VulkanUniformBuffer>(name, data, size);

	case API::RenderAPI::OpenGL:
		return std::make_unique<API::OpenGLUniformBuffer>(name, data, size);

	default:
		//Shouldn't be reached
		return nullptr;
	}
}