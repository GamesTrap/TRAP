#include "TRAPPCH.h"
#include "CustomImage.h"

#include "Embed.h"
#include "TARGA/TGAImage.h"

TRAP::INTERNAL::CustomImage::CustomImage(std::string filepath, const uint32_t width, const uint32_t height, const ColorFormat format, std::vector<uint8_t> pixelData)
	: m_data(std::move(pixelData))
{
	TP_PROFILE_FUNCTION();

	switch (format)
	{
	case ColorFormat::RGB:
		m_bitsPerPixel = 24;
		break;
	case ColorFormat::RGBA:
		m_bitsPerPixel = 32;
		break;
	case ColorFormat::GrayScale:
		m_bitsPerPixel = 8;
		break;
	case ColorFormat::GrayScaleAlpha:
		m_bitsPerPixel = 16;
		break;

	case ColorFormat::NONE:
	default:
	{
		TP_ERROR(Log::ImagePrefix, "ColorFormat is invalid!");
		TP_WARN(Log::ImagePrefix, "Using Default Image!");
		m_width = 32;
		m_height = 32;
		m_colorFormat = ColorFormat::RGBA;
		m_bitsPerPixel = 32;
		m_data = std::vector<uint8_t>{ Embed::DefaultImageData.begin(), Embed::DefaultImageData.end() };
		m_isHDR = false;
		return;
	}
	}
	m_width = width;
	m_height = height;
	m_colorFormat = format;
	m_filepath = std::move(filepath);
	m_isHDR = false;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::CustomImage::CustomImage(std::string filepath, const uint32_t width, const uint32_t height, const ColorFormat format, std::vector<uint16_t> pixelData)
	: m_data2Byte(std::move(pixelData))
{
	TP_PROFILE_FUNCTION();

	switch (format)
	{
	case ColorFormat::RGB:
		m_bitsPerPixel = 48;
		break;
	case ColorFormat::RGBA:
		m_bitsPerPixel = 64;
		break;
	case ColorFormat::GrayScale:
		m_bitsPerPixel = 16;
		break;
	case ColorFormat::GrayScaleAlpha:
		m_bitsPerPixel = 32;
		break;

	case ColorFormat::NONE:
	default:
	{
		TP_ERROR(Log::ImagePrefix, "ColorFormat is invalid!");
		TP_WARN(Log::ImagePrefix, "Using Default Image!");
		m_width = 32;
		m_height = 32;
		m_colorFormat = ColorFormat::RGBA;
		m_bitsPerPixel = 32;
		m_data = std::vector<uint8_t>{ Embed::DefaultImageData.begin(), Embed::DefaultImageData.end() };
		m_isHDR = false;
		return;
	}
	}
	m_width = width;
	m_height = height;
	m_colorFormat = format;
	m_filepath = std::move(filepath);
	m_isHDR = false;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::CustomImage::CustomImage(std::string filepath, const uint32_t width, const uint32_t height, const ColorFormat format, std::vector<float> pixelData)
	: m_dataHDR(std::move(pixelData))
{
	TP_PROFILE_FUNCTION();

	switch (format)
	{
	case ColorFormat::RGB:
		m_bitsPerPixel = 96;
		break;
	case ColorFormat::RGBA:
		m_bitsPerPixel = 128;
		break;
	case ColorFormat::GrayScale:
		m_bitsPerPixel = 32;
		break;
	case ColorFormat::GrayScaleAlpha:
		m_bitsPerPixel = 64;
		break;

	case ColorFormat::NONE:
	default:
	{
		TP_ERROR(Log::ImagePrefix, "ColorFormat is invalid!");
		TP_WARN(Log::ImagePrefix, "Using Default Image!");
		m_width = 32;
		m_height = 32;
		m_colorFormat = ColorFormat::RGBA;
		m_bitsPerPixel = 32;
		m_data = std::vector<uint8_t>{ Embed::DefaultImageData.begin(), Embed::DefaultImageData.end() };
		m_isHDR = false;
		return;
	}
	}
	m_width = width;
	m_height = height;
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

uint64_t TRAP::INTERNAL::CustomImage::GetPixelDataSize() const
{
	if (m_data.empty())
	{
		if (m_data2Byte.empty())
			return m_dataHDR.size() * sizeof(float);
		
		return m_data2Byte.size() * sizeof(uint16_t);
	}

	return m_data.size();
}