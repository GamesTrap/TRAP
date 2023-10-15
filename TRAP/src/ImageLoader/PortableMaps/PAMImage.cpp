#include "TRAPPCH.h"
#include "PAMImage.h"

#include "FileSystem/FileSystem.h"
#include "Utils/Memory.h"
#include "Utils/Utils.h"

TRAP::INTERNAL::PAMImage::PAMImage(std::filesystem::path filepath)
	: Image(std::move(filepath))
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	TP_DEBUG(Log::ImagePAMPrefix, "Loading image: ", m_filepath, "");

	if (!FileSystem::Exists(m_filepath))
		return;

	std::ifstream file(m_filepath, std::ios::binary);
	if (!file.is_open())
	{
		TP_ERROR(Log::ImagePAMPrefix, "Couldn't open file path: ", m_filepath, "!");
		TP_WARN(Log::ImagePAMPrefix, "Using default image!");
		return;
	}

	std::string tempTrash;
	Header header{};
	file >> header.MagicNumber >> tempTrash >> header.Width >> tempTrash >> header.Height >> tempTrash >>
	        header.Depth >> tempTrash >> header.MaxValue >> tempTrash >> header.TuplType >> tempTrash;

	if (header.MagicNumber != "P7")
	{
		file.close();
		TP_ERROR(Log::ImagePAMPrefix, "Invalid magic number ", header.MagicNumber, "!");
		TP_WARN(Log::ImagePAMPrefix, "Using default image!");
		return;
	}
	if (header.Width < 1)
	{
		file.close();
		TP_ERROR(Log::ImagePAMPrefix, "Width is < 1 (", header.Width, ")!");
		TP_WARN(Log::ImagePAMPrefix, "Using default image!");
		return;
	}
	if (header.Height < 1)
	{
		file.close();
		TP_ERROR(Log::ImagePAMPrefix, "Height is < 1 (", header.Height, ")!");
		TP_WARN(Log::ImagePAMPrefix, "Using default image!");
		return;
	}
	if (header.MaxValue < 1 || header.MaxValue > 65535)
	{
		file.close();
		TP_ERROR(Log::ImagePAMPrefix, "MaxValue ", header.MaxValue, " is unsupported or invalid!");
		TP_WARN(Log::ImagePAMPrefix, "Using default image!");
		return;
	}

	if (header.TuplType != "GRAYSCALE" && header.TuplType != "RGB" &&
	    header.TuplType != "GRAYSCALE_ALPHA" && header.TuplType != "RGB_ALPHA")
	{
		file.close();
		TP_ERROR(Log::ImagePAMPrefix, "TuplType ", header.TuplType, " is unsupported or invalid!");
		TP_WARN(Log::ImagePAMPrefix, "Using default image!");
		return;
	}

	m_width = header.Width;
	m_height = header.Height;

	file.ignore(256, '\n'); //Skip ahead to the pixel data.

	if(header.MaxValue > 255)
	{
		if (header.TuplType == "GRAYSCALE" && header.Depth == 1)
		{
			//GrayScale
			m_bitsPerPixel = 16;
			m_colorFormat = ColorFormat::GrayScale;
		}
		else if (header.TuplType == "RGB" && header.Depth == 3)
		{
			//RGB
			m_bitsPerPixel = 48;
			m_colorFormat = ColorFormat::RGB;
		}
		else if (header.TuplType == "GRAYSCALE_ALPHA" && header.Depth == 2)
		{
			//GrayScaleAlpha
			m_bitsPerPixel = 32;
			m_colorFormat = ColorFormat::GrayScaleAlpha;
		}
		else if (header.TuplType == "RGB_ALPHA" && header.Depth == 4)
		{
			//RGBA
			m_bitsPerPixel = 64;
			m_colorFormat = ColorFormat::RGBA;
		}

		m_data2Byte.resize(NumericCast<std::size_t>(m_width) * m_height * header.Depth);
		if (!file.read(reinterpret_cast<char*>(m_data2Byte.data()),
			           NumericCast<std::streamsize>(m_width) * m_height * header.Depth *
					   NumericCast<std::streamsize>(sizeof(uint16_t))))
		{
			file.close();
			m_data2Byte.clear();
			TP_ERROR(Log::ImagePAMPrefix, "Couldn't load pixel data!");
			TP_WARN(Log::ImagePAMPrefix, "Using default image!");
			return;
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
		if (header.TuplType == "GRAYSCALE" && header.Depth == 1)
		{
			//GrayScale
			m_bitsPerPixel = 8;
			m_colorFormat = ColorFormat::GrayScale;
		}
		else if (header.TuplType == "RGB" && header.Depth == 3)
		{
			//RGB
			m_bitsPerPixel = 24;
			m_colorFormat = ColorFormat::RGB;
		}
		else if (header.TuplType == "GRAYSCALE_ALPHA" && header.Depth == 2)
		{
			//GrayScaleAlpha
			m_bitsPerPixel = 16;
			m_colorFormat = ColorFormat::GrayScaleAlpha;
		}
		else if (header.TuplType == "RGB_ALPHA" && header.Depth == 4)
		{
			//RGBA
			m_bitsPerPixel = 32;
			m_colorFormat = ColorFormat::RGBA;
		}

		m_data.resize(NumericCast<std::size_t>(m_width) * m_height * header.Depth);
		if(!file.read(reinterpret_cast<char*>(m_data.data()), NumericCast<std::streamsize>(m_width) * m_height * header.Depth))
		{
			file.close();
			m_data.clear();
			TP_ERROR(Log::ImagePAMPrefix, "Couldn't load pixel data!");
			TP_WARN(Log::ImagePAMPrefix, "Using default image!");
			return;
		}

		file.close();
	}
}
