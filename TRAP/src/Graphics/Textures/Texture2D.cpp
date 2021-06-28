#include "TRAPPCH.h"
#include "Texture2D.h"

#include "Graphics/API/RendererAPI.h"
#include "VFS/VFS.h"
#include "Graphics/API/ResourceLoader.h"

std::vector<std::pair<TRAP::Graphics::Texture2D*, std::future<TRAP::Scope<TRAP::Image>>>> TRAP::Graphics::Texture2D::m_loadingTextures{};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture2D::Texture2D()
{
	m_textureType = TextureType::Texture2D;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Graphics::Texture2D::GetFilePath() const
{
	return m_filepath;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::CreateFromFile(const std::string& name,
	const std::string_view filepath, const TextureUsage usage)
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
		texture->m_texture->SetTextureName(texture->m_name);
		texture->m_filepath = filepath;
		texture->m_textureType = TextureType::Texture2D;
		texture->m_textureUsage = usage;

		return texture;
	}

	default:
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

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::CreateFromImage(const std::string& name,
	const Scope<Image>& img, const TextureUsage usage)
{
	TP_PROFILE_FUNCTION();

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		//TODO
		return nullptr;
		//return MakeScope<API::VulkanTexture2D>(name, img);

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::CreateEmpty(uint32_t width, uint32_t height,
	uint32_t bitsPerPixel, Image::ColorFormat format, const TextureUsage usage)
{
	TP_PROFILE_FUNCTION();

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		//TODO
		return nullptr;
		//return MakeScope<API::VulkanTexture2D>(width, height, bitsPerPixel, format);

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::Create(const TextureUsage usage)
{
	return nullptr; //TODO
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Texture2D::UploadImage(const TRAP::Scope<TRAP::Image>& image)
{
	//TODO
}

//-------------------------------------------------------------------------------------------------------------------//

//TODO Move to Texture (TextureCube support?!)
void TRAP::Graphics::Texture2D::UpdateLoadingTextures()
{
	for (uint32_t i = 0; i < m_loadingTextures.size(); i++)
	{
		auto& [texturePtr, image] = m_loadingTextures[i];
		if (texturePtr && image.valid())
		{
			if (image.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
			{
				//Image finished loading
				texturePtr->UploadImage(image.get());

				//Image isn't needed anymore so remove it from vector
				//O(1) way of removing from an unsorted vector like this :D
				m_loadingTextures[i] = std::move(m_loadingTextures.back());
				m_loadingTextures.pop_back();
			}
		}
	}
}