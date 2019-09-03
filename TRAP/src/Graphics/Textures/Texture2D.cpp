#include "TRAPPCH.h"
#include "Texture2D.h"

#include "Graphics/API/Context.h"
#include "Graphics/API/OpenGL/Textures/OpenGLTexture2D.h"
#include "Graphics/API/Vulkan/Textures/VulkanTexture2D.h"
#ifdef TRAP_PLATFORM_WINDOWS
#include "Graphics/API/D3D12/Textures/D3D12Texture2D.h"
#endif

uint32_t TRAP::Graphics::Texture2D::s_maxTextureSize = 0;

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::CreateFromFile(const std::string& name, const std::string& filepath, TextureParameters parameters)
{
	switch(API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		return std::make_unique<API::D3D12Texture2D>(name, filepath, parameters);
#endif

	case API::RenderAPI::Vulkan:
		return std::make_unique<API::VulkanTexture2D>(name, filepath, parameters);

	case API::RenderAPI::OpenGL:
		return std::make_unique<API::OpenGLTexture2D>(name, filepath, parameters);

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::CreateEmpty(ImageFormat format, uint32_t width, uint32_t height, TextureParameters parameters)
{
	switch (API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		return std::make_unique<API::D3D12Texture2D>(format, width, height, parameters);
#endif

	case API::RenderAPI::Vulkan:
		return std::make_unique<API::VulkanTexture2D>(format, width, height, parameters);

	case API::RenderAPI::OpenGL:
		return std::make_unique<API::OpenGLTexture2D>(format, width, height, parameters);

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::Create(TextureParameters parameters)
{
	switch (API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		return std::make_unique<API::D3D12Texture2D>(parameters);
#endif

	case API::RenderAPI::Vulkan:
		return std::make_unique<API::VulkanTexture2D>(parameters);

	case API::RenderAPI::OpenGL:
		return std::make_unique<API::OpenGLTexture2D>(parameters);

	default:
		return nullptr;
	}
}
