#include "TRAPPCH.h"
#include "Texture2D.h"

#include "Graphics/API/RendererAPI.h"
#include "VFS/VFS.h"
#include "Graphics/API/ResourceLoader.h"
#include "TextureBase.h"

TRAP::Scope<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::CreateFromFile(const std::string& name,
	                                                                             const std::string_view filepath,
																				 const TextureUsage usage)
{
	TP_PROFILE_FUNCTION();

	if(name.empty())
	{
		TP_WARN(Log::Texture2DPrefix, "Name is empty! Using Filename as Texture2D Name!");
		return CreateFromFile(filepath, usage);
	}

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		TRAP::Scope<Texture2D> texture = TRAP::Scope<Texture2D>(new Texture2D());

		//Load Texture
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Filepaths[0] = filepath;
		desc.IsCubemap = false;
		desc.Texture = &texture->m_texture;

		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &texture->m_syncToken);
		texture->m_name = name;
		texture->m_filepath = filepath;
		texture->m_textureType = TextureType::Texture2D;
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

TRAP::Scope<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::CreateFromFile(const std::string_view filepath,
	                                                                             const TextureUsage usage)
{
	TP_PROFILE_FUNCTION();

	std::string name = VFS::GetFileName(VFS::MakeVirtualPathCompatible(filepath));

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		TRAP::Scope<Texture2D> texture = TRAP::Scope<Texture2D>(new Texture2D());

		//Load Texture
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Filepaths[0] = filepath;
		desc.IsCubemap = false;
		desc.Texture = &texture->m_texture;

		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &texture->m_syncToken);
		texture->m_name = name;
		texture->m_filepath = filepath;
		texture->m_textureType = TextureType::Texture2D;
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

TRAP::Scope<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::CreateFromImage(const std::string& name,
	                                                                              const Scope<Image>& img,
																				  const TextureUsage usage)
{
	TP_PROFILE_FUNCTION();

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		//TODO
		return nullptr;
		//return MakeScope<API::VulkanTexture2D>(name, img);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::CreateEmpty(uint32_t width, uint32_t height,
	                                                                          uint32_t bitsPerPixel,
																			  Image::ColorFormat format,
																			  const TextureUsage usage)
{
	TP_PROFILE_FUNCTION();

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		API::ImageFormat imageFormat = ColorFormatBPPToImageFormat(format, bitsPerPixel);

		if(imageFormat == API::ImageFormat::Undefined)
			return nullptr;

		TRAP::Scope<Texture2D> texture = TRAP::Scope<Texture2D>(new Texture2D());

		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Desc = TRAP::MakeRef<RendererAPI::TextureDesc>();
		desc.Desc->Width = width;
		desc.Desc->Height = height;
		desc.Desc->Format = imageFormat;
		desc.Desc->Descriptors = RendererAPI::DescriptorType::Texture;
		desc.IsCubemap = false;
		desc.Texture = &texture->m_texture;

		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &texture->m_syncToken);
		texture->m_textureType = TextureType::Texture2D;
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

TRAP::Scope<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::Create(const TextureUsage usage)
{
	return CreateFromImage("Fallback2D", TRAP::Image::LoadFallback(), TextureUsage::Static);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture2D::GetDepth() const
{
	return 1;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture2D::GetArraySize() const
{
	return m_texture->GetArraySize();
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Graphics::Texture2D::GetFilePath() const
{
	return m_filepath;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture2D::Texture2D()
{
	m_textureType = TextureType::Texture2D;
}