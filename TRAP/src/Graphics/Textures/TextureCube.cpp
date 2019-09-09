#include "TRAPPCH.h"

#include "TextureCube.h"
#include "Graphics/API/Context.h"
#include "Graphics/API/OpenGL/Textures/OpenGLTextureCube.h"
#include "Graphics/API/Vulkan/Textures/VulkanTextureCube.h"
#include "VFS/VFS.h"
#include "Utils/String.h"
#ifdef TRAP_PLATFORM_WINDOWS
#include "Graphics/API/D3D12/Textures/D3D12TextureCube.h"
#endif

uint32_t TRAP::Graphics::TextureCube::s_maxCubeTextureSize = 0;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::TextureCube> TRAP::Graphics::TextureCube::CreateFromFiles(const std::string& name, const std::array<std::string, 6>& filepaths, TextureParameters parameters)
{
	if(name.empty())
	{
		TP_ERROR("[TextureCube] Invalid Name!");
		return nullptr;
	}
	
	switch (API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		return MakeScope<API::D3D12TextureCube>(name, filepaths, parameters);
#endif

	case API::RenderAPI::OpenGL:
		return MakeScope<API::OpenGLTextureCube>(name, filepaths, parameters);

	case API::RenderAPI::Vulkan:
		return MakeScope<API::VulkanTextureCube>(name, filepaths, parameters);

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::TextureCube> TRAP::Graphics::TextureCube::CreateFromCross(const std::string& name, const std::string& filepath, InputFormat format, TextureParameters parameters)
{
	if(name.empty())
	{
		TP_WARN("[TextureCube] Name is empty! Using Filename as TextureCube Name!");
		return CreateFromCross(filepath, format, parameters);
	}
	
	switch (API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		return MakeScope<API::D3D12TextureCube>(name, filepath, format, parameters);
#endif

	case API::RenderAPI::OpenGL:
		return MakeScope<API::OpenGLTextureCube>(name, filepath, format, parameters);

	case API::RenderAPI::Vulkan:
		return MakeScope<API::VulkanTextureCube>(name, filepath, format, parameters);

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::TextureCube> TRAP::Graphics::TextureCube::CreateFromCross(const std::string& filepath, InputFormat format, TextureParameters parameters)
{
	std::string name = VFS::GetFileName(VFS::MakeVirtualPathCompatible(filepath));
	
	switch (API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		return MakeScope<API::D3D12TextureCube>(name, filepath, format, parameters);
#endif

	case API::RenderAPI::OpenGL:
		return MakeScope<API::OpenGLTextureCube>(name, filepath, format, parameters);

	case API::RenderAPI::Vulkan:
		return MakeScope<API::VulkanTextureCube>(name, filepath, format, parameters);

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::TextureCube> TRAP::Graphics::TextureCube::Create(TextureParameters parameters)
{
	switch (API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		return MakeScope<API::D3D12TextureCube>(parameters);
#endif

	case API::RenderAPI::Vulkan:
		return MakeScope<API::VulkanTextureCube>(parameters);

	case API::RenderAPI::OpenGL:
		return MakeScope<API::OpenGLTextureCube>(parameters);

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureType TRAP::Graphics::TextureCube::GetType() const
{
	return TextureType::TextureCube;
}