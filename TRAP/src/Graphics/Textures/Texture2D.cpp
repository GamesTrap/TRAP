#include "TRAPPCH.h"
#include "Texture2D.h"

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/Textures/VulkanTexture2D.h"
#include "VFS/VFS.h"

uint32_t TRAP::Graphics::Texture2D::s_maxTextureSize = 0;

//-------------------------------------------------------------------------------------------------------------------//

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

TRAP::Scope<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::CreateFromFile(const std::string& name, const std::string_view filepath, TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();

	if(name.empty())
	{
		TP_WARN(Log::Texture2DPrefix, "Name is empty! Using Filename as Texture2D Name!");
		return CreateFromFile(filepath, parameters);
	}
	
	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return MakeScope<API::VulkanTexture2D>(name, filepath, parameters);

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::CreateFromFile(const std::string_view filepath, TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();

	std::string name = VFS::GetFileName(VFS::MakeVirtualPathCompatible(filepath));
	
	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return MakeScope<API::VulkanTexture2D>(name, filepath, parameters);

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::CreateFromImage(const std::string& name, const Scope<Image>& img, TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return MakeScope<API::VulkanTexture2D>(name, img, parameters);

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::CreateEmpty(uint32_t width, uint32_t height, uint32_t bitsPerPixel, Image::ColorFormat format, TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return MakeScope<API::VulkanTexture2D>(width, height, bitsPerPixel, format, parameters);

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::Create(TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return MakeScope<API::VulkanTexture2D>(parameters);

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

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