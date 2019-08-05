#include "TRAPPCH.h"
#include "TGAImage.h"

#include "Utils/String.h"
#include "VFS/VFS.h"
#include "VFS/FileSystem.h"

namespace TRAP::INTERNAL
{
	std::vector<uint8_t> DecodeMap8(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height, const uint32_t channels, std::vector<uint8_t>& colorMap)
	{
		std::vector<uint8_t> data{};
		data.reserve(width * height * channels);

		for(unsigned int i = 0; i < width * height; i++)
		{
			if(channels == 1)
			{
				data.emplace_back(colorMap[source[i] * channels]);
			}
			else if(channels == 3)
			{
				data.emplace_back(colorMap[source[i] * channels + 2]);
				data.emplace_back(colorMap[source[i] * channels + 1]);
				data.emplace_back(colorMap[source[i] * channels + 0]);
			}
			else if(channels == 4)
			{
				data.emplace_back(colorMap[source[i] * channels + 2]);
				data.emplace_back(colorMap[source[i] * channels + 1]);
				data.emplace_back(colorMap[source[i] * channels + 0]);
				data.emplace_back(colorMap[source[i] * channels + 3]);
			}
		}

		return data;
	}

	std::vector<uint8_t> DecodeRGB16(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
	{
		std::vector<uint8_t> data{};
		data.reserve(width * height * 3);

		for(unsigned int i = 0; i < width * height * 2 - 1; i += 2)
		{
			data.emplace_back((source[i + 1] << 1) & 0xF8);
			data.emplace_back(((source[i + 1] << 6) | (source[i] >> 2)) & 0xF8);
			data.emplace_back((source[i] << 3) & 0xF8);
		}

		return data;
	}

	std::vector<uint8_t> DecodeRGB24(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
	{
		std::vector<uint8_t> data{};
		data.reserve(width * height * 3);

		for (unsigned int i = 0; i < width * height * 3 - 1; i += 3)
		{
			data.emplace_back(source[i + 2]);
			data.emplace_back(source[i + 1]);
			data.emplace_back(source[i]);
		}

		return data;
	}

	std::vector<uint8_t> DecodeRGB32(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
	{
		std::vector<uint8_t> data{};
		data.reserve(width * height * 4);

		for (unsigned int i = 0; i < width * height * 4 - 1; i += 4)
		{
			data.emplace_back(source[i + 2]);
			data.emplace_back(source[i + 1]);
			data.emplace_back(source[i]);
			data.emplace_back(source[i + 3]);
		}

		return data;
	}

	std::vector<uint8_t> DecodeRLEMap8(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height, const uint32_t channels, std::vector<uint8_t>& colorMap)
	{
		std::vector<uint8_t> data{};
		data.reserve(width * height * channels);

		for(unsigned int i = 0, l = 0; i < width * height - 1;)
		{
			//Pixels encoded in "packets"

			//First byte is RAW/RLE flag(upper bit) and count(1-128 as 0-127 in lower 7 bits)
			//If RAW, the next count channels-byte color values in the file are taken verbatim
			//If RLE, the next single channels-byte color value speaks for the next count pixels

			const int raw = (source[i] & 0x80) == 0; //Is this packet RAW pixels or a repeating color
			int count = (source[i] & 0x7F) + 1; //How many RAW pixels or color repeats
			i++;

			//Prevent from writing out of data range
			if (count * channels + l > width * height * channels)
				count = static_cast<int>((width * height * channels - l) / channels);

			for(int j = 0; j < count; j++)
			{
				if (channels == 1)
				{
					data.emplace_back(colorMap[source[i] * channels]);
					l++;
				}
				else if (channels == 3)
				{
					data.emplace_back(colorMap[source[i] * channels + 2]);
					data.emplace_back(colorMap[source[i] * channels + 1]);
					data.emplace_back(colorMap[source[i] * channels + 0]);
					l += 3;
				}
				else if (channels == 4)
				{
					data.emplace_back(colorMap[source[i] * channels + 2]);
					data.emplace_back(colorMap[source[i] * channels + 1]);
					data.emplace_back(colorMap[source[i] * channels + 0]);
					data.emplace_back(colorMap[source[i] * channels + 3]);
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

	std::vector<uint8_t> DecodeRLEGrayScale(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
	{
		std::vector<uint8_t> data{};
		data.reserve(width * height);

		for (unsigned int i = 0, l = 0; i < width * height - 1;)
		{
			//Pixels encoded in "packets"

			//First byte is RAW/RLE flag(upper bit) and count(1-128 as 0-127 in lower 7 bits)
			//If RAW, the next count byte color values in the file are taken verbatim
			//If RLE, the next single byte color value speaks for the next count pixels

			const int raw = (source[i] & 0x80) == 0; //Is this packet RAW pixels or a repeating color
			int count = (source[i] & 0x7F) + 1; //How many RAW pixels or color repeats
			i++;

			//Prevent from writing out of data range
			if (count + l > width * height)
				count = static_cast<int>(width * height - l);

			for (int j = 0; j < count; j++)
			{
				data.emplace_back(source[i]);

				if (raw) //In RAW mode, keep advancing to subsequent values
					i++; //In RLE mode, just repeat the packet[1] color
				l++;
			}
			if (!raw) //After outputting count values, advance if RLE
				i++;
		}

		return data;
	}

	std::vector<uint8_t> DecodeRLERGB16(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
	{
		std::vector<uint8_t> data{};
		data.reserve(width * height * 3);

		for (unsigned int i = 0, l = 0; i < width * height * 2 - 1;)
		{
			//Pixels encoded in "packets"
			//First byte is RAW/RLE flags(upper bit) and count(1-128 as 0-127 in lower 7 bits)
			//If RAW, the next count channel-byte color values in the file are taken verbatim
			//If RLE, the next single channel-byte color value speaks for the next count pixels

			const int raw = (source[i] & 0x80) == 0; //Is this packet RAW pixels or a repeating color
			int count = (source[i] & 0x7F) + 1; //How many RAW pixels or color repeats
			i++;

			//Prevent from writing out of data range
			if (count * 3 + l > width * height * 3)
				count = static_cast<int>((width * height * 3 - l) / 3);

			for (int j = 0; j < count; j++)
			{
				data.emplace_back((source[i + 1] << 1) & 0xF8);
				data.emplace_back(((source[i + 1] << 6) | (source[i] >> 2)) & 0xF8);
				data.emplace_back((source[i] << 3) & 0xF8);

				if (raw) //In RAW mode, keep advancing to subsequent values
					i += 2; //IN RLE mode, just repeat the packet[1] RGB color
				l += 3;
			}
			if (!raw) //After outputting count RGB values, advance if RLE
				i += 2;
		}

		return data;
	}

	std::vector<uint8_t> DecodeRLERGB24(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
	{
		std::vector<uint8_t> data{};
		data.reserve(width * height * 3);

		for(unsigned int i = 0, l = 0; i < width * height * 3 - 1;)
		{
			//Pixels encoded in "packets"
			//First byte is RAW/RLE flags(upper bit) and count(1-128 as 0-127 in lower 7 bits)
			//If RAW, the next count channel-byte color values in the file are taken verbatim
			//If RLE, the next single channel-byte color value speaks for the next count pixels

			const int raw = (source[i] & 0x80) == 0; //Is this packet RAW pixels or a repeating color
			int count = (source[i] & 0x7F) + 1; //How many RAW pixels or color repeats
			i++;

			//Prevent from writing out of data range
			if (count * 3 + l > width * height * 3)
				count = static_cast<int>((width * height * 3 - l) / 3);

			for(int j = 0; j < count; j++)
			{
				data.emplace_back(source[i + 2]); //Red
				data.emplace_back(source[i + 1]); //Green
				data.emplace_back(source[i]);     //Blue

				if (raw) //In RAW mode, keep advancing to subsequent values
					i += 3; //IN RLE mode, just repeat the packet[1] RGB color
				l += 3;
			}
			if (!raw) //After outputting count RGB values, advance if RLE
				i += 3;
		}

		return data;
	}

	std::vector<uint8_t> DecodeRLERGB32(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
	{
		std::vector<uint8_t> data{};
		data.reserve(width * height * 4);

		for (unsigned int i = 0, l = 0; i < width * height * 4 - 1;)
		{
			//Pixels encoded in "packets"
			//First byte is RAW/RLE flags(upper bit) and count(1-128 as 0-127 in lower 7 bits)
			//If RAW, the next count channel-byte color values in the file are taken verbatim
			//If RLE, the next single channel-byte color value speaks for the next count pixels

			const int raw = (source[i] & 0x80) == 0; //Is this packet RAW pixels or a repeating color
			int count = (source[i] & 0x7F) + 1; //How many RAW pixels or color repeats
			i++;

			//Prevent from writing out of data range
			if (count * 4 + l > width * height * 4)
				count = static_cast<int>((width * height * 4 - l) / 4);

			for (int j = 0; j < count; j++)
			{
				data.emplace_back(source[i + 2]); //Red
				data.emplace_back(source[i + 1]); //Green
				data.emplace_back(source[i]);     //Blue
				data.emplace_back(source[i + 3]); //Alpha

				if (raw) //In RAW mode, keep advancing to subsequent values
					i += 4; //IN RLE mode, just repeat the packet[1] RGBA color
				l += 4;
			}
			if (!raw) //After outputting count RGBA values, advance if RLE
				i += 4;
		}

		return data;
	}
}

TRAP::INTERNAL::TGAImage::TGAImage(std::string filepath)
	: m_filepath(std::move(filepath)),
	m_bitsPerPixel(0),
	m_width(0),
	m_height(0),
	m_isImageGrayScale(false),
	m_isImageColored(false),
	m_isImageCompressed(false),
	m_hasAlphaChannel(false),
	m_imageFormat(ImageFormat::NONE)
{
	TP_DEBUG("[Image][TGA] Loading Image: \"", Utils::String::SplitString(m_filepath, '/').back(), "\"");

	std::filesystem::path physicalPath;
	if (!VFS::Get()->SilentResolveReadPhysicalPath(m_filepath, physicalPath))
	{
		TP_ERROR("[Image][TGA] Couldn't resolve FilePath: ", m_filepath, "!");
		TP_WARN("[Image][TGA] Using Default Image!");
		return;
	}

	if (FileSystem::FileOrFolderExists(physicalPath))
	{
		std::ifstream file(physicalPath, std::ios::binary);
		if (!file.is_open())
		{
			TP_ERROR("[Image][TGA] Couldn't open FilePath: ", m_filepath, "!");
			TP_WARN("[Image][TGA] Using Default Image!");
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

		struct ColorMapData
		{
			std::string ImageID = "";
			std::vector<uint8_t> ColorMap{};
			std::vector<uint8_t> ImageData{};
		} colorMapData;

		if (header.ImageType == 0)
		{
			TP_ERROR("[Image][TGA] Image doesn't contain pixel data!");
			TP_WARN("[Image][TGA] Using Default Image!");
			return;
		}
		if (header.IDLength != 0)
		{
			colorMapData.ImageID.reserve(header.IDLength);
			file.read(static_cast<char*>(colorMapData.ImageID.data()), header.IDLength);
		}
		if (header.ColorMapType == 1)
		{
			colorMapData.ColorMap.reserve((header.ColorMapDepth / 8) * header.NumOfColorMaps);
			uint8_t temp;
			for (unsigned int i = 0; i < static_cast<unsigned int>((header.ColorMapDepth / 8) * header.NumOfColorMaps); i++)
			{
				file.read(reinterpret_cast<char*>(&temp), sizeof(uint8_t));
				colorMapData.ColorMap.emplace_back(temp);
			}
		}
		if ((header.ImageDescriptor & 0x30) != 0x20 && (header.ImageDescriptor & 0x30) != 0x00)
		{
			TP_ERROR("[Image][TGA] Not Top/Bottom Left origin!");
			TP_WARN("[Image][TGA] Using Default Image!");
			return;
		}
		bool needXFlip = false;
		bool needYFlip = false;
		if ((header.ImageDescriptor & 0x30) == 0x30 || (header.ImageDescriptor & 0x30) == 0x10) //1. If Image is stored Top/Right | 2. If Image is stored Bottom/Right
			needXFlip = true;
		if ((header.ImageDescriptor & 0x30) == 0x00 || (header.ImageDescriptor & 0x30) == 0x10) //1. If Image is stored Bottom/Left | 2. If Image is stored Bottom/Right
			needYFlip = true;
		if (header.Width < 1 || header.Height < 1)
		{
			TP_ERROR("[Image][TGA] Image Width/Height is invalid!");
			TP_WARN("[Image][TGA] Using Default Image!");
			return;
		}
		colorMapData.ImageData.reserve(header.Width * header.Height * (header.BitsPerPixel / 8));
		uint8_t temp;
		for (unsigned int i = 0; i < static_cast<unsigned int>(header.Width * header.Height * (header.BitsPerPixel / 8)); i++)
		{
			file.read(reinterpret_cast<char*>(&temp), sizeof(uint8_t));
			colorMapData.ImageData.emplace_back(temp);
		}

		m_width = header.Width;
		m_height = header.Height;
		m_bitsPerPixel = header.BitsPerPixel;
		switch (header.ImageType)
		{
		case 1:
		{
			m_isImageColored = true;
			if (header.BitsPerPixel > 8)
			{
				TP_ERROR("[Image][TGA] Bad ColorMapped index size: ", header.BitsPerPixel, " bits/pixel");
				TP_WARN("[Image][TGA] Using Default Image!");
				return;
			}
			if (header.BitsPerPixel == 8)
			{
				m_data = DecodeMap8(colorMapData.ImageData, m_width, m_height, header.ColorMapDepth / 8, colorMapData.ColorMap);
				m_bitsPerPixel = header.ColorMapDepth;
				if (m_bitsPerPixel == 24)
					m_imageFormat = ImageFormat::RGB;					
				else if (m_bitsPerPixel == 32)
				{
					m_imageFormat = ImageFormat::RGBA;
					m_hasAlphaChannel = true;
				}
				break;
			}
			break;
		}

		case 9:
		{
			m_isImageColored = true;
			m_imageFormat = ImageFormat::RGB;
			m_isImageCompressed = true;
			if (header.BitsPerPixel > 8)
			{
				TP_ERROR("[Image][TGA] Bad ColorMapped RLE index size: ", header.BitsPerPixel, " bits/pixel");
				TP_WARN("[Image][TGA] Using Default Image!");
				return;
			}
			if (header.BitsPerPixel == 8)
			{
				m_data = DecodeRLEMap8(colorMapData.ImageData, m_width, m_height, header.ColorMapDepth / 8, colorMapData.ColorMap);
				m_bitsPerPixel = header.ColorMapDepth;
				if (m_bitsPerPixel == 24)
					m_imageFormat = ImageFormat::RGB;
				else if (m_bitsPerPixel == 32)
				{
					m_imageFormat = ImageFormat::RGBA;
					m_hasAlphaChannel = true;
				}
				TP_CRITICAL("[Image][TGA] Compressed(RLE) ColorMapped is WIP");
				TP_WARN("[Image][TGA] Using Default Image!");
				return;
			}
			break;
		}

		case 3:
		{
			m_isImageGrayScale = true;
			m_imageFormat = ImageFormat::Gray_Scale;
			if (header.BitsPerPixel == 8)
				m_data = colorMapData.ImageData;
			if (header.BitsPerPixel > 8)
			{
				TP_ERROR("[Image][TGA] Bad GrayScale pixel size: ", header.BitsPerPixel, " bits/pixel");
				TP_WARN("[Image][TGA] Using Default Image!");
				return;
			}
			break;
		}

		case 11:
		{
			m_isImageGrayScale = true;
			m_imageFormat = ImageFormat::Gray_Scale;
			m_isImageCompressed = true;
			if (header.BitsPerPixel == 8)
				m_data = DecodeRLEGrayScale(colorMapData.ImageData, header.Width, header.Height);
			if (header.BitsPerPixel > 8)
			{
				TP_ERROR("[Image][TGA] Bad GrayScale RLE pixel size: ", header.BitsPerPixel, " bits/pixel");
				TP_WARN("[Image][TGA] Using Default Image!");
				return;
			}
			break;
		}

		case 2:
		{
			m_isImageColored = true;
			switch (header.BitsPerPixel)
			{
			case 16:
			{
				m_imageFormat = ImageFormat::RGB;
				m_data = DecodeRGB16(colorMapData.ImageData, m_width, m_height);
				break;
			}

			case 24:
			{
				m_imageFormat = ImageFormat::RGB;
				m_data = DecodeRGB24(colorMapData.ImageData, m_width, m_height);
				break;
			}

			case 32:
			{
				m_imageFormat = ImageFormat::RGBA;
				m_hasAlphaChannel = true;
				m_data = DecodeRGB32(colorMapData.ImageData, m_width, m_height);
				break;
			}

			default:
				TP_ERROR("[Image][TGA] Bad TrueColor pixel size: ", header.BitsPerPixel, " bits/pixel");
				TP_WARN("[Image][TGA] Using Default Image!");
				return;
			}
			break;
		}

		case 10:
		{
			m_isImageColored = true;
			switch (header.BitsPerPixel)
			{
			case 16:
			{
				m_imageFormat = ImageFormat::RGB;
				m_data = DecodeRLERGB16(colorMapData.ImageData, m_width, m_height);
				break;
			}

			case 24:
			{
				m_imageFormat = ImageFormat::RGB;
				m_data = DecodeRLERGB24(colorMapData.ImageData, m_width, m_height);
				break;
			}

			case 32:
			{
				m_imageFormat = ImageFormat::RGBA;
				m_hasAlphaChannel = true;
				m_data = DecodeRLERGB32(colorMapData.ImageData, m_width, m_height);
				break;
			}

			default:
				TP_ERROR("[Image][TGA] Bad TrueColor RLE pixel size: ", header.BitsPerPixel, " bits/pixel");
				TP_WARN("[Image][TGA] Using Default Image!");
				return;
			}
			break;
		}

		case 0: //Shouldn't be reached because of the above check!
			TP_ERROR("[Image][TGA] Image doesn't contain pixel data!");
			TP_WARN("[Image][TGA] Using Default Image!");
			return;

		default:
			TP_ERROR("[Image][TGA] Unknown or invalid Image Type!");
			TP_WARN("[Image][TGA] Using Default Image!");
			return;
		}

		if (needXFlip)
		{
			TP_WARN("[Image][TGA] Image Flipping on X axis is untested!");
			m_data = FlipX(m_width, m_height, m_imageFormat, m_data.data());
		}
		if (needYFlip)
			m_data = FlipY(m_width, m_height, m_imageFormat, m_data.data());

		file.close();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void* TRAP::INTERNAL::TGAImage::GetPixelData()
{
	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::TGAImage::GetPixelDataSize() const
{
	return static_cast<uint32_t>(m_data.size());
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::TGAImage::GetBitsPerPixel() const
{
	return m_bitsPerPixel;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::TGAImage::GetBytesPerPixel() const
{
	return m_bitsPerPixel / 8;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::TGAImage::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::TGAImage::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::TGAImage::HasAlphaChannel() const
{
	return m_hasAlphaChannel;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::TGAImage::IsImageCompressed() const
{
	return m_isImageCompressed;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::TGAImage::IsImageGrayScale() const
{
	return m_isImageGrayScale;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::TGAImage::IsImageColored() const
{
	return m_isImageColored;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::TGAImage::IsHDR() const
{
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::INTERNAL::TGAImage::GetFilePath() const
{
	return m_filepath;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::ImageFormat TRAP::INTERNAL::TGAImage::GetFormat() const
{
	return m_imageFormat;
}