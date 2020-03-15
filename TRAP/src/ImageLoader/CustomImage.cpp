#include "TRAPPCH.h"
#include "CustomImage.h"
#include "TARGA/TGAImage.h"

TRAP::INTERNAL::CustomImage::CustomImage(std::string filepath, const uint32_t width, const uint32_t height, const uint32_t bitsPerPixel, const ColorFormat format, const std::vector<uint8_t> pixelData)
	: m_data(pixelData)
{
	TP_PROFILE_FUNCTION();

	m_width = width;
	m_height = height;
	m_bitsPerPixel = bitsPerPixel;
	m_colorFormat = format;
	m_filepath = std::move(filepath);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::CustomImage::CustomImage(std::string filepath, const uint32_t width, const uint32_t height, const uint32_t bitsPerPixel, const ColorFormat format, const std::vector<uint16_t> pixelData)
	: m_data2Byte(pixelData)
{
	TP_PROFILE_FUNCTION();

	m_width = width;
	m_height = height;
	m_bitsPerPixel = bitsPerPixel;
	m_colorFormat = format;
	m_filepath = std::move(filepath);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::CustomImage::CustomImage(std::string filepath, const uint32_t width, const uint32_t height, const uint32_t bitsPerPixel, const ColorFormat format, const std::vector<float> pixelData)
	: m_dataHDR(pixelData)
{
	TP_PROFILE_FUNCTION();

	m_width = width;
	m_height = height;
	m_bitsPerPixel = bitsPerPixel;
	m_colorFormat = format;
	m_filepath = std::move(filepath);
	m_isHDR = true;
}

//-------------------------------------------------------------------------------------------------------------------//

const void* TRAP::INTERNAL::CustomImage::GetPixelData() const
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