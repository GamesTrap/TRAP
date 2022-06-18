#include "TRAPPCH.h"
#include "Texture.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureParameters::TextureParameters()
	: Filter(TextureFilter::Nearest), Wrap(TextureWrap::Clamp_To_Edge)
{
	TP_PROFILE_FUNCTION();

}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureParameters::TextureParameters(const TextureFilter filter)
	: Filter(filter), Wrap(TextureWrap::Clamp_To_Edge)
{
	TP_PROFILE_FUNCTION();

}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureParameters::TextureParameters(const TextureFilter filter, const TextureWrap wrap)
	: Filter(filter), Wrap(wrap)
{
	TP_PROFILE_FUNCTION();

}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture::Texture()
	: m_textureType(TextureType::Texture2D)
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

TRAP::Graphics::TextureParameters TRAP::Graphics::Texture::GetParameters() const
{
	return m_textureParameters;
}

//-------------------------------------------------------------------------------------------------------------------//

uint8_t TRAP::Graphics::Texture::GetStrideFromFormat(const Image::ColorFormat format)
{
	switch(format)
	{
	case Image::ColorFormat::RGB:
		return 3;

	case Image::ColorFormat::RGBA:
		return 4;

	case Image::ColorFormat::GrayScale:
		return 1;

	case Image::ColorFormat::GrayScaleAlpha:
		return 2;

	default:
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::TRAPImageFormatToOpenGL(const Image::ColorFormat format)
{
	switch (format)
	{
	case Image::ColorFormat::RGB:
		return GL_RGB;

	case Image::ColorFormat::RGBA:
		return GL_RGBA;

	case Image::ColorFormat::GrayScale:
		return GL_RED;

	case Image::ColorFormat::GrayScaleAlpha:
		return GL_RG;

	default:
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::TRAPImageFormatToOpenGLPrecise(const Image::ColorFormat format, const uint32_t bytesPerPixel)
{
	switch (format)
	{
	case Image::ColorFormat::RGB:
	{
		if (bytesPerPixel == 3)
			return GL_RGB8;
		if (bytesPerPixel == 6)
			return GL_RGB16;
		if (bytesPerPixel == 12)
			return GL_RGB32F;

		return 0;
	}

	case Image::ColorFormat::RGBA:
	{
		if (bytesPerPixel == 4)
			return GL_RGBA8;
		if (bytesPerPixel == 8)
			return GL_RGBA16;
		if (bytesPerPixel == 16)
			return GL_RGBA32F;

		return 0;
	}

	case Image::ColorFormat::GrayScale:
	{
		if (bytesPerPixel == 1)
			return GL_R8;
		if (bytesPerPixel == 2)
			return GL_R16;
		if (bytesPerPixel == 4)
			return GL_R32F;

		return 0;
	}

	case Image::ColorFormat::GrayScaleAlpha:
	{
		if (bytesPerPixel == 2)
			return GL_RG8;
		if (bytesPerPixel == 4)
			return GL_RG16;
		if (bytesPerPixel == 8)
			return GL_RG32F;

		return 0;
	}

	default:
		return 0;
	}
}


//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::TRAPTextureWrapToOpenGL(const TextureWrap wrap)
{
	switch (wrap)
	{
	case TextureWrap::Clamp_To_Border:
		return GL_CLAMP_TO_BORDER;

	case TextureWrap::Clamp_To_Edge:
		return GL_CLAMP_TO_EDGE;

	case TextureWrap::Repeat:
		return GL_REPEAT;

	case TextureWrap::Mirrored_Repeat:
		return GL_MIRRORED_REPEAT;

	default:
		return 0;
	}
}