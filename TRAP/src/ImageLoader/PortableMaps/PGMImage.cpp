#include "TRAPPCH.h"
#include "PGMImage.h"

#include "Utils/String/String.h"
#include "VFS/VFS.h"
#include "Utils/ByteSwap.h"
#include "Utils/Utils.h"

TRAP::INTERNAL::PGMImage::PGMImage(std::string filepath)
{
	TP_PROFILE_FUNCTION();

	m_filepath = std::move(filepath);
	m_colorFormat = ColorFormat::GrayScale;

	TP_DEBUG(Log::ImagePGMPrefix, "Loading image: \"", Utils::String::SplitStringView(m_filepath, '/').back(), "\"");

	std::filesystem::path physicalPath;
	if (!VFS::ResolveReadPhysicalPath(m_filepath, physicalPath, true))
	{
		TP_ERROR(Log::ImagePGMPrefix, "Couldn't resolve file path: ", m_filepath, "!");
		TP_WARN(Log::ImagePGMPrefix, "Using default image!");
		return;
	}

	if (!VFS::FileOrFolderExists(physicalPath))
		return;

	std::ifstream file(physicalPath, std::ios::binary);
	if (!file.is_open())
	{
		TP_ERROR(Log::ImagePGMPrefix, "Couldn't open file path: ", m_filepath, "!");
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
		m_data2Byte.resize(m_width * m_height);
		if(!file.read(reinterpret_cast<char*>(m_data2Byte.data()), m_width * m_height * sizeof(uint16_t)))
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
		m_data.resize(m_width * m_height);
		if(!file.read(reinterpret_cast<char*>(m_data.data()), m_width * m_height))
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