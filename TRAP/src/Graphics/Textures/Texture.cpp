#include "TRAPPCH.h"
#include "Texture.h"

TRAP::Graphics::TextureWrap TRAP::Graphics::Texture::s_WrapMode = TextureWrap::Clamp_To_Edge;
TRAP::Graphics::TextureFilter TRAP::Graphics::Texture::s_FilterMode = TextureFilter::Nearest;

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