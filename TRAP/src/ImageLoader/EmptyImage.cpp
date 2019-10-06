#include "TRAPPCH.h"

#include "EmptyImage.h"

TRAP::INTERNAL::EmptyImage::EmptyImage(const ImageFormat format, const uint32_t width, const uint32_t height)
	: m_format(format), m_isImageColored(false), m_isImageGrayScale(false), m_hasAlphaChannel(false), m_width(width), m_height(height), m_bitPerPixel(0)
{	
	switch(format)
	{
	case ImageFormat::Gray_Scale:
		m_bitPerPixel = 8;
		m_isImageGrayScale = true;
		break;
		
	case ImageFormat::Gray_Scale_Alpha:
		m_bitPerPixel = 16;
		m_isImageGrayScale = true;
		m_hasAlphaChannel = true;
		break;
		
	case ImageFormat::RGB:
		m_bitPerPixel = 24;
		m_isImageColored = true;
		break;

	case ImageFormat::RGBA:
		m_bitPerPixel = 32;
		m_isImageColored = true;
		m_hasAlphaChannel = true;
		break;
		
	default:
		break;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void* TRAP::INTERNAL::EmptyImage::GetPixelData()
{
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::EmptyImage::GetPixelDataSize() const
{
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::EmptyImage::GetBitsPerPixel() const
{
	return m_bitPerPixel;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::EmptyImage::GetBytesPerPixel() const
{
	return m_bitPerPixel / 8;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::EmptyImage::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::EmptyImage::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::EmptyImage::HasAlphaChannel() const
{
	return m_hasAlphaChannel;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::EmptyImage::IsImageCompressed() const
{
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::EmptyImage::IsImageGrayScale() const
{
	return m_isImageGrayScale;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::EmptyImage::IsImageColored() const
{
	return m_isImageColored;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::EmptyImage::IsHDR() const
{
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::INTERNAL::EmptyImage::GetFilePath() const
{
	return "";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::ImageFormat TRAP::INTERNAL::EmptyImage::GetFormat() const
{
	return m_format;
}