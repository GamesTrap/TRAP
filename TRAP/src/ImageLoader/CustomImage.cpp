#include "TRAPPCH.h"
#include "CustomImage.h"
#include "TARGA/TGAImage.h"

TRAP::INTERNAL::CustomImage::CustomImage(std::string filepath, const uint32_t width, const uint32_t height, const uint32_t bitsPerPixel, const ImageFormat format, const std::vector<uint8_t> pixelData)
	: m_data(pixelData), m_width(width), m_height(height), m_bitsPerPixel(bitsPerPixel), m_format(format), m_filepath(std::move(filepath))
{
	TP_PROFILE_FUNCTION();

	TP_DEBUG("[Image][Custom] Loading a custom image");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::CustomImage::CustomImage(std::string filepath, const uint32_t width, const uint32_t height, const uint32_t bitsPerPixel, const ImageFormat format, const std::vector<uint16_t> pixelData)
	: m_data2Byte(pixelData), m_width(width), m_height(height), m_bitsPerPixel(bitsPerPixel), m_format(format), m_filepath(std::move(filepath))
{
	TP_PROFILE_FUNCTION();

	TP_DEBUG("[Image][Custom] Loading a custom image");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::CustomImage::CustomImage(std::string filepath, const uint32_t width, const uint32_t height, const uint32_t bitsPerPixel, const ImageFormat format, const std::vector<float> pixelData)
	: m_dataHDR(pixelData), m_width(width), m_height(height), m_bitsPerPixel(bitsPerPixel), m_format(format), m_filepath(std::move(filepath))
{
	TP_PROFILE_FUNCTION();

	TP_DEBUG("[Image][Custom] Loading a custom image");
}

//-------------------------------------------------------------------------------------------------------------------//

void* TRAP::INTERNAL::CustomImage::GetPixelData()
{
	if (m_data.empty())
	{
		if (m_data2Byte.empty())
			return m_dataHDR.data();

		return m_data2Byte.data();
	}

	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::CustomImage::GetPixelDataSize() const
{
	if (m_data.empty())
	{
		if (m_data2Byte.empty())
			return static_cast<uint32_t>(m_dataHDR.size());
		
		return static_cast<uint32_t>(m_data2Byte.size());
	}

	return static_cast<uint32_t>(m_data.size());
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::CustomImage::GetBitsPerPixel() const
{
	return m_bitsPerPixel;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::CustomImage::GetBytesPerPixel() const
{
	return m_bitsPerPixel / 8;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::CustomImage::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::CustomImage::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::CustomImage::HasAlphaChannel() const
{
	return HasAlpha(m_format);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::CustomImage::IsImageGrayScale() const
{
	return IsGrayScale(m_format);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::CustomImage::IsImageColored() const
{
	return IsColored(m_format);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::CustomImage::IsHDR() const
{
	return !m_dataHDR.empty();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::INTERNAL::CustomImage::GetFilePath() const
{
	return m_filepath;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::ImageFormat TRAP::INTERNAL::CustomImage::GetFormat() const
{
	return m_format;
}