#include "TRAPPCH.h"
#include "UniformBuffer.h"

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/Buffers/VulkanUniformBuffer.h"

TRAP::Scope<TRAP::Graphics::UniformBuffer> TRAP::Graphics::UniformBuffer::Create(const char* name, uint32_t size, BufferUsage usage)
{
	TP_PROFILE_FUNCTION();

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return MakeScope<API::VulkanUniformBuffer>(name, size, usage);

	default:
		//Shouldn't be reached
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::UniformBuffer> TRAP::Graphics::UniformBuffer::Create(const char* name, void* data, uint32_t size, BufferUsage usage)
{
	TP_PROFILE_FUNCTION();

	switch (RendererAPI::GetRenderAPI())
	{

	case RenderAPI::Vulkan:
		return MakeScope<API::VulkanUniformBuffer>(name, data, size, usage);

	default:
		//Shouldn't be reached
		return nullptr;
	}
}