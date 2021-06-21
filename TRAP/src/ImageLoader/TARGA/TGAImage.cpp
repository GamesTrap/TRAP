#include "TRAPPCH.h"
#include "TGAImage.h"

#include "Utils/String/String.h"
#include "VFS/VFS.h"
#include "Application.h"
#include "Utils/ByteSwap.h"

TRAP::INTERNAL::TGAImage::TGAImage(std::string filepath)
{
	TP_PROFILE_FUNCTION();

	m_filepath = std::move(filepath);

	TP_DEBUG(Log::ImageTGAPrefix, "Loading Image: \"", Utils::String::SplitStringView(m_filepath, '/').back(), "\"");

	std::filesystem::path physicalPath;
	if (!VFS::ResolveReadPhysicalPath(m_filepath, physicalPath, true))
	{
		TP_ERROR(Log::ImageTGAPrefix, "Couldn't resolve FilePath: ", m_filepath, "!");
		TP_WARN(Log::ImageTGAPrefix, "Using Default Image!");
		return;
	}

	if (VFS::FileOrFolderExists(physicalPath))
	{
		std::ifstream file(physicalPath, std::ios::binary);
		if (!file.is_open())
		{
			TP_ERROR(Log::ImageTGAPrefix, "Couldn't open FilePath: ", m_filepath, "!");
			TP_WARN(Log::ImageTGAPrefix, "Using Default Image!");
			return;
		}

		//Start TGA Loading here
		struct Header
		{
			uint8_t IDLength = 0; //0 = No ImageID
			uint8_t ColorMapType = 0; //0 = No Color Map | 1 = Has Color Map
			uint8_t ImageType = 0; //0 = No Data | 1 = Uncompressed ColorMapped | 2 = Uncompressed TrueColor | 3 = Uncompressed BlackWhite | 9 = RunLengthEncoded ColorMapped | 10 = RunLengthEncoded TrueColor | 11 = RunLengthEncoded BlackWhite
			//ColorMap spec
			uint16_t ColorMapOffset = 0;
			uint16_t NumOfColorMaps = 0;
			uint8_t ColorMapDepth = 0;
			//Image spec
			uint16_t XOffset = 0;
			uint16_t YOffset = 0;
			uint16_t Width = 0;
			uint16_t Height = 0;
			uint8_t BitsPerPixel = 0;
			uint8_t ImageDescriptor = 0;
		} header;

		file.read(reinterpret_cast<char*>(&header.IDLength), 1);
		file.read(reinterpret_cast<char*>(&header.ColorMapType), 1);
		file.read(reinterpret_cast<char*>(&header.ImageType), 1);
		file.read(reinterpret_cast<char*>(&header.ColorMapOffset), 2);
		file.read(reinterpret_cast<char*>(&header.NumOfColorMaps), 2);
		file.read(reinterpret_cast<char*>(&header.ColorMapDepth), 1);
		file.read(reinterpret_cast<char*>(&header.XOffset), 2);
		file.read(reinterpret_cast<char*>(&header.YOffset), 2);
		file.read(reinterpret_cast<char*>(&header.Width), 2);
		file.read(reinterpret_cast<char*>(&header.Height), 2);
		file.read(reinterpret_cast<char*>(&header.BitsPerPixel), 1);
		file.read(reinterpret_cast<char*>(&header.ImageDescriptor), 1);

		//File uses little-endian
		//Convert to machines endian
		bool needSwap = Application::GetEndian() != Application::Endian::Little;
		if (needSwap)
		{
			Utils::Memory::SwapBytes(header.ColorMapOffset);
			Utils::Memory::SwapBytes(header.NumOfColorMaps);
			Utils::Memory::SwapBytes(header.XOffset);
			Utils::Memory::SwapBytes(header.YOffset);
			Utils::Memory::SwapBytes(header.Width);
			Utils::Memory::SwapBytes(header.Height);
		}

		struct ColorMapData
		{
			std::string ImageID = "";
			std::vector<uint8_t> ColorMap{};
			std::vector<uint8_t> ImageData{};
		} colorMapData;

		if (header.ImageType == 0)
		{
			file.close();
			TP_ERROR(Log::ImageTGAPrefix, "Image doesn't contain pixel data!");
			TP_WARN(Log::ImageTGAPrefix, "Using Default Image!");
			return;
		}
		if (header.IDLength != 0)
		{
			colorMapData.ImageID.resize(header.IDLength);
			file.read(colorMapData.ImageID.data(), header.IDLength);
		}
		if (header.ColorMapType == 1)
		{
			colorMapData.ColorMap.resize((header.ColorMapDepth / 8) * header.NumOfColorMaps);
			if(!file.read(reinterpret_cast<char*>(colorMapData.ColorMap.data()), (header.ColorMapDepth / 8) * header.NumOfColorMaps))
			{
				file.close();
				TP_ERROR(Log::ImageTGAPrefix, "Couldn't load Color Map!");
				TP_WARN(Log::ImageTGAPrefix, "Using Default Image!");
				return;
			}
		}
		if(header.BitsPerPixel == 15)
		{
			file.close();
			TP_ERROR(Log::ImageTGAPrefix, "BitsPerPixel 15 is unsupported!");
			TP_WARN(Log::ImageTGAPrefix, "Using Default Image!");
			return;
		}
		bool needXFlip = false;
		bool needYFlip = false;
		if ((header.ImageDescriptor & 0x30) == 0x30 || (header.ImageDescriptor & 0x30) == 0x10) //1. If Image is stored Top/Right | 2. If Image is stored Bottom/Right
			needXFlip = true;
		if ((header.ImageDescriptor & 0x30) == 0x00 || (header.ImageDescriptor & 0x30) == 0x10) //1. If Image is stored Bottom/Left | 2. If Image is stored Bottom/Right
			needYFlip = true;
		if (header.Width < 2 || header.Height < 2)
		{
			file.close();
			TP_ERROR(Log::ImageTGAPrefix, "Image Width/Height is invalid/unsupported!");
			TP_WARN(Log::ImageTGAPrefix, "Using Default Image!");
			return;
		}
		if(header.ImageType == 9 || header.ImageType == 11 || header.ImageType == 10) //All RLE formats
		{
			uint32_t currentPosition = static_cast<uint32_t>(file.tellg()); //Store current position in file
			file.seekg(0, std::ios::end); //Go to the end of file
			uint32_t pixelDataSize = static_cast<uint32_t>(file.tellg()) - currentPosition;
			file.seekg(-18, std::ios::end); //Check if there is a footer
			std::string offsetCheck(16, '\0');
			file.read(offsetCheck.data(), 16);
			if (offsetCheck == "TRUEVISION-XFILE")
				pixelDataSize -= 26; //If a footer was found subtract the 26 bytes from pixelDataSize
			file.seekg(currentPosition); //Go back to starting position
			
			colorMapData.ImageData.resize(pixelDataSize);
			if (!file.read(reinterpret_cast<char*>(colorMapData.ImageData.data()), pixelDataSize))
			{
				file.close();
				TP_ERROR(Log::ImageTGAPrefix, "Couldn't read pixel data!");
				TP_WARN(Log::ImageTGAPrefix, "Using Default Image!");
				return;
			}
		}
		else
		{
			colorMapData.ImageData.resize(header.Width* header.Height* (header.BitsPerPixel / 8));
			if (!file.read(reinterpret_cast<char*>(colorMapData.ImageData.data()), header.Width * header.Height * (header.BitsPerPixel / 8)))
			{
				file.close();
				TP_ERROR(Log::ImageTGAPrefix, "Couldn't read pixel data!");
				TP_WARN(Log::ImageTGAPrefix, "Using Default Image!");
				return;
			}
		}

		file.close();

		m_width = header.Width;
		m_height = header.Height;
		m_bitsPerPixel = header.BitsPerPixel;
		switch (header.ImageType)
		{
		case 1:
		{
			if (header.BitsPerPixel > 8)
			{
				TP_ERROR(Log::ImageTGAPrefix, "Bad ColorMapped index size: ", header.BitsPerPixel, " bits/pixel");
				TP_WARN(Log::ImageTGAPrefix, "Using Default Image!");
				return;
			}
			if (header.BitsPerPixel == 8)
			{
				m_data = DecodeBGRAMap(colorMapData.ImageData, m_width, m_height, header.ColorMapDepth / 8, colorMapData.ColorMap);
				m_bitsPerPixel = header.ColorMapDepth;
				if(m_bitsPerPixel == 16)
				{
					m_colorFormat = ColorFormat::RGB;
					m_bitsPerPixel = 24;
				}
				else if (m_bitsPerPixel == 24)
					m_colorFormat = ColorFormat::RGB;
				else if (m_bitsPerPixel == 32)
					m_colorFormat = ColorFormat::RGBA;
				break;
			}
			break;
		}

		case 9:
		{
			if (header.BitsPerPixel > 8)
			{
				TP_ERROR(Log::ImageTGAPrefix, "Bad ColorMapped RLE index size: ", header.BitsPerPixel, " bits/pixel");
				TP_WARN(Log::ImageTGAPrefix, "Using Default Image!");
				return;
			}
			if (header.BitsPerPixel == 8)
			{
				m_data = DecodeRLEBGRAMap(colorMapData.ImageData, m_width, m_height, header.ColorMapDepth / 8, colorMapData.ColorMap);
				m_bitsPerPixel = header.ColorMapDepth;
				if(m_bitsPerPixel == 16)
				{
					m_colorFormat = ColorFormat::RGB;
					m_bitsPerPixel = 24;
				}
				if (m_bitsPerPixel == 24)
					m_colorFormat = ColorFormat::RGB;
				else if (m_bitsPerPixel == 32)
					m_colorFormat = ColorFormat::RGBA;
			}
			break;
		}

		case 3:
		{
			m_colorFormat = ColorFormat::GrayScale;
			if (header.BitsPerPixel == 8)
				m_data = colorMapData.ImageData;
			if (header.BitsPerPixel > 8)
			{
				TP_ERROR(Log::ImageTGAPrefix, "Bad GrayScale pixel size: ", header.BitsPerPixel, " bits/pixel");
				TP_WARN(Log::ImageTGAPrefix, "Using Default Image!");
				return;
			}
			break;
		}

		case 11:
		{
			m_colorFormat = ColorFormat::GrayScale;
			if (header.BitsPerPixel == 8)
				m_data = DecodeRLEGrayScale(colorMapData.ImageData, header.Width, header.Height);
			if (header.BitsPerPixel > 8)
			{
				TP_ERROR(Log::ImageTGAPrefix, "Bad GrayScale RLE pixel size: ", header.BitsPerPixel, " bits/pixel");
				TP_WARN(Log::ImageTGAPrefix, "Using Default Image!");
				return;
			}
			break;
		}

		case 2:
		{
			switch (header.BitsPerPixel)
			{
			case 16:
			{
				m_colorFormat = ColorFormat::RGB;
				m_bitsPerPixel = 24;
				m_data = ConvertBGR16ToRGB24(colorMapData.ImageData, m_width, m_height);
				break;
			}

			case 24:
			{
				m_colorFormat = ColorFormat::RGB;
				m_data = ConvertBGR24ToRGB24(colorMapData.ImageData, m_width, m_height);
				break;
			}

			case 32:
			{
				m_colorFormat = ColorFormat::RGBA;
				m_data = ConvertBGRA32ToRGBA32(colorMapData.ImageData, m_width, m_height);
				break;
			}

			default:
				TP_ERROR(Log::ImageTGAPrefix, "Bad TrueColor pixel size: ", header.BitsPerPixel, " bits/pixel");
				TP_WARN(Log::ImageTGAPrefix, "Using Default Image!");
				return;
			}
			break;
		}

		case 10:
		{
			switch (header.BitsPerPixel)
			{
			case 16:
			{
				m_colorFormat = ColorFormat::RGB;
				m_bitsPerPixel = 24;
				m_data = ConvertRLEBGR16ToRGB24(colorMapData.ImageData, m_width, m_height);
				break;
			}

			case 24:
			{
				m_colorFormat = ColorFormat::RGB;
				m_data = ConvertRLEBGR24ToRGB24(colorMapData.ImageData, m_width, m_height);
				break;
			}

			case 32:
			{
				m_colorFormat = ColorFormat::RGBA;
				m_data = ConvertRLEBGRA32ToRGBA(colorMapData.ImageData, m_width, m_height);
				break;
			}

			default:
				TP_ERROR(Log::ImageTGAPrefix, "Bad TrueColor RLE pixel size: ", header.BitsPerPixel, " bits/pixel");
				TP_WARN(Log::ImageTGAPrefix, "Using Default Image!");
				return;
			}
			break;
		}

		case 0: //Shouldn't be reached because of the above check!
			TP_ERROR(Log::ImageTGAPrefix, "Image doesn't contain pixel data!");
			TP_WARN(Log::ImageTGAPrefix, "Using Default Image!");
			return;

		default:
			TP_ERROR(Log::ImageTGAPrefix, "Unknown or invalid Image Type!");
			TP_WARN(Log::ImageTGAPrefix, "Using Default Image!");
			return;
		}

		if (needXFlip)
			m_data = FlipX(m_width, m_height, m_colorFormat, m_data.data());
		if (needYFlip)
			m_data = FlipY(m_width, m_height, m_colorFormat, m_data.data());
	}
}

//-------------------------------------------------------------------------------------------------------------------//

const void* TRAP::INTERNAL::TGAImage::GetPixelData() const
{
	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

uint64_t TRAP::INTERNAL::TGAImage::GetPixelDataSize() const
{
	return m_data.size();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::INTERNAL::TGAImage::DecodeRLEBGRAMap(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height, const uint32_t channels, std::vector<uint8_t>& colorMap)
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

std::vector<uint8_t> TRAP::INTERNAL::TGAImage::DecodeRLEGrayScale(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
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

std::vector<uint8_t> TRAP::INTERNAL::TGAImage::ConvertRLEBGR16ToRGB24(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
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

std::vector<uint8_t> TRAP::INTERNAL::TGAImage::ConvertRLEBGR24ToRGB24(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
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

std::vector<uint8_t> TRAP::INTERNAL::TGAImage::ConvertRLEBGRA32ToRGBA(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
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