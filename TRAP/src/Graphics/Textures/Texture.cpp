#include "TRAPPCH.h"
#include "Texture.h"

#include "TextureBase.h"

TRAP::Graphics::Texture::Texture()
	: m_textureType(TextureType::Texture2D),
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

		TRAP_ASSERT(false, "Invalid bits per pixel & color format combination provided!");
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

		TRAP_ASSERT(false, "Invalid bits per pixel & color format combination provided!");
		return API::ImageFormat::Undefined;
	}
	else if(colorFormat == Image::ColorFormat::RGB)
	{
		TRAP_ASSERT(false, "Color format RGB is not allowed on empty textures as GPU needs an alpha channel!");
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

		TRAP_ASSERT(false, "Invalid bits per pixel & color format combination provided!");
		return API::ImageFormat::Undefined;
	}

	TRAP_ASSERT(false, "Invalid color format provided!");
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

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Image> TRAP::Graphics::Texture::Rotate90Clockwise(const TRAP::Scope<TRAP::Image>& img)
{
	if(img->GetBitsPerChannel() == 32)
	{
		std::vector<float> rotated(img->GetWidth() * img->GetHeight() * static_cast<uint32_t>(img->GetColorFormat()));
		for(uint32_t y = 0, destCol = img->GetHeight() - 1; y < img->GetHeight(); ++y, --destCol)
		{
			const uint32_t offset = y * img->GetWidth();

			for(uint32_t x = 0; x < img->GetWidth(); ++x)
			{
				for(uint32_t channel = 0; channel < static_cast<uint32_t>(img->GetColorFormat()); ++channel)
				{
					rotated[(x * img->GetHeight() + destCol) * static_cast<uint32_t>(img->GetColorFormat()) + channel] =
					static_cast<const float*>(img->GetPixelData())[(offset + x) * static_cast<uint32_t>(img->GetColorFormat()) + channel];
				}
			}
		}
		return TRAP::Image::LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), rotated);
	}
	else if(img->GetBitsPerChannel() == 16)
	{
		std::vector<uint16_t> rotated(img->GetWidth() * img->GetHeight() * static_cast<uint32_t>(img->GetColorFormat()));
		for(uint32_t y = 0, destCol = img->GetHeight() - 1; y < img->GetHeight(); ++y, --destCol)
		{
			const uint32_t offset = y * img->GetWidth();

			for(uint32_t x = 0; x < img->GetWidth(); ++x)
			{
				for(uint32_t channel = 0; channel < static_cast<uint32_t>(img->GetColorFormat()); ++channel)
				{
					rotated[(x * img->GetHeight() + destCol) * static_cast<uint32_t>(img->GetColorFormat()) + channel] =
					static_cast<const uint16_t*>(img->GetPixelData())[(offset + x) * static_cast<uint32_t>(img->GetColorFormat()) + channel];
				}
			}
		}
		return TRAP::Image::LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), rotated);
	}

	std::vector<uint8_t> rotated(img->GetWidth() * img->GetHeight() * static_cast<uint32_t>(img->GetColorFormat()));
	for(uint32_t y = 0, destCol = img->GetHeight() - 1; y < img->GetHeight(); ++y, --destCol)
	{
		const uint32_t offset = y * img->GetWidth();

		for(uint32_t x = 0; x < img->GetWidth(); ++x)
		{
			for(uint32_t channel = 0; channel < static_cast<uint32_t>(img->GetColorFormat()); ++channel)
			{
				rotated[(x * img->GetHeight() + destCol) * static_cast<uint32_t>(img->GetColorFormat()) + channel] =
				static_cast<const uint8_t*>(img->GetPixelData())[(offset + x) * static_cast<uint32_t>(img->GetColorFormat()) + channel];
			}
		}
	}

	return TRAP::Image::LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), rotated);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Image> TRAP::Graphics::Texture::Rotate90CounterClockwise(const TRAP::Scope<TRAP::Image>& img)
{
	if(img->GetBitsPerChannel() == 32)
	{
		std::vector<float> rotated(img->GetWidth() * img->GetHeight() * static_cast<uint32_t>(img->GetColorFormat()));
		std::memcpy(rotated.data(), img->GetPixelData(), img->GetPixelDataSize());
		for(uint32_t x = 0; x < img->GetWidth(); ++x)
		{
			for(uint32_t y = 0; y < img->GetHeight(); ++y)
			{
				uint32_t I = y;
				uint32_t J = img->GetWidth() - 1 - x;
				while((x * img->GetHeight() + y) > (I * img->GetWidth() + J))
				{
					const uint32_t p = I * img->GetWidth() + J;
					I = p % img->GetHeight();
					J = img->GetWidth() - 1 - (p / img->GetHeight());
				}
				for(uint32_t channel = 0; channel < static_cast<uint32_t>(img->GetColorFormat()); ++channel)
				{
					std::swap(rotated[(x * img->GetHeight() + y) * static_cast<uint32_t>(img->GetColorFormat()) + channel],
							  rotated[(I * img->GetWidth() + J) * static_cast<uint32_t>(img->GetColorFormat()) + channel]);
				}
			}
		}

		return TRAP::Image::LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), rotated);
	}
	else if(img->GetBitsPerChannel() == 16)
	{
		std::vector<uint16_t> rotated(img->GetWidth() * img->GetHeight() * static_cast<uint32_t>(img->GetColorFormat()));
		std::memcpy(rotated.data(), img->GetPixelData(), img->GetPixelDataSize());
		for(uint32_t x = 0; x < img->GetWidth(); ++x)
		{
			for(uint32_t y = 0; y < img->GetHeight(); ++y)
			{
				uint32_t I = y;
				uint32_t J = img->GetWidth() - 1 - x;
				while((x * img->GetHeight() + y) > (I * img->GetWidth() + J))
				{
					const uint32_t p = I * img->GetWidth() + J;
					I = p % img->GetHeight();
					J = img->GetWidth() - 1 - (p / img->GetHeight());
				}
				for(uint32_t channel = 0; channel < static_cast<uint32_t>(img->GetColorFormat()); ++channel)
				{
					std::swap(rotated[(x * img->GetHeight() + y) * static_cast<uint32_t>(img->GetColorFormat()) + channel],
							  rotated[(I * img->GetWidth() + J) * static_cast<uint32_t>(img->GetColorFormat()) + channel]);
				}
			}
		}

		return TRAP::Image::LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), rotated);
	}

	std::vector<uint8_t> rotated(img->GetWidth() * img->GetHeight() * static_cast<uint32_t>(img->GetColorFormat()));
	std::memcpy(rotated.data(), img->GetPixelData(), img->GetPixelDataSize());
	for(uint32_t x = 0; x < img->GetWidth(); ++x)
	{
		for(uint32_t y = 0; y < img->GetHeight(); ++y)
		{
			uint32_t I = y;
			uint32_t J = img->GetWidth() - 1 - x;
			while((x * img->GetHeight() + y) > (I * img->GetWidth() + J))
			{
				const uint32_t p = I * img->GetWidth() + J;
				I = p % img->GetHeight();
				J = img->GetWidth() - 1 - (p / img->GetHeight());
			}
			for(uint32_t channel = 0; channel < static_cast<uint32_t>(img->GetColorFormat()); ++channel)
			{
				std::swap(rotated[(x * img->GetHeight() + y) * static_cast<uint32_t>(img->GetColorFormat()) + channel],
						  rotated[(I * img->GetWidth() + J) * static_cast<uint32_t>(img->GetColorFormat()) + channel]);
			}
		}
	}

	return TRAP::Image::LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), rotated);
}