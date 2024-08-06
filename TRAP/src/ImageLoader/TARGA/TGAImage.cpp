#include "TRAPPCH.h"
#include "TGAImage.h"

#include "FileSystem/FileSystem.h"
#include "Utils/Memory.h"
#include "Utils/Utils.h"

namespace
{
	struct Header
	{
		u8 IDLength = 0; //0 = No ImageID
		u8 ColorMapType = 0; //0 = No Color Map | 1 = Has Color Map
		u8 ImageType = 0; //0 = No Data | 1 = Uncompressed ColorMapped | 2 = Uncompressed TrueColor |
								//3 = Uncompressed BlackWhite | 9 = RunLengthEncoded ColorMapped |
								//10 = RunLengthEncoded TrueColor | 11 = RunLengthEncoded BlackWhite
		//ColorMap spec
		u16 ColorMapOffset = 0;
		u16 NumOfColorMaps = 0;
		u8 ColorMapDepth = 0;
		//Image spec
		u16 XOffset = 0;
		u16 YOffset = 0;
		u16 Width = 0;
		u16 Height = 0;
		u8 BitsPerPixel = 0;
		u8 ImageDescriptor = 0;
	};

	//-------------------------------------------------------------------------------------------------------------------//

	struct ColorMapData
	{
		std::string ImageID;
		std::vector<u8> ColorMap{};
		std::vector<u8> ImageData{};
	};

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Decode run length encoded indexed BGRA data.
	/// Output format depends on channel count, if it is 4, output is RGBA, if it is 3, output is RGB and so on.
	/// @param source Run length encoded indexed BGRA data.
	/// @param width Width of the image.
	/// @param height Height of the image.
	/// @param channels Amount of channels, i.e. 4 = RGBA, 3 = RGB and so on.
	/// @param colorMap Color table.
	/// @return Decoded pixel data.
	[[nodiscard]] constexpr std::vector<u8> DecodeRLEBGRAMap(std::vector<u8>& source, const u32 width,
	                                                         const u32 height, const u32 channels,
															 std::vector<u8>& colorMap)
	{
		std::vector<u8> data{};
		data.resize(NumericCast<usize>(width) * height * channels);

		u32 index = 0;
		for (u32 i = 0, l = 0; i < source.size();)
		{
			//Pixels encoded in "packets"

			//First byte is RAW/RLE flag(upper bit) and count(1-128 as 0-127 in lower 7 bits)
			//If RAW, the next count channels-byte color values in the file are taken verbatim
			//If RLE, the next single channels-byte color value speaks for the next count pixels

			const bool raw = (source[i] & 0x80u) == 0; //Is this packet RAW pixels or a repeating color
			u32 count = (source[i] & 0x7Fu) + 1u; //How many RAW pixels or color repeats
			i++;

			//Prevent from writing out of data range
			if ((count * channels + l) > (width * height * channels))
				count = (width * height * channels - l) / channels;

			for (u32 j = 0; j < count; j++)
			{
				if (channels == 1)
				{
					data[index++] = colorMap[NumericCast<usize>(source[i]) * channels];
					l++;
				}
				else if (channels == 2)
				{
					data[index++] = NumericCast<u8>(colorMap[source[i] * channels + 1u] << 1u) & 0xF8u;
					data[index++] = NumericCast<u8>(NumericCast<u8>((colorMap[source[i] * channels + 1u]) << 6u) |
													NumericCast<u8>(colorMap[NumericCast<usize>(source[i]) * channels] >> 2u)) & 0xF8u;
					data[index++] = NumericCast<u8>(colorMap[NumericCast<usize>(source[i]) * channels] << 3u) & 0xF8u;
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

	/// @brief Decode run length encoded grayscale data.
	/// @param source Run length encoded grayscale data.
	/// @param width Width of the image.
	/// @param height Height of the image.
	/// @return Decoded grayscale pixel data.
	[[nodiscard]] constexpr std::vector<u8> DecodeRLEGrayScale(std::vector<u8>& source, const u32 width,
	                                                           const u32 height)
	{
		std::vector<u8> data{};
		data.resize(NumericCast<usize>(width) * height);

		u32 index = 0;
		for (u32 i = 0, l = 0; i < source.size();)
		{
			//Pixels encoded in "packets"

			//First byte is RAW/RLE flag(upper bit) and count(1-128 as 0-127 in lower 7 bits)
			//If RAW, the next count byte color values in the file are taken verbatim
			//If RLE, the next single byte color value speaks for the next count pixels

			const bool raw = (source[i] & 0x80u) == 0; //Is this packet RAW pixels or a repeating color
			u32 count = (source[i] & 0x7Fu) + 1u; //How many RAW pixels or color repeats
			i++;

			//Prevent from writing out of data range
			if ((count + l) > (width * height))
				count = width * height - l;

			for (u32 j = 0; j < count; j++)
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

	/// @brief Convert run length encoded BGR16 data to RGB24.
	/// @param source Run length encoded BGR16 data.
	/// @param width Width of the image.
	/// @param height Height of the image.
	/// @return Decoded RGB24 pixel data.
	[[nodiscard]] constexpr std::vector<u8> ConvertRLEBGR16ToRGB24(std::vector<u8>& source, const u32 width,
	                                                               const u32 height)
	{
		std::vector<u8> data{};
		data.resize(NumericCast<usize>(width) * height * 3);

		u32 index = 0;
		for (u32 i = 0, l = 0; i < source.size();)
		{
			//Pixels encoded in "packets"
			//First byte is RAW/RLE flags(upper bit) and count(1-128 as 0-127 in lower 7 bits)
			//If RAW, the next count channel-byte color values in the file are taken verbatim
			//If RLE, the next single channel-byte color value speaks for the next count pixels

			const bool raw = (source[i] & 0x80u) == 0; //Is this packet RAW pixels or a repeating color
			u32 count = (source[i] & 0x7Fu) + 1u; //How many RAW pixels or color repeats
			i++;

			//Prevent from writing out of data range
			if ((count * 3 + l) > (width * height * 3))
				count = (width * height * 3 - l) / 3;

			for (u32 j = 0; j < count; j++)
			{
				data[index++] = NumericCast<u8>(source[i + 1u] << 1u) & 0xF8u;
				data[index++] = NumericCast<u8>(NumericCast<u8>(source[i + 1u] << 6u) | NumericCast<u8>(source[i] >> 2u)) & 0xF8u;
				data[index++] = NumericCast<u8>(source[i] << 3u) & 0xF8u;

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

	/// @brief Convert run length encoded BGR24 data to RGB24.
	/// @param source Run length encoded BGR24 data.
	/// @param width Width of the image.
	/// @param height Height of the image.
	/// @return Decoded RGB24 pixel data.
	[[nodiscard]] constexpr std::vector<u8> ConvertRLEBGR24ToRGB24(std::vector<u8>& source, const u32 width,
	                                                               const u32 height)
	{
		std::vector<u8> data{};
		data.resize(NumericCast<usize>(width) * height * 3);

		u32 index = 0;
		for (u32 i = 0, l = 0; i < source.size();)
		{
			//Pixels encoded in "packets"
			//First byte is RAW/RLE flags(upper bit) and count(1-128 as 0-127 in lower 7 bits)
			//If RAW, the next count channel-byte color values in the file are taken verbatim
			//If RLE, the next single channel-byte color value speaks for the next count pixels

			const bool raw = (source[i] & 0x80u) == 0; //Is this packet RAW pixels or a repeating color
			u32 count = (source[i] & 0x7Fu) + 1u; //How many RAW pixels or color repeats
			i++;

			//Prevent from writing out of data range
			if ((count * 3 + l) > (width * height * 3))
				count = (width * height * 3 - l) / 3;

			for (u32 j = 0; j < count; j++)
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

	/// @brief Convert run length encoded BGRA32 data to RGBA32.
	/// @param source Run length encoded BGRA32 data.
	/// @param width Width of the image.
	/// @param height Height of the image.
	/// @return Decoded RGBA32 pixel data.
	[[nodiscard]] constexpr std::vector<u8> ConvertRLEBGRA32ToRGBA(std::vector<u8>& source, const u32 width,
	                                                               const u32 height)
	{
		std::vector<u8> data{};
		data.resize(NumericCast<usize>(width) * height * 4);

		u32 index = 0;
		for (u32 i = 0, l = 0; i < source.size();)
		{
			//Pixels encoded in "packets"
			//First byte is RAW/RLE flags(upper bit) and count(1-128 as 0-127 in lower 7 bits)
			//If RAW, the next count channel-byte color values in the file are taken verbatim
			//If RLE, the next single channel-byte color value speaks for the next count pixels

			const bool raw = (source[i] & 0x80u) == 0; //Is this packet RAW pixels or a repeating color
			u32 count = (source[i] & 0x7Fu) + 1u; //How many RAW pixels or color repeats
			i++;

			//Prevent from writing out of data range
			if ((count * 4 + l) > (width * height * 4))
				count = (width * height * 4 - l) / 4;

			for (u32 j = 0; j < count; j++)
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
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::TGAImage::TGAImage(std::filesystem::path filepath)
	: Image(std::move(filepath))
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	TP_DEBUG(Log::ImageTGAPrefix, "Loading image: ", m_filepath);

	if (!FileSystem::Exists(m_filepath))
		return;

	std::ifstream file(m_filepath, std::ios::binary);
	if (!file.is_open())
	{
		TP_ERROR(Log::ImageTGAPrefix, "Couldn't open file path: ", m_filepath, "!");
		TP_WARN(Log::ImageTGAPrefix, "Using default image!");
		return;
	}

	//Start TGA Loading here
	Header header{};

	header.IDLength = NumericCast<u8>(file.get());
	header.ColorMapType = NumericCast<u8>(file.get());
	header.ImageType = NumericCast<u8>(file.get());
	file.read(reinterpret_cast<char*>(&header.ColorMapOffset), sizeof(u16));
	file.read(reinterpret_cast<char*>(&header.NumOfColorMaps), sizeof(u16));
	header.ColorMapDepth = NumericCast<u8>(file.get());
	file.read(reinterpret_cast<char*>(&header.XOffset), sizeof(u16));
	file.read(reinterpret_cast<char*>(&header.YOffset), sizeof(u16));
	file.read(reinterpret_cast<char*>(&header.Width), sizeof(u16));
	file.read(reinterpret_cast<char*>(&header.Height), sizeof(u16));
	header.BitsPerPixel = NumericCast<u8>(file.get());
	header.ImageDescriptor = NumericCast<u8>(file.get());

	//File uses little-endian
	//Convert to machines endian
	if constexpr (Utils::GetEndian() != Utils::Endian::Little)
	{
		Utils::Memory::SwapBytes(header.ColorMapOffset);
		Utils::Memory::SwapBytes(header.NumOfColorMaps);
		Utils::Memory::SwapBytes(header.XOffset);
		Utils::Memory::SwapBytes(header.YOffset);
		Utils::Memory::SwapBytes(header.Width);
		Utils::Memory::SwapBytes(header.Height);
	}

	ColorMapData colorMapData{};

	if (header.ImageType == 0)
	{
		file.close();
		TP_ERROR(Log::ImageTGAPrefix, "Image doesn't contain pixel data!");
		TP_WARN(Log::ImageTGAPrefix, "Using default image!");
		return;
	}
	if (header.IDLength != 0)
	{
		colorMapData.ImageID.resize(header.IDLength);
		file.read(colorMapData.ImageID.data(), header.IDLength);
	}
	if (header.ColorMapType == 1)
	{
		colorMapData.ColorMap.resize(NumericCast<usize>(header.ColorMapDepth / 8) * header.NumOfColorMaps);
		if(!file.read(reinterpret_cast<char*>(colorMapData.ColorMap.data()),
		              NumericCast<std::streamsize>(header.ColorMapDepth / 8) * header.NumOfColorMaps))
		{
			file.close();
			TP_ERROR(Log::ImageTGAPrefix, "Couldn't load color map!");
			TP_WARN(Log::ImageTGAPrefix, "Using default image!");
			return;
		}
	}
	if(header.BitsPerPixel == 15)
	{
		file.close();
		TP_ERROR(Log::ImageTGAPrefix, "Bits per pixel 15 is unsupported!");
		TP_WARN(Log::ImageTGAPrefix, "Using default image!");
		return;
	}
	bool needXFlip = false;
	bool needYFlip = false;
	//1. If Image is stored Top/Right | 2. If Image is stored Bottom/Right
	if ((header.ImageDescriptor & 0x30u) == 0x30u || (header.ImageDescriptor & 0x30u) == 0x10u)
		needXFlip = true;
	//1. If Image is stored Bottom/Left | 2. If Image is stored Bottom/Right
	if ((header.ImageDescriptor & 0x30u) == 0x00u || (header.ImageDescriptor & 0x30u) == 0x10u)
		needYFlip = true;
	if (header.Width < 2)
	{
		file.close();
		TP_ERROR(Log::ImageTGAPrefix, "Image width ", header.Width, " is invalid or unsupported!");
		TP_WARN(Log::ImageTGAPrefix, "Using default image!");
		return;
	}
	if(header.Height < 2)
	{
		file.close();
		TP_ERROR(Log::ImageTGAPrefix, "Image height ", header.Height, " is invalid or unsupported!");
		TP_WARN(Log::ImageTGAPrefix, "Using default image!");
		return;
	}
	if(header.ImageType == 9 || header.ImageType == 11 || header.ImageType == 10) //All RLE formats
	{
		const u32 currentPosition = static_cast<u32>(file.tellg()); //Store current position in file
		file.seekg(0, std::ios::end); //Go to the end of file
		u32 pixelDataSize = static_cast<u32>(file.tellg()) - currentPosition;
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
			TP_WARN(Log::ImageTGAPrefix, "Using default image!");
			return;
		}
	}
	else
	{
		colorMapData.ImageData.resize(NumericCast<usize>(header.Width) * header.Height *
		                              (header.BitsPerPixel / 8));
		if (!file.read(reinterpret_cast<char*>(colorMapData.ImageData.data()),
		                                       NumericCast<std::streamsize>(header.Width) *
											   header.Height * (header.BitsPerPixel / 8)))
		{
			file.close();
			TP_ERROR(Log::ImageTGAPrefix, "Couldn't read pixel data!");
			TP_WARN(Log::ImageTGAPrefix, "Using default image!");
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
			TP_ERROR(Log::ImageTGAPrefix, "Bad color mapped index size: ", header.BitsPerPixel, " bits per pixel");
			TP_WARN(Log::ImageTGAPrefix, "Using default image!");
			return;
		}
		if (header.BitsPerPixel == 8)
		{
			m_data = DecodeBGRAMap(colorMapData.ImageData, m_width, m_height,
			                       header.ColorMapDepth / 8, colorMapData.ColorMap);
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
			TP_ERROR(Log::ImageTGAPrefix, "Bad color mapped RLE index size: ", header.BitsPerPixel, " bits per pixel");
			TP_WARN(Log::ImageTGAPrefix, "Using default image!");
			return;
		}
		if (header.BitsPerPixel == 8)
		{
			m_data = DecodeRLEBGRAMap(colorMapData.ImageData, m_width, m_height,
			                          header.ColorMapDepth / 8, colorMapData.ColorMap);
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
		if (header.BitsPerPixel > 8)
		{
			TP_ERROR(Log::ImageTGAPrefix, "Bad GrayScale pixel size: ", header.BitsPerPixel, " bits per pixel");
			TP_WARN(Log::ImageTGAPrefix, "Using default image!");
			return;
		}
		if (header.BitsPerPixel == 8)
			m_data = colorMapData.ImageData;
		m_colorFormat = ColorFormat::GrayScale;
		break;
	}

	case 11:
	{
		if (header.BitsPerPixel > 8)
		{
			TP_ERROR(Log::ImageTGAPrefix, "Bad GrayScale RLE pixel size: ", header.BitsPerPixel, " bits per pixel");
			TP_WARN(Log::ImageTGAPrefix, "Using default image!");
			return;
		}
		if (header.BitsPerPixel == 8)
			m_data = DecodeRLEGrayScale(colorMapData.ImageData, header.Width, header.Height);
		m_colorFormat = ColorFormat::GrayScale;
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
			TP_ERROR(Log::ImageTGAPrefix, "Bad TrueColor pixel size: ", header.BitsPerPixel, " bits per pixel");
			TP_WARN(Log::ImageTGAPrefix, "Using default image!");
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
			TP_ERROR(Log::ImageTGAPrefix, "Bad TrueColor RLE pixel size: ", header.BitsPerPixel, " bits per pixel");
			TP_WARN(Log::ImageTGAPrefix, "Using default image!");
			return;
		}
		break;
	}

	case 0: //Shouldn't be reached because of the above check!
		TP_ERROR(Log::ImageTGAPrefix, "Image doesn't contain pixel data!");
		TP_WARN(Log::ImageTGAPrefix, "Using default image!");
		return;

	default:
		TP_ERROR(Log::ImageTGAPrefix, "Unknown or invalid image type!");
		TP_WARN(Log::ImageTGAPrefix, "Using default image!");
		return;
	}

	if (needXFlip)
		m_data = FlipPixelDataX<u8>(m_width, m_height, GetChannelsPerPixel(), m_data);
	if (needYFlip)
		m_data = FlipPixelDataY<u8>(m_width, m_height, GetChannelsPerPixel(), m_data);
}
