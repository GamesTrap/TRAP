#include "TRAPPCH.h"
#include "PGMImage.h"

#include "Utils/String.h"
#include "VFS/VFS.h"
#include "VFS/FileSystem.h"

TRAP::INTERNAL::PGMImage::PGMImage(std::string filepath)
	: m_filepath(std::move(filepath)), m_bitsPerPixel(0), m_width(0), m_height(0)
{
	TP_DEBUG("[Image][PGM] Loading Image: \"", Utils::String::SplitString(m_filepath, '/').back(), "\"");

	std::filesystem::path physicalPath;
	if (!VFS::Get()->SilentResolveReadPhysicalPath(m_filepath, physicalPath))
	{
		TP_ERROR("[Image][PGM] Couldn't resolve FilePath: ", m_filepath, "!");
		TP_WARN("[Image][PGM] Using Default Image!");
		return;
	}

	if (FileSystem::FileOrFolderExists(physicalPath))
	{
		std::ifstream file(physicalPath, std::ios::binary);
		if (!file.is_open())
		{
			TP_ERROR("[Image][PGM] Couldn't open FilePath: ", m_filepath, "!");
			TP_WARN("[Image][PGM] Using Default Image!");
			return;
		}

		struct Header
		{
			std::string MagicNumber = "";
			std::uint32_t Width = 0;
			std::uint32_t Height = 0;
			std::uint32_t MaxValue = 255;
		} header;
		file >> header.MagicNumber >> header.Width >> header.Height >> header.MaxValue;
		file.ignore(256, '\n'); //Skip ahead to the pixel data.

		if (!(header.MagicNumber == "P2" || header.MagicNumber == "P5"))
		{
			file.close();
			TP_ERROR("[Image][PGM] Unsupported Format or invalid Magic Number!");
			TP_WARN("[Image][PGM] Using Default Image!");
			return;
		}
		if (header.Width < 1)
		{
			file.close();
			TP_ERROR("[Image][PGM] Width is < 1!");
			TP_WARN("[Image][PGM] Using Default Image!");
			return;
		}
		if (header.Height < 1)
		{
			file.close();
			TP_ERROR("[Image][PGM] Height is < 1!");
			TP_WARN("[Image][PGM] Using Default Image!");
			return;
		}
		if(header.MaxValue < 1 || header.MaxValue > 65535)
		{
			file.close();
			TP_ERROR("[Image][PGM] Max Value is unsupported/invalid!");
			TP_WARN("[Image][PGM] Using Default Image!");
			return;
		}

		m_width = header.Width;
		m_height = header.Height;

		if(header.MaxValue > 255)
		{
			m_bitsPerPixel = 16;
			m_data2Byte.reserve(m_width* m_height);
			short temp;
			for (unsigned int i = 0; i < m_width * m_height * 3; i++)
			{
				file.read(reinterpret_cast<char*>(&temp), sizeof(uint16_t));
				m_data2Byte.emplace_back(temp);
			}
		}
		else
		{
			m_bitsPerPixel = 8;
			m_data.reserve(m_width * m_height);
			m_data.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
		}

		file.close();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void* TRAP::INTERNAL::PGMImage::GetPixelData()
{
	if (!m_data2Byte.empty())
		return m_data2Byte.data();

	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PGMImage::GetPixelDataSize() const
{
	if (!m_data2Byte.empty())
		return static_cast<uint32_t>(m_data2Byte.size());

	return static_cast<uint32_t>(m_data.size());
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PGMImage::GetBitsPerPixel() const
{
	return m_bitsPerPixel;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PGMImage::GetBytesPerPixel() const
{
	return m_bitsPerPixel / 8;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PGMImage::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PGMImage::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PGMImage::HasAlphaChannel() const
{
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PGMImage::IsImageCompressed() const
{
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PGMImage::IsImageGrayScale() const
{
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PGMImage::IsImageColored() const
{
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PGMImage::IsHDR() const
{
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::INTERNAL::PGMImage::GetFilePath() const
{
	return m_filepath;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::ImageFormat TRAP::INTERNAL::PGMImage::GetFormat() const
{
	return ImageFormat::Gray_Scale;
}