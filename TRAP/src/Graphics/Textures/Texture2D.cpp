#include "TRAPPCH.h"
#include "Texture2D.h"

#include "Graphics/API/Context.h"
#include "Graphics/API/OpenGL/Textures/OpenGLTexture2D.h"
#include "Graphics/API/Vulkan/Textures/VulkanTexture2D.h"
#include "VFS/VFS.h"
#ifdef TRAP_PLATFORM_WINDOWS
#include "Graphics/API/D3D12/Textures/D3D12Texture2D.h"
#endif

uint32_t TRAP::Graphics::Texture2D::s_maxTextureSize = 0;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::CreateFromFile(const std::string& name, const std::string& filepath, TextureParameters parameters)
{
	if(name.empty())
	{
		TP_WARN("[Texture2D] Name is empty! Using Filename as Texture2D Name!");
		return CreateFromFile(filepath, parameters);
	}
	
	switch(API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		return MakeScope<API::D3D12Texture2D>(name, filepath, parameters);
#endif

	case API::RenderAPI::Vulkan:
		return MakeScope<API::VulkanTexture2D>(name, filepath, parameters);

	case API::RenderAPI::OpenGL:
		return MakeScope<API::OpenGLTexture2D>(name, filepath, parameters);

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::CreateFromFile(const std::string& filepath, TextureParameters parameters)
{
	std::string name = VFS::GetFileName(VFS::MakeVirtualPathCompatible(filepath));
	
	switch (API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		return MakeScope<API::D3D12Texture2D>(name, filepath, parameters);
#endif

	case API::RenderAPI::Vulkan:
		return MakeScope<API::VulkanTexture2D>(name, filepath, parameters);

	case API::RenderAPI::OpenGL:
		return MakeScope<API::OpenGLTexture2D>(name, filepath, parameters);

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::CreateFromMemory(const std::string& name, uint32_t width, uint32_t height, uint32_t bitsPerPixel, ImageFormat format,
                                                                                       const std::vector<uint8_t>&
                                                                                       pixelData, TextureParameters parameters)
{
	switch (API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		return MakeScope<API::D3D12Texture2D>(name, width, height, bitsPerPixel, format, pixelData, parameters);
#endif

	case API::RenderAPI::Vulkan:
		return MakeScope<API::VulkanTexture2D>(name, width, height, bitsPerPixel, format, pixelData, parameters);

	case API::RenderAPI::OpenGL:
		return MakeScope<API::OpenGLTexture2D>(name, width, height, bitsPerPixel, format, pixelData, parameters);

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::CreateFromMemory(const std::string& name, uint32_t width, uint32_t height, uint32_t bitsPerPixel, ImageFormat format,
                                                                                       const std::vector<uint16_t>&
                                                                                       pixelData, TextureParameters parameters)
{
	switch (API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		return MakeScope<API::D3D12Texture2D>(name, width, height, bitsPerPixel, format, pixelData, parameters);
#endif

	case API::RenderAPI::Vulkan:
		return MakeScope<API::VulkanTexture2D>(name, width, height, bitsPerPixel, format, pixelData, parameters);

	case API::RenderAPI::OpenGL:
		return MakeScope<API::OpenGLTexture2D>(name, width, height, bitsPerPixel, format, pixelData, parameters);

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::CreateFromMemory(const std::string& name, uint32_t width, uint32_t height, uint32_t bitsPerPixel, ImageFormat format,
                                                                                       const std::vector<float>&
                                                                                       pixelData, TextureParameters parameters)
{
	switch (API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		return MakeScope<API::D3D12Texture2D>(name, width, height, bitsPerPixel, format, pixelData, parameters);
#endif

	case API::RenderAPI::Vulkan:
		return MakeScope<API::VulkanTexture2D>(name, width, height, bitsPerPixel, format, pixelData, parameters);

	case API::RenderAPI::OpenGL:
		return MakeScope<API::OpenGLTexture2D>(name, width, height, bitsPerPixel, format, pixelData, parameters);

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::CreateEmpty(uint32_t width, uint32_t height, uint32_t bitsPerPixel, ImageFormat format, TextureParameters parameters)
{
	switch (API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		return MakeScope<API::D3D12Texture2D>(width, height, bitsPerPixel, format, parameters);
#endif

	case API::RenderAPI::Vulkan:
		return MakeScope<API::VulkanTexture2D>(width, height, bitsPerPixel, format, parameters);

	case API::RenderAPI::OpenGL:
		return MakeScope<API::OpenGLTexture2D>(width, height, bitsPerPixel, format, parameters);

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::Create(TextureParameters parameters)
{
	switch (API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		return MakeScope<API::D3D12Texture2D>(parameters);
#endif

	case API::RenderAPI::Vulkan:
		return MakeScope<API::VulkanTexture2D>(parameters);

	case API::RenderAPI::OpenGL:
		return MakeScope<API::OpenGLTexture2D>(parameters);

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureType TRAP::Graphics::Texture2D::GetType() const
{
	return TextureType::Texture2D;
}