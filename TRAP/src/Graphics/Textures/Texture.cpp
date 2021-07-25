#include "TRAPPCH.h"
#include "Texture.h"

#include "TextureBase.h"

TRAP::Graphics::Texture::Texture()
	: m_textureType(TextureType::Texture2D),
	  m_textureUsage(TextureUsage::Static),
	  m_syncToken(),
	  m_texture(nullptr)
{
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Graphics::Texture::GetName() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureType TRAP::Graphics::Texture::GetType() const
{
	return m_textureType;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetWidth() const
{
	return m_texture->GetWidth();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetHeight() const
{
	return m_texture->GetHeight();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetMipLevels() const
{
	return m_texture->GetMipLevels();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureUsage TRAP::Graphics::Texture::GetTextureUsage() const
{
	return m_textureUsage;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Image::ColorFormat TRAP::Graphics::Texture::GetColorFormat() const
{
	return ImageFormatToColorFormat(m_texture->GetImageFormat());
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetBitsPerChannel() const
{
	return GetBitsPerChannelFromImageFormat(m_texture->GetImageFormat());
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetBytesPerChannel() const
{
	return GetBitsPerChannel() / 8;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetBitsPerPixel() const
{
	return GetBitsPerChannelFromImageFormat(m_texture->GetImageFormat()) * static_cast<uint32_t>(GetColorFormat());
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetBytesPerPixel() const
{
	return GetBitsPerPixel() / 8;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetMipWidth(const uint32_t mipLevel) const
{
	if(mipLevel >= m_texture->GetMipLevels())
		TP_ERROR(Log::TexturePrefix, "GetMipWidth: Invalid mip level provided!");

	return Math::Max(1u, m_texture->GetWidth() >> mipLevel);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetMipHeight(const uint32_t mipLevel) const
{
	if(mipLevel >= m_texture->GetMipLevels())
		TP_ERROR(Log::TexturePrefix, "GetMipHeight: Invalid mip level provided!");

	return Math::Max(1u, m_texture->GetHeight() >> mipLevel);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2ui TRAP::Graphics::Texture::GetMipSize(const uint32_t mipLevel) const
{
	return {GetMipWidth(mipLevel), GetMipHeight(mipLevel)};
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::TextureBase> TRAP::Graphics::Texture::GetTexture() const
{
	return m_texture;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::Texture::IsLoaded() const
{
	return RendererAPI::GetResourceLoader()->IsTokenCompleted(&m_syncToken);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Texture::AwaitLoading() const
{
	RendererAPI::GetResourceLoader()->WaitForToken(&m_syncToken);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::CalculateMipLevels(const uint32_t width, const uint32_t height)
{
	return Math::Max(1u, static_cast<uint32_t>(Math::Floor(Math::Log2(Math::Max(static_cast<float>(width),
					                                                            static_cast<float>(height))))) + 1);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ImageFormat TRAP::Graphics::Texture::ColorFormatBitsPerPixelToImageFormat(const Image::ColorFormat colorFormat,
	                             											                   const uint32_t bpp)
{
	if(colorFormat == Image::ColorFormat::GrayScale)
	{
		if(bpp == 8)
			return API::ImageFormat::R8_UNORM;
		if(bpp == 16)
			return API::ImageFormat::R16_UNORM;
		if(bpp == 32)
			return API::ImageFormat::R32_SFLOAT;

		TRAP_ASSERT(false, "Invalid BitsPerPixel & Image::ColorFormat combination provided!");
		return API::ImageFormat::Undefined;
	}
	else if(colorFormat == Image::ColorFormat::GrayScaleAlpha)
	{
		if(bpp == 16)
			return API::ImageFormat::R8G8_UNORM;
		if(bpp == 32)
		    return API::ImageFormat::R16G16_UNORM;
		if(bpp == 64)
		    return API::ImageFormat::R32G32_SFLOAT;

		TRAP_ASSERT(false, "Invalid BitsPerPixel & Image::ColorFormat combination provided!");
		return API::ImageFormat::Undefined;
	}
	else if(colorFormat == Image::ColorFormat::RGB)
	{
		TRAP_ASSERT(false, "Image::ColorFormat::RGB is not allowed on empty Textures as GPUs need an alpha channel!");
		return API::ImageFormat::Undefined;
	}
	else if(colorFormat == Image::ColorFormat::RGBA)
	{
		if(bpp == 32)
			return API::ImageFormat::R8G8B8A8_UNORM;
		if(bpp == 64)
			return API::ImageFormat::R16G16B16A16_UNORM;
		if(bpp == 128)
			return API::ImageFormat::R32G32B32A32_SFLOAT;

		TRAP_ASSERT(false, "Invalid BitsPerPixel & Image::ColorFormat combination provided!");
		return API::ImageFormat::Undefined;
	}

	TRAP_ASSERT(false, "Invalid Image::ColorFormat provided!");
	return API::ImageFormat::Undefined;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Image::ColorFormat TRAP::Graphics::Texture::ImageFormatToColorFormat(const API::ImageFormat imageFormat)
{
	switch(imageFormat)
	{
	case API::ImageFormat::R8_UNORM:
	case API::ImageFormat::R16_UNORM:
	case API::ImageFormat::R32_SFLOAT:
		return Image::ColorFormat::GrayScale;

	case API::ImageFormat::R8G8_UNORM:
	case API::ImageFormat::R16G16_UNORM:
	case API::ImageFormat::R32G32_SFLOAT:
		return Image::ColorFormat::GrayScaleAlpha;

	case API::ImageFormat::R8G8B8A8_UNORM:
	case API::ImageFormat::R16G16B16A16_UNORM:
	case API::ImageFormat::R32G32B32A32_SFLOAT:
		return Image::ColorFormat::RGBA;

	default:
		return Image::ColorFormat::NONE;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetBitsPerChannelFromImageFormat(const API::ImageFormat imageFormat)
{
	switch(imageFormat)
	{
	case API::ImageFormat::R8_UNORM:
	case API::ImageFormat::R8G8_UNORM:
	case API::ImageFormat::R8G8B8A8_UNORM:
		return 8;

	case API::ImageFormat::R16_UNORM:
	case API::ImageFormat::R16G16_UNORM:
	case API::ImageFormat::R16G16B16A16_UNORM:
		return 16;

	case API::ImageFormat::R32_SFLOAT:
	case API::ImageFormat::R32G32_SFLOAT:
	case API::ImageFormat::R32G32B32A32_SFLOAT:
		return 32;

	default:
		return 0;
	}
}