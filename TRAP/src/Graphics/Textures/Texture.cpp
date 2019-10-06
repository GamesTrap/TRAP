#include "TRAPPCH.h"
#include "Texture.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureParameters::TextureParameters()
	: Filter(TextureFilter::Nearest), Wrap(TextureWrap::Clamp_To_Edge)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureParameters::TextureParameters(const TextureFilter filter)
	: Filter(filter), Wrap(TextureWrap::Clamp_To_Edge)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureParameters::TextureParameters(const TextureFilter filter, const TextureWrap wrap)
	: Filter(filter), Wrap(wrap)
{
}

//-------------------------------------------------------------------------------------------------------------------//

uint8_t TRAP::Graphics::Texture::GetStrideFromFormat(const ImageFormat format)
{
	switch(format)
	{
	case ImageFormat::RGB:
		return 3;

	case ImageFormat::RGBA:
		return 4;

	case ImageFormat::Gray_Scale:
		return 1;

	case ImageFormat::Gray_Scale_Alpha:
		return 2;

	default:
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::TRAPImageFormatToOpenGL(const ImageFormat format)
{
	switch (format)
	{
	case ImageFormat::RGB:
		return GL_RGB;

	case ImageFormat::RGBA:
		return GL_RGBA;

	case ImageFormat::Gray_Scale:
		return GL_RED;

	case ImageFormat::Gray_Scale_Alpha:
		return GL_RG;

	default:
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::TRAPImageFormatToOpenGLPrecise(const ImageFormat format, const uint32_t bytesPerPixel)
{
	switch (format)
	{
	case ImageFormat::RGB:
	{
		if (bytesPerPixel == 3)
			return GL_RGB8;
		if (bytesPerPixel == 6)
			return GL_RGB16;
		if (bytesPerPixel == 12)
			return GL_RGB32F;

	}

	case ImageFormat::RGBA:
	{
		if (bytesPerPixel == 4)
			return GL_RGBA8;
		if (bytesPerPixel == 8)
			return GL_RGBA16;
		if (bytesPerPixel == 16)
			return GL_RGBA32F;
	}

	case ImageFormat::Gray_Scale:
	{
		if (bytesPerPixel == 1)
			return GL_R8;
		if (bytesPerPixel == 2)
			return GL_R16;
		if (bytesPerPixel == 4)
			return GL_R32F;
	}

	case ImageFormat::Gray_Scale_Alpha:
	{
		if (bytesPerPixel == 2)
			return GL_RG8;
		if (bytesPerPixel == 4)
			return GL_RG16;
		if (bytesPerPixel == 8)
			return GL_RG32F;
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