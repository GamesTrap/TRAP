#include "TRAPPCH.h"
#include "Image.h"

#include "VFS/VFS.h"
#include "Utils/String/String.h"

#include "PortableMaps/PGMImage.h"
#include "PortableMaps/PPMImage.h"
#include "PortableMaps/PNMImage.h"
#include "PortableMaps/PAMImage.h"
#include "PortableMaps/PFMImage.h"
#include "TARGA/TGAImage.h"
#include "Bitmap/BMPImage.h"
#include "PortableNetworkGraphics/PNGImage.h"
#include "RadianceHDR/RadianceImage.h"
#include "CustomImage.h"
#include "Embed.h"

TRAP::Image::Image()
	: m_width(0), m_height(0), m_isHDR(false), m_colorFormat(ColorFormat::NONE), m_filepath(""), m_bitsPerPixel(0)
{
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Image::GetBitsPerPixel() const
{
	return m_bitsPerPixel;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Image::GetBytesPerPixel() const
{
	return m_bitsPerPixel / 8;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Image::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Image::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2ui TRAP::Image::GetSize() const
{
	return Math::Vec2ui{ m_width, m_height };
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Image::HasAlphaChannel() const
{
	return HasAlpha(m_colorFormat);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Image::IsImageGrayScale() const
{
	return IsGrayScale(m_colorFormat);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Image::IsImageColored() const
{
	return IsColored(m_colorFormat);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Image::IsHDR() const
{
	return m_isHDR;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Image::GetFilePath() const
{
	return m_filepath;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Image::ColorFormat TRAP::Image::GetColorFormat() const
{
	return m_colorFormat;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::Image::ConvertBGR16ToRGB24(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
{
	TP_PROFILE_FUNCTION();

	std::vector<uint8_t> data{};
	data.resize(width * height * 3);

	uint32_t index = 0;
	for (uint32_t i = 0; i < width * height * 2; i += 2)
	{
		data[index++] = (source[i + 1] << 1) & 0xF8;
		data[index++] = ((source[i + 1] << 6) | (source[i] >> 2)) & 0xF8;
		data[index++] = (source[i] << 3) & 0xF8;
	}

	return data;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::Image::ConvertBGR24ToRGB24(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
{
	TP_PROFILE_FUNCTION();

	for (uint32_t i = 0; i < width * height * 3; i += 3)
		source[i] ^= source[i + 2] ^= source[i] ^= source[i + 2];

	return source;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::Image::ConvertBGRA32ToRGBA32(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
{
	TP_PROFILE_FUNCTION();

	for (uint32_t i = 0; i < width * height * 4; i += 4)
		source[i] ^= source[i + 2] ^= source[i] ^= source[i + 2];

	return source;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::Image::DecodeBGRAMap(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height, const uint32_t channels, std::vector<uint8_t>& colorMap)
{
	TP_PROFILE_FUNCTION();

	std::vector<uint8_t> data{};
	data.resize(width * height * channels);

	uint32_t index = 0;
	for (uint32_t i = 0; i < width * height; i++)
	{
		if (channels == 1)
			data[index++] = colorMap[source[i] * channels];
		else if (channels == 2)
		{
			data[index++] = (colorMap[source[i] * channels + 1] << 1) & 0xF8;
			data[index++] = ((colorMap[source[i] * channels + 1] << 6) | (colorMap[source[i] * channels] >> 2)) & 0xF8;
			data[index++] = (colorMap[source[i] * channels] << 3) & 0xF8;
		}
		else if (channels == 3)
		{
			data[index++] = colorMap[source[i] * channels + 2];
			data[index++] = colorMap[source[i] * channels + 1];
			data[index++] = colorMap[source[i] * channels + 0];
		}
		else if (channels == 4)
		{
			data[index++] = colorMap[source[i] * channels + 2];
			data[index++] = colorMap[source[i] * channels + 1];
			data[index++] = colorMap[source[i] * channels + 0];
			data[index++] = colorMap[source[i] * channels + 3];
		}
	}

	return data;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::Image::DecodeRLEBGRAMap(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height, const uint32_t channels, std::vector<uint8_t>& colorMap)
{
	TP_PROFILE_FUNCTION();

	std::vector<uint8_t> data{};
	data.resize(width * height * channels);

	uint32_t index = 0;
	for (uint32_t i = 0, l = 0; i < source.size();)
	{
		//Pixels encoded in "packets"

		//First byte is RAW/RLE flag(upper bit) and count(1-128 as 0-127 in lower 7 bits)
		//If RAW, the next count channels-byte color values in the file are taken verbatim
		//If RLE, the next single channels-byte color value speaks for the next count pixels

		const int32_t raw = (source[i] & 0x80) == 0; //Is this packet RAW pixels or a repeating color
		int32_t count = (source[i] & 0x7F) + 1; //How many RAW pixels or color repeats
		i++;

		//Prevent from writing out of data range
		if (count * channels + l > width * height * channels)
			count = static_cast<int32_t>((width * height * channels - l) / channels);

		for (int32_t j = 0; j < count; j++)
		{
			if (channels == 1)
			{
				data[index++] = colorMap[source[i] * channels];
				l++;
			}
			else if (channels == 2)
			{
				data[index++] = (colorMap[source[i] * channels + 1] << 1) & 0xF8;
				data[index++] = ((colorMap[source[i] * channels + 1] << 6) | (colorMap[source[i] * channels] >> 2)) & 0xF8;
				data[index++] = (colorMap[source[i] * channels] << 3) & 0xF8;
			}
			else if (channels == 3)
			{
				data[index++] = colorMap[source[i] * channels + 2];
				data[index++] = colorMap[source[i] * channels + 1];
				data[index++] = colorMap[source[i] * channels + 0];
				l += 3;
			}
			else if (channels == 4)
			{
				data[index++] = colorMap[source[i] * channels + 2];
				data[index++] = colorMap[source[i] * channels + 1];
				data[index++] = colorMap[source[i] * channels + 0];
				data[index++] = colorMap[source[i] * channels + 3];
				l += 4;
			}

			if (raw) //In RAW mode, keep advancing to subsequent values
				i++; //In RLE mode, just repeat the packet[1] color
		}
		if (!raw) //After outputting count values, advance if RLE
			i++;
	}

	return data;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::Image::DecodeRLEGrayScale(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
{
	TP_PROFILE_FUNCTION();

	std::vector<uint8_t> data{};
	data.resize(width * height);

	uint32_t index = 0;
	for (uint32_t i = 0, l = 0; i < source.size();)
	{
		//Pixels encoded in "packets"

		//First byte is RAW/RLE flag(upper bit) and count(1-128 as 0-127 in lower 7 bits)
		//If RAW, the next count byte color values in the file are taken verbatim
		//If RLE, the next single byte color value speaks for the next count pixels

		const int32_t raw = (source[i] & 0x80) == 0; //Is this packet RAW pixels or a repeating color
		int32_t count = (source[i] & 0x7F) + 1; //How many RAW pixels or color repeats
		i++;

		//Prevent from writing out of data range
		if (count + l > width * height)
			count = static_cast<int32_t>(width * height - l);

		for (int32_t j = 0; j < count; j++)
		{
			data[index++] = source[i];

			if (raw) //In RAW mode, keep advancing to subsequent values
				i++; //In RLE mode, just repeat the packet[1] color
			l++;
		}
		if (!raw) //After outputting count values, advance if RLE
			i++;
	}

	return data;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::Image::ConvertRLEBGR16ToRGB24(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
{
	TP_PROFILE_FUNCTION();

	std::vector<uint8_t> data{};
	data.resize(width * height * 3);

	uint32_t index = 0;
	for (uint32_t i = 0, l = 0; i < source.size();)
	{
		//Pixels encoded in "packets"
		//First byte is RAW/RLE flags(upper bit) and count(1-128 as 0-127 in lower 7 bits)
		//If RAW, the next count channel-byte color values in the file are taken verbatim
		//If RLE, the next single channel-byte color value speaks for the next count pixels

		const int32_t raw = (source[i] & 0x80) == 0; //Is this packet RAW pixels or a repeating color
		int32_t count = (source[i] & 0x7F) + 1; //How many RAW pixels or color repeats
		i++;

		//Prevent from writing out of data range
		if (count * 3 + l > width * height * 3)
			count = static_cast<int32_t>((width * height * 3 - l) / 3);

		for (int32_t j = 0; j < count; j++)
		{
			data[index++] = (source[i + 1] << 1) & 0xF8;
			data[index++] = ((source[i + 1] << 6) | (source[i] >> 2)) & 0xF8;
			data[index++] = (source[i] << 3) & 0xF8;

			if (raw) //In RAW mode, keep advancing to subsequent values
				i += 2; //IN RLE mode, just repeat the packet[1] RGB color
			l += 3;
		}
		if (!raw) //After outputting count RGB values, advance if RLE
			i += 2;
	}

	return data;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::Image::ConvertRLEBGR24ToRGB24(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
{
	TP_PROFILE_FUNCTION();

	std::vector<uint8_t> data{};
	data.resize(width * height * 3);

	uint32_t index = 0;
	for (uint32_t i = 0, l = 0; i < source.size();)
	{
		//Pixels encoded in "packets"
		//First byte is RAW/RLE flags(upper bit) and count(1-128 as 0-127 in lower 7 bits)
		//If RAW, the next count channel-byte color values in the file are taken verbatim
		//If RLE, the next single channel-byte color value speaks for the next count pixels

		const int32_t raw = (source[i] & 0x80) == 0; //Is this packet RAW pixels or a repeating color
		int32_t count = (source[i] & 0x7F) + 1; //How many RAW pixels or color repeats
		i++;

		//Prevent from writing out of data range
		if (count * 3 + l > width * height * 3)
			count = static_cast<int32_t>((width * height * 3 - l) / 3);

		for (int32_t j = 0; j < count; j++)
		{
			data[index++] = source[i + 2]; //Red
			data[index++] = source[i + 1]; //Green
			data[index++] = source[i];     //Blue

			if (raw) //In RAW mode, keep advancing to subsequent values
				i += 3; //IN RLE mode, just repeat the packet[1] RGB color
			l += 3;
		}
		if (!raw) //After outputting count RGB values, advance if RLE
			i += 3;
	}

	return data;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::Image::ConvertRLEBGRA32ToRGBA(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
{
	TP_PROFILE_FUNCTION();

	std::vector<uint8_t> data{};
	data.resize(width * height * 4);

	uint32_t index = 0;
	for (uint32_t i = 0, l = 0; i < source.size();)
	{
		//Pixels encoded in "packets"
		//First byte is RAW/RLE flags(upper bit) and count(1-128 as 0-127 in lower 7 bits)
		//If RAW, the next count channel-byte color values in the file are taken verbatim
		//If RLE, the next single channel-byte color value speaks for the next count pixels

		const int32_t raw = (source[i] & 0x80) == 0; //Is this packet RAW pixels or a repeating color
		int32_t count = (source[i] & 0x7F) + 1; //How many RAW pixels or color repeats
		i++;

		//Prevent from writing out of data range
		if (count * 4 + l > width * height * 4)
			count = static_cast<int32_t>((width * height * 4 - l) / 4);

		for (int32_t j = 0; j < count; j++)
		{
			data[index++] = source[i + 2]; //Red
			data[index++] = source[i + 1]; //Green
			data[index++] = source[i];     //Blue
			data[index++] = source[i + 3]; //Alpha

			if (raw) //In RAW mode, keep advancing to subsequent values
				i += 4; //IN RLE mode, just repeat the packet[1] RGBA color
			l += 4;
		}
		if (!raw) //After outputting count RGBA values, advance if RLE
			i += 4;
	}

	return data;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Image> TRAP::Image::LoadFromFile(const std::string& filepath)
{
	TP_PROFILE_FUNCTION();

	std::string virtualFilePath = VFS::MakeVirtualPathCompatible(filepath);
	const std::string fileFormat = Utils::String::GetSuffix(Utils::String::ToLower(virtualFilePath));

	Scope<Image> result;

	if (fileFormat == "pgm")
		result = MakeScope<INTERNAL::PGMImage>(virtualFilePath);
	else if (fileFormat == "ppm")
		result = MakeScope<INTERNAL::PPMImage>(virtualFilePath);
	else if (fileFormat == "pnm")
		result = MakeScope<INTERNAL::PNMImage>(virtualFilePath);
	else if (fileFormat == "pam")
		result = MakeScope<INTERNAL::PAMImage>(virtualFilePath);
	else if (fileFormat == "pfm")
		result = MakeScope<INTERNAL::PFMImage>(virtualFilePath);
	else if (fileFormat == "tga" || fileFormat == "icb" || fileFormat == "vda" || fileFormat == "vst")
		result = MakeScope<INTERNAL::TGAImage>(virtualFilePath);
	else if (fileFormat == "bmp" || fileFormat == "dib")
		result = MakeScope<INTERNAL::BMPImage>(virtualFilePath);
	else if (fileFormat == "png")
		result = MakeScope<INTERNAL::PNGImage>(virtualFilePath);
	else if (fileFormat == "hdr" || fileFormat == "pic")
		result = MakeScope<INTERNAL::RadianceImage>(virtualFilePath);
	else
	{
		TP_ERROR(Log::ImagePrefix, "Unsupported or unknown Image Format!");
		TP_WARN(Log::ImagePrefix, "Using Default Image!");
		return MakeScope<INTERNAL::CustomImage>(virtualFilePath, 32, 32, 32, ColorFormat::RGBA, std::vector<uint8_t>{ Embed::DefaultImageData.begin(), Embed::DefaultImageData.end() });
	}

	//Test for Errors
	if (result->GetPixelDataSize() == 0 || result->GetColorFormat() == ColorFormat::NONE)
		result = MakeScope<INTERNAL::CustomImage>(virtualFilePath, 32, 32, 32, ColorFormat::RGBA, std::vector<uint8_t>{ Embed::DefaultImageData.begin(), Embed::DefaultImageData.end() });

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<TRAP::Image> TRAP::Image::LoadFromMemory(uint32_t width, uint32_t height, uint32_t bitsPerPixel, ColorFormat format, const std::vector<uint8_t>& pixelData)
{
	TP_PROFILE_FUNCTION();

	return MakeScope<INTERNAL::CustomImage>("", width, height, bitsPerPixel, format, pixelData);
}

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<TRAP::Image> TRAP::Image::LoadFromMemory(uint32_t width, uint32_t height, uint32_t bitsPerPixel, ColorFormat format, const std::vector<uint16_t>& pixelData)
{
	TP_PROFILE_FUNCTION();

	return MakeScope<INTERNAL::CustomImage>("", width, height, bitsPerPixel, format, pixelData);
}

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<TRAP::Image> TRAP::Image::LoadFromMemory(uint32_t width, uint32_t height, uint32_t bitsPerPixel, ColorFormat format, const std::vector<float>& pixelData)
{
	TP_PROFILE_FUNCTION();

	return MakeScope<INTERNAL::CustomImage>("", width, height, bitsPerPixel, format, pixelData);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Image> TRAP::Image::LoadFallback()
{
	TP_PROFILE_FUNCTION();

	return MakeScope<INTERNAL::CustomImage>("", 32, 32, 32, ColorFormat::RGBA, std::vector<uint8_t>{ Embed::DefaultImageData.begin(), Embed::DefaultImageData.end() });
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Image::IsGrayScale(const ColorFormat format)
{
	switch(format)
	{
	case ColorFormat::GrayScale:
	case ColorFormat::GrayScaleAlpha:
		return true;

	case ColorFormat::RGB:
	case ColorFormat::RGBA:
	case ColorFormat::NONE:
		return false;

	default:
		return false;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Image::IsColored(const ColorFormat format)
{
	switch (format)
	{
	case ColorFormat::GrayScale:
	case ColorFormat::GrayScaleAlpha:
		return false;

	case ColorFormat::RGB:
	case ColorFormat::RGBA:
		return true;

	case ColorFormat::NONE:
		return false;

	default:
		return false;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Image::HasAlpha(const ColorFormat format)
{
	switch (format)
	{
	case ColorFormat::GrayScale:
		return false;

	case ColorFormat::GrayScaleAlpha:
		return true;

	case ColorFormat::RGB:
		return false;

	case ColorFormat::RGBA:
		return true;

	case ColorFormat::NONE:
		return false;

	default:
		return false;
	}
}