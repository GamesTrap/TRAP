#include "TRAPPCH.h"

#include "EmptyImage.h"

TRAP::INTERNAL::EmptyImage::EmptyImage(ImageFormat format, uint32_t width, uint32_t height)
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