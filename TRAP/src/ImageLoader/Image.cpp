#include "TRAPPCH.h"
#include "Image.h"

#include "VFS/VFS.h"
#include "Utils/String.h"

#include "DefaultImage.h"
#include "PortableMaps/PGMImage.h"
#include "PortableMaps/PPMImage.h"
#include "PortableMaps/PNMImage.h"
#include "PortableMaps/PAMImage.h"
#include "PortableMaps/PFMImage.h"
#include "TARGA/TGAImage.h"
#include "Bitmap/BMPImage.h"

std::vector<uint8_t> TRAP::INTERNAL::ConvertBGR16ToRGB24(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
{
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

std::vector<uint8_t> TRAP::INTERNAL::ConvertBGR24ToRGB24(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
{
	for (uint32_t i = 0; i < width * height * 3; i += 3)
		source[i] ^= source[i + 2] ^= source[i] ^= source[i + 2];

	return source;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::INTERNAL::ConvertBGRA32ToRGBA32(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
{	
	for (uint32_t i = 0; i < width * height * 4; i += 4)
		source[i] ^= source[i + 2] ^= source[i] ^= source[i + 2];

	return source;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::INTERNAL::DecodeBGRAMap(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height, const uint32_t channels, std::vector<uint8_t>& colorMap)
{
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

std::vector<uint8_t> TRAP::INTERNAL::DecodeRLEBGRAMap(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height, const uint32_t channels, std::vector<uint8_t>& colorMap)
{
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

std::vector<uint8_t> TRAP::INTERNAL::DecodeRLEGrayScale(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
{
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

std::vector<uint8_t> TRAP::INTERNAL::ConvertRLEBGR16ToRGB24(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
{
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

std::vector<uint8_t> TRAP::INTERNAL::ConvertRLEBGR24ToRGB24(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
{
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

std::vector<uint8_t> TRAP::INTERNAL::ConvertRLEBGRA32ToRGBA(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
{
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
	else
	{
		TP_ERROR("[Image] Unsupported or unknown Image Format!");
		TP_WARN("[Image] Using Default Image!");
		return MakeScope<INTERNAL::DefaultImage>(virtualFilePath);
	}

	//Test for Errors
	if (result->GetPixelDataSize() == 0 || result->GetFormat() == ImageFormat::NONE)
		result = MakeScope<INTERNAL::DefaultImage>(virtualFilePath);

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Image> TRAP::Image::LoadFallback()
{
	return MakeScope<INTERNAL::DefaultImage>("");
}