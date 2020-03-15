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

TRAP::Graphics::Texture2D::Texture2D()
{
	m_textureType = TextureType::Texture2D;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::Graphics::Texture2D::GetFilePath() const
{
	return m_filepath;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::CreateFromFile(const std::string& name, const std::string& filepath, TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();

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
	TP_PROFILE_FUNCTION();

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

TRAP::Scope<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::CreateFromImage(const std::string& name, const Scope<Image>& img, TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();

	switch(API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		return MakeScope<API::D3D12Texture2D>(name, img, parameters);
#endif

	case API::RenderAPI::Vulkan:
		return MakeScope<API::VulkanTexture2D>(name, img, parameters);

	case API::RenderAPI::OpenGL:
		return MakeScope<API::OpenGLTexture2D>(name, img, parameters);

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::CreateEmpty(uint32_t width, uint32_t height, uint32_t bitsPerPixel, Image::ColorFormat format, TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();

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
	TP_PROFILE_FUNCTION();

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