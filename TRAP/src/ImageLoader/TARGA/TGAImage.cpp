#include "TRAPPCH.h"
#include "TGAImage.h"

#include "Utils/String/String.h"
#include "FileSystem/FileSystem.h"
#include "Utils/Memory.h"
#include "Utils/Utils.h"

TRAP::INTERNAL::TGAImage::TGAImage(std::filesystem::path filepath)
	: Image(std::move(filepath))
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

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

	header.IDLength = NumericCast<uint8_t>(file.get());
	header.ColorMapType = NumericCast<uint8_t>(file.get());
	header.ImageType = NumericCast<uint8_t>(file.get());
	file.read(reinterpret_cast<char*>(&header.ColorMapOffset), sizeof(uint16_t));
	file.read(reinterpret_cast<char*>(&header.NumOfColorMaps), sizeof(uint16_t));
	header.ColorMapDepth = NumericCast<uint8_t>(file.get());
	file.read(reinterpret_cast<char*>(&header.XOffset), sizeof(uint16_t));
	file.read(reinterpret_cast<char*>(&header.YOffset), sizeof(uint16_t));
	file.read(reinterpret_cast<char*>(&header.Width), sizeof(uint16_t));
	file.read(reinterpret_cast<char*>(&header.Height), sizeof(uint16_t));
	header.BitsPerPixel = NumericCast<uint8_t>(file.get());
	header.ImageDescriptor = NumericCast<uint8_t>(file.get());

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
		colorMapData.ColorMap.resize(NumericCast<std::size_t>(header.ColorMapDepth / 8) * header.NumOfColorMaps);
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
		const uint32_t currentPosition = static_cast<uint32_t>(file.tellg()); //Store current position in file
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
			TP_WARN(Log::ImageTGAPrefix, "Using default image!");
			return;
		}
	}
	else
	{
		colorMapData.ImageData.resize(NumericCast<std::size_t>(header.Width) * header.Height *
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
		m_data = FlipX(m_width, m_height, m_colorFormat, m_data.data());
	if (needYFlip)
		m_data = FlipY(m_width, m_height, m_colorFormat, m_data.data());
}
