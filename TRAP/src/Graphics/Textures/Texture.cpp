#include "TRAPPCH.h"
#include "Texture.h"

TRAP::Graphics::API::TextureWrap TRAP::Graphics::API::Texture::s_WrapMode = TextureWrap::CLAMP_TO_EDGE;
TRAP::Graphics::API::TextureFilter TRAP::Graphics::API::Texture::s_FilterMode = TextureFilter::NEAREST;

//-------------------------------------------------------------------------------------------------------------------//

uint8_t TRAP::Graphics::API::Texture::GetStrideFromFormat(const TextureFormat format)
{
	switch(format)
	{
	case TextureFormat::RGB:
		return 3;

	case TextureFormat::RGBA:
		return 4;

	default:
		return 0;
	}
}