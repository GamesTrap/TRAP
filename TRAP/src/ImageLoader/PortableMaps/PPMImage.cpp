#include "TRAPPCH.h"
#include "PPMImage.h"

#include "Utils/String.h"
#include "VFS/VFS.h"
#include "VFS/FileSystem.h"
#include "Application.h"
#include "Utils/ByteSwap.h"

TRAP::INTERNAL::PPMImage::PPMImage(std::string filepath)
	: m_filepath(std::move(filepath)), m_bitsPerPixel(0), m_width(0), m_height(0)
{
	TP_PROFILE_FUNCTION();

	TP_DEBUG("[Image][PPM] Loading Image: \"", Utils::String::SplitString(m_filepath, '/').back(), "\"");

	std::filesystem::path physicalPath;
	if (!VFS::SilentResolveReadPhysicalPath(m_filepath, physicalPath))
	{
		TP_ERROR("[Image][PPM] Couldn't resolve FilePath: ", m_filepath, "!");
		TP_WARN("[Image][PPM] Using Default Image!");
		return;
	}

	if (FileSystem::FileOrFolderExists(physicalPath))
	{
		std::ifstream file(physicalPath, std::ios::binary);
		if (!file.is_open())
		{
			TP_ERROR("[Image][PPM] Couldn't open FilePath: ", m_filepath, "!");
			TP_WARN("[Image][PPM] Using Default Image!");
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

		if (!(header.MagicNumber == "P3" || header.MagicNumber == "P6"))
		{
			file.close();
			TP_ERROR("[Image][PPM] Unsupported Format or invalid Magic Number!");
			TP_WARN("[Image][PPM] Using Default Image!");
			return;
		}
		if (header.Width < 1)
		{
			file.close();
			TP_ERROR("[Image][PPM] Width is < 1!");
			TP_WARN("[Image][PPM] Using Default Image!");
			return;
		}
		if (header.Height < 1)
		{
			file.close();
			TP_ERROR("[Image][PPM] Height is < 1!");
			TP_WARN("[Image][PPM] Using Default Image!");
			return;
		}
		if (header.MaxValue < 1 || header.MaxValue > 65535)
		{
			file.close();
			TP_ERROR("[Image][PPM] Max Value is unsupported/invalid!");
			TP_WARN("[Image][PPM] Using Default Image!");
			return;
		}		

		m_width = header.Width;
		m_height = header.Height;
		
		file.ignore(256, '\n'); //Skip ahead to the pixel data.
		
		if(header.MaxValue > 255)
		{
			m_bitsPerPixel = 48;
			m_data2Byte.resize(m_width * m_height * 3);
			if(!file.read(reinterpret_cast<char*>(m_data2Byte.data()), m_width * m_height * 3 * sizeof(uint16_t)))
			{
				file.close();
				TP_ERROR("[Image][PPM] Couldn't load pixel data!");
				TP_WARN("[Image][PPM] Using Default Image!");
				return;
			}

			//File uses big-endian
			//Convert to machines endian
			bool needSwap = static_cast<bool>(Application::GetEndian() != Application::Endian::Big);
			if (needSwap)
				for (uint16_t& element : m_data2Byte)
					Utils::Memory::SwapBytes(element);
		}
		else
		{
			m_bitsPerPixel = 24;
			m_data.resize(m_width * m_height * 3);
			if (!file.read(reinterpret_cast<char*>(m_data.data()), m_width * m_height * 3))
			{
				file.close();
				TP_ERROR("[Image][PPM] Couldn't load pixel data!");
				TP_WARN("[Image][PPM] Using Default Image!");
				return;
			}
		}

		file.close();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void* TRAP::INTERNAL::PPMImage::GetPixelData()
{
	if (!m_data2Byte.empty())
		return m_data2Byte.data();
	
	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PPMImage::GetPixelDataSize() const
{
	if (!m_data2Byte.empty())
		return static_cast<uint32_t>(m_data2Byte.size());
	
	return static_cast<uint32_t>(m_data.size());
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PPMImage::GetBitsPerPixel() const
{
	return m_bitsPerPixel;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PPMImage::GetBytesPerPixel() const
{
	return m_bitsPerPixel / 8;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PPMImage::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PPMImage::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PPMImage::HasAlphaChannel() const
{
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PPMImage::IsImageGrayScale() const
{
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PPMImage::IsImageColored() const
{
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PPMImage::IsHDR() const
{
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::INTERNAL::PPMImage::GetFilePath() const
{
	return m_filepath;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::ImageFormat TRAP::INTERNAL::PPMImage::GetFormat() const
{
	return ImageFormat::RGB;
}