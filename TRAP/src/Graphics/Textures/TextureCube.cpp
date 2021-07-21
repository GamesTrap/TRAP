#include "TRAPPCH.h"
#include "TextureCube.h"

#include "Graphics/API/RendererAPI.h"
#include "VFS/VFS.h"
#include "TextureBase.h"

TRAP::Scope<TRAP::Graphics::TextureCube> TRAP::Graphics::TextureCube::CreateFromFiles(const std::string& name,
	                                                                                  const std::array<std::string, 6>& filepaths,
																					  const TextureUsage usage)
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
	{
		TRAP::Scope<TextureCube> texture = TRAP::Scope<TextureCube>(new TextureCube());

		//Load Texture
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Filepaths = filepaths;
		desc.IsCubemap = true;
		desc.Type = RendererAPI::TextureCubeType::MultiFile;
		desc.Texture = &texture->m_texture;

		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &texture->m_syncToken);
		texture->m_name = name;
		texture->m_filepaths = filepaths;
		texture->m_textureType = TextureType::TextureCube;
		texture->m_textureUsage = usage;

		return texture;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::TextureCube> TRAP::Graphics::TextureCube::CreateFromFile(const std::string& name,
	                                                                                 const std::string& filepath,
																					 const TextureCubeFormat format,
																					 const TextureUsage usage)
{
	TP_PROFILE_FUNCTION();

	if(name.empty())
	{
		TP_WARN(Log::TextureCubePrefix, "Name is empty! Using Filename as TextureCube Name!");
		return CreateFromFile(filepath, format, usage);
	}

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		TRAP::Scope<TextureCube> texture = TRAP::Scope<TextureCube>(new TextureCube());

		//Load Texture
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Filepaths[0] = filepath;
		desc.IsCubemap = true;
		/*desc.Type = format == TextureCubeFormat::Cross ? RendererAPI::TextureCubeType::Cross :
		              RendererAPI::TextureCubeType::Equirectangular;*/
		desc.Type = RendererAPI::TextureCubeType::Cross; //TODO Replace when Equirectangular is implemented
		desc.Texture = &texture->m_texture;

		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &texture->m_syncToken);
		texture->m_name = name;
		texture->m_filepaths[0] = filepath;
		texture->m_textureType = TextureType::TextureCube;
		texture->m_textureUsage = usage;

		return texture;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::TextureCube> TRAP::Graphics::TextureCube::CreateFromFile(const std::string& filepath,
	                                                                                 const TextureCubeFormat format,
																					 const TextureUsage usage)
{
	TP_PROFILE_FUNCTION();

	std::string name = VFS::GetFileName(VFS::MakeVirtualPathCompatible(filepath));

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		TRAP::Scope<TextureCube> texture = TRAP::Scope<TextureCube>(new TextureCube());

		//Load Texture
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Filepaths[0] = filepath;
		desc.IsCubemap = true;
		/*desc.Type = format == TextureCubeFormat::Cross ? RendererAPI::TextureCubeType::Cross :
		              RendererAPI::TextureCubeType::Equirectangular;*/
		desc.Type = RendererAPI::TextureCubeType::Cross; //TODO Replace when Equirectangular is implemented
		desc.Texture = &texture->m_texture;

		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &texture->m_syncToken);
		texture->m_name = name;
		texture->m_filepaths[0] = filepath;
		texture->m_textureType = TextureType::TextureCube;
		texture->m_textureUsage = usage;

		return texture;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::TextureCube> TRAP::Graphics::TextureCube::CreateFromImage(const std::string& name,
	                                                                                  const Scope<Image>& img,
																					  const TextureCubeFormat format,
																					  const TextureUsage usage)
{
	TP_PROFILE_FUNCTION();

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		//TODO
		return nullptr;
		//return MakeScope<API::VulkanTextureCube>(name, img, format);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::TextureCube> TRAP::Graphics::TextureCube::Create(const TextureUsage usage)
{
	return nullptr; //TODO
}

//-------------------------------------------------------------------------------------------------------------------//

std::array<std::string, 6> TRAP::Graphics::TextureCube::GetFilePaths() const
{
	return m_filepaths;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureCubeFormat TRAP::Graphics::TextureCube::GetTextureCubeFormat() const
{
	return m_textureFormat;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::TextureCube::GetDepth() const
{
	return 1;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::TextureCube::GetArraySize() const
{
	return 6;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureCube::UploadImage(const TRAP::Scope<TRAP::Image>& image)
{
	//TODO
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureCube::TextureCube()
	: m_textureFormat(TextureCubeFormat::Cross)
{
	m_textureType = TextureType::TextureCube;
}