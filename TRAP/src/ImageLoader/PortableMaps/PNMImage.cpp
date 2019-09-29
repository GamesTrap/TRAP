#include "TRAPPCH.h"
#include "PNMImage.h"

#include "Utils/String.h"
#include "VFS/VFS.h"
#include "VFS/FileSystem.h"

TRAP::INTERNAL::PNMImage::PNMImage(std::string filepath)
	: m_filepath(std::move(filepath)), m_bitsPerPixel(0), m_isImageGrayScale(false), m_isImageColored(false), m_width(0), m_height(0), m_format(ImageFormat::NONE)
{
	TRAP_DEBUG("[Image][PNM] Loading Image: \"", Utils::String::SplitString(m_filepath, '/').back(), "\"");

	std::filesystem::path physicalPath;
	if (!VFS::Get()->SilentResolveReadPhysicalPath(m_filepath, physicalPath))
	{
		TRAP_ERROR("[Image][PNM] Couldn't resolve FilePath: ", m_filepath, "!");
		TRAP_WARN("[Image][PNM] Using Default Image!");
		return;
	}

	if (FileSystem::FileOrFolderExists(physicalPath))
	{
		std::ifstream file(physicalPath, std::ios::binary);
		if (!file.is_open())
		{
			TRAP_ERROR("[Image][PNM] Couldn't open FilePath: ", m_filepath, "!");
			TRAP_WARN("[Image][PNM] Using Default Image!");
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

		if (!(header.MagicNumber == "P2" || header.MagicNumber == "P5" || header.MagicNumber == "P3" || header.MagicNumber == "P6"))
		{
			file.close();
			TRAP_ERROR("[Image][PNM] Unsupported Format or invalid Magic Number!");
			TRAP_WARN("[Image][PNM] Using Default Image!");
			return;
		}
		if (header.Width < 1)
		{
			file.close();
			TRAP_ERROR("[Image][PNM] Width is < 1!");
			TRAP_WARN("[Image][PNM] Using Default Image!");
			return;
		}
		if (header.Height < 1)
		{
			file.close();
			TRAP_ERROR("[Image][PNM] Height is < 1!");
			TRAP_WARN("[Image][PNM] Using Default Image!");
			return;
		}
		if (header.MaxValue < 1 || header.MaxValue > 65535)
		{
			file.close();
			TRAP_ERROR("[Image][PNM] Max Value is unsupported/invalid!");
			TRAP_WARN("[Image][PNM] Using Default Image!");
			return;
		}

		m_width = header.Width;
		m_height = header.Height;

		if(header.MaxValue > 255)
		{
			if (header.MagicNumber == "P2" || header.MagicNumber == "P5")
			{
				//GrayScale
				m_format = ImageFormat::Gray_Scale;
				m_isImageGrayScale = true;
				m_bitsPerPixel = 16;
				m_data2Byte.resize(m_width * m_height);
				if(!file.read(reinterpret_cast<char*>(m_data2Byte.data()), m_width * m_height * sizeof(uint16_t)))
				{
					file.close();
					TRAP_ERROR("[Image][PNM] Couldn't load pixel data!");
					TRAP_WARN("[Image][PNM] Using Default Image!");
					return;
				}
			}
			else if (header.MagicNumber == "P3" || header.MagicNumber == "P6")
			{
				//RGB
				m_format = ImageFormat::RGB;
				m_isImageColored = true;
				m_bitsPerPixel = 48;
				m_data2Byte.resize(m_width * m_height * 3);
				if (!file.read(reinterpret_cast<char*>(m_data2Byte.data()), m_width * m_height * 3 * sizeof(uint16_t)))
				{
					file.close();
					TRAP_ERROR("[Image][PNM] Couldn't load pixel data!");
					TRAP_WARN("[Image][PNM] Using Default Image!");
					return;
				}
			}
		}
		else 
		{
			if (header.MagicNumber == "P2" || header.MagicNumber == "P5")
			{
				//GrayScale
				m_format = ImageFormat::Gray_Scale;
				m_isImageGrayScale = true;
				m_bitsPerPixel = 8;
				m_data.resize(m_width * m_height);
				if(!file.read(reinterpret_cast<char*>(m_data.data()), m_width * m_height))
				{
					file.close();
					TRAP_ERROR("[Image][PNM] Couldn't load pixel data!");
					TRAP_WARN("[Image][PNM] Using Default Image!");
					return;
				}
			}
			else if (header.MagicNumber == "P3" || header.MagicNumber == "P6")
			{
				//RGB
				m_format = ImageFormat::RGB;
				m_isImageColored = true;
				m_bitsPerPixel = 24;
				m_data.resize(m_width * m_height * 3);
				if (!file.read(reinterpret_cast<char*>(m_data.data()), m_width * m_height * 3))
				{
					file.close();
					TRAP_ERROR("[Image][PNM] Couldn't load pixel data!");
					TRAP_WARN("[Image][PNM] Using Default Image!");
					return;
				}
			}
		}

		file.close();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void* TRAP::INTERNAL::PNMImage::GetPixelData()
{
	if (!m_data2Byte.empty())
		return m_data2Byte.data();

	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PNMImage::GetPixelDataSize() const
{
	if (!m_data2Byte.empty())
		return static_cast<uint32_t>(m_data2Byte.size());

	return static_cast<uint32_t>(m_data.size());
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PNMImage::GetBitsPerPixel() const
{
	return m_bitsPerPixel;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PNMImage::GetBytesPerPixel() const
{
	return m_bitsPerPixel / 8;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PNMImage::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PNMImage::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNMImage::HasAlphaChannel() const
{
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNMImage::IsImageCompressed() const
{
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNMImage::IsImageGrayScale() const
{
	return m_isImageGrayScale;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNMImage::IsImageColored() const
{
	return m_isImageColored;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNMImage::IsHDR() const
{
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::INTERNAL::PNMImage::GetFilePath() const
{
	return m_filepath;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::ImageFormat TRAP::INTERNAL::PNMImage::GetFormat() const
{
	return m_format;
}