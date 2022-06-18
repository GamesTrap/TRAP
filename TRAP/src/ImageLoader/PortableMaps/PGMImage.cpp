#include "TRAPPCH.h"
#include "PGMImage.h"

#include "Utils/String/String.h"
#include "FS/FS.h"
#include "Utils/ByteSwap.h"
#include "Utils/Utils.h"

TRAP::INTERNAL::PGMImage::PGMImage(std::filesystem::path filepath)
{
	TP_PROFILE_FUNCTION();

	m_filepath = std::move(filepath);
	m_colorFormat = ColorFormat::GrayScale;

	TP_DEBUG(Log::ImagePGMPrefix, "Loading image: \"", m_filepath.generic_u8string(), "\"");

	if (!FS::FileOrFolderExists(m_filepath))
		return;

	std::ifstream file(m_filepath, std::ios::binary);
	if (!file.is_open())
	{
		TP_ERROR(Log::ImagePGMPrefix, "Couldn't open file path: ", m_filepath.generic_u8string(), "!");
		TP_WARN(Log::ImagePGMPrefix, "Using default image!");
		return;
	}

	Header header{};
	file >> header.MagicNumber >> header.Width >> header.Height >> header.MaxValue;

	if (!(header.MagicNumber == "P2" || header.MagicNumber == "P5"))
	{
		file.close();
		TP_ERROR(Log::ImagePGMPrefix, "Unsupported format or invalid magic number (", header.MagicNumber, ")!");
		TP_WARN(Log::ImagePGMPrefix, "Using default image!");
		return;
	}
	if (header.Width < 1)
	{
		file.close();
		TP_ERROR(Log::ImagePGMPrefix, "Width is < 1 (", header.Width, ")!");
		TP_WARN(Log::ImagePGMPrefix, "Using default image!");
		return;
	}
	if (header.Height < 1)
	{
		file.close();
		TP_ERROR(Log::ImagePGMPrefix, "Height is < 1 (", header.Height, ")!");
		TP_WARN(Log::ImagePGMPrefix, "Using default image!");
		return;
	}
	if(header.MaxValue < 1 || header.MaxValue > 65535)
	{
		file.close();
		TP_ERROR(Log::ImagePGMPrefix, "MaxValue ", header.MaxValue, " is unsupported or invalid!");
		TP_WARN(Log::ImagePGMPrefix, "Using default image!");
		return;
	}

	m_width = header.Width;
	m_height = header.Height;

	file.ignore(256, '\n'); //Skip ahead to the pixel data.

	if(header.MaxValue > 255)
	{
		m_bitsPerPixel = 16;
		m_data2Byte.resize(static_cast<std::size_t>(m_width) * m_height);
		if(!file.read(reinterpret_cast<char*>(m_data2Byte.data()),
		              static_cast<std::streamsize>(m_width) * m_height *
					  static_cast<std::streamsize>(sizeof(uint16_t))))
		{
			file.close();
			m_data.clear();
			TP_ERROR(Log::ImagePGMPrefix, "Couldn't load pixel data!");
			TP_WARN(Log::ImagePGMPrefix, "Using default image!");
			return;
		}

		file.close();

		//File uses big-endian
		//Convert to machines endian
		if (Utils::GetEndian() != Utils::Endian::Big)
		{
			for (uint16_t& element : m_data2Byte)
				Utils::Memory::SwapBytes(element);
		}
	}
	else
	{
		m_bitsPerPixel = 8;
		m_data.resize(static_cast<std::size_t>(m_width) * m_height);
		if(!file.read(reinterpret_cast<char*>(m_data.data()), static_cast<std::streamsize>(m_width) * m_height))
		{
			file.close();
			m_data.clear();
			TP_ERROR(Log::ImagePGMPrefix, "Couldn't load pixel data!");
			TP_WARN(Log::ImagePGMPrefix, "Using default image!");
			return;
		}

		file.close();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

const void* TRAP::INTERNAL::PGMImage::GetPixelData() const
{
	if (!m_data2Byte.empty())
		return m_data2Byte.data();

	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

uint64_t TRAP::INTERNAL::PGMImage::GetPixelDataSize() const
{
	if (!m_data2Byte.empty())
		return m_data2Byte.size() * sizeof(uint16_t);

	return m_data.size();
}