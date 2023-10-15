#include "TRAPPCH.h"
#include "PNMImage.h"

#include "FileSystem/FileSystem.h"
#include "Utils/Memory.h"
#include "Utils/Utils.h"

TRAP::INTERNAL::PNMImage::PNMImage(std::filesystem::path filepath)
	: Image(std::move(filepath))
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	TP_DEBUG(Log::ImagePNMPrefix, "Loading image: ", m_filepath);

	if (!FileSystem::Exists(m_filepath))
		return;

	std::ifstream file(m_filepath, std::ios::binary);
	if (!file.is_open())
	{
		TP_ERROR(Log::ImagePNMPrefix, "Couldn't open file path: ", m_filepath, "!");
		TP_WARN(Log::ImagePNMPrefix, "Using default image!");
		return;
	}

	Header header{};
	file >> header.MagicNumber >> header.Width >> header.Height >> header.MaxValue;

	if (header.MagicNumber != "P2" && header.MagicNumber != "P5" &&
	    header.MagicNumber != "P3" && header.MagicNumber != "P6")
	{
		file.close();
		TP_ERROR(Log::ImagePNMPrefix, "Unsupported format or invalid magic number (", header.MagicNumber, ")!");
		TP_WARN(Log::ImagePNMPrefix, "Using default image!");
		return;
	}
	if (header.Width < 1)
	{
		file.close();
		TP_ERROR(Log::ImagePNMPrefix, "Width is < 1 (", header.Width, ")!");
		TP_WARN(Log::ImagePNMPrefix, "Using default image!");
		return;
	}
	if (header.Height < 1)
	{
		file.close();
		TP_ERROR(Log::ImagePNMPrefix, "Height is < 1 (", header.Height, ")!");
		TP_WARN(Log::ImagePNMPrefix, "Using default image!");
		return;
	}
	if (header.MaxValue < 1 || header.MaxValue > 65535)
	{
		file.close();
		TP_ERROR(Log::ImagePNMPrefix, "MaxValue ", header.MaxValue, " is unsupported or invalid!");
		TP_WARN(Log::ImagePNMPrefix, "Using default image!");
		return;
	}

	m_width = header.Width;
	m_height = header.Height;

	file.ignore(256, '\n'); //Skip ahead to the pixel data.

	if(header.MaxValue > 255)
	{
		if (header.MagicNumber == "P2" || header.MagicNumber == "P5")
		{
			//GrayScale
			m_colorFormat = ColorFormat::GrayScale;
			m_bitsPerPixel = 16;
			m_data2Byte.resize(NumericCast<std::size_t>(m_width) * m_height);
			if(!file.read(reinterpret_cast<char*>(m_data2Byte.data()),
			              NumericCast<std::streamsize>(m_width) * m_height *
						  NumericCast<std::streamsize>(sizeof(uint16_t))))
			{
				file.close();
				TP_ERROR(Log::ImagePNMPrefix, "Couldn't load pixel data!");
				TP_WARN(Log::ImagePNMPrefix, "Using default image!");
				return;
			}
		}
		else if (header.MagicNumber == "P3" || header.MagicNumber == "P6")
		{
			//RGB
			m_colorFormat = ColorFormat::RGB;
			m_bitsPerPixel = 48;
			m_data2Byte.resize(NumericCast<std::size_t>(m_width) * m_height * 3);
			if (!file.read(reinterpret_cast<char*>(m_data2Byte.data()),
						   NumericCast<std::streamsize>(m_width) * m_height * 3 *
						   NumericCast<std::streamsize>(sizeof(uint16_t))))
			{
				file.close();
				TP_ERROR(Log::ImagePNMPrefix, "Couldn't load pixel data!");
				TP_WARN(Log::ImagePNMPrefix, "Using default image!");
				return;
			}
		}

		file.close();

		//File uses big-endian
		//Convert to machines endian
		if constexpr (Utils::GetEndian() != Utils::Endian::Big)
		{
			for (uint16_t& element : m_data2Byte)
				Utils::Memory::SwapBytes(element);
		}
	}
	else
	{
		if (header.MagicNumber == "P2" || header.MagicNumber == "P5")
		{
			//GrayScale
			m_colorFormat = ColorFormat::GrayScale;
			m_bitsPerPixel = 8;
			m_data.resize(NumericCast<std::size_t>(m_width) * m_height);
			if(!file.read(reinterpret_cast<char*>(m_data.data()),
						  NumericCast<std::streamsize>(m_width) * m_height))
			{
				file.close();
				TP_ERROR(Log::ImagePNMPrefix, "Couldn't load pixel data!");
				TP_WARN(Log::ImagePNMPrefix, "Using default image!");
				return;
			}
		}
		else if (header.MagicNumber == "P3" || header.MagicNumber == "P6")
		{
			//RGB
			m_colorFormat = ColorFormat::RGB;
			m_bitsPerPixel = 24;
			m_data.resize(NumericCast<std::size_t>(m_width) * m_height * 3);
			if (!file.read(reinterpret_cast<char*>(m_data.data()),
						   NumericCast<std::streamsize>(m_width) * m_height * 3))
			{
				file.close();
				TP_ERROR(Log::ImagePNMPrefix, "Couldn't load pixel data!");
				TP_WARN(Log::ImagePNMPrefix, "Using default image!");
				return;
			}
		}

		file.close();
	}
}
