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