#include "TRAPPCH.h"
#include "TextureCube.h"


#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/Textures/VulkanTextureCube.h"
#include "VFS/VFS.h"

uint32_t TRAP::Graphics::TextureCube::s_maxCubeTextureSize = 0;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureCube::TextureCube()
	: m_inputFormat(InputFormat::NONE)
{
	m_textureType = TextureType::TextureCube;
}

//-------------------------------------------------------------------------------------------------------------------//

std::array<std::string, 6> TRAP::Graphics::TextureCube::GetFilePaths() const
{
	return m_filepaths;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::InputFormat TRAP::Graphics::TextureCube::GetInputFormat() const
{
	return m_inputFormat;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::TextureCube> TRAP::Graphics::TextureCube::CreateFromFiles(const std::string& name, const std::array<std::string_view, 6>& filepaths, TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();

	if(name.empty())
	{
		TP_ERROR(Log::TextureCubePrefix, "Invalid Name!");
		return nullptr;
	}
	
	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return MakeScope<API::VulkanTextureCube>(name, filepaths, parameters);

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::TextureCube> TRAP::Graphics::TextureCube::CreateFromCross(const std::string& name, const std::string_view filepath, InputFormat format, TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();

	if(name.empty())
	{
		TP_WARN(Log::TextureCubePrefix, "Name is empty! Using Filename as TextureCube Name!");
		return CreateFromCross(filepath, format, parameters);
	}
	
	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return MakeScope<API::VulkanTextureCube>(name, filepath, format, parameters);

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::TextureCube> TRAP::Graphics::TextureCube::CreateFromCross(const std::string_view filepath, InputFormat format, TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();

	std::string name = VFS::GetFileName(VFS::MakeVirtualPathCompatible(filepath));
	
	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return MakeScope<API::VulkanTextureCube>(name, filepath, format, parameters);

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::TextureCube> TRAP::Graphics::TextureCube::CreateFromCrossImage(const std::string& name, const Scope<Image>& img, InputFormat format, TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return MakeScope<API::VulkanTextureCube>(name, img, format, parameters);
		
	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::TextureCube> TRAP::Graphics::TextureCube::Create(TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return MakeScope<API::VulkanTextureCube>(parameters);

	default:
		return nullptr;
	}
}